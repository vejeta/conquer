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

/*	screen subroutines	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <curses.h>
#include <pwd.h>
#include "header.h"
#include "data.h"

extern FILE	*fexe, *fnews;
extern short	country,redraw;
extern long	startgold;
extern short	Gaudy;

static char helplist[MAXHELP][20]={"Commands", "General Info",
	"Military","Magic","Designations","Other"};

#define RPT_LINES 14
#define RPT_COLS 13
#define BUF_LINES 10
#define BUF_COLS 15
#define MAXINROW ((COLS-BUF_COLS)/RPT_COLS)
#define MAXINSCR (((LINES-BUF_LINES)/RPT_LINES)*MAXINROW)
void
showscore()
{
	int i;
	int done=FALSE;
	int xpos,ypos;
	int count,count2;   /*number of time through the loop on this screen*/
	int nationid; 	     /*current nation id */

	count2=1;
	clear();
	while(done==FALSE) {
		move(0,0);
		clrtobot();
		standout();
		mvaddstr(0,(COLS/2)-10,"NATION SCORE SCREEN");
		standend();

		nationid=count2;
		xpos=BUF_COLS;
		ypos=3;
		count=0;
		while((nationid<NTOTAL)&&(count<MAXINSCR)){
			if((nationid<NTOTAL)
			&&(isntn(ntn[nationid].active))) {
				if (count%MAXINROW==0) {
					/* display header information */
					mvaddstr(ypos,0,"nationid is:");
					mvaddstr(ypos+1,0,"name is:");
					mvaddstr(ypos+2,0,"leader:");
					mvaddstr(ypos+3,0,"race:");
					mvaddstr(ypos+4,0,"class:");
					mvaddstr(ypos+5,0,"alignment:");
					mvaddstr(ypos+6,0,"score:");
#ifdef NOSCORE
					mvaddstr(ypos+7,0,"npc nation:");
					if (country==0) {
						mvaddstr(ypos+8,0,"talons:");
						mvaddstr(ypos+9,0,"military:");
						mvaddstr(ypos+10,0,"civilians:");
						mvaddstr(ypos+11,0,"sectors:");
					}
#else
					mvaddstr(ypos+7,0,"talons:");
					mvaddstr(ypos+8,0,"military:");
					mvaddstr(ypos+9,0,"civilians:");
					mvaddstr(ypos+10,0,"sectors:");
					mvaddstr(ypos+11,0,"npc nation:");
#endif /* NOSCORE */
				}

				/* display nation information */
				mvprintw(ypos,xpos,"%d",nationid);
				standout();
				mvprintw(ypos+1,xpos,"%s",ntn[nationid].name);
				standend();
				mvprintw(ypos+2,xpos,"%s",ntn[nationid].leader);
				for(i=1;i<8;i++)
					if(ntn[nationid].race==*(races+i)[0])
						mvprintw(ypos+3,xpos,"%s",*(races+i));
				mvprintw(ypos+4,xpos,"%s",*(Class+ntn[nationid].class));
				mvprintw(ypos+5,xpos,"%s",alignment[npctype(ntn[nationid].active)]);
				mvprintw(ypos+6,xpos,"%ld",ntn[nationid].score);
#ifdef NOSCORE
				if(isnpc(ntn[nationid].active))
					mvaddstr(ypos+7,xpos,"Yes");
				else mvaddstr(ypos+7,xpos,"No");
				if (country==0) {
					mvprintw(ypos+8,xpos,"%ld",ntn[nationid].tgold);
					mvprintw(ypos+9,xpos,"%ld",ntn[nationid].tmil);
					mvprintw(ypos+10,xpos,"%ld",ntn[nationid].tciv);
					mvprintw(ypos+11,xpos,"%d",ntn[nationid].tsctrs);
				}
#else
				mvprintw(ypos+7,xpos,"%ld",ntn[nationid].tgold);
				mvprintw(ypos+8,xpos,"%ld",ntn[nationid].tmil);
				mvprintw(ypos+9,xpos,"%ld",ntn[nationid].tciv);
				mvprintw(ypos+10,xpos,"%d",ntn[nationid].tsctrs);
				if(isnpc(ntn[nationid].active))
					mvaddstr(ypos+11,xpos,"Yes");
				else mvaddstr(ypos+11,xpos,"No");
#endif /* NOSCORE */
				count++;
				if(count<MAXINSCR && count%MAXINROW==0) {
					ypos+=RPT_LINES;
					xpos=BUF_COLS;
				} else {
					xpos+=RPT_COLS;
				}
			}
			nationid++;
		}
		standout();
		mvaddstr(LINES-6,(COLS/2)-12,"HIT ANY KEY TO CONTINUE");
		mvaddstr(LINES-5,(COLS/2)-9,"HIT SPACE IF DONE");
		standend();
		refresh();
		if (getch()==' ') done=TRUE;
		else {
			/* go to start of next page in list */
			while(nationid<NTOTAL && !isntn(ntn[nationid].active))
				nationid++;
			if(nationid==NTOTAL) count2=1;
			else count2=nationid;
		}
	}
}

