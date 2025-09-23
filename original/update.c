/* Conquer: Copyright (c) 1988 by Edward M Barlow
 *  I spent a long time writing this code & I hope that you respect this.
 *  I give permission to alter the code, but not to copy or redistribute
 *  it without my explicit permission.  If you alter the code,
 *  please document changes and send me a copy, so all can have it.
 *  This code, to the best of my knowledge works well,  but it is my first
 *  'C' program and should be treated as such.  I disclaim any
 *  responsibility for the codes actions (use at your own risk).  I guess
 *  I am saying "Happy gaming", and am trying not to get sued in the process.
 *							Ed
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <ctype.h>
#ifndef	XENIX
#include <sys/types.h>
#include <sys/file.h>
#else
#include <unistd.h>
#endif
#include "header.h"
#include "data.h"

extern FILE *fnews;

extern short country;
int	disarray;		/* TRUE if nation in disarray */
int	**attr;			/* sector attractiveness */
long	**newpop;		/* storage for old population */

long
dtol(d) double d;
{
	char tempstr[BIGLTH];
	long l;
	sprintf(tempstr,"%-60.0lf",d);
	sscanf(tempstr,"%ld",&l);
	return(l);
}

/****************************************************************/
/*	UPDATE() - updates the whole world			*/
/****************************************************************/
void
update()
{
	char command[BIGLTH],filename[FILELTH];

	sprintf(filename,"%s%d",newsfile,TURN);
	if ((fnews=fopen(filename,"w"))==NULL) {
		printf("error opening news file\n");
		exit(FAIL);
	}
	check();

	updexecs();	/*run each nation in a random order*/
	check();

#ifdef MONSTER
	check();
	monster();	/* update monster nations */
	check();
#endif

	check();
	combat();	/* run combat */
	check();
	updcapture();	/* capture unoccupied sectors */

#ifdef TRADE
	uptrade();	/* update trade */
#endif

	updmil();	/* reset military stuff for whole world */

#ifdef RANEVENT
	randomevent();	/*run random events after setting movements */
#endif /* RANEVENT */

	updsectors();	/* for whole map, update one sector at a time*/
	updcomodities();/* commodities & food, metal, jewels */
	updleader();	/* new leaders are born, old leaders become wiser */

	/* check for destroyed nations */
	for(country=1;country<NTOTAL;country++)
	if (isntn(ntn[country].active)) {
		if (ntn[country].tciv < 100 &&
		    ntn[country].tmil < TAKESECTOR)
			destroy(country);
	}

	fprintf(fnews,"1\tIMPORTANT WORLD NEWS\n");
	fprintf(fnews,"5\tGLOBAL ANNOUNCEMENTS (see mail)\n");
	fclose(fnews);

#ifdef CHEAT
	cheat();
#endif /* CHEAT */

	score();	/* score all nations */

	/* check for mercenary increase 5% chance*/
	if (rand()%20==0) {
		printf("increasing mercenary bonuses\n");
		MERCATT++;
		MERCDEF++;
	}
	sprintf(command,"/bin/rm -f %s*",exefile);
	printf("%s\n",command);
	system(command);

	sprintf( command,"%s/%s %s %s", EXEDIR, sortname, filename, filename );
	printf("%s\n",command);
	system(command);

	/* remove old news files */
	if (TURN>MAXNEWS) {
		sprintf(filename,"%s%d",newsfile,TURN-MAXNEWS);
		unlink(filename);
	}

	/* increase turn number by one */
	TURN++;
	att_base();	/* calculate base for nation attributes */
	att_bonus();	/* calculate tradegood bonus for nation attributes */
}

