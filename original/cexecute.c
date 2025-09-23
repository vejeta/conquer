/*conquer : Copyright (c) 1988 by Ed Barlow.
 *  I spent a long time writing this code & I hope that you respect this.
 *  I give permission to alter the code, but not to copy or redistribute
 *  it without my explicit permission.  If you alter the code,
 *  please document changes and send me a copy, so all can have it.
 *  This code, to the best of my knowledge works well,  but it is my first
 *  'C' program and should be treated as such.  I disclaim any
 *  responsibility for the codes actions (use at your own risk).  I guess
 *  I am saying "Happy gaming", and am trying not to get sued in the process.
 *                                                Ed
 */

/*EXECUTE THE PROGRAM*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include "header.h"
#include "data.h"
extern long startgold;
extern short country;
extern FILE *fexe, *fnews;
extern char fison[];

#ifdef CONQUER
extern int roads_this_turn;
extern int terror_adj;
#endif

int
execute(isupdate)
int	isupdate;	/* 0 if not update, 1 if update */
{
	FILE *fp, *fopen();
	int cmd,savectry;
	char comment[LINELTH];
	char temp[LINELTH];
	long longvar,long2var;
	int armynum;
	short int x,y;
	int execed=0;
	int done=FALSE;
	char line[BIGLTH];

	/* initialize startgold */
	if( isupdate==0) startgold = curntn->tgold;

	/* initialize i_people */
	for(x=0;x<MAPX;x++)
		for(y=0;y<MAPY;y++)
			if(( sct[x][y].owner == country)&&
			((sct[x][y].designation == DTOWN)
			||( sct[x][y].designation == DCAPITOL)
			||( sct[x][y].designation == DCITY)))
/* note: i_people is a short, so we must scale to allow for people >= 32K */
				sct[x][y].i_people = sct[x][y].people/256;
			else
				sct[x][y].i_people = -1;

	/*open exefile file*/
	sprintf(line,"%s%d",exefile,country);
	if ((fp=fopen(line,"r"))==NULL) {
		/*THIS MEANS THAT THE NATION HAS NOT MOVED YET*/
		return(0);
	}
	savectry=country;

	/*read in file*/
	if(fgets(line,80,fp)==NULL) done=TRUE;
	while(done==FALSE) {
		/*read and parse a new line*/
		/*CODE IF YOU USE LONG VAR IS L_*/
		if( line[0] == 'L' && line[1] == '_' ) {
			sscanf(line,"%s %d %hd %ld %ld %hd %s",
				temp,&cmd,&country,&longvar,&long2var,&y,comment);
		} else {
			sscanf(line,"%s %d %hd %d %hd %hd %s",
				temp,&cmd,&country,&armynum,&x,&y,comment);
		}
		curntn = &ntn[country];

		execed=1;
		switch(cmd){
		case XASTAT:		/*Aadjstat*/
			if(x>0)  P_ASTAT=x;
			break;
		case XAMEN:	/*Aadjmen*/
			armynum= (int) longvar;
			P_ASOLD= (int) long2var;
			P_ATYPE= y;
			break;
		case XBRIBE:	/* nation has been bribed */
			if(!isupdate) break;	/* only work on update */
			ntn[y].tgold += longvar;
#ifdef DEBUG
	fprintf(stderr,"DEBUG: BRIBE BY %s of %s\n",ntn[country].name,ntn[y].name);
#endif /* DEBUG */
			/* x represents chance of successful bribe */
			if(npctype(curntn->active)==npctype(ntn[y].active))
				x = 50;
			else if(isneutral(ntn[y].active)) x=30;
			else if(npctype(ntn[y].active)==ISOLATIONIST) x=15;
			else	x = 20;
			if(curntn->race==ntn[y].race) x+= 20;
			if( rand()%100 < x){
#ifdef DEBUG
	fprintf(stderr,"DEBUG: BRIBE IS SUCCESS\n");
#endif /* DEBUG */
				ntn[y].dstatus[country]--;
			}
			break;
		case XALOC:	/*Aadjloc*/
			P_AXLOC=x;
			P_AYLOC=y;
			break;
		case MSETA:	/*Aadjmerc*/
#ifdef CONQUER
			mercgot+=armynum;
#endif
#ifdef ADMIN
			MERCMEN-=armynum;
#endif
			break;
		case MSETB:	/*Aadjdisb*/
			/* only allow raising of merc bonus */
			if (x>MERCATT)
			MERCATT=(MERCMEN*MERCATT+armynum*x)/(MERCMEN+armynum);
			if (y>MERCDEF)
			MERCDEF=(MERCMEN*MERCDEF+armynum*y)/(MERCMEN+armynum);
			MERCMEN+=armynum;
			break;
		case XNLOC: /*nadjloc*/
			curntn->nvy[armynum].xloc=x;
			curntn->nvy[armynum].yloc=y;
			break;
		case XNACREW:
			curntn->nvy[armynum].crew=x;
			curntn->nvy[armynum].armynum=y;
			break;
		case XNAMER: /*nadjmer*/
			curntn->nvy[armynum].merchant=x;
			break;
		case XNAWAR: /*nadjwar*/
			curntn->nvy[armynum].warships=x;
			break;
		case XNAGAL: /*nadjgal*/
			curntn->nvy[armynum].galleys=x;
			break;
		case XNAHOLD: /*nadjhld*/
			curntn->nvy[armynum].people=(unsigned char)y;
			curntn->nvy[armynum].armynum=(unsigned char)x;
			break;
		case XECNAME:	/*Nadjname*/
			strcpy(curntn->name,comment);
			break;
		case XECPAS:	/*Nadjpas*/
			strncpy(curntn->passwd,comment,PASSLTH);
			break;
		case NPOP:	/* set various nation attributes */
#ifdef CONQUER
			terror_adj++;
#endif /* CONQUER */
			curntn->popularity = armynum;
			curntn->terror = x;
			curntn->reputation = y;
			break;
		case NTAX:	/* set nations tax rate */
#ifdef ADMIN
			if (ispc(curntn->active) && !ispc(x)) {
			  fprintf(fnews, "1.\tNation %s is now being run by the computer.\n", curntn->name);
			}
			if (ispc(x) && !ispc(curntn->active)) {
			  fprintf(fnews, "1.\tNation %s is no longer being run by the computer.\n", curntn->name);
			}
#endif /*ADMIN*/
			curntn->tax_rate = armynum;
			curntn->active = x;
			curntn->charity = y;
			break;
		case EDSPL:	/*Edecspl*/
			curntn->spellpts-=armynum;
			break;
		case XSADES:	/*Sadjdes*/
			if((sct[x][y].owner!=country)&&(country!=0)) {
				fprintf(stderr,"ERROR: <%s> redesignate sector %d,%d that is not owned\n",curntn->name,x,y);
				break;
			}

			sct[x][y].designation=comment[0];
			if(sct[x][y].designation==DCAPITOL){
				curntn->capx=x;
				curntn->capy=y;
			}
#ifdef CONQUER
			if (sct[x][y].designation==DROAD)
				roads_this_turn++;
#endif
			break;
		case XSACIV:	/*Sadjciv*/
			/* if for some reason you dont own it, put people
			back into your capitol */
			if((sct[x][y].owner!=country)&&(country!=0)) {
				sct[curntn->capx][curntn->capy].people+=armynum;
				break;
			}
			sct[x][y].people=armynum;
			break;
		case XSACIV3:	/*Sadjciv3 - incremental people adjust */
			/* if you dont own it, put people in your capitol */
			if((sct[x][y].owner!=country)&&(country!=0)) {
				sct[curntn->capx][curntn->capy].people+=armynum;
			}
			else
			{
				sct[x][y].people+=armynum;
			}
			break;
		case XSIFORT:	/*Sincfort*/
			sct[x][y].fortress++;
			break;
		case XNAGOLD:	/*Nadjgold:*/
			curntn->tgold = longvar;
			break;
		case XAMOV:
			P_AMOVE=x;
			break;
		case XNMOV:
			curntn->nvy[armynum].smove=x;
			break;
		case XSAOWN:
			/* if not own it, and if people there, problem */
			if((sct[x][y].owner!=country)
			&&( country!=0)
			&&( sct[x][y].owner!=0 )
			&&( sct[x][y].people>0 )
			&&( magic(country,SLAVER)==FALSE )
			&&( ntn[sct[x][y].owner].race != curntn->race)){
				sct[ntn[sct[x][y].owner].capx][ntn[sct[x][y].owner].capy].people+= sct[x][y].people;
				sct[x][y].people=0;
				fprintf(stderr,"ERROR: <%s> taking sector %d %d but civilians exist of other race - puting them in their capitol\n",curntn->name,x,y);
			}
			if(curntn->popularity<MAXTGVAL) curntn->popularity++;
			sct[x][y].owner=country;
			break;
		case EDADJ:
			curntn->dstatus[armynum]=x;
			break;
		case XNARGOLD:
			curntn->jewels = longvar;
			break;
		case XNAMETAL:
			curntn->metals = longvar;
			break;
		case INCAPLUS:
			curntn->aplus++;
			break;
		case INCDPLUS:
			curntn->dplus++;
			break;
		case DESTRY:
			sct[ntn[armynum].capx][ntn[armynum].capy].owner=savectry;
			country=armynum;
			if (isupdate) destroy(country);
			country=savectry;
			break;
		case CHG_MGK:
			curntn->powers|=long2var;
			if(curntn->powers!=longvar){
			printf("\nERROR ON MAGIC READ country=%d %ld != %ld (or of %ld)",country,longvar,curntn->powers,long2var);
			getchar();
			}
			for(armynum=0;armynum<=MAXPOWER;armynum++) {
				longvar = 1L << armynum;
				if (longvar & long2var)
					exenewmgk(longvar);
			}
			long2var=0;
			break;
		}
		if(fgets(line,80,fp)==NULL) done=TRUE;
	}
	fclose(fp);
	/*return 1 if it did something*/
	country=savectry;
	curntn = &ntn[country];
	if(execed==1) return(1);
	else return(0);
}
#ifdef CONQUER
void
hangup()
{
	char line[LINELTH];

	if(country==0) writedata();
	else {
		fprintf(fexe,"L_NGOLD\t%d \t%d \t%ld \t0 \t0 \t%s\n",
		XNAGOLD ,country,curntn->tgold,"null");
		fprintf(fexe,"L_NMETAL\t%d \t%d \t%ld \t0 \t0 \t%s\n",
		XNAMETAL ,country,curntn->metals,"null");
		fprintf(fexe,"L_NJWLS\t%d \t%d \t%ld \t0 \t0 \t%s\n",
		XNARGOLD ,country,curntn->jewels,"null");
	}
	/*close file*/
	fclose(fexe);

	/* remove any existing mail reading/writing file */
	if (mailok!=DONEMAIL) {
		extern char tmp_mail_name[];
		unlink(tmp_mail_name);
	}

	/* remove the lock file */
	unlink(fison);
	sprintf(line,"%s%hd.tmp",msgfile,country);
	unlink(line);

	/*send a message to God*/
	if(mailopen( 0 )!=(-1)) {
		fprintf(fm,"WARNING: Nation %s hungup on me.\n",curntn->name);
		mailclose(0);
	}

	/* exit program */
	exit(FAIL);
}
#endif /* CONQUER */