#define	MAXINCOL	(LINES-10)
#define	MAXONSCR	(MAXINCOL*(COLS/40))
void
diploscrn()
{
	int i,j;
	char k,name[LINELTH];
	short nation, offset, count, count2, temp;
	short isgod=FALSE;
	long	bribecost;
	if(country==0) {
		isgod=TRUE;
		if (get_god()) return;
	}
	clear();
	count2=1;
	while(1){
		count=1;
		offset=0;
		move(0,0);
		clrtobot();
		standout();
		mvaddstr(0,(COLS/2)-12,"NATION DIPLOMACY SUMMARY");
		standend();
		mvaddstr(2,0,"                BY YOU,        TO YOU");
		for(i=count2;i<NTOTAL && count<MAXONSCR;i++)
		if((isntnorp(ntn[i].active))&&(i!=country)) {
			if(count%MAXINCOL==0) {
				offset+=40;
				mvaddstr(2,offset,"                BY YOU,        TO YOU");
				count++;
			}
			mvprintw(count%MAXINCOL+2,offset, "%d. %s",i,ntn[i].name);
			if((curntn->dstatus[i]==WAR)
			||(curntn->dstatus[i]==JIHAD)) standout();
			mvprintw(count%MAXINCOL+2,offset+14, "=> %s",*(diploname+curntn->dstatus[i]));
			standend();
			if((ntn[i].dstatus[country]==WAR)
			||(ntn[i].dstatus[country]==JIHAD)) standout();
			mvprintw(count%MAXINCOL+2,offset+28, "=> %s",*(diploname+ntn[i].dstatus[country]));
			standend();
			count++;
		}

		/* display options */
		standout();
		mvaddstr(LINES-7,COLS/2-26,"HIT RETURN KEY TO CHANGE STATUS; HIT SPACE IF DONE");
		mvprintw(LINES-6,COLS/2-31,"HIT 'B' KEY TO BRIBE NPC NATION (cost=$%ld per 1000 Mil.)",BRIBE);
		mvaddstr(LINES-5,COLS/2-21,"HIT ANY OTHER KEY TO SCROLL NATIONS LIST");
		standend();
		refresh();
		k=getch();
		if(k==' ') {
			if(isgod==TRUE) reset_god();
			return;
		}
		if((k!='B')&&(k!='\r')&&(k!='\n')) {
			/* find start for next listing */
			for(; i<NTOTAL && (!isntnorp(ntn[i].active)||i==country); i++) ;
			if(i==NTOTAL) count2=1;
			else count2=i;
			continue;
		}
		if(k=='B'){
			if(curntn->tgold<=BRIBE){
				mvaddstr(LINES-3,0,"NOT ENOUGH GOLD");
				refresh();
				getch();
				if(isgod==TRUE) reset_god();
				return;
			}
			mvaddstr(LINES-4,0,"BRIBES DONT ALWAYS WORK (only the update will show)");
			mvaddstr(LINES-3,0,"WHAT NATION:");
			refresh();
			nation = get_country();

			/* may only change with NPCs */
			if((nation<=0)
			||(nation>=NTOTAL)
			||(!isnpc(ntn[nation].active))){
				errormsg("That nation is not a Non-Player Country");
				if(isgod==TRUE) reset_god();
				return;
			}
			if(ntn[nation].dstatus[country]==UNMET){
				errormsg("They have no knowledge that you exist");
				if(isgod==TRUE) reset_god();
				return;
			}
			if((isgod!=TRUE)
			    &&((ntn[nation].dstatus[country]==ALLIED)
			    ||(ntn[nation].dstatus[country]==JIHAD)
			    ||(ntn[nation].dstatus[country]==UNMET)
			    ||(ntn[nation].dstatus[country]==TREATY))){
				errormsg("Sorry, you cannot bribe them");
				return;
			}

			if( isgod==TRUE ) {
				bribecost = 0;
			} else if( ntn[nation].tmil > 1000 ) {
				bribecost = BRIBE * ntn[nation].tmil / 1000;
			} else	bribecost = BRIBE;
			mvprintw(LINES-3,0,"This will cost %ld gold talons. continue (y or n)",bribecost);
			refresh();

			if( getch() != 'y' ) return;
			if(curntn->tgold<=bribecost){
				errormsg("Sorry Not Enough Gold");
				if(isgod==TRUE) reset_god();
				return;
			}
			curntn->tgold-=bribecost;

			sprintf(name,"%s%d",exefile,nation);
			if ((fm=fopen(name,"a+"))==NULL) {
				printf("error opening news file\n");
				exit(FAIL);
			}
			BRIBENATION;

			ntn[nation].dstatus[country]--;

			if(isgod==TRUE) reset_god();
			return;
		}
		mvaddstr(LINES-3,0,"WHAT NATION:");
		refresh();
		nation = get_country();
		/* can't change with nomads...*/
		if((nation<=0)||(nation>NTOTAL)||(!isntnorp(ntn[nation].active))){
			if(isgod==TRUE) reset_god();
			return;
		}
		if((isgod==FALSE)
		    &&(curntn->tgold < BREAKJIHAD )
		    &&(((curntn->dstatus[nation]==TREATY)
		    &&(ntn[nation].dstatus[country]<WAR))
		    ||(curntn->dstatus[nation]==JIHAD))) {

			mvprintw(LINES-1,0,"Sorry, need %d talons to change status with ntn %s",BREAKJIHAD,ntn[nation].name);
			clrtoeol();
        		mvaddstr(LINES-1, 60, "PRESS ANY KEY");
			refresh();
			getch();
			if(isgod==TRUE) reset_god();
			return;
		} else if (isgod==FALSE && curntn->dstatus[nation]==UNMET
		&& ntn[nation].dstatus[country]==UNMET) {
			errormsg("Sorry, you have not met that nation yet.");
			if(isgod==TRUE) reset_god();
			return;
		} else {
			clear();
			mvaddstr(0,0,"WHAT NEW STATUS");
			j=2;
			mvprintw(j++,0,"1) TREATY (%d talons to break)",BREAKJIHAD);
			mvaddstr(j++,0,"2) ALLIED");
			mvaddstr(j++,0,"3) FRIENDLY");
			mvaddstr(j++,0,"4) NEUTRAL");
			mvaddstr(j++,0,"5) HOSTILE");
			mvaddstr(j++,0,"6) WAR");
			mvprintw(j++,0,"7) JIHAD (%d talons to break):",BREAKJIHAD);
			j++;
			for( i=1; i<NTOTAL; i++ )
				if((isntnorp(ntn[i].active)) && (ntn[nation].dstatus[i]==TREATY))
				mvprintw(j++,10,"%s has treaty with %s",ntn[nation].name,ntn[i].name);

			j++;
			mvaddstr(j++,0,"INPUT:");
			refresh();
			temp = get_number();
			if(temp<0) {
				if(isgod==TRUE) reset_god();
				return;
			}
			if((temp<=UNMET)||(temp>JIHAD)
			||((isgod==FALSE)&&(temp==UNMET))){
				errormsg("SORRY, Invalid inputs -- hit return");
				if(isgod==TRUE) reset_god();
				return;
			}

			if((curntn->dstatus[nation]==JIHAD)&&(temp!=JIHAD))
		    		curntn->tgold -= BREAKJIHAD;
			else if ((curntn->dstatus[nation]==TREATY)&&(temp!=TREATY)) {
				if (ntn[nation].dstatus[country]!=TREATY) {
					errormsg("Non-binding Treaty broken... fee waived.");
				} else {
					curntn->tgold -= BREAKJIHAD;
				}
			}

			curntn->dstatus[nation]=temp;
			EADJDIP(country,nation);

			if((temp>HOSTILE)
			&&(ispc(ntn[nation].active))
			&&(ntn[nation].dstatus[country]<WAR)) {
				if(mailopen(nation)!=(-1)) {
					fprintf(fm,"Message to %s from CONQUER\n",ntn[nation].name);
					fprintf(fm,"    During the %s of Year %d,\n",PSEASON(TURN),YEAR(TURN));
					fprintf(fm,"      %s declared war on you\n",curntn->name);
					mailclose(nation);
				}
			}

			/*prevent ron from being sneaky*/
			if((temp>HOSTILE)
			&&(isactive(ntn[nation].active))
			&&(ntn[nation].dstatus[country]<WAR)) {
				ntn[nation].dstatus[country]=WAR;
				EADJDIP(nation,country);
				for( i=1; i<NTOTAL; i++ )
				if(ntn[i].dstatus[nation]==TREATY){
					ntn[i].dstatus[country]=WAR;
					EADJDIP(i,country);
					if (mailopen(country)==(-1)) {
					fprintf(fm,"Message to %s from %s\n",ntn[country].name,ntn[i].name);
					switch( rand()%4 ) {
					case 0: fprintf(fm,"You just attacked my friend %s - your loss!!!\n",ntn[nation].name);
						break;
					case 1: fprintf(fm,"Our imperial forces will assist %s!!!\n",ntn[nation].name);
						fprintf(fm,"in repulsing your toy armies!!!\n");
						break;
					case 2: fprintf(fm,"Sucker - your aggression against %s is your doom!!!\n",ntn[nation].name);
						break;
					case 3: fprintf(fm,"We deplore your aggression against %s and are taking\n",ntn[nation].name);
						fprintf(fm,"appropriate counter measures!!! DIE!!!\n");
						break;
					}
					}
					mailclose(country);
				}
			}
		}
	}
}