/****************************************************************/
/*	ATTRACT() - how attractive is sector to civilians	*/
/* returns attractiveness 					*/
/****************************************************************/
int
attract(x,y,race)
{
	register struct s_sector	*sptr = &sct[x][y];
	int	designation;
	int	Attr = 0;

	designation=sptr->designation;
	if(sptr->tradegood != TG_none
	&& *(tg_stype+sptr->tradegood)==designation ) {
		if((designation!=DMINE)
		&& (designation!=DGOLDMINE))
		Attr += ( tg_value[sptr->tradegood] - '0' )*TGATTR;
	}

	if(designation==DGOLDMINE){
		if(sptr->jewels>=6) Attr+=GOLDATTR*sptr->jewels*2;
		else	Attr+=GOLDATTR*sptr->jewels;
	} else if(designation==DFARM){
		if(ntn[sptr->owner].tfood*250 <= ntn[sptr->owner].eatrate*(ntn[sptr->owner].tciv*11))
			Attr+=50*FARMATTR;
		else Attr+=tofood(sptr,sptr->owner)*FARMATTR;
	}
	else if(designation==DCITY) Attr+=CITYATTR;
	else if(designation==DCAPITOL) Attr+=CITYATTR;
	else if(designation==DTOWN) Attr+=TOWNATTR;
	else if(designation==DMINE) {
		if(sptr->metal>6) Attr+=MINEATTR*sptr->metal*2;
		else Attr+=MINEATTR*sptr->metal;
	} else if((designation!=DROAD)&&(designation!=DNODESIG)
	&&(designation!=DDEVASTATED)&& is_habitable(x,y) ) Attr+= OTHRATTR;

	switch(race){
	case DWARF:
		if((designation==DGOLDMINE)&&(sptr->jewels>3))
			Attr += DGOLDATTR;
		else if((designation==DMINE)&&(sptr->metal>3))
			Attr += DMINEATTR;
		else if(designation==DTOWN) Attr += DTOWNATTR;
		else if(designation==DCITY) Attr += DCITYATTR;
		else if(designation==DCAPITOL) Attr += DCITYATTR;

		if(sptr->vegetation==WOOD) Attr += DWOODATTR;
		else if(sptr->vegetation==FOREST) Attr += DFOREATTR;

		if(sptr->altitude==MOUNTAIN) Attr += DMNTNATTR;
		else if(sptr->altitude==HILL) Attr += DHILLATTR;
		else if(sptr->altitude==CLEAR) Attr += DCLERATTR;
		else Attr=0;
		break;
	case ELF:
		if((designation==DGOLDMINE)&&(sptr->jewels>3))
			Attr += EGOLDATTR;
		else if((designation==DMINE)&&(sptr->metal>3))
			Attr += EMINEATTR;
		else if(designation==DTOWN) Attr += ECITYATTR;
		else if(designation==DCITY) Attr += ECITYATTR;
		else if(designation==DCAPITOL) Attr += ECITYATTR;

		if(sptr->vegetation==WOOD) Attr += EWOODATTR;
		else if(sptr->vegetation==FOREST) Attr += EFOREATTR;

		if(sptr->altitude==MOUNTAIN) Attr += EMNTNATTR;
		else if(sptr->altitude==HILL) Attr += EHILLATTR;
		else if(sptr->altitude==CLEAR) Attr += ECLERATTR;
		else Attr=0;
		break;
	case HUMAN:
		if((designation==DGOLDMINE)&&(sptr->jewels>3))
			Attr += HGOLDATTR;
		else if((designation==DMINE)&&(sptr->metal>3))
			Attr += HMINEATTR;
		else if(designation==DTOWN) Attr += HCITYATTR;
		else if(designation==DCITY) Attr += HCITYATTR;
		else if(designation==DCAPITOL) Attr += HCITYATTR;

		if(sptr->vegetation==WOOD) Attr += HWOODATTR;
		else if(sptr->vegetation==FOREST) Attr += HFOREATTR;

		if(sptr->altitude==MOUNTAIN) Attr += HMNTNATTR;
		else if(sptr->altitude==HILL) Attr += HHILLATTR;
		else if(sptr->altitude==CLEAR) Attr += HCLERATTR;
		else Attr=0;
		break;
	case ORC:
		if((designation==DGOLDMINE)&&(sptr->jewels>3))
			Attr += OGOLDATTR;
		else if((designation==DMINE)&&(sptr->metal>3))
			Attr += OMINEATTR;
		else if(designation==DTOWN) Attr += OCITYATTR;
		else if(designation==DCITY) Attr += OCITYATTR;
		else if(designation==DCAPITOL) Attr += OCITYATTR;

		if(sptr->vegetation==WOOD) Attr += OWOODATTR;
		else if(sptr->vegetation==FOREST) Attr += OFOREATTR;

		if(sptr->altitude==MOUNTAIN) Attr += OMNTNATTR;
		else if(sptr->altitude==HILL) Attr += OHILLATTR;
		else if(sptr->altitude==CLEAR) Attr += OCLERATTR;
		else Attr=0;
		break;
	default:
		break;
	}
	if((designation==DDEVASTATED)||(Attr<0)||(movecost[x][y]<0)) Attr=0;
	return(Attr);
}
/****************************************************************/
/*	ARMYMOVE() 						*/
/* armymove moves an army... and returns the # of sectors taken	*/
/****************************************************************/
int
armymove(armynum)
int armynum;
{
	long		sum, where;
#ifdef XENIX
	register int z;
#endif /*XENIX*/
	register int	x, y;
	int	i;
	long	menok;			/* enough men in the army? */
	int	leadflag=FALSE;		/* leader w/o group */
	int	takesctr=FALSE; 	/* takesctr is # unowned sctrs*/

	if(P_ASTAT>=NUMSTATUS || P_AMOVE==0) return(takesctr);

	/* if leader w/o a group, set leadflag */
	if((P_ATYPE>=MINLEADER)&&(P_ATYPE<MINMONSTER)&&(P_ASTAT!=GENERAL)) {
		leadflag=TRUE;
		/* the king stays in capitol on RULE */
		if(P_ATYPE == getleader(curntn->class)-1 ){
			P_AXLOC=curntn->capx;
			P_AYLOC=curntn->capy;
			P_ASTAT=RULE;
			return(takesctr);
		}
	}

	sum=0;
	if(leadflag) {		/* Move based on unattached soldiers */
		for(i=0;i<MAXARM;i++)
		if(( curntn->arm[i].unittyp<MINLEADER )
		&&( curntn->arm[i].stat!=MILITIA )
		&&( curntn->arm[i].stat!=ONBOARD )
		&&( curntn->arm[i].stat!=GARRISON )
		&&( curntn->arm[i].stat!=TRADED )
		&&( curntn->arm[i].stat<NUMSTATUS ))
			sum += curntn->arm[i].sold;
	} else	{		/* not leader w/o group */
		/* use menok as a temp vbl now == men in army */
		menok=0;
		if((P_ATYPE>=MINLEADER)
		&&(P_ATYPE<MINMONSTER)
		&&(P_ASTAT==GENERAL)) {
			for(x=0;x<MAXARM;x++)
				if((curntn->arm[x].stat==(NUMSTATUS+armynum))
				&& (curntn->arm[x].unittyp<MINLEADER))
					menok+=P_ASOLD;
		} else	menok=P_ASOLD;
		if((menok > TAKESECTOR ) 
		||( P_ATYPE>=MINLEADER)) menok=TRUE;
		else menok=FALSE;
		/* range of 4 if menok is FALSE else 2 */
		for(x=(int)P_AXLOC-4+menok*2;x<=(int)P_AXLOC+4-menok*2;x++)
		for(y=(int)P_AYLOC-4+menok*2;y<=(int)P_AYLOC+4-menok*2;y++) {
			if(ONMAP(x,y)) {
				if( menok==TRUE || ISCITY(sct[x][y].designation) )
					sum+=attr[x][y];
			}
		}
	}

	if(sum==0) {		/* nowhere to go */
		P_AXLOC=curntn->capx;
		P_AYLOC=curntn->capy;
		P_ASTAT=DEFEND;
	} else if(leadflag) {	/* find leader a group! */
		where=rand()%sum;
		for(x=0;x<MAXARM;x++)
		if((curntn->arm[x].unittyp<MINLEADER )
		&&( curntn->arm[x].stat!=MILITIA )
		&&( curntn->arm[x].stat!=GARRISON )
		&&( curntn->arm[x].stat!=ONBOARD )
		&&( curntn->arm[x].stat!=TRADED )
		&&( curntn->arm[x].stat<NUMSTATUS )){
			if ((where-=curntn->arm[x].sold) <= 0) {
				P_AXLOC=curntn->arm[x].xloc;
				P_AYLOC=curntn->arm[x].yloc;
				break;
			}
		}
		if(x!=MAXARM) for(x=0;x<MAXARM;x++) {
			if((curntn->arm[x].unittyp<MINLEADER )
			&&( curntn->arm[x].stat<NUMSTATUS )
			&&( curntn->arm[x].sold>=0 )
			&&( curntn->arm[x].stat!=MILITIA )
			&&( curntn->arm[x].stat!=GARRISON )
			&&( curntn->arm[x].stat!=SIEGED )
			&&( curntn->arm[x].stat!=SCOUT )
			&&( curntn->arm[x].stat!=ONBOARD )
			&&( curntn->arm[x].stat!=TRADED )
			&&( curntn->arm[x].unittyp!=A_ZOMBIE )
			&&( P_AXLOC==curntn->arm[x].xloc )
			&&( P_AYLOC==curntn->arm[x].yloc )){
				curntn->arm[x].stat=NUMSTATUS+armynum;
				P_ASTAT=GENERAL;
				break;
			}
		}
	} else {	/* move a normal unit */
		where=rand()%sum;
		/* range of 4 if menok is FALSE else 2 */
		for(x=(int)P_AXLOC-4+menok*2;x<=(int)P_AXLOC+4-menok*2;x++)
		for(y=(int)P_AYLOC-4+menok*2;y<=(int)P_AYLOC+4-menok*2;y++)
		if(ONMAP(x,y)){
			if( menok==TRUE || ISCITY(sct[x][y].designation) )
				where -= attr[x][y];
			if( (where < 0 )
			&& movecost[x][y]>=1
			&& movecost[x][y]<=P_AMOVE
			&&(land_reachp((int)P_AXLOC,(int)P_AYLOC,x,y,P_AMOVE,country))){
				P_AXLOC=x;
				P_AYLOC=y;
				if(P_ATYPE == getleader(curntn->class)-1 ){
					P_AXLOC=curntn->capx;
					P_AYLOC=curntn->capy;
				}

				/* ARMIES MOVE PSEUDO INDEPENDANTLY */
				if((sct[x][y].designation != DCITY)
				&&(sct[x][y].designation != DCAPITOL)
				&&(sct[x][y].designation != DTOWN)
				&&(sct[x][y].owner==country)) {
#ifdef XENIX
					z = attr[x][y];
					z /= 8;
					attr[x][y] = z;
#else
					attr[x][y] /= 8;
#endif /*XENIX*/
				}
				if(sct[x][y].owner==0){
					sct[x][y].owner=country;
					if (curntn->popularity<MAXTGVAL) curntn->popularity++;
#ifdef XENIX
					z = attr[x][y];
					z /= 8;
					attr[x][y] = z;
#else
					attr[x][y]/=8;
#endif /*XENIX*/
					takesctr++;
				}
		
				if((P_ATYPE>=MINLEADER)&&(P_ASTAT==GENERAL))
				for(x=0;x<MAXARM;x++) 
				if((curntn->arm[x].sold>0 )
				&&( curntn->arm[x].stat==armynum+NUMSTATUS)){
					curntn->arm[x].xloc=P_AXLOC;
					curntn->arm[x].yloc=P_AYLOC;
				}
				return(takesctr);
			} /* if */
		} /* for for */

		/*do again - have this block if lots of bad terrain*/
		/*what could happen is that it won't find a move first time*/
		for(x=(int)P_AXLOC-2;x<=(int)P_AXLOC+2;x++)
		for(y=(int)P_AYLOC-2;y<=(int)P_AYLOC+2;y++) {
			if(!ONMAP(x,y))
				continue;

			if(leadflag) where -= solds_in_sector(x,y,country);
			else where -= attr[x][y];
			if( (where < 0 )
			&& movecost[x][y]>=1
			&& movecost[x][y]<=P_AMOVE
			&&(land_reachp(P_AXLOC,P_AYLOC,x,y,P_AMOVE,country))){
				P_AXLOC=x;
				P_AYLOC=y;
				if(sct[x][y].owner==0){
					if (curntn->popularity<MAXTGVAL) curntn->popularity++;
					sct[x][y].owner=country;
					attr[x][y] = 1;
					takesctr++;
				}
				if((P_ATYPE>=MINLEADER)&&(P_ASTAT==GENERAL))
				for(i=0;i<MAXARM;i++) 
				if((curntn->arm[i].sold>0 )
				&&( curntn->arm[i].stat==armynum+NUMSTATUS)){
					curntn->arm[i].xloc=P_AXLOC;
					curntn->arm[i].yloc=P_AYLOC;
				}
				return(takesctr);
			} /* if */
		} /* for for */
	} /* if */
	return(takesctr);
}

