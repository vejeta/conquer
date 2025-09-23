/*
* additions made in tut.fi (winter 1987 and spring 1988)
* for more information ask from mta@tut.fi or esc@tut.fi
*/

/* THESE COMMANDS are neat and I have used them throughout my code. 
			- The Ed */

/* ext_cmd()			run extended commands	*/
/* clear_bottom(i)		clear bottom min(i,4) lines of the map */
/* errormsg(str)		print error message to bottom line */
/* splitarmy(army)		split ? men from army */
/* reducearmy(army,men)		subtract men from army */
/* change_status(army,stat)	change army stat to integer status */
/* combinearmies(army1,army2)	add army2 to army1 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <curses.h>
#include "header.h"
#include "data.h"

extern FILE *fexe;
extern short selector;
extern short country;
extern short xcurs, ycurs, xoffset, yoffset;

void
ext_cmd(armie)
int	armie;
{
	int stat=0,armynum,army2,men;

	clear_bottom(0);

	if( armie == -1 ) armynum = getselunit();
	else armynum=armie;

	mvaddstr(LINES-2, 0,
	" (+) combine, (-) split n men, (/) divide by 2, (G)roup");
	mvaddstr(LINES-3, 0,
	" (a)ttack, (d)efend, (s)cout, (m)arch, (g)arrison");
	if(fort_val(&sct[P_AXLOC][P_AYLOC]) > 0) {
		if (sct[P_AXLOC][P_AYLOC].owner==country) addstr(", (S)ortie");
		else addstr(", (S)iege");
		if (P_ATYPE>=MINLEADER && P_ATYPE<MINMONSTER &&
		ISCITY(sct[P_AXLOC][P_AYLOC].designation)) addstr(", (R)ule");
	}
	mvaddstr(LINES-4, 0, "Extended command:");
	refresh();

	switch(getch()) {
	case '-':		/* split army */
		splitarmy(armynum);
		break;
	case '/':		/* divide army */
		men = P_ASOLD/2;
		reducearmy(armynum,men);
		break;
	case '+':		/* combine armies */
		selector +=2;
		army2 = getselunit();
		selector -=2;
		combinearmies(armynum,army2);
		break;
	case 'G':
		addgroup(armynum);
		break;
	case 'a': if(stat==0) stat=ATTACK;
	case 'd': if(stat==0) stat=DEFEND;
	case 'm': if(stat==0) stat=MARCH;
	case 's': if(stat==0) stat=SCOUT;
	case 'g': if(stat==0) stat=GARRISON;
	case 'R': if(stat==0) stat=RULE;
	case 'S':
		if(stat==0) {
			if(sct[P_AXLOC][P_AYLOC].owner==country) stat=SORTIE;
			else stat=SIEGE;
		}
		change_status(armynum,stat);
		break;
	default:		/* unimplemented? */
		errormsg("Unimplemented extended command");
	}
}

/* returns TRUE if uncombinable FALSE if combinable */
int
nocomb_stat(astat)
	unsigned char astat;
{
	int hold;

	switch(astat) {
#ifdef TRADE
	case TRADED:
#endif /* TRADE */
	case FLIGHT:
	case MAGATT:
	case MAGDEF:
	case SCOUT:
	case ONBOARD:
		hold = TRUE;
		break;
	default:
		hold = FALSE;
		break;
	}
	return(hold);
}

void
combinearmies(armynum,army2)
int armynum, army2;
{
	int nocomb_stat();

	if (armynum < 0 || armynum >= MAXARM
	    || army2 < 0 || army2 >= MAXARM || P_ASOLD == 0
	    || curntn->arm[army2].sold == 0) {
		errormsg("Selected unit doesn't exist");
		return;
	}
	if (P_ASTAT != curntn->arm[army2].stat
	    || P_ATYPE != curntn->arm[army2].unittyp
	    || P_ATYPE >= MINLEADER
	    || P_ASTAT == ONBOARD || P_ASTAT == TRADED) {
	  if ((nocomb_stat(P_ASTAT) == TRUE) ||
		(nocomb_stat(curntn->arm[army2].stat) == TRUE) ||
	        curntn->arm[army2].stat == MARCH ||
		curntn->arm[army2].stat == SIEGE ||    /* may not jump out  */
		curntn->arm[army2].stat == SORTIE ||   /* of these statuses */
		P_ATYPE >= MINLEADER ||
		P_ATYPE!=curntn->arm[army2].unittyp) {
			errormsg("Selected armies not legal");
			return;
		}
	}
	if((curntn->arm[army2].xloc!=P_AXLOC)
	||(curntn->arm[army2].yloc!=P_AYLOC)) {
		errormsg("Selected armies not together");
		return;
	}
	P_ASOLD += curntn->arm[army2].sold;
	curntn->arm[army2].sold = 0;
	P_AMOVE=min(P_AMOVE,curntn->arm[army2].smove);
	AADJMEN;
	AADJMOV;
	if (P_ASTAT>NUMSTATUS) {
		curntn->arm[P_ASTAT-NUMSTATUS].smove=P_AMOVE;
		armynum = P_ASTAT-NUMSTATUS;
		AADJMOV;
	}
	armynum = army2;
	AADJMEN;
}