int terror_adj=0;
void
change()
{
	float temp;
	char string[LINELTH], command[BIGLTH];
	int i, intval;
	long	cost,men;
	short armynum;
	char passwd[PASSLTH+1];
	short isgod=FALSE;
#ifdef OGOD
	FILE *ftmp;
#endif /* OGOD */
	char filename[FILELTH];

	if(country==0) {
		isgod=TRUE;
		if (get_god()) return;
	}
 
	/* continuous loop */
	clear();
	while(1) {
	move(0,0);
	clrtobot();
	standout();
	mvaddstr(0,(COLS/2)-10,"NATION STATS SUMMARY");
	mvprintw(3,0,"nation name is %s",curntn->name);
	mvprintw(4,0,"alignment is %s",alignment[npctype(curntn->active)]);

	if (isgod==TRUE)
		mvprintw(5,0,"active is %d",curntn->active);
	mvprintw(6,0,"tax_rate...... %2d%%",curntn->tax_rate);
	mvprintw(7,0,"inflation..... %2d%%",curntn->inflation);
	i=(int)10*curntn->tax_rate-curntn->popularity-curntn->terror-3*curntn->charity;
	if( i<0 ) i=0;
	mvprintw(8,0,"peasant revolt %2d%%",(i*PREVOLT)/100);
	i=(int)5*curntn->tax_rate - curntn->prestige;
	if( i<0 ) i=0;
	mvprintw(9,0,"other revolt.. %2d%%",(i*PREVOLT)/100);
	standend();
	mvprintw(10,0,"capitol loc: x is %d",curntn->capx);
	mvprintw(11,0,"             y is %d",curntn->capy);
	mvprintw(13,0,"class is %s",*(Class+curntn->class));
	if(ispc( curntn->active ) )
		addstr(" (PC)");
	else if(isnpc( curntn->active ) )
		addstr(" (NPC)");
	else	addstr(" (MON)");
	mvprintw(14,0,"nations mark is...%c ",curntn->mark);
	for(i=1;i<8;i++) if(curntn->race==*(races+i)[0]){
		mvprintw(15,0, "nation race is....%s  ",*(races+i));
		break;
	}

	mvprintw(3,COLS/2-12, "terror........ %3d",curntn->terror);
	mvprintw(4,COLS/2-12, "popularity.... %3d",curntn->popularity);
	mvprintw(5,COLS/2-12, "prestige...... %3d",curntn->prestige);
	mvprintw(6,COLS/2-12, "knowledge..... %3d",curntn->knowledge);
	temp = P_EATRATE;
	mvprintw(7,COLS/2-12, "eatrate.......%3.2f",temp);
	mvprintw(8,COLS/2-12, "wealth........ %3d",curntn->wealth);
	mvprintw(9,COLS/2-12, "charity....... %2d%%",curntn->charity);
	mvprintw(10,COLS/2-12,"communication.%3.2f",(float) P_NTNCOM);
	mvprintw(11,COLS/2-12,"reputation.... %3d",curntn->reputation);
	mvprintw(12,COLS/2-12,"spoilrate.....%3d%%",curntn->spoilrate);
	mvprintw(13,COLS/2-12,"farm ability.. %3d",curntn->farm_ability);
	mvprintw(14,COLS/2-12,"mine ability.. %3d",curntn->mine_ability);
	mvprintw(15,COLS/2-12,"poverty rate.. %2d%%",curntn->poverty);
	mvprintw(16,COLS/2-12,"power......... %3d",curntn->power);

	if (country!=0) {
		mvprintw(12,0,"leader is %s",curntn->leader);
		mvprintw(16,0,"score currently...%ld",curntn->score);
		mvprintw(3,COLS-30, "attack bonus.........%+4d%%",curntn->aplus);
		mvprintw(4,COLS-30, "defense bonus........%+4d%%",curntn->dplus);
		mvprintw(12,COLS-30,"total soldiers....%8ld",curntn->tmil);
	} else {
		mvprintw(12,0,"demigod is %s",curntn->leader);
		mvprintw(16,0,"turn currently....%ld",TURN);
		mvprintw(3,COLS-30, "mercs attack bonus....+%2d%%",MERCATT);
		mvprintw(4,COLS-30, "mercs defense bonus...+%2d%%",MERCDEF);
		mvprintw(12,COLS-30,"total mercs.......%8ld",MERCMEN);
	}

  	mvprintw(5,COLS-30, "maximum move rate.......%2d",curntn->maxmove);
  	mvprintw(6,COLS-30, "reproduction rate......%2d%%",curntn->repro);
  	mvprintw(8,COLS-30, "gold talons......$%8ld",curntn->tgold);

	mvprintw(9,COLS-30,"jewels ..........$%8ld",curntn->jewels);
	mvprintw(10,COLS-30,"metal & minerals..%8ld",curntn->metals);
	if(curntn->tfood<2*curntn->tciv) standout();
	mvprintw(11,COLS-30,"food in granary...%8ld",curntn->tfood);
	standend();

	mvprintw(13,COLS-30,"total civilians...%8ld",curntn->tciv);
	mvprintw(14,COLS-30,"total ships..........%5d",curntn->tships);
	mvprintw(15,COLS-30,"total sectors........%5d",curntn->tsctrs);
	mvprintw(16,COLS-30,"spell points.........%5d",curntn->spellpts);

	standout();
	mvaddstr(LINES-6,COLS/2-26,"HIT 'B' FOR BUDGET SCREEN, 'P' FOR PRODUCTION SCREEN");
	mvaddstr(LINES-5,COLS/2-25,"1) NAME 2) PASSWD 3) TAX RATE 4) CHARITY 5) TERROR");
	if(magic(country,VAMPIRE)!=1 || isgod==TRUE)
		mvaddstr(LINES-4,COLS/2-21,"6) ADJUST TO COMBAT BONUS");
	else	mvaddstr(LINES-4,COLS/2-22,"CANT ADD TO COMBAT BONUS");
	addstr(" 7) TOGGLE PC <-> NPC");

#ifdef OGOD
	if(isgod==TRUE) mvaddstr(LINES-3,COLS/2-33,"HIT 8 TO DESTROY, 9 TO CHANGE COMMODITY OR '0' TO CHANGE DEMI-GOD");
#else OGOD
	if(isgod==TRUE) mvaddstr(LINES-3,COLS/2-24,"HIT 8 TO DESTROY NATION, OR '0' TO CHANGE DEMI-GOD");
#endif /* OGOD */
	else mvaddstr(LINES-3,COLS/2-14,"HIT ANY OTHER KEY TO CONTINUE");
	standend();
	refresh();
	switch(getch()){
	case '1': /*get name*/
		clear();
		mvaddstr(0,0,"What name would you like:");
		clrtoeol();
		refresh();
		get_nname(string);
		if((strlen(string)<=1)||(strlen(string)>NAMELTH)){
			errormsg("Invalid name length");
			break;
		}
		/*check if already used*/
		else for(i=1;i<NTOTAL;i++){
		    if (isactive(ntn[i].active)) {
			if((strcmp(ntn[i].name,string)==0)&&(i!=country)) {
				errormsg("Name already used");
				break;
			}
		    }
		}
		errormsg("New name can be used following next update");
		strcpy(curntn->name,string);
		ECHGNAME;
		break;
	case '2': /*change password */
		clear();
		/* minimum password length setting */
		if (country != 0) intval=2;
		else intval=4;
		if(isgod!=TRUE){
			mvaddstr(0,0,"What is your current password:");
			refresh();
			(void) get_pass(command);
			strncpy(passwd,crypt(command,SALT),PASSLTH+1);
			if((strncmp(passwd,ntn[0].passwd,PASSLTH)!=0)
			&&(strncmp(passwd,curntn->passwd,PASSLTH)!=0)){
				break;
			}
		}
		mvaddstr(2,0,"What is your new password:");
		refresh();
		i = get_pass(command);
		if (i<intval) {
			errormsg("Password too short");
			break;
		} if (i>PASSLTH) {
			errormsg("Password too long");
			break;
		}
		strncpy(passwd,command,PASSLTH);
		mvaddstr(4,0,"Reenter your new password:");
		refresh();
		(void) get_pass(command);
		if(strncmp(passwd,command,PASSLTH)!=0) {
			errormsg("Invalid password match; Password unchanged");
			break;
		}
		errormsg("New password can be used following next update");
		strncpy(curntn->passwd,crypt(command,SALT),PASSLTH);
		ECHGPAS;
		break;
	case '3': /* change tax rate */
		standout();
		mvaddstr(LINES-1,0,"WHAT TAX RATE DO YOU WISH:");
		standend();
		refresh();
		intval = get_number();
		if( intval < 0 )
			break;
		else if( intval > 20 )
			errormsg("NO WAY! the peasants will revolt!!!");
		else if( intval > 10 && (curntn->tsctrs<20 || curntn->score<20) )
			errormsg("Sorry, you may not go above 10% yet");
		else {
			curntn->tax_rate = (unsigned char) intval;
			NADJNTN;
		}
		break;
	case '4':	/* charity */
		standout();
		mvaddstr(LINES-1,0,"GIVE WHAT PERCENT OF YOUR INCOME TO THE POOR:");
		standend();
		refresh();
		intval = get_number();
		if (intval < 0) {
			break;
		} else if( intval > 100 ) {
			errormsg("ERROR - invalid charity rate");
		} else if (intval > 25) {
			errormsg("You are kidding, right?");
		/* this will protect from both underflow and overflow */
		} else if(intval > curntn->charity) {
			if (2 * (intval - (int)curntn->charity) + (int) curntn->popularity > 100) {
				errormsg("ERROR - you may not increase charity that much");
			} else {
				curntn->popularity += (unsigned char) 2*(intval - (int) curntn->charity);
				curntn->charity = intval;
				NADJNTN;
				NADJNTN2;
			}
		} else {
			if (2 * (intval - (int)curntn->charity) < - (int) curntn->popularity) {
				errormsg("ERROR - you may not decrease charity that much");
			} else {
				curntn->popularity += (unsigned char) 2*(intval - (int) curntn->charity);
				curntn->charity = intval;
				NADJNTN;
				NADJNTN2;
			}
		}
		break;
	case '5':	/* terror */
		mvaddstr(LINES-2,0,"YOU CAN TERRORIZE YOUR PEOPLE AN ADDITONAL 1-5%:");
		standout();
		mvaddstr(LINES-1,0,"HOW MUCH MORE TO TERRORIZE THEM: ");
		standend();
		refresh();
		intval = get_number();
		if( intval < 0 )
			break;
		else if( intval+curntn->terror > 100 )
			errormsg("Cant go over 100 terror!!!");
		else if((intval > curntn->popularity )
		|| (intval > curntn->reputation ))
			errormsg("Sorry - this would cause underflow");
		else if(intval>5) {
			errormsg("That is over the allowed 5%");
		} else if(terror_adj>0) {
			errormsg("Terror may only be adjusted once per turn");
		} else if(intval>0) {
			terror_adj++;
			curntn->terror += (unsigned char) intval;
			curntn->popularity -= (unsigned char) intval;
			curntn->reputation -= (unsigned char) (intval+1)/2;
			NADJNTN2;
		}
		break;
	case '6':	/* combat bonus */
#ifdef OGOD
		if(isgod==TRUE) {
			mvaddstr(LINES-2,0,"SUPER-USER: Change (A)ttack or (D)efense Bonus? ");
			refresh();
			if ((intval=getch())=='A' || intval=='a')  {
				mvaddstr(LINES-1,0,"Enter new value for Attack Bonus: ");
				refresh();
				curntn->aplus = get_number();
				if (curntn->aplus < 0) curntn->aplus = 0;
			} else if (intval=='d' || intval == 'D') {
				mvaddstr(LINES-1,0,"Enter new value for Defense Bonus: ");
				refresh();
				curntn->dplus = get_number();
				if (curntn->dplus < 0) curntn->dplus = 0;
			}
			break;
		}
#endif /*OGOD*/
		if(magic(country,VAMPIRE)==1) {
			errormsg("VAMPIRES CAN'T ADD TO COMBAT BONUS");
			break;
		}
		if(magic(country,WARLORD)==1)      intval=30;
		else if(magic(country,CAPTAIN)==1) intval=20;
		else if(magic(country,WARRIOR)==1) intval=10;
		else intval=0;
		men=0;
		for(armynum=0;armynum<MAXARM;armynum++)
			if((P_ASOLD>0)&&(P_ATYPE<MINLEADER)) men+=P_ASOLD;
		men = max( men, 1500);
		armynum = max( curntn->aplus-intval, 10 ) / 10;
		cost = METALORE*men*armynum*armynum;
		if( curntn->race == ORC) cost*=3;
		mvprintw(LINES-1,0,"Do You Wish Spend %ld Metal On Attack (enter y or n):",cost);
		refresh();
		if(getch()=='y'){
			if(curntn->metals> cost){
				curntn->aplus+=1;
				I_APLUS;
				curntn->metals-=cost;
			} else {
				errormsg("SORRY");
			}
		}
		armynum = max( curntn->dplus-intval, 10 ) / 10;
		cost=METALORE*men*armynum*armynum;
		if( curntn->race == ORC) cost*=3;
		mvprintw(LINES-1,0,"Do You Wish Spend %ld Metal On Defense (enter y or n):",cost);
		refresh();
		if(getch()=='y'){
			if(curntn->metals>cost){
				curntn->dplus+=1;
				I_DPLUS;
				curntn->metals-=cost;
			} else {
				errormsg("SORRY");
			}
		}
		break;
	case '7':
		if(ispc(curntn->active)) {
			errormsg("Note: you get no mail while playing as an NPC!");
			curntn->active *= 4;
		} else if(isnpc(curntn->active)) {
			errormsg("Okay.... you have now come back to reality.");
			curntn->active /= 4;
		}
		NADJNTN;
		break;
	case '8':
		if(isgod==TRUE){
			standout();
			mvaddstr(LINES-1,0,"DO YOU WANT TO DESTROY THIS NATION (y or n)");
			standend();
			refresh();

			if(getch()=='y') {
				/* save to last turns news file */
				sprintf(filename,"%s%d",newsfile,TURN-1);
				if ((fnews=fopen(filename,"a+"))==NULL) {
					printf("error opening news file\n");
					exit(FAIL);
				}
				destroy(country);
				fclose(fnews);
				sprintf(command,"%s/%s", EXEDIR, sortname);
				sprintf(command,"%s %s %s", command, filename, filename);
				system(command);
			}
		}
		break;
#ifdef OGOD
	case '9':
		if (isgod==TRUE) {
			/* open the target country's files */
			sprintf(filename,"%s%d",exefile,country);
			if ((ftmp=fopen(filename,"a"))==NULL) {
				beep();
				errormsg("error opening country's file");
				reset_god();
				return;
			}
			/* adjust commodities */
			mvaddstr(LINES-2,0,"CHANGE: 1) Gold 2) Jewels 3) Iron 4) Food ?");
			clrtoeol();
			refresh();
			switch(getch()) {
			case '1':
				mvaddstr(LINES-1,0,"WHAT IS NEW VALUE FOR TREASURY? ");
				refresh();
				curntn->tgold = (long) get_number();
				if (curntn->tgold < 0L) curntn->tgold=0L;
				fprintf(ftmp,"L_NGOLD\t%d \t%d \t%ld \t0 \t0 \t%s\n", XNAGOLD ,country,curntn->tgold,"null");
				break;
			case '2':
				mvaddstr(LINES-1,0,"WHAT IS NEW AMOUNT OF JEWELS? ");
				refresh();
				curntn->jewels = (long) get_number();
				if (curntn->jewels < 0L) curntn->jewels=0L;
				fprintf(ftmp,"L_NJWLS\t%d \t%d \t%ld \t0 \t0 \t%s\n", XNARGOLD ,country,curntn->jewels,"null");
				break;
			case '3':
				mvaddstr(LINES-1,0,"WHAT IS NEW AMOUNT OF METAL? ");
				refresh();
				curntn->metals = (long) get_number();
				if (curntn->metals < 0L) curntn->metals=0L;
				fprintf(ftmp,"L_NMETAL\t%d \t%d \t%ld \t0 \t0 \t%s\n", XNAMETAL ,country,curntn->metals,"null");
				break;
			case '4':
				mvaddstr(LINES-1,0,"WHAT IS NEW AMOUNT OF FOOD? ");
				refresh();
				curntn->tfood = (long) get_number();
				if (curntn->tfood < 0L) curntn->tfood = 0L;
				break;
			default:
				break;
			}
			fclose(ftmp);
		}
		break;
#endif /* OGOD */
	case '0':
		if(isgod==TRUE) {
			mvaddstr(LINES-2,0,"ENTER CONQUER SUPER-USER PASSWORD:");
			refresh();
			(void) get_pass(string);
			strcpy(passwd,crypt(string,SALT));
			if(strncmp(passwd,ntn[0].passwd,PASSLTH)!=0) break;
			mvaddstr(LINES-1,0,"PROMOTE WHAT USER TO DEMI-GOD? ");
			refresh();
			get_nname(string);
			if (strlen(string)!=0 && getpwnam(string)!=NULL) {
				strncpy(ntn[0].leader,string,LEADERLTH);
			}
		}
		break;
	case 'p':
	case 'P': 
		produce();
		if (isgod==TRUE) reset_god();
		return;
	case 'b':
	case 'B':
		budget();
		if(isgod==TRUE) reset_god();
		return;
	default:
		if(isgod==TRUE) reset_god();
		return;
	}
	} /* end of continuous loop */
}