/****************************************************************/
/*	SCORE() 						*/
/* score updates the scores of all nations			*/
/****************************************************************/
void
score()
{
	int x;
	printf("\nUpdating nation's scores\n");
	for(x=1;x<NTOTAL;x++) if(isntn(ntn[x].active))
		ntn[x].score += score_one(x);
}

#ifdef CHEAT
/****************************************************************/
/*	CHEAT() 						*/
/* this routine cheats in favor of npc nations 			*/
/*								*/
/* I take pride in this code... it needs not to cheat to play a */
/* good	game.  This routine is the only cheating that it will	*/
/* do, and it is fairly minor.					*/
/****************************************************************/
void
cheat()
{
	int x,y;
	int bonus=0, count=0, npcavg, pcavg, avgscore=0;
	char realnpc[NTOTAL],tempc[LINELTH];

	/* take inventory of countries */
	for(x=1;x<NTOTAL;x++) {
		sprintf(tempc,"%s%d", exefile, x);
		if (isnpc(ntn[x].active) && access(tempc,00)==0) {
			realnpc[x]=TRUE;
		} else {
			realnpc[x]=FALSE;
		}
	}

	/* add gold */
	for(x=1;x<NTOTAL;x++) if(realnpc[x]==TRUE) {
		if((ntn[x].tgold<ntn[x].tciv)
		&&( rand()%5==0)){
			ntn[x].tgold+=10000;
			printf("npc cheat routine - add $10000 to nation %s\n",ntn[x].name);
		}
	}

	for(x=1;x<NTOTAL;x++)
		if(realnpc[x]==FALSE)  {
			bonus+=ntn[x].aplus+ntn[x].dplus;
			avgscore+=ntn[x].score;
			count++;
		}

	if(count==0) return;
	pcavg = bonus / count;
	avgscore /= count;
	printf("pc average score is %d count is %d\n",avgscore,count);

	bonus=0;
	count=0;
	for(x=1;x<NTOTAL;x++) 
		if(realnpc[x]==TRUE)  {
			bonus+=ntn[x].aplus+ntn[x].dplus;
			count++;
		}
	if(count==0) return;
	npcavg = bonus / count;
	for(x=1;x<NTOTAL;x++) 
	if((realnpc[x]==TRUE)
	&&(ntn[x].score < avgscore)
	&&(ntn[x].race != ORC )
	&&(rand()%100 < (pcavg-npcavg))) {
		if(ntn[x].aplus>ntn[x].dplus) ntn[x].dplus+=1;
		else ntn[x].aplus+=1;
		printf("npc cheat routine - add 1%% to nation %s combat skill\n",ntn[x].name);
	}

	/* cheat by making npc's frendlier to each other if they are */
	/* of the same race */
	for(x=1;x<NTOTAL;x++) if(realnpc[x]==TRUE)
		for(y=1;y<NTOTAL;y++) if(realnpc[x]==TRUE)
			if((ntn[x].dstatus[y]!=TREATY)
			&&(ntn[x].dstatus[y]!=UNMET)){
				if(ntn[x].race == ntn[y].race){
					ntn[x].dstatus[y]--;
				} else {
					if(ntn[x].dstatus[y]!=JIHAD)
						if(rand()%4==0)
							ntn[x].dstatus[y]--;
				}
			}
}
#endif /* CHEAT */