void
change_status(armynum,new_stat)
int armynum,new_stat;
{
	int	i;
	if(armynum<0 || armynum >= MAXARM || P_ASTAT==SCOUT ||
		P_ASTAT==TRADED || P_ASTAT==ONBOARD || P_ASTAT==SORTIE){
		errormsg("Can't change status on that army");
		return;
	}
	if( P_ATYPE == A_MILITIA && new_stat!=MILITIA ) {
		errormsg("Militia is all you get");
		return;
	}
	if( P_ATYPE == A_ZOMBIE && new_stat==MARCH ) {
		errormsg("Duh?  Your Zombies just lumber about.");
		return;
	}
	if(P_ASTAT == SIEGED && new_stat != SORTIE && new_stat != RULE) {
		errormsg("Sieged troops may only switch to sortie");
		return;
	}
	if(P_ASTAT == new_stat) {
		errormsg("Uh, sir.  Aren't we doing that already?");
		return;
	}
	if(new_stat == SCOUT) {
		if(( P_ATYPE != A_SCOUT )&&( P_ASOLD >= 25)) {
			errormsg("Need less than 25 men to scout");
			return;
		} else {
			P_ATYPE=A_SCOUT;
			AADJMEN;
		}
	}
	if(P_ASTAT == GENERAL ){
		i=armynum;
		for(armynum=0;armynum<MAXARM;armynum++){
			if(( P_ASOLD>0) && (P_ASTAT==i+NUMSTATUS)){
				P_ASTAT = ATTACK;
				AADJSTAT;
			}
		}
		armynum=i;
	}
	if(new_stat == GARRISON) {
		if((sct[P_AXLOC][P_AYLOC].owner!=country)
		||(fort_val(&sct[P_AXLOC][P_AYLOC])==0)) {
			errormsg("Just how do you want to garrison that?");
			return;
		}
	} else if(new_stat == SORTIE) {
		if((sct[P_AXLOC][P_AYLOC].owner!=country)
		||(fort_val(&sct[P_AXLOC][P_AYLOC])==0)) {
			errormsg("Hmmm... How do you figure to sortie from there?");
			return;
		}
	} else if(new_stat == SIEGE) {
		if((sct[P_AXLOC][P_AYLOC].owner==country)
		||(fort_val(&sct[P_AXLOC][P_AYLOC])==0)) {
			errormsg("You want to lay seige to this??");
			return;
		}
	} else if(new_stat == RULE) {
		if((sct[P_AXLOC][P_AYLOC].owner!=country)
		||(!ISCITY(sct[P_AXLOC][P_AYLOC].designation))
		||(P_ATYPE<MINLEADER)
		||(P_ATYPE>=MINMONSTER)) {
			errormsg("Sorry, but you can't rule in that sector");
			return;
		}
	}

	/* eliminate half starting movement if start out on march */
	if( P_ASTAT==MARCH && new_stat!=MARCH ) {
		if(P_AMOVE<(curntn->maxmove * *(unitmove+(P_ATYPE%UTYPE)))/50){
			errormsg("That troop has gone too far to stop marching");
			return;
		}
		P_AMOVE-=(curntn->maxmove * *(unitmove+(P_ATYPE%UTYPE)))/50;
		AADJMOV;
	}

	/* location dependent statuses make armies stay in place */
	if((new_stat==RULE)||(new_stat==SORTIE)
	||(new_stat==SIEGE)||(new_stat==GARRISON)) {
		P_AMOVE=0;
		AADJMOV;
	}
	P_ASTAT = new_stat;
	AADJSTAT;
	if( P_AMOVE != 0 ) P_AMOVE--;
	AADJMOV;
}