void
help()
{
	int lineno;
	FILE *fp, *fopen();
	int i,xcnt,ycnt,done=FALSE;
	char line[LINELTH],fname[FILELTH];

	/*find out which helpfile to read in */
	clear_bottom(0);
	ycnt = LINES - 3;
	xcnt = 0;
	mvaddstr(LINES-4,0,"Help on which topic:");
	for (i=0;i<MAXHELP;i++) {
		sprintf(line,"  %d) %s",i,helplist[i]);
		mvaddstr(ycnt,xcnt,line);
		xcnt += 20;
		if (i==2) {
			xcnt = 0;
			ycnt ++;
		}
	}
	refresh();
	i = getch()-'0';

	/* quick exit on invalid entry */
	if (i<0 || i>MAXHELP) {
		redraw=DONE;
		makebottom();
		return;
	}

	/*open help file*/
	sprintf(fname,"%s/%s%d",DEFAULTDIR,helpfile,i);
	if ((fp=fopen(fname,"r"))==NULL) {
		mvprintw(0,0,"\nerror on read of %s\n",fname);
		refresh();
		getch();
		return;
	}

	while(done==FALSE){
		/*read in screen (until DONE statement)*/
		fgets(line,80,fp);
		if(strncmp(line,"DONE",4)==0) done=TRUE;
		else {
			clear();
			lineno=0;
			while(strncmp(line,"END",3)!=0) {
				if(lineno==0) {
					/* highlight topic line */
					for(i=0;line[i]==' ';i++) ;
					i--;		  /* back up 1 */
					standout();
					mvaddstr(lineno,i,line+i);
					/* add a blank space on the end */
					mvaddch(lineno,strlen(line)-1,' ');
					standend();
				} else mvaddstr(lineno,0,line);
				lineno++;
				if(lineno>LINES-3) strcpy(line,"END");
				else fgets(line,80,fp);
			}
			standout();
			/* help screen 80 col format; constants needed */
			mvaddstr(LINES-2,16,"HIT ANY KEY TO CONTINUE HELP SCREENS");
			mvaddstr(LINES-1,21,"TO END HELP HIT SPACE KEY");
			standend();
			refresh();
			if(getch()==' ') done=TRUE;
		}
	}
	fclose(fp);
}