/****************************************************************/
/*	UPDEXECS() 						*/
/* update all nations in a random order				*/
/* move civilians of that nation 				*/
/****************************************************************/
void
updexecs()
{
	register struct s_sector	*sptr;
	register int x, y;
#ifdef XENIX
	register int z;
#endif /*XENIX*/
	int	armynum;
	int done, loop=0, number=0;
	void move_people();
	int execed[NTOTAL];

	check();
	attr = (int **) m2alloc(MAPX,MAPY,sizeof(int));
	check();
	newpop = (long **) m2alloc(5,MAPY,sizeof(long));
	check();

	for(country=0;country<NTOTAL;country++) 
		if( isntn(ntn[country].active) ) execed[country]=FALSE;
		else {
			execed[country]=TRUE;
			loop++;
		}

	for(;loop<NTOTAL;loop++){
		number=(rand()%(NTOTAL-loop))+1; /*get random active nation*/

		done=FALSE;
		/*Find the appropiate nation*/
		for(country=0;done==FALSE && country<NTOTAL;country++) {
			if (execed[country]==FALSE) number--;
			if (number==0) {
				execed[country]=TRUE;
				done=TRUE;
				country--;	/* compensate for inc */
			}
		}

		curntn = &ntn[country];
		if(curntn->active == INACTIVE) continue;

		printf("updating nation number %d -> %s\n",country,curntn->name);
	check();

		disarray=FALSE;
#ifdef TRADE
		if(isntn(curntn->active)) checktrade();
#endif /* TRADE */

		/*if execute is 0 and PC nation then they did not move*/
		if((execute(TRUE)==0)&&(ispc(curntn->active))){
			printf("\tnation %s did not move\n",curntn->name);
#ifdef NPC
#ifdef CMOVE
			printf("\tthe computer will move for %s\n",curntn->name);
			fprintf(fnews,"1.\tthe computer will move for %s\n",curntn->name);
			if (mailopen( country )!=(-1)) {
				fprintf(fm,"Message to %s from CONQUER\n\n",curntn->name);
				fprintf(fm,"The computer moved for you in the %s of Year %d\n",PSEASON(TURN),YEAR(TURN));
				mailclose(country);
			}
			check();
			nationrun();
			check();
#endif /*CMOVE*/
		}
		/* run npc nations */
		if(isnpc(curntn->active)) {
			check();
			nationrun();
			check();
#ifdef ORCTAKE
			/*do npc nation magic*/
			if(magic(country,MA_MONST)==TRUE) {
				if((x=takeover(5,0))==1)
				printf("SUCCESSFUL TAKEOVER OF %d by %s",x,curntn->name);
			} else if(magic(country,AV_MONST)==TRUE) {
				if((x=takeover(3,0))==1)
				printf("SUCCESSFUL TAKEOVER OF %d by %s",x,curntn->name);
			} else if(magic(country,MI_MONST)==TRUE){
				if((x=takeover(1,0))==1)
				printf("SUCCESSFUL TAKEOVER OF %d by %s",x,curntn->name);
			}
#endif /* ORCTAKE */
#endif /*NPC*/
		}

		/* is leader killed - put nation into disarray */
		disarray=TRUE;
		x = getleader((int)curntn->class) - 1;
		for(armynum=0;armynum<MAXARM;armynum++)
		if(P_ATYPE == x && P_ASOLD>0) {
			disarray=FALSE;
			break;
		}
#ifdef DEBUG
printf("checking for leader in nation %s: armynum=%d\n",curntn->name,armynum);
#endif /* DEBUG */

		if(disarray == TRUE) {
			if(rand()%100 < 30) {	/* new leader takes over */
				x++;
				for(armynum=0;armynum<MAXARM;armynum++)
					if(P_ATYPE == x && P_ASOLD > 0) break;
				if( armynum<MAXARM) {
					P_ATYPE=x-1;
					P_ASOLD= *(unitminsth+(x-1)%UTYPE);
					disarray=FALSE;
					fprintf(stderr,"new leader in nation %s\n",curntn->name);
					fprintf(fnews,"1.\tnation %s has a new leader\n",curntn->name);
					if(ispc(curntn->active)){
						if (mailopen(country)!=(-1)) {
							fprintf(fm,"MESSAGE FROM CONQUER:\n\n");
							fprintf(fm,"YOU HAVE A NEW NATIONAL LEADER.\n");
							fprintf(fm,"YOUR TROOPS MAY NOW MOVE NORMALLY.\n");
							mailclose(country);
						}
					}
				}
			}
		}

		if( disarray ==  TRUE) {
			fprintf(stderr,"no leader in nation %s\n",curntn->name);
			fprintf(fnews,"1.\tnation %s still has no national leader\n",curntn->name);
			if(ispc(curntn->active)){
				if (mailopen(country)!=(-1)) {
					fprintf(fm,"MESSAGE FROM CONQUER\n\n");
					fprintf(fm,"YOU DON'T HAVE A COUNTRY LEADER;\n");
					fprintf(fm,"YOUR TROOPS MAY NOT MOVE\n");
					fprintf(fm,"THERE IS A 30%% CHANCE/TURN OF GETTING A NEW ONE\n");
					mailclose(country);
				}
			}
		}

		updmove(curntn->race,country);	/*update movement array*/

		/* Recalculate ATTR MATRIX for civilians */
		/*calculate sector attractiveness*/
		for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) {
			sptr = &sct[x][y];
			if((sptr->owner==country)
			&&(tofood(sptr,sptr->owner)!=0)){
				attr[x][y]=attract(x,y,curntn->race);
			} else attr[x][y]=0;
		}

		/*if near capitol add to attr*/
		for(x=(int)curntn->capx-2;x<=(int)curntn->capx+2;x++)
			for(y=(int)curntn->capy-2;y<=(int)curntn->capy+2;y++)
				if((ONMAP(x,y))&&(attr[x][y]>0)) attr[x][y]+=20;
		move_people();

	} /* for */

	/*zero out all recalculated values; do not clear god */
	for(country=1;country<NTOTAL;country++) if(isntn(ntn[country].active)){
		ntn[country].tships=0;
		ntn[country].tmil=0;
#ifdef XENIX
		if (rand()%4 == 0) {
			z = ntn[country].spellpts;
			z /= 2;
			ntn[country].spellpts = z;
		}
#else
		if(rand()%4==0) ntn[country].spellpts/=2;
#endif /*XENIX*/
		if(magic(country,SUMMON)==TRUE) {
			ntn[country].spellpts+=4;
			if(magic(country,WYZARD)==TRUE)
				ntn[country].spellpts+=3;
			if(magic(country,SORCERER)==TRUE)
				ntn[country].spellpts+=3;
		}
		if(magic(country,MA_MONST)==TRUE) ntn[country].spellpts+=2;
		if(magic(country,AV_MONST)==TRUE) ntn[country].spellpts+=1;
		if((magic(country,MI_MONST)==TRUE)
			&&( rand()%2==0)) ntn[country].spellpts+=1;
	}
	free(attr);
	free(newpop);
}

/****************************************************************/
/*	DO_LIZARD() 						*/
/* update lizards	 					*/
/****************************************************************/
void
do_lizard()
{
#ifdef XENIX
	register int x;
#endif /*XENIX*/
	register int i, j;
	int armynum;

	printf("updating lizard (nation %d)\n",country);
	curntn = &ntn[country];
	for(armynum=0;armynum<MAXARM;armynum++)
	if((P_ASOLD>0)) {
		P_AMOVE =20;	/* just in case god wants to move them */
		/* increase population */
#ifdef XENIX
		x = P_ASOLD * 102;
		x /= 100;
		P_ASOLD = x;
#else
		P_ASOLD*=102;
		P_ASOLD/=100;
#endif /*XENIX*/
		if(armynum%2==0) {
			if(P_ASTAT!=SIEGED) P_ASTAT=GARRISON;
		} else {
			if(ntn[country].arm[armynum-1].sold<=0) {
				P_ASOLD=0;
				continue;
			}
			P_AXLOC = ntn[country].arm[armynum-1].xloc;
			P_AYLOC = ntn[country].arm[armynum-1].yloc;
			/* try to relieve sieges */
			if(P_ASTAT!=SIEGED
			&& ntn[country].arm[armynum-1].stat!=SIEGED) {
			for(i=(int)ntn[country].arm[armynum-1].xloc-1;i<=ntn[country].arm[armynum-1].xloc+1;i++) {
				for(j=(int)ntn[country].arm[armynum-1].yloc-1;j<=ntn[country].arm[armynum-1].yloc+1;j++) {
					if(ONMAP(i,j)
					&&(sct[i][j].altitude!=WATER) 
					&&(sct[i][j].altitude!=PEAK) 
					&&(sct[i][j].owner != country) 
					&&(rand()%3==0)){
						P_AXLOC = i;
						P_AYLOC = j;
					}
				}
			}
			}
			/* this cheats by giving garrison bonus with movement */
			if((sct[P_AXLOC][P_AYLOC].designation==DFORT)
			&&(sct[P_AXLOC][P_AYLOC].owner==country)) {
				if(P_ASTAT!=SIEGED) P_ASTAT=GARRISON;
			} else P_ASTAT=ATTACK;
		}
	}
#ifdef DEBUG
	for(armynum=0;armynum<MAXARM;armynum++) {
		if((P_ASOLD>0)&&(sct[P_AXLOC][P_AYLOC].altitude==WATER))
			printf("ERROR line %d... %s army %d in water (army %d: x: %d y: %d)\n",__LINE__,ntn[country].name,armynum,armynum-1, ntn[country].arm[armynum-1].xloc, ntn[country].arm[armynum-1].yloc);
	}
#endif /* DEBUG */
}

