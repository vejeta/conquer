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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <curses.h>
#include "header.h"
#include "data.h"

extern FILE *fexe;
extern short redraw;
extern short selector;
extern short pager;
extern short xcurs,ycurs,xoffset,yoffset;
extern short hilmode;   /*highlight modes: 0=owned sectors, 1= armies, 2=none*/

extern short country;
int armornvy=AORN;	

void
mymove()
{
	int	mveleft;	/* movement remaining to army group */
	long	groupmen;	/* infantry types in current army group */
	long	othermen;	/* leader & monster strength in current group */
	int	i,j,x;
	int	total,Tarmynum,Tnation,fmove;
	int	valid=FALSE;     /* TRUE if move was a valid move */
	short	armynum;
	short	nvynum;
	int	oldxcurs,oldycurs,mvused;
	int	done=FALSE;	/*done is TRUE if done with this move*/

	armornvy=AORN;
	clear_bottom(0);

	if((armynum=getselunit())<0) {		/*get selected army or navy*/
		errormsg("Invalid Unit Selected");
		armornvy=AORN;
		return;
	}

	if(armynum>=MAXARM) {			/*navy*/
		nvynum = armynum-MAXARM;
		armynum=(-1);
		mvprintw(LINES-4,0,"NAVY %d: move %d",nvynum,P_NMOVE);
		standend();
		clrtoeol();
		if((P_NMOVE==0)) {
			errormsg("That Fleet is Not Able Move Any Farther");
			redraw=DONE;
			armornvy=AORN;
			return;
		}
		mvused=P_NMOVE;
	} else {	/*army*/
		mvprintw(LINES-4,0,"ARMY %d: ",armynum);
		clrtoeol();
		if (P_ASTAT>=NUMSTATUS) {
			groupmen = P_ASTAT - NUMSTATUS;
			P_AMOVE = curntn->arm[groupmen].smove;
		}
		if(P_AMOVE==0){
			errormsg("That Unit is Not Able to Move");
			redraw=DONE;
			armornvy=AORN;
			return;
		}

		/* check if ok to continue if part of army group */
		if(P_ASTAT>=NUMSTATUS) {
			mvprintw(LINES-4,10,"Member of Army Group %d!! Continue? ",P_ASTAT-NUMSTATUS);
			refresh();
			if( getch() == 'y' )  {
				P_ASTAT=ATTACK;
				AADJSTAT;
			} else {
				redraw=DONE;
				armornvy=AORN;
				return;
			}
		}

		/* add up infantry types in army group */
		othermen = groupmen = 0;
		if(P_ASTAT==GENERAL) {
			x=armynum;
			for(armynum=0;armynum<MAXARM;armynum++) 
			if(curntn->arm[armynum].stat==x+NUMSTATUS){
				if(P_ATYPE<MINLEADER) 
					groupmen += P_ASOLD;
				else	othermen += P_ASOLD;
			}
			armynum=x;
		} else if(P_ATYPE<MINLEADER) {
			groupmen = P_ASOLD;
		} else	othermen = P_ASOLD;
	}

	clear_bottom(3);
	standout();
	mvaddstr(LINES-2,0,"MOVEMENT SCREEN - see documentation");
	clrtoeol();
	mvaddstr(LINES-1,0,"HIT SPACE IF DONE         ");
	clrtoeol();
	standend();
	move(ycurs,xcurs*2);
	refresh();

	while(done==FALSE){
		valid=TRUE;
		if(armornvy==NAVY) mveleft=P_NMOVE;
		else mveleft=P_AMOVE;
		oldxcurs=xcurs;
		oldycurs=ycurs;
		switch(getch()) {
		case '1':
		case 'b':
			xcurs--;
			ycurs++;
			break;
		case '4':
		case 'h':
			xcurs--;
			break;
		case '2':
		case 'j':		/*move down*/
			ycurs++;
			break;
		case '8':
		case 'k':		/*move up*/
			ycurs--;
			break;
		case '6':
		case 'l':		/*move east*/
			xcurs++;
			break;
		case '3':
		case 'n':		/*move south-east*/
			ycurs++;
			xcurs++;
			break;
		case '9':
		case 'u':		/*move north-east*/
			ycurs--;
			xcurs++;
			break;
		case '7':
		case 'y':		/*move north-west*/
			ycurs--;
			xcurs--;
			break;
		case '':		/* redraw map */
			valid=FALSE;
			centermap();
			clear();
			redraw=PART;
			coffmap();
			/*see within one sector of unit*/
			if(hilmode==3) {
				for(i=XREAL-xoffset-1;i<=XREAL-xoffset+1;i++){
					for(j=YREAL-yoffset-1;j<=YREAL-yoffset+1;j++){
						highlight(i,j,hilmode);
						see(i,j);
					}
				}
				for(i=0;i<MAXARM;i++) if(curntn->arm[i].sold>0){
					standout();
					see((int)curntn->arm[i].xloc-xoffset,(int)curntn->arm[i].yloc-yoffset);
				}
					for(i=0;i<MAXNAVY;i++) if(curntn->nvy[i].merchant+curntn->nvy[i].warships+curntn->nvy[i].galleys!=0){
					standout();
					see((int)curntn->nvy[i].xloc-xoffset,(int)curntn->nvy[i].yloc-yoffset);

				}
			} else for(i=XREAL-xoffset-1;i<=XREAL-xoffset+1;i++){
				for(j=YREAL-yoffset-1;j<=YREAL-yoffset+1;j++){
					highlight(i,j,hilmode);
					see(i,j);
				}
			}
			move(ycurs,xcurs*2);
			refresh();
			break;
		case ' ':
			valid=FALSE;
			if(mveleft>0 && armornvy==ARMY && sct[XREAL][YREAL].altitude==WATER && P_ASTAT==FLIGHT)
				errormsg("Please Sir! If we stop here we'll drown");
			else done=TRUE;
			break;
		default:
			beep();
			valid=FALSE;
		}

		if (!ONMAP(XREAL,YREAL)) {
			errormsg("We refuse to go off the edge of the world");
			valid=FALSE;
			xcurs=oldxcurs;
			ycurs=oldycurs;
		}
		/*if valid move check if have enough movement points*/
		if(valid==TRUE)
		if(armornvy==ARMY) {
			if (P_ASTAT==FLIGHT) {
				fmove=flightcost(XREAL,YREAL);
				if (movecost[XREAL][YREAL]>0 && fmove>movecost[XREAL][YREAL])
					fmove=movecost[XREAL][YREAL];
				if (fmove<0 || fmove>mveleft) {
					beep();
					valid=FALSE;
					xcurs=oldxcurs;
					ycurs=oldycurs;
				} else {
					P_AMOVE-=fmove;
					if(P_AMOVE==0) done=TRUE;
				}
			}
			else if((movecost[XREAL][YREAL]<0)
			||(movecost[XREAL][YREAL]>mveleft)) {
				if(movecost[XREAL][YREAL]>mveleft)
					errormsg("Costs Too Much To Move Here!!!");
				else	errormsg("Can't Move Here");
				valid=FALSE;
				xcurs=oldxcurs;
				ycurs=oldycurs;
			} else {

				/* CANT MOVE IN NON ALLIED / NON WAR/JIHAD COUNTRIES */
				if((P_ASTAT!=SCOUT)
				&&(P_ATYPE!=A_NINJA)
				&&(P_ATYPE<MINLEADER || P_ATYPE>=MINMONSTER || P_ASTAT==GENERAL)
				&&(sct[XREAL][YREAL].owner!=0)
				&&(sct[XREAL][YREAL].owner!=country)
				&&(sct[XREAL][YREAL].people>100)
				&&(ntn[sct[XREAL][YREAL].owner].dstatus[country]>ALLIED)
				&&(curntn->dstatus[sct[XREAL][YREAL].owner]<WAR)){
					errormsg("You May Not Enter Non-Allied Land Without Declaring War.");
					valid=FALSE;
					xcurs=oldxcurs;
					ycurs=oldycurs;
				}
				else if((sct[XREAL][YREAL].owner!=country)
				&&(sct[XREAL][YREAL].owner!=0)
				&&(P_ASTAT!=SCOUT)
				&&(curntn->dstatus[sct[XREAL][YREAL].owner]==UNMET)){
					errormsg("Can't Enter Unmet Nations Land");
					valid=FALSE;
					xcurs=oldxcurs;
					ycurs=oldycurs;
				} else {
					P_AMOVE-=movecost[XREAL][YREAL];
					if(P_AMOVE==0) done=TRUE;
				}
			}
		} else if(armornvy==NAVY) {
				
			if(abs(movecost[XREAL][YREAL])>mveleft){
				errormsg("Costs Too Much To Move Here!!!");
				valid=FALSE;
				xcurs=oldxcurs;
				ycurs=oldycurs;
			} else if(movecost[XREAL][YREAL] >= 0
				  || movecost[XREAL][YREAL] == -2){
				/* LAND OF SOME TYPE */
				valid=FALSE;
				/* check for nearby water */
				for(i=XREAL-1;i<=XREAL+1;i++)
				for(j=YREAL-1;j<=YREAL+1;j++)
				if(ONMAP(i,j) && sct[i][j].altitude==WATER) valid=TRUE;

				if(valid==FALSE) {
					errormsg("There isn't a waterway over there!");
					xcurs=oldxcurs;
					ycurs=oldycurs;
				} else
				if(((sct[XREAL][YREAL].designation==DTOWN)
				||(sct[XREAL][YREAL].designation==DCAPITOL)
				||(sct[XREAL][YREAL].designation==DCITY))
				&&(sct[XREAL][YREAL].owner==country
				||(ntn[SOWN].dstatus[country]!=UNMET&&
				ntn[SOWN].dstatus[country]<=NEUTRAL))) {
					/* harbor */
					if(P_NMOVE>=3) P_NMOVE-=3;
					else {
						errormsg("You need 3 move points for that");
						valid=FALSE;
						xcurs=oldxcurs;
						ycurs=oldycurs;
					}
				} else {	/* coastland */
				standout();
				if (sct[XREAL][YREAL].owner==country) mvaddstr(LINES-3,0,"Do you wish to land?");
				else mvaddstr(LINES-3,0,"Do you wish to invade?");
				standend();
				clrtoeol();
				refresh();
				if(getch()=='y') {
					move(LINES-3,0);
					clrtoeol();
					if(P_NMOVE>=4) P_NMOVE=0;
					else {
						beep();
						valid=FALSE;
						xcurs=oldxcurs;
						ycurs=oldycurs;
						errormsg("You need 4 move points to land");
						move(ycurs,xcurs*2);
						refresh();
					}
				} else {
					move(LINES-3,0);
					clrtoeol();
					valid=FALSE;
					xcurs=oldxcurs;
					ycurs=oldycurs;
					move(ycurs,xcurs*2);
					refresh();
				}
				}
			} else if((movecost[XREAL][YREAL]!=(-1))
			&&(P_NGAL(N_LIGHT)!=0||P_NWAR(N_LIGHT)!=0||P_NMER(N_LIGHT)!=0)) {
				/* warship going into deep water */
				errormsg("Light Ships May Not Go Into Deep Water!");
				valid=FALSE;
				xcurs=oldxcurs;
				ycurs=oldycurs;
			} else {
				P_NMOVE -= abs( movecost[XREAL][YREAL] );
			}

			if(P_NMOVE==0) done=TRUE;

		} else if(armornvy==AORN){
			errormsg("ERROR - NOT ARMY OR NAVY");
			return;
		}

		/*if moved and not done*/
		if((valid==TRUE)&&(done==FALSE)){
			/*check if offmap and correct*/
			coffmap();

			/*calc enemy soldiers */
			total=0;
			if(armornvy==ARMY) {
				for(Tnation=0;Tnation<NTOTAL;Tnation++)
				if(Tnation!=country)
				for(Tarmynum=0;Tarmynum<MAXARM;Tarmynum++)
				if((ntn[Tnation].arm[Tarmynum].sold>0)
				&&(ntn[Tnation].arm[Tarmynum].xloc==XREAL)
				&&(ntn[Tnation].arm[Tarmynum].yloc==YREAL)
				&&((curntn->dstatus[Tnation]>=HOSTILE)
				  ||(ntn[Tnation].dstatus[country]>=HOSTILE))
				&&(ntn[Tnation].arm[Tarmynum].stat!=SCOUT)
				&&(ntn[Tnation].arm[Tarmynum].unittyp!=A_NINJA))
					total+=ntn[Tnation].arm[Tarmynum].sold;
			} else {
		/*naval total is number of at war WARSHIPS within one sector*/
				for(Tnation=0;Tnation<NTOTAL;Tnation++)
				if(Tnation!=country)
				for(Tarmynum=0;Tarmynum<MAXNAVY;Tarmynum++)
				if((ntn[Tnation].nvy[Tarmynum].warships!=0)
				  &&(ntn[Tnation].nvy[Tarmynum].xloc<=XREAL+1)
				  &&(ntn[Tnation].nvy[Tarmynum].xloc>=XREAL-1)
				  &&(ntn[Tnation].nvy[Tarmynum].yloc<=YREAL+1)
				  &&(ntn[Tnation].nvy[Tarmynum].yloc>=YREAL-1)
				&&(ntn[Tnation].dstatus[country]>=HOSTILE)) {
					total+=SHIPS(ntn[Tnation].nvy[Tarmynum].warships,N_LIGHT);
					total+=SHIPS(ntn[Tnation].nvy[Tarmynum].warships,N_MEDIUM);
					total+=SHIPS(ntn[Tnation].nvy[Tarmynum].warships,N_HEAVY);
				}
			}

			move(LINES-3,0);
			clrtoeol();

			/*scouts/ninja and flying units ignore zoc's */
			if((armornvy==ARMY)
			&&(P_ASTAT!=SCOUT)
			&&(P_ATYPE!=A_NINJA)
			&&((P_ATYPE<MINLEADER || P_ASTAT==GENERAL || P_ATYPE>=MINMONSTER))
			&&(P_ASTAT!=FLIGHT)){
				if(groupmen+othermen < total){
					/*stop if you have < total*/
					P_AMOVE=0;
					AADJMOV;
					errormsg("Zone Of Control - Stopping Movement!");
				} else if(total>0) {
					/* remove proportion of starting move */
					P_AMOVE-= total * curntn->maxmove * *(unitmove+(P_ATYPE%UTYPE))/(10*(groupmen+othermen));
					if( P_AMOVE>150 )
						P_AMOVE=0;
					AADJMOV;
					if( P_AMOVE==0 )
						errormsg("Zone Of Control - Stopping Movement");
					else	errormsg("Zone Of Control - Reducing Movement");
				}
				if( P_AMOVE==0 ) done=TRUE;
			} else if((armornvy==NAVY)&&(total>0)){
				/*25% stop if they have > total*/
				if((P_NWAR(N_LIGHT)+P_NWAR(N_MEDIUM)+
				P_NWAR(N_HEAVY) < total)
				&&(rand()%4==0)){
					P_NMOVE=0;
					NADJMOV;
					mvprintw(LINES-3,0,"%d Enemy Warships Sighted ",total);
					beep();
					done=TRUE;
				}
			}
		}

		if( done==FALSE ) {
			standout();
			if(armornvy==ARMY){
				mvprintw(LINES-2,0,"MOVESCREEN: move left: %d",P_AMOVE);
				if (P_ASTAT==FLIGHT) {
					fmove=flightcost(XREAL,YREAL);
					if (movecost[XREAL][YREAL]>0 && fmove>movecost[XREAL][YREAL])
						fmove=movecost[XREAL][YREAL];
					printw(" move cost is %d", fmove);
				} else printw(" move cost is %d", movecost[XREAL][YREAL]);
			} else 	mvprintw(LINES-2,0,"MOVESCREEN: move left: %d  move cost is %d",P_NMOVE,abs(movecost[XREAL][YREAL]));
			clrtoeol();
			standend();
			mvaddstr(LINES-1,0,"HIT SPACE IF DONE");
			clrtoeol();

			/*see within one sector of unit*/
			for(i=XREAL-1;i<=XREAL+1;i++)
			for(j=YREAL-1;j<=YREAL+1;j++) if(ONMAP(i,j))
				if(!canbeseen((int)i,(int)j)) {
					highlight(i-xoffset,j-yoffset,hilmode);
					see(i-xoffset,j-yoffset);
				}
		}
		makeside(TRUE);
		move(ycurs,xcurs*2);
		refresh();
	}

	/*at this point you are done with move*/
	/*move unit now to XREAL,YREAL*/
	selector=0;
	pager=0;
	mvaddstr(LINES-1,0,"DONE MOVEMENT");
	clrtoeol();
	if(armornvy==ARMY){

		P_AXLOC=XREAL;
		P_AYLOC=YREAL;
		AADJLOC;
		if (P_ASTAT==FLIGHT) {
			P_ASTAT=DEFEND;	/* landed; must stay on ground */
			AADJSTAT;
			/* check for drowning */
			if (sct[XREAL][YREAL].altitude==WATER) {
				clear_bottom(0);
				mvprintw(LINES-3,0,"Army #%d drowns in water",armynum);
				if( P_ATYPE==A_MERCENARY) {
				mvprintw(LINES-2,0,"Paying mercenary families %ld talons", *(u_encost+(P_ATYPE%UTYPE)) * P_ASOLD);
				curntn->tgold -= *(u_encost+(P_ATYPE%UTYPE)) * P_ASOLD;
				}
				mvaddstr(LINES-1,60,"HIT ANY KEY");
				refresh();
				getch();
				groupmen=0;
				P_ASOLD=0;
				AADJMEN;
			}
		}
		AADJMOV;
		/*if sector unowned take it*/
		/*first check if occupied*/
		/* if (other owner and unoccupied) or (no owner) you take*/
		if(((P_ATYPE<MINLEADER)||(P_ASTAT==GENERAL))
		&&(P_ASOLD>0)){
			/*calc enemy soldiers */
			total=0;
			for(Tnation=0;Tnation<NTOTAL;Tnation++)
			if(Tnation!=country)
			for(Tarmynum=0;Tarmynum<MAXARM;Tarmynum++)
			if((ntn[Tnation].arm[Tarmynum].sold>0)
			&&(ntn[Tnation].arm[Tarmynum].xloc==XREAL)
			&&(ntn[Tnation].arm[Tarmynum].yloc==YREAL)
			&&((curntn->dstatus[Tnation]>=HOSTILE)
			  ||(ntn[Tnation].dstatus[country]>=HOSTILE))
			&&(ntn[Tnation].arm[Tarmynum].stat!=SCOUT)
			&&(ntn[Tnation].arm[Tarmynum].unittyp!=A_NINJA))
				total+=ntn[Tnation].arm[Tarmynum].sold;
			if((groupmen>=TAKESECTOR)&&(SOWN==0 )){
				mvaddstr(LINES-2,0,"Taking Unowned Sector");
				clrtoeol();
				refresh();
				sleep(2);
				SOWN=country;
				curntn->popularity++;
				SADJOWN;
				P_AMOVE=0;
				AADJMOV;
			} else if((sct[P_AXLOC][P_AYLOC].owner!=country)
			&&((sct[P_AXLOC][P_AYLOC].designation==DTOWN)
			  ||(sct[P_AXLOC][P_AYLOC].designation==DCAPITOL)
			  ||(sct[P_AXLOC][P_AYLOC].designation==DCITY))){
				mvaddstr(LINES-2,0,"Entering Town/City sector");
				clrtoeol();
				refresh();
				sleep(2);
			} else if((SOWN!=country)
			&&(groupmen>=TAKESECTOR)
			&&((occ[XREAL][YREAL]==0)
				||(occ[XREAL][YREAL]==country)
				||(groupmen+othermen > 7*total))
			&&(curntn->dstatus[SOWN]>HOSTILE)
			&&(P_ASTAT>=DEFEND)) {	/* atk, def, and group */
				/*people flee if not of same race*/
				if((sct[XREAL][YREAL].people>0)
				&&(ntn[sct[XREAL][YREAL].owner].race!=curntn->race))
				if(magic(country,SLAVER)==TRUE) {
					flee(XREAL,YREAL,0,TRUE);
				} else{
					flee(XREAL,YREAL,0,FALSE);
				}
				mvaddstr(LINES-2,0,"TAKING SECTOR");
				clrtoeol();
				refresh();
				sleep(2);
				SOWN=country;
				curntn->popularity++;
				SADJOWN;
				P_AMOVE=0;
				AADJMOV;
			} else if(( sct[XREAL][YREAL].owner!=country )
			&&(total>0)
			&&(groupmen<TAKESECTOR)&&(P_ASTAT!=SCOUT)
			&&((P_ATYPE<MINLEADER)||(P_ASTAT==GENERAL))){
				clear_bottom(0);
				mvprintw(LINES-3,0,"army has too few men (%d) to take sector (need %d) - hit any key",groupmen,TAKESECTOR);
				refresh();
				getch();
			}
			clrtoeol();
			refresh();
		} 

		/*set move for parts of group*/
		if((P_ASOLD>=0)&&(P_ASTAT==GENERAL)) {
			x=armynum;
			for(armynum=0;armynum<MAXARM;armynum++) 
			if(curntn->arm[armynum].stat==x+NUMSTATUS){
				P_AXLOC=XREAL;
				P_AYLOC=YREAL;
				AADJLOC;
				P_AMOVE=curntn->arm[x].smove;
				AADJMOV;
			}
			armynum=x;
		}
	} else if(armornvy==AORN){
		errormsg("Error in move.c");
		return;
	} else {
		/*else navy*/
		mvaddstr(LINES-1,0,"NAVY DONE: ");
		clrtoeol();
		P_NXLOC=XREAL;
		P_NYLOC=YREAL;
		NADJLOC;
		NADJMOV;
		armynum=P_NARMY;
		/* move army but do not take land -- still in ship */
		if((armynum>=0)&&(armynum<MAXARM)) {
			P_AXLOC=XREAL;
			P_AYLOC=YREAL;
			AADJLOC;
			P_AMOVE=0;
			AADJMOV;
			mvprintw(LINES-1,20,"Army (%d) transported",armynum);
		}
		/*calculate civilian survival*/
		mvused-=P_NMOVE;
		if (mvused > LONGTRIP) mvused=LONGTRIP;
		if (magic(country,SAILOR)==TRUE) mvused/=2;
		if (mvused!=0) mvused= (rand()%mvused);
		P_NPEOP = (unsigned char) (P_NPEOP*(LONGTRIP-mvused)/LONGTRIP);
		mvaddstr(LINES-1,60,"HIT ANY KEY");
		refresh();
		getch();
	}
	redraw=DONE;
	prep(country,FALSE);
	whatcansee();
	move(0,0);
	clrtobot();
	makemap();
	armornvy=AORN;
	pager=0;
	selector=0;
}

/************************************************************************/
/*	GETSELUNIT()	returns id of selected unit (army or navy)	*/
/*	if navy, number is MAXARM+nvynum.  set armornvy			*/
/*	current selected unit is selector/2+4*pager			*/
/************************************************************************/
int
getselunit()
{
	int	selunit=(-1);
	short	armynum=0, nvynum=0;
	int	count=0; 
	for(armynum=0;armynum<MAXARM;armynum++){
		if((P_ASOLD>0)&&(P_AXLOC==XREAL)&&(P_AYLOC==YREAL)) {
			if((SCRARM*pager)+(selector/2)==count) selunit=armynum;
			count++;
		}
	}

	if(selunit==(-1)){
	for(nvynum=0;nvynum<MAXNAVY;nvynum++)
		if(((P_NWSHP!=0)||(P_NMSHP!=0)||(P_NGSHP!=0))
		&&(P_NXLOC==XREAL)&&(P_NYLOC==YREAL)) {
			if((SCRARM*pager)+(selector/2)==count)
				selunit=MAXARM+nvynum;
			count++;
		}
	}
	if(selunit>=0){
		if(selunit>=MAXARM) armornvy=NAVY;
		else armornvy=ARMY;
	}
	return(selunit);
}