void
reducearmy(armynum,men)
int armynum,men;
{
	int army2;
	int oldx, oldy, army;

	if(P_ATYPE>=MINMONSTER){
		errormsg("sorry -- army is monster");
		return;
	}
	if(P_ATYPE>=MINLEADER){
		errormsg("sorry -- army is leader");
		return;
	}
#ifdef TRADE
	if(men<25 || armynum < 0 || armynum >= MAXARM || P_ASOLD < men+25 || P_ASTAT==ONBOARD || P_ASTAT==TRADED) {
#else
	if(men<25 || armynum < 0 || armynum >= MAXARM || P_ASOLD < men+25 || P_ASTAT==ONBOARD) {
#endif /* TRADE */
		errormsg("Selected army too small or illegal");
		return;
	}

	if(P_ASOLD < men) {
		errormsg("Too many men to split from army");
		return;
	}

	oldx=(int)P_AXLOC;
	oldy=(int)P_AYLOC;
	army2=armynum;

	armynum=(-1);
	for(army=0;army<MAXARM;army++)
		if(curntn->arm[army].sold==0){
			if(armynum==(-1)) armynum=army;
		}
	/*overflow*/
	if(armynum==(-1)) {
		errormsg("TOO MANY ARMIES");
		return;
	} else {
		P_AMOVE=curntn->arm[army2].smove;
		P_ATYPE=curntn->arm[army2].unittyp;
		P_ASTAT=curntn->arm[army2].stat;
		P_AXLOC=oldx;
		P_AYLOC=oldy;
		P_ASOLD=men;
		curntn->arm[army2].sold-=P_ASOLD;
		AADJSTAT;
		AADJMEN;
		AADJLOC;
		AADJMOV;
		armynum=army2;
		AADJMEN;
	}
}

void
splitarmy(armynum)
int armynum;
{
	int men;
	clear_bottom(2);
	mvaddstr(LINES-2, 0, "How many men to split? ");
	refresh();
	men = get_number();
	if(men <= 0) {
		return;
	}
	reducearmy(armynum,men);
}

void
errormsg(str)
char *str;
{
	mvaddstr(LINES-1, 0, str);
	clrtoeol();
	mvaddstr(LINES-1, COLS-16, "PRESS ANY KEY");
	beep();
	refresh();
	getch();
}

void
clear_bottom(i)
int i;
{
	if(i==0) i=4;
	for (; i>0 ; i--) {
		move( LINES-i, 0);
		clrtoeol();
	}
}

/* add army to a group */
void
addgroup(armynum)
int	armynum;
{
	int	moverate,group;

	if((P_ATYPE>=MINLEADER)&&(P_ATYPE<MINMONSTER)){
		errormsg("Can't add leader to group");
		return;
	}
	if(P_ATYPE==A_ZOMBIE) {
		errormsg("Your Zombies just don't seem inspired");
		return;
	}
	if(P_ASTAT==MARCH) {
		errormsg("Your troops must stop marching before they can be lead");
		return;
	}
	if(armynum<0 || armynum >= MAXARM || P_ASTAT==SCOUT ||
		P_ASTAT==TRADED || P_ASTAT==ONBOARD || P_ATYPE==A_MILITIA ){
		errormsg("Selected army not groupable");
		return;
	}
	mvaddstr(LINES-4,0,"enter a valid leader unit/army group id: ");
	refresh();
	group = get_number();
	if(group < 0) {
		return;
	}
	if(group > MAXARM || curntn->arm[group].sold <=0){
		errormsg("invalid unit number");
		return;
	}
	if((P_AXLOC != curntn->arm[group].xloc)
	||(P_AYLOC != curntn->arm[group].yloc)){
		errormsg("units are not in same sector");
		return;
	}
	if((curntn->arm[group].unittyp <MINLEADER)
	||( curntn->arm[group].unittyp >=MINMONSTER)){
		errormsg("Group leader not a leader unit");
		return;
	} else if(curntn->arm[group].stat==ONBOARD){
		errormsg("Cannot group onto a transport");
		return;
	}

	P_ASTAT=group+NUMSTATUS;
	moverate = P_AMOVE;
	AADJSTAT;
	armynum = group;
	P_ASTAT=GENERAL;
	AADJSTAT;
	if( P_AMOVE > moverate ){
		P_AMOVE = moverate;
		AADJMOV;
	}
}