/****************************************************************/
/*	UPDCAPTURE() 						*/
/* capture unoccupied sectors					*/
/****************************************************************/
void
updcapture()
{
	register struct s_sector	*sptr;
	int armynum, occval;

	fprintf(fnews,"3\tNEWS ON WHAT SECTORS HAVE BEEN CAPTURED\n");
	printf("distributing captured sectors\n");

	/*look for any areas where armies alone in sector*/
	prep(0,-1);

	for(country=1;country<NTOTAL;country++) 
	if(ntn[country].active!=INACTIVE){
		curntn = &ntn[country];
		for(armynum=0;armynum<MAXARM;armynum++)
/* cheat in favor of npcs as the create army routines assume 75 man armies */
		if(P_ATYPE<MINLEADER) {
			if((ispc(curntn->active)&&(P_ASOLD>=TAKESECTOR))
			||((isnotpc(curntn->active))&&(P_ASOLD>75))){
				/* may not capture land while on a fleet */
				if(P_ASTAT==ONBOARD) continue;
				/* may not capture water */
				if(sct[P_AXLOC][P_AYLOC].altitude==WATER) {
					fprintf(stderr,"Nation %s Army %d in Water\n",curntn->name,armynum);
					continue;
				}
				if(occ[P_AXLOC][P_AYLOC] != country) continue;
				sptr = &sct[P_AXLOC][P_AYLOC];
				if(sptr->owner==0){
					sptr->owner=country;
					if (curntn->popularity<MAXTGVAL) curntn->popularity++;
				} else if((sptr->owner!=country)
				&&(curntn->dstatus[sptr->owner]>=WAR)) {
					if(ntn[sptr->owner].race!=curntn->race)
						if(magic(country,SLAVER)==TRUE){
							flee(P_AXLOC,P_AYLOC,1,TRUE);
						} else {
							flee(P_AXLOC,P_AYLOC,1,FALSE);
						}

					if((isntn( curntn->active ))
					   &&(isntn( ntn[sptr->owner].active))) {
#ifdef HIDELOC
						fprintf(fnews,"3.\tarea captured by %s from %s\n",curntn->name,ntn[sptr->owner].name);
#else
						fprintf(fnews,"3.\tarea %d,%d captured by %s from %s\n",P_AXLOC,P_AYLOC,curntn->name,ntn[sptr->owner].name);
#endif /* HIDELOC */
					}
					sptr->owner=country;
					curntn->popularity++;
				}
			}
		} else if(P_ASTAT==A_SCOUT && P_ATYPE!=A_SPY && P_ASOLD>0) {
			occval=occ[P_AXLOC][P_AYLOC];
			/* capture situations:
			 *   - alone with a hostile army     [PFINDSCOUT% chance]
			 *   - alone in someone else's territory with one of
			 *      their armies (non-allied)    [(PFINDSCOUT/5)% chance]
			 * NOTE: do not remove chance to capture in unmet territory.
			 */
			if (occval!=0 && occval!=country && occval<NTOTAL) {
				if(((ntn[occval].dstatus[country]>=HOSTILE)
				  &&(rand()%100<PFINDSCOUT))
				||((sct[P_AXLOC][P_AYLOC].owner==occval)
				  &&(ntn[occval].dstatus[country]!=TREATY)
				  &&(ntn[occval].dstatus[country]!=ALLIED)
				  &&(rand()%100<PFINDSCOUT/5))) {
					/* capture the scout */
					P_ASOLD=0;
					if (ispc(curntn->active)) {
						if(mailopen(country)!=(-1)) {
							fprintf(fm,"Message from Conquer\n\n");
							fprintf(fm,"\tYour Scouting Unit %d was captured\n",armynum);
							fprintf(fm,"\t  by %s military in sector %d,%d\n",
								   ntn[occval].name,(int)P_AXLOC,(int)P_AYLOC);
							mailclose(country);
						}
					}
					if (ispc(ntn[occval].active)) {
						if(mailopen(occval)!=(-1)) {
							fprintf(fm,"Message from Conquer\n\n");
							fprintf(fm,"\tA Scout from nation %s was captured\n",curntn->name);
							fprintf(fm,"\t  in sector %d,%d.\n",(int)P_AXLOC,(int)P_AYLOC);
							mailclose(occval);
						}
					}
				}
			}
		}
	}

	/* capture countries */
	for(country=1;country<NTOTAL;country++)
	if (isntn(ntn[country].active)) {

		/* check for capitols being sacked */
		if(sct[ntn[country].capx][ntn[country].capy].owner != country)
			sackem(country);
	}
}

/**************************************************************/
/*	UPDSECTORS() 						*/
/* update sectors one at a time				*/
/**************************************************************/
void
updsectors()
{
	register struct s_sector	*sptr;
	register struct s_nation		*nptr;
	long	charity;	/* talons to the poor */
	register int i, j;
	register int x,y;

	printf("\nupdating all sectors\n");
	for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) {
		int rephold;

		sptr = &sct[x][y];
		if(sptr->owner == 0) continue;
		nptr = &ntn[sptr->owner];

		/* add to contents of sector */
		if(rand()%100<FINDPERCENT) {
			if( sct[x][y].tradegood == TG_none )
			if(rand()%2==0) getmetal( &sct[x][y] );
			else getjewel( &sct[x][y] );
		}

		/* calculate reproduction per season */ 
		rephold = nptr->repro/4;
		if ((SEASON(TURN)!=WINTER) &&
		(SEASON(TURN)<=(nptr->repro%4)))
			rephold++;

		/* only one capitol per nation */
		if (sptr->designation==DCAPITOL) {
			if (nptr->capx!=x || nptr->capy!=y) {
				sptr->designation=DCITY;
			}
		}

		/* if huge number dont reproduce */
		if(sptr->people >= ABSMAXPEOPLE) {
			sptr->people = ABSMAXPEOPLE;
			if(sptr->people * sptr->metal > 2*(rand()%100)*TOMUCHMINED)
				if(sptr->designation==DMINE) sptr->metal--;
			if(sptr->people * sptr->jewels > 2*(rand()%100)*TOMUCHMINED)
				if(sptr->designation==DGOLDMINE) sptr->jewels--;
		} else if((sptr->people > TOMANYPEOPLE)
		&&(sptr->designation!=DTOWN)
		&&(sptr->designation!=DCAPITOL)
		&&(sptr->designation!=DCITY)){
			sptr->people += (rephold * sptr->people)/200;
			if(sptr->people > ABSMAXPEOPLE)
				sptr->people = ABSMAXPEOPLE;
			if(sptr->people * sptr->metal > 2*(rand()%100)*TOMUCHMINED)
				if(sptr->designation==DMINE) sptr->metal--;
			if(sptr->people * sptr->jewels > 2*(rand()%100)*TOMUCHMINED)
				if(sptr->designation==DGOLDMINE) sptr->jewels--;
		} else if(sptr->people<100) {
			sptr->people+=sptr->people/10;
		} else {
			sptr->people += (rephold * sptr->people)/100; 
			if(sptr->people * sptr->metal > (rand()%100)*TOMUCHMINED)
				if(sptr->designation==DMINE) sptr->metal--;
			if(sptr->people * sptr->jewels > (rand()%100)*TOMUCHMINED)
				if(sptr->designation==DGOLDMINE) sptr->jewels--;
		}
		/* if no metal/gold left, remove tradegood */
		if(((sptr->designation==DGOLDMINE)&&(sptr->jewels==0))
		||((sptr->designation==DMINE)&&(sptr->metal==0))){
			sptr->tradegood = TG_none;
			sptr->designation = DDEVASTATED;
		}

		/*check all adjacent sectors and decide if met */
		for(i=x-MEETNTN;i<=x+MEETNTN;i++)
		for(j=y-MEETNTN;j<=y+MEETNTN;j++)
		if(ONMAP(i,j)&&(sct[i][j].owner!=0)) {
			if(sptr->owner!=sct[i][j].owner) {
				if(nptr->dstatus[sct[i][j].owner]==UNMET)
					newdip(sptr->owner,sct[i][j].owner);
				if(ntn[sct[i][j].owner].dstatus[sptr->owner]==UNMET)
					newdip(sct[i][j].owner,sptr->owner);
			}
		}

		/* if desert sector... reverts to desert */
		if(tofood(sptr,sptr->owner)<DESFOOD){
			if((sptr->designation != DSTOCKADE)
			&&(sptr->designation != DFORT)
			&&(sptr->designation != DROAD))
			sptr->designation=DNODESIG;
		}
	}

	for(country=1;country<NTOTAL;country++) {
		curntn = &ntn[country];
		if(isntn(curntn->active)){

			/* check for depletion of country through */
			/* lack of a capitol                      */
			if((sct[curntn->capx][curntn->capy].designation!=DCAPITOL)
			  ||(sct[curntn->capx][curntn->capy].owner!=country)) {

				printf("depleting nation %s\n",curntn->name);
				deplete(country);
			}

			spreadsheet(country);
			if ((int)curntn->popularity-2*curntn->inflation < (int)MAXTGVAL) {
				curntn->popularity = max(0,(int)(curntn->popularity-2*curntn->inflation));
			} else curntn->popularity = (char) MAXTGVAL;
			curntn->tsctrs = spread.sectors;
			curntn->tciv=spread.civilians;
			curntn->tfood=spread.food;

			/* take out for charity */
			charity=((spread.gold-curntn->tgold)*curntn->charity)/100;

			if(charity < 0) charity = 0;
			curntn->tgold = spread.gold - charity;

			if(curntn->tciv > 0) charity /= curntn->tciv;
			else charity = 0;

			/* calculate poverty base */
			if (curntn->tgold < 0L) {
				curntn->poverty = 95;
			} else if (curntn->tciv < 100L) {
				/* give some check on civilians */
				curntn->poverty = (unsigned char)20;
			} else if (curntn->tgold/curntn->tciv < 30L) {
				curntn->poverty = (unsigned char)(95L - curntn->tgold/curntn->tciv);
			} else if (curntn->tgold/curntn->tciv < 80L) {
				curntn->poverty = (unsigned char)(65L - (curntn->tgold/curntn->tciv-30L)/2L);
			} else if (curntn->tgold/curntn->tciv < 120L) {
				curntn->poverty = (unsigned char)(40L - (curntn->tgold/curntn->tciv-80L)/4L);
			} else if (curntn->tgold/curntn->tciv < 200L) {
				curntn->poverty = (unsigned char)(30L - (curntn->tgold/curntn->tciv-120L)/8L);
			} else {
				curntn->poverty = (unsigned char)20;
			}

			/* charity increase to popularity */
			curntn->popularity = min(curntn->popularity+5*charity,MAXTGVAL);

			/* charity adjustment to poverty; rounding upward */
			if(curntn->poverty < (charity+1)/2 )
				curntn->poverty = 0;
			else	curntn->poverty -= (charity+1)/2;

			/* Calculate inflation base */
			if(curntn->inflation > 0) 
				curntn->inflation = rand()%(curntn->inflation/2+1);
			else curntn->inflation = 0;
			curntn->inflation += (curntn->tax_rate/4 + (rand()%(curntn->tax_rate*3/4+1)));

			/* adjustment for military */
			if (spread.civilians>0)
				curntn->inflation += ((curntn->tmil*100/spread.civilians - 15)/5);
			/* adjustment for poverty */
			curntn->inflation += (curntn->poverty-50)/2;

			/* plus maybe an adjustment for jewel production as a ratio */
			/* for whatever is produced by the country.                 */

			/* now find new total gold talons in nations*/
			if (curntn->tgold > 1000000L) {
				curntn->tgold = (curntn->tgold /
					(400L + curntn->inflation)) * 400L;
			} else {
				curntn->tgold = (curntn->tgold * 400L) /
					(400L + curntn->inflation);
			}

			/* provide goods production */
			curntn->metals=spread.metal;
			curntn->jewels=spread.jewels;
		}
	}
}