/* routine to highlight a line for news display */
/* if country name is mentioned.  By T. Kivinen */
mvaddstrnahil(li,col,p)
     int li;
     int col;
     char *p;
{
	int i,j;
	move(li,col);
	for (i=0;p[i];i++) {
		for (j=0;p[i+j]==ntn[country].name[j];j++);
		if (ntn[country].name[j]==0 && p[i+j]!='-') {
			standout();
			addstr(ntn[country].name);
			standend();
			i+=j-1;
		}
		else
			addch(p[i]);
	}
}

void
newspaper()
{
	int lineno;
	FILE *fp, *fopen();
	int newpage,choice,done;
	short pagenum,subpage;
	int i,ydist,xdist;
	char line[LINELTH],name[FILELTH];
	int readold;
	int c;
	
	/* check to make sure that there are newspapers */
	if (TURN==0) {
		clear_bottom(0);
		errormsg("no news to read");
		redraw=DONE;
		makebottom();
		return;
	}

	/* set to 1 if news already read and redraw is needed */
	readold=0;
	readoldp:   /* label to jump if reading old pages */
	clear_bottom(0);
	ydist=LINES-3;
	xdist=0;

	/* check for all newspapers up until the current turn */
	for (i=TURN-1;i>=0 && i>=TURN-MAXNEWS;i--) {
		sprintf(line,"   %d) %s of Year %d",TURN-i,
			PSEASON(i), YEAR(i));
		/* align all strings */
		mvprintw(ydist,xdist,"%s",line);
		xdist += strlen(line);
		if (xdist>60) {
			xdist=0;
			ydist++;
		}
	}
	mvaddstr(LINES-4,0,"Read Which Newspaper:");
	standend();
	refresh();

	/* get the choice */
	choice = getch() - '0';
	/* make sure the choice is valid */
	if (choice<1 || choice > MAXNEWS) {
	        if (readold)
	               return;
		makebottom();
		redraw=DONE;
		return;
	}
	/* select page to read */
	pagenum=1;
	backpage:    /* label for reading previous pages. pagenum set to page */
	sprintf(name,"%s%d",newsfile,TURN-choice);
	if ((fp=fopen(name,"r"))==NULL) {
		clear_bottom(0);
		sprintf(line,"unable to open news file <%s>",name);
		errormsg(line);
		if (readold)
		  return;
		redraw=DONE;
		makebottom();
		return;
	}

	/*open and read one page */
     forpage:  /* label for reading forward pages. pagenum set to page */
	subpage=1;
	newpage=FALSE;
	line[0]='\0';
	strcpy(name,"");
	/* reading to correct page */
	i=0;
	done=FALSE;
	while(done==FALSE && i<pagenum) {
		if(fgets(name,80,fp)==NULL) done=TRUE;
		if(name[0]!='\0' && name[1]!='.' && name[1]!=':')
			i=todigit(name[0]);
	}
	if (i!=pagenum) {
		errormsg("Page not found");
		pagenum=1;
	} else {
		pagenum=i;
	}

	do {
		if(newpage==FALSE){
			clear();
			lineno=5;
			newpage=TRUE;
			standout();
			mvprintw(0,21,"CONQUER NEWS REPORT  Page %d.%d",pagenum,subpage++);
			mvprintw(1,28,"%s of Year %d",PSEASON(TURN-choice),YEAR(TURN-choice));
			mvprintw(3,37-strlen(name)/2,"%s",name+2);
			standend();
			/* display any pending non-blank lines */
			if(strcmp(line,name)!=0 && strlen(line)>2) {
				if (Gaudy) mvaddstrnahil(lineno++,0,line+2);
				else mvaddstr(lineno++,0,line+2);
			}
		} else if(fgets(line,80,fp)==NULL) done=TRUE;
		else {
			if(line[1]!='.'  && line[1]!=':') {
				strcpy(name,line);
				newpage=FALSE;
				pagenum=todigit(line[0]);
				subpage=1;
			} else {
				if(todigit(line[0])!=pagenum) {
					newpage=FALSE;
					pagenum=todigit(line[0]);
					subpage=1;
				}
				else if(lineno>LINES-4) newpage=FALSE;
				else if(strlen(line)>2) {
					if (Gaudy) mvaddstrnahil(lineno++,0,line+2);
					else mvaddstr(lineno++,0,line+2);
				}
			}
		}
		if(newpage==FALSE||done==TRUE){
			standout();
			/* constants since news is 80 col format */
			mvaddstr(LINES-2,4,"N=next page, P=previous page, 1-5=jump to page, O=Read other news");
			mvaddstr(LINES-1,12,"ANY OTHER KEY TO ADVANCE -- TO END NEWS HIT SPACE");
			standend();
			refresh();
			stayhere:
			c=getch();
			switch (c) {
			case 'N':
			case 'n':
				/* go forward a page */
				if (subpage!=1 && done!=TRUE) {
					pagenum++;
					if (pagenum>5) pagenum=5;
					goto forpage;
				} else if (done==TRUE) {
					goto stayhere;
				}
				break;
			case 'P':
			case 'p':
				/* go backward a page */
				if (subpage==1) pagenum--;
				pagenum--;
				if (pagenum<1) pagenum=1;
				fclose(fp);
				goto backpage;
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
				/* goto a specific page */
				i=todigit(c);
				if (i<=pagenum) {
					pagenum=i;
					fclose(fp);
					goto backpage;
				} else {
					pagenum=i;
					goto forpage;
				}
				break;
			case ' ':
				done=TRUE;
				break;
			case 'O':
			case 'o':
				readold=1;
				fclose(fp);
				goto readoldp;
				break;
			}
		}
	} while (done==FALSE);
	fclose(fp);
}