/****************************************************************/
/*	UPDMIL() 						*/
/* reset military stuff 					*/
/****************************************************************/
#define MAXSIEGE (NTOTAL)
void
updmil()
{
	struct	army	*A;
	int	AX, AY, AT;	/* armies x,y locations, type : for speed */
	int armynum,nvynum,flag,dfltunit;
	int army2,asmen,dsmen,nation,sieges=0;
	unsigned char siegex[MAXSIEGE],siegey[MAXSIEGE],siegok[MAXSIEGE];

	fprintf(stderr,"updating armies and navies\n");
	for(country=1;country<NTOTAL;country++) 
	if(isntn(ntn[country].active)){
		curntn = &ntn[country];

		disarray=TRUE;
		dfltunit=(getleader(curntn->class)-1);
		for(armynum=0;armynum<MAXARM;armynum++)
			if (P_ATYPE==dfltunit && P_ASOLD>0) {
				disarray=FALSE;
				break;
			}

		if(ispc(curntn->active)) {
		prep( country, TRUE );	/* occ[][] now >0 if leader near */
		dfltunit = defaultunit(country);
		} else dfltunit = A_INFANTRY;

		for(armynum=0;armynum<MAXARM;armynum++) if(P_ASOLD>0) {

			A = &curntn->arm[armynum];
			AX = A->xloc;
			AY = A->yloc;
			if(A->unittyp==A_INFANTRY)
				A->unittyp = dfltunit;
			AT=A->unittyp;

			if( AT< MINLEADER ) {
				curntn->tmil+=A->sold;
				if( AT==A_MILITIA ) A->stat=MILITIA;
			}

			/* if group does not have a leader anymore */
			if((A->stat >= NUMSTATUS)
			&&((curntn->arm[A->stat-NUMSTATUS].unittyp<MINLEADER)
			||(curntn->arm[A->stat-NUMSTATUS].sold==0))) {
				A->stat=ATTACK;
			}
			flag=TRUE;

			/*add movement to all armies */
			/*unitmove is 10 times movement rate*/
			if(disarray) A->smove=0;
			else switch(A->stat) {
			case MARCH:
				A->smove=(curntn->maxmove * *(unitmove+(AT%UTYPE)))/5;
				break;
			case MILITIA:
			case ONBOARD:
				A->smove=0;
				break;
			case SIEGE:
				if((sct[AX][AY].owner!=country)
				&&(fort_val(&sct[AX][AY]) > 0)) {
					A->smove=0;
					flag=FALSE;
					for (army2=0;flag==FALSE && army2<sieges;army2++)
						if ((AX==siegex[army2])
						&&(AY==siegey[army2])) flag=TRUE;
					/* if this is a new SIEGE... check it */
					if (flag==FALSE && sieges<MAXSIEGE) {
						siegex[sieges]=AX;
						siegey[sieges]=AY;
						siegok[sieges]=FALSE;
						asmen=0;
						dsmen=0;
						for(nation=0;nation<NTOTAL;nation++){
							for(army2=0;army2<MAXARM;army2++)
							if((ntn[nation].arm[army2].xloc==AX)
							&&(ntn[nation].arm[army2].yloc==AY)
							&&(ntn[nation].arm[army2].stat==SIEGE)){
								if (ntn[nation].arm[army2].unittyp==A_SIEGE)
								asmen+=3*ntn[nation].arm[army2].sold;
								else asmen+=ntn[nation].arm[army2].sold;
							}
						}
						nation=sct[siegex[sieges]][siegey[sieges]].owner;
						for(army2=0;army2<MAXARM;army2++)
						if((ntn[nation].arm[army2].xloc==AX)
						&&(ntn[nation].arm[army2].yloc==AY)){
							if (ntn[nation].arm[army2].unittyp==A_MILITIA)
							dsmen+=ntn[nation].arm[army2].sold/2;
							else dsmen+=ntn[nation].arm[army2].sold;
						}
						if(asmen > 2*dsmen) {
							siegok[sieges]=TRUE;
							sieges++;
							/* keep SIEGE status */
							break;
						}
					} else {
						/* keep SIEGE status */
						if (siegok[army2-1]==TRUE) break;
					}
				}
				flag=FALSE;
				/* should drop through to defend reset */
			case GARRISON:
				if((flag==TRUE)
				&&(fort_val(&sct[AX][AY]) > 0)
				&&(sct[AX][AY].owner==country)) {
					A->smove=0;
					P_AMOVE=0;
					break;
				}
				flag=FALSE;
				/* reset to defend for improper garrison */
			case RULE:
				if((flag==TRUE)
				&&(ISCITY(sct[AX][AY].designation))
				&&(AT>=MINLEADER)&&(AT<MINMONSTER)
				&&(sct[AX][AY].owner==country)) {
					A->smove=0;
					break;
				}
				/* reset to defend for improper Rule */
			case SIEGED:
			case SORTIE:
				/* reset besieged or sortie troops to DEFEND */
			case FLIGHT:
			case MAGDEF:
			case MAGATT:
				/* reset magical stats to DEFEND */
				A->stat=DEFEND;
			default:
				A->smove=(curntn->maxmove * *(unitmove+(AT%UTYPE)))/10;
				break;
			}

			/* empower flight */
			if((avian(AT)==TRUE)
 			&&( A->stat!=ONBOARD )
			&&( A->stat<NUMSTATUS ))
				A->stat=FLIGHT;

			if((magic(country,ROADS)==TRUE)
			&&(sct[AX][AY].owner!=country)){
				if(A->smove>7) A->smove-=4;
				else if (A->smove>4) A->smove=4;
			}

			if((magic(country,SAPPER)==1)
			&&((AT==A_CATAPULT)||(AT==A_SIEGE))){
				curntn->tgold -= A->sold * (*(unitmaint+(AT))) / 2;
			} else if (AT<MINLEADER) {
				curntn->tgold -= A->sold * (*(unitmaint+(AT%UTYPE)));
				if((ispc(ntn[country].active))
				&&(occ[AX][AY] == 0)) 
					A->smove /= 2;
			} else if (AT>=MINMONSTER) {
				curntn->tgold -= 5L * (*(unitmaint+(AT%UTYPE)));
				if(curntn->jewels > (*(unitmaint+(AT%UTYPE))))
				curntn->jewels -= (long) (*(unitmaint+(AT%UTYPE)));
				else {
					if(ispc(curntn->active)) {
					if(mailopen(country)!=(-1)) {
						fprintf(fm,"Message to %s from Conquer\n\n",curntn->name);
						fprintf(fm,"Your %s (unit %d) leaves due to lack of jewels\n",
							   *(unittype+(AT%UTYPE)),armynum);
						mailclose(country);
						A->sold=0;
					}
					}
				}
			}
		}
		/* group moves at rate of slowest +2 */
		for(armynum=0;armynum<MAXARM;armynum++) 
		if(( P_ASTAT == GENERAL )&&(P_ASOLD>0)){
			flag=FALSE;
			for(nvynum=0;nvynum<MAXARM;nvynum++) {
				if((curntn->arm[nvynum].sold>0)
				&&(curntn->arm[nvynum].stat==armynum+NUMSTATUS)){
				flag=TRUE;
				if(P_AMOVE > curntn->arm[nvynum].smove)
					P_AMOVE = curntn->arm[nvynum].smove;
				}
			}
			if(flag==FALSE) P_ASTAT=DEFEND;
			else P_AMOVE+=2;
		}
		/*add to movement of fleets*/
		for(nvynum=0;nvynum<MAXNAVY;nvynum++) {
			/*update sea sectors*/
			if(P_NWSHP!=0 || P_NMSHP!=0 || P_NGSHP!=0) {
#ifdef STORMS
				if(sct[P_NXLOC][P_NYLOC].altitude==WATER) {
/*
 *	Storms should stay around and slowly move
 *	around the world.
 */
				/*all ships sunk on percentage PSTORM*/
				/*pirates never are sunk (implicitly)*/
				if((ntn[country].active != NPC_PIRATE )
				&&(magic(country,SAILOR)==FALSE)
				&&( rand()%100 < PSTORM) ) {
#ifdef HIDELOC
					fprintf(fnews,"3.\tstorm sinks %s fleet at sea\n",curntn->name);
#else
					fprintf(fnews,"3.\tstorm sinks %s fleet in %d,%d\n",curntn->name,P_NXLOC,P_NYLOC);
#endif /* HIDELOC */
					P_NWSHP=0;
					P_NMSHP=0;
					P_NGSHP=0;
					armynum=P_NARMY;
					if(armynum>=0&&armynum<MAXARM) {
						P_ASOLD=0;
					}
					P_NARMY=MAXARM;
					P_NPEOP=0;
					P_NCREW=0;
				}
				/* destroy ships without crew */
				if(P_NCREW==0) {
					P_NWSHP=0;
					P_NMSHP=0;
					P_NGSHP=0;
					armynum=P_NARMY;
					if(armynum>=0&&armynum<MAXARM) {
						P_ASOLD=0;
					}
					P_NARMY=0;
				}
				}
#endif
				if(disarray) P_NMOVE=0;
				else P_NMOVE = (fltspeed(nvynum)*P_NCREW)/SHIPCREW;
				if(magic(country,SAILOR)==TRUE) P_NMOVE*=2;

				curntn->tships += fltships(country,nvynum);
				curntn->tgold -= flthold(nvynum)*SHIPMAINT;
			} else {
				P_NWSHP=0;
				P_NMSHP=0;
				P_NGSHP=0;
			}
		} /* for */
	}
	fprintf(stderr,"doing sieges\n");

	/* kill movement on SIEGED troops */
	for(army2=0;army2<sieges;army2++) {
		if (siegok[army2]==FALSE) continue;
		country= sct[siegex[army2]][siegey[army2]].owner;
		curntn = &ntn[country];
#ifdef HIDELOC
		fprintf(fnews,"2.\tSector in nation %s is under siege\n",
			curntn->name);
#else
		fprintf(fnews,"2.\tNation %s under siege in sector %d,%d\n",
			curntn->name,siegex[army2],siegey[army2]);
#endif /* HIDELOC */
		if(ispc(curntn->active)) {
			if (mailopen( country )!=(-1)) {
				fprintf(fm, "Message to %s from Conquer\n\n",ntn[nation].name);
				fprintf(fm, "\tYou are under siege in sector %d,%d.\n",
					   siegex[army2],siegey[army2]);
				mailclose(country);
			}
		}
		for(armynum=0;armynum<MAXARM;armynum++) if(P_ASOLD>0){
			if(P_ASTAT!=FLIGHT&&(P_AXLOC==siegex[army2])
			&&(P_AYLOC==siegey[army2])) {
				P_AMOVE=0;
				if((P_ASTAT!=ONBOARD)&&(P_ASTAT!=RULE)&&(P_ASTAT!=TRADED))
				P_ASTAT=SIEGED;
			}
		}
	}
	printf("done with military\n");
}

/****************************************************************/
/*	UPDCOMODITIES()						*/
/* update commodities						*/
/****************************************************************/
void
updcomodities()
{
	register struct s_sector	*sptr;
	register int x,y;
	long xx;
	float tempflt;
	long dead;

	fprintf(fnews,"2\tWORLD ECONOMY & DECLARATIONS OF WAR\n");
	printf("working on world economy\n");
	for(country=1;country<NTOTAL;country++) 
	if(isntn(ntn[country].active)){
		curntn = &ntn[country];
		/*soldiers eat  2 times as much */
		curntn->tfood-=curntn->tmil*P_EATRATE*2;
		curntn->tfood-=curntn->tciv*P_EATRATE;

		/*starve people*/
		if(curntn->tfood<0) for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) {
			sptr = &sct[x][y];
			if((sptr->owner==country)
			&&((sptr->designation==DTOWN)
			||(sptr->designation==DCAPITOL)
			||(sptr->designation==DCITY))
			&&(curntn->tfood<0)){
				/*lose one person in city per three food*/
				/*maximum of 1/3 people in city lost*/
				if(sptr->people < curntn->tfood){
					sptr->people+=curntn->tfood/3;
					curntn->tfood=0;
				} else {
					curntn->tfood+=sptr->people;
					dead = sptr->people/3;
					sptr->people -= dead;
				}
#ifdef HIDELOC
				fprintf(fnews,"2.\tfamine hits town in %s.\n",curntn->name);
#else
				fprintf(fnews,"2.\tfamine hits town at %d,%d in %s.\n",x,y,curntn->name);
#endif /* HIDELOC */
				printf("famine hits town at %d,%d in %s.\n",x,y,curntn->name);
				if(ispc(curntn->active)){
				if (mailopen( country )!=(-1)) {
					fprintf(fm,"Message to %s from CONQUER\n\n",curntn->name);
					fprintf(fm,"During the %s of Year %d,\n",PSEASON(TURN),YEAR(TURN));
					fprintf(fm,"a famine hit your town at %d,%d.\n",x,y);
					fprintf(fm,"%d out of %d people died.\n",dead,sptr->people);
					mailclose(country);
				}
				}
			}
		}
		/*this state can occur if few people live in cities*/
		if(curntn->tfood<0) curntn->tfood=0L;
		tempflt = (float) curntn->tfood * (100-curntn->spoilrate);
		curntn->tfood = (long) (tempflt / 100.0);

		if((0.0+curntn->tgold) - GOLDTHRESH*(0.0+curntn->jewels) > 0.0){
			/* buy jewels off commodities board */
			xx=curntn->tgold-GOLDTHRESH*curntn->jewels;
			if (ispc(curntn->active)) {
				if (mailopen(country)!=(-1)) {
					fprintf(fm,"Message from Conquer\n\n");
					fprintf(fm,"Gold imbalance forced your treasury to purchase\n");
					fprintf(fm,"%ld jewels for %ld gold talons to compensate.\n",
						dtol((double)xx*GODJEWL/GODPRICE)
						,xx);
					mailclose(country);
				}
			}
			curntn->jewels += dtol((double)xx*GODJEWL/GODPRICE);
			curntn->tgold  -= xx;
		}

		/* fix overflow problems */
		if(curntn->tgold < -1*BIG)  {
			fprintf(fnews,"2.\tVariable Overflow - gold in nation %s\n",curntn->name);
			curntn->tgold=BIG;
		}
		if(curntn->tfood < -1*BIG)  {
			fprintf(fnews,"2.\tVariable Overflow - food in nation %s\n",curntn->name);
			curntn->tfood=BIG;
		}
		if(curntn->jewels < -1*BIG) {
			fprintf(fnews,"2.\tVariable Overflow - jewels in nation %s\n",curntn->name);
			curntn->jewels=BIG;
		}
		if(curntn->metals < -1*BIG)  {
			fprintf(fnews,"2.\tVariable Overflow - metal in nation %s\n",curntn->name);
			curntn->metals=BIG;
		}
	}
}

/****************************************************************/
/* Conquer: Copyright (c) 1988 by Edward M Barlow
/*	UPDLEADER()						*/
/****************************************************************/
void
updleader()
{
	int	nation,armynum,born,type;
	printf("working on national leaders\n");
	for(nation=0;nation<NTOTAL;nation++) {
		curntn = &ntn[nation];
		if(!isntn(curntn->active)) continue;

		/* monster nations get monsters */
		if((SEASON(TURN) == SPRING)&&(magic(nation,MI_MONST)==TRUE)) {
			born=100;	/* born represents strength of monst */
			if(magic(nation,AV_MONST)==TRUE) born=200;
			if(magic(nation,MA_MONST)==TRUE) born=BIG;

			do type = MINMONSTER + rand()%(MAXMONSTER-MINMONSTER+1);
			while( *(unitminsth+(type%UTYPE)) > born);

			for(armynum=0;armynum < MAXARM;armynum++) {
				if(P_ASOLD != 0) continue;
				P_ATYPE = type;
				P_ASOLD = *(unitminsth+(type%UTYPE));
				P_AXLOC = curntn->capx;
				P_AYLOC = curntn->capy;
				P_ASTAT = DEFEND;
				P_AMOVE = 2*curntn->maxmove;
				if( ispc( ntn[nation].active ) ){
					if (mailopen( nation )!=(-1)) {
					fprintf(fm,"Message to %s from Conquer:\n\n",ntn[nation].name);
					fprintf(fm,"\t\tMonster born in your nation!\n");
					mailclose(nation);
					}
				}
				printf("\tmonster born in nation %s\n",curntn->name); 
				break;
			}
		}

		switch(curntn->class){	/* get national born rate */
		case C_NPC:
		case C_KING:
		case C_TRADER:
		case C_EMPEROR:	born = 50; break;
		case C_WIZARD:
		case C_PRIEST:
		case C_PIRATE:
		case C_WARLORD:
		case C_DEMON:	born = 25; break;
		case C_DRAGON:
		case C_SHADOW:	born = 2; break;
		default:
			printf("ERROR - national class (%d) undefined\n",curntn->class);
			abrt();
		}
		/* born represents yearly birth rate */
		if( rand()%400 >= born ) continue;

		for(armynum=0;armynum < MAXARM;armynum++) { /* add one leader */
			if(P_ASOLD != 0) continue;
			P_ATYPE = getleader(curntn->class);
			P_ASOLD = *(unitminsth+(P_ATYPE%UTYPE));
			P_AXLOC = curntn->capx;
			P_AYLOC = curntn->capy;
			P_ASTAT = DEFEND;
			P_AMOVE = 2*curntn->maxmove;
			if( ispc( ntn[nation].active ) ){
				if(mailopen( nation )!=(-1)) {
					fprintf(fm,"Message to %s from Conquer:\n\n",ntn[nation].name);
					fprintf(fm,"\t\tLeader born in your nation!\n");
					mailclose(nation);
				}
			}
			printf("\tleader born in nation %s\n",curntn->name); 
			break;
		}
	}
}

/* MOVE CIVILIANS based on the ratio of attractivenesses
 *
 * EQUILIBRIUM(1) = A1 / (A1 + A2) * (P1 + P2)
 * EQUILIBRIUM(2) = A2 / (A1 + A2) * (P1 + P2)
 * MOVE 1/5 of way to equilibrium each turn
 * DELTA(1) = (EQUILIBRIUM(1) - P1) / 5 =(A1P2 - P1A2) / 5(A1 + A2)
 * DELTA(2) = (EQUILIBRIUM(2) - P2) / 5 =(A2P1 - P2A1) / 5(A1 + A2) = -DELTA(1)
 * (i, j) is refered to as 1, (x, y) as 2
 */
void
move_people()
{
	register int x, y, i, j;
	register struct s_sector *sptr;
	int t_attr;
	long *curpop, moved;

	/* store the first two rows for computation */
	for (x = 0; x < 3; x++) {
		for (y = 0; y < MAPY; y++) {
			if (sct[x][y].owner == country)
				newpop[x][y] = sct[x][y].people;
			else newpop[x][y] = 0L;
		}
	}

	/* go through entire map to move civilians */
	for (x = 0; x < MAPX; x++) {
		for (y = 0; y < MAPY; y++) {
			sptr = &sct[x][y];
			if ((sptr->owner == country) && (sptr->people != 0)) {

				for (t_attr = 0, i = x - 2; i < x + 3; i++)
				for (j = y - 2; j < y + 3; j++)
				if (ONMAP(i, j))
					t_attr += attr[i][j];

				if (t_attr > 0) {
					t_attr *= 5;
					curpop = &newpop[x%5][y];
					for (i = x - 2; i < x + 3; i++)
					for (j = y - 2; j < y + 3; j++)
					if (ONMAP(i, j)) {
						moved = sptr->people * attr[i][j];
						if (moved > 0) {
							moved /= t_attr;
							*curpop -= moved;
							newpop[i%5][j] += moved;
						}
					}
				}
			}
		}
		/* store old row, and read in new */
		for (y = 0; y < MAPY; y++) {
			if (x > 1) {
				if (sct[x-2][y].owner == country)
					sct[x-2][y].people = newpop[(x-2)%5][y];
			}
			if (x < MAPX - 3) {
				if (sct[x+3][y].owner == country)
					newpop[(x+3)%5][y] = sct[x+3][y].people;
				else newpop[(x+3)%5][y] = 0L;
			}
		}
	}

	/* assign values for final rows */
	for (x = MAPX - 2; x < MAPX; x++) {
		for (y = 0; y < MAPY; y++) {
			if (sct[x][y].owner == country)
				sct[x][y].people = newpop[x%5][y];
		}
	}
}
