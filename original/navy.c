/* conquer : Copyright (c) 1988 by Ed Barlow. */
/* this code was initially written in January, 1989 by Adam Bryant */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include "header.h"
#include "data.h"

/* increase fleet by nships of given type */
int
addwships (nvynum, shipsize, nships)
	short nvynum, shipsize, nships;
{
	short hold=nships;

	/* return FALSE if fails, TRUE otherwise */

	/* check for valid input */
	if (nvynum<0 || nvynum>=MAXNAVY ||
	shipsize<N_LIGHT || shipsize>N_HEAVY)
		return(FALSE);

	hold += P_NWAR(shipsize);
	if (hold > N_MASK) {
		return(FALSE);
	}

	/* place new value in proper place */
	hold <<= (N_BITSIZE*shipsize);

	/* set it in variable */
	P_NWSHP |= hold;

	/* form mask so other values won't be changed */
	hold |= ~(N_MASK<<(N_BITSIZE*shipsize));

	/* now change the variable */
	P_NWSHP &= hold;
	return(TRUE);
}

int
addmships (nvynum, shipsize, nships)
	short nvynum, shipsize, nships;
{
	short hold=nships;

	/* return FALSE if fails, TRUE otherwise */

	/* check for valid input */
	if (nvynum<0 || nvynum>=MAXNAVY ||
	shipsize<N_LIGHT || shipsize>N_HEAVY)
		return(FALSE);

	hold += P_NMER(shipsize);
	if (hold > N_MASK) {
		return(FALSE);
	}

	/* place new value in proper place */
	hold <<= (N_BITSIZE*shipsize);

	/* set it in variable */
	P_NMSHP |= hold;

	/* form mask so other values won't be changed */
	hold |= ~(N_MASK<<(N_BITSIZE*shipsize));

	/* now change the variable */
	P_NMSHP &= hold;
	return(TRUE);
}

int
addgships (nvynum, shipsize, nships)
	short nvynum, shipsize, nships;
{
	short hold=nships;

	/* return FALSE if fails, TRUE otherwise */

	/* check for valid input */
	if (nvynum<0 || nvynum>=MAXNAVY ||
	shipsize<N_LIGHT || shipsize>N_HEAVY)
		return(FALSE);

	hold += P_NGAL(shipsize);
	if (hold > N_MASK) {
		return(FALSE);
	}

	/* place new value in proper place */
	hold <<= (N_BITSIZE*shipsize);

	/* set it in variable */
	P_NGSHP |= hold;

	/* form mask so other values won't be changed */
	hold |= ~(N_MASK<<(N_BITSIZE*shipsize));

	/* now change the variable */
	P_NGSHP &= hold;
	return(TRUE);
}

/* remove nships of given shipsize for a given fleet */
void
subwships (nvynum, shipsize, nships)
	short nvynum, shipsize, nships;
{
	short hold;

	/* return FALSE if fails, TRUE otherwise */

	/* check for valid input */
	if (nvynum<0 || nvynum>=MAXNAVY ||
	shipsize<N_LIGHT || shipsize>N_HEAVY)
		return;

	hold = P_NWAR(shipsize) - nships;
	if (hold < 0) {
		return;
	}

	/* place new value in proper place */
	hold <<= (N_BITSIZE*shipsize);

	/* set it in variable */
	P_NWSHP |= hold;

	/* form mask so other values won't be changed */
	hold |= ~(N_MASK<<(N_BITSIZE*shipsize));

	/* now change the variable */
	P_NWSHP &= hold;
}

void
submships (nvynum, shipsize, nships)
	short nvynum, shipsize, nships;
{
	short hold;

	/* return FALSE if fails, TRUE otherwise */

	/* check for valid input */
	if (nvynum<0 || nvynum>=MAXNAVY ||
	shipsize<N_LIGHT || shipsize>N_HEAVY)
		return;

	hold = P_NMER(shipsize) - nships;
	if (hold < 0) {
		return;
	}
	/* place new value in proper place */
	hold <<= (N_BITSIZE*shipsize);

	/* set it in variable */
	P_NMSHP |= hold;

	/* form mask so other values won't be changed */
	hold |= ~(N_MASK<<(N_BITSIZE*shipsize));

	/* now change the variable */
	P_NMSHP &= hold;
}

void
subgships (nvynum, shipsize, nships)
	short nvynum, shipsize, nships;
{
	short hold;

	/* return FALSE if fails, TRUE otherwise */

	/* check for valid input */
	if (nvynum<0 || nvynum>=MAXNAVY ||
	shipsize<N_LIGHT || shipsize>N_HEAVY)
		return;

	hold = P_NGAL(shipsize) - nships;
	if (hold < 0) {
		return;
	}
	/* place new value in proper place */
	hold <<= (N_BITSIZE*shipsize);

	/* set it in variable */
	P_NGSHP |= hold;

	/* form mask so other values won't be changed */
	hold |= ~(N_MASK<<(N_BITSIZE*shipsize));

	/* now change the variable */
	P_NGSHP &= hold;
	return;
}

/* this function finds the total number of ships in a given fleet */
int
fltships(country,nvynum)
	int country, nvynum;
{
	struct s_nation *savntn=curntn;
	int i, hold=0;

	curntn= &ntn[country];
	for(i=N_LIGHT;i<=N_HEAVY;i++) {
		hold += (int) P_NWAR(i);
		hold += (int) P_NMER(i);
		hold += (int) P_NGAL(i);
	}
	curntn= savntn;
	return(hold);
}

#ifdef ADMIN
/* this function returns the speed of a given fleet */
/*  which is the speed of the slowest member.       */
unsigned short
fltspeed(nvynum)
	int nvynum;
{
	int i,hold=99;

	/* light ships faster than heavy ships */
	for (i=N_LIGHT;i<=N_HEAVY && hold>N_WSPD;i++)
		if (P_NWAR(i)>0) hold=N_WSPD+(2-i)*N_SIZESPD;

	for (i=N_LIGHT;i<=N_HEAVY && hold>N_MSPD;i++)
		if (P_NMER(i)>0) hold=N_MSPD+(2-i)*N_SIZESPD;

	for (i=N_LIGHT;i<=N_HEAVY && hold>N_GSPD;i++)
		if (P_NGAL(i)>0) hold=N_GSPD+(2-i)*N_SIZESPD;

	if (hold==99) hold=N_NOSPD;
	return(hold);
}
#endif /* ADMIN */

/* this function returns the amount of storage space in a */
/*   given fleet.  heavy+=3  medium+=2  light+=1.         */
int
flthold(nvynum)
	int nvynum;
{
	int i,hold=0;

	/* light ships faster than heavy ships */
	for (i=N_LIGHT;i<=N_HEAVY;i++)
	{
		hold += (i+1) * (int)P_NWAR(i);
		hold += (i+1) * (int)P_NMER(i);
		hold += (i+1) * (int)P_NGAL(i);
	}
	return(hold);
}

#ifdef ADMIN
/* this function returns the amount of storage space in warships */
int
fltwhold(nvynum)
	int nvynum;
{
	int i,hold=0;

	for (i=N_LIGHT;i<=N_HEAVY;i++)
	{
		hold += (i+1) * (int)P_NWAR(i);
	}
	return(hold);
}
#endif /* ADMIN */

/* this function returns the amount of storage space in galleys */
int
fltghold(nvynum)
	int nvynum;
{
	int i,hold=0;

	for (i=N_LIGHT;i<=N_HEAVY;i++)
	{
		hold += (i+1) * (int)P_NGAL(i);
	}
	return(hold);
}

/* this function returns the amount of storage space in merchants */
int
fltmhold(nvynum)
	int nvynum;
{
	int i,hold=0;

	for (i=N_LIGHT;i<=N_HEAVY;i++)
	{
		hold += (i+1) * (int)P_NMER(i);
	}
	return(hold);
}

#ifdef CONQUER
extern short country;
extern short selector, pager;
extern short xcurs, ycurs, xoffset, yoffset;
extern FILE *fexe;

/* function to ask for cargo choice */
static int
get_cargo(str)
	char *str;
{
	int choice=(-1);

	mvprintw(LINES-3,0,"%s (A)rmy or (P)eople?",str);
	clrtoeol();
	refresh();
	switch(getch()) {
	case 'a':
	case 'A':
	     choice=TRUE;
	     break;
	case 'p':
	case 'P':
	     choice=FALSE;
	     break;
	default:
	     break;
	}

	return(choice);
}

/* this function returns false if loading is invalid */
int
loadstat(status)
	int status;
{
	switch(status) {
	case TRADED:
	case GENERAL:
	case MILITIA:
	case GARRISON:
	case ONBOARD:
		return(FALSE);
		break;
	default:
		break;
	}
	return(TRUE);
}

/* this function loads a fleet with an item */
void
loadfleet()
{
	short nvynum;
	/* merchant holding unused */
	int ghold, mhold, unload, doarmy;
	int gcargo, mcargo, amount, armynum;

	clear_bottom(0);
	if((nvynum=getselunit()-MAXARM)>=0){
		if(nvynum>=MAXNAVY){
			errormsg("Invalid Navy");
			return;
		}
	} else {
		errormsg("Invalid Navy");
		return;
	}

	if(sct[XREAL][YREAL].altitude==WATER) {
		errormsg("Fleet must be landed");
		return;
	}

	/* process loading or unloading */
	ghold = fltghold(nvynum);
	mhold = fltmhold(nvynum);
	if((ghold==0)&&(mhold == 0)) {
		errormsg("No storage space available");
		return;
	} else if((P_NARMY==MAXARM)&&(P_NPEOP==0)) {
		unload=FALSE;
	} else {
		mvprintw(LINES-4,0,"Cargo:   People %d",P_NPEOP*mhold);
		if(P_NARMY==MAXARM) mvaddstr(LINES-4,25,"Army (none)");
		else mvprintw(LINES-4,25,"Army (%d)",P_NARMY);
		mvaddstr(LINES-3,0,"Do you wish to (L)oad or (U)nload?");
		refresh();
		switch(getch()) {
		case 'l':
		case 'L':
		     unload=FALSE;
		     break;
		case 'u':
		case 'U':
		     unload=TRUE;
		     break;
		default:
		     return;
		}
	}

	if(unload==TRUE) {
		if(P_NARMY==MAXARM) doarmy=FALSE;
		else if(P_NPEOP==0) doarmy=TRUE;
		else {
			doarmy=get_cargo("Unload");
		}
		if(doarmy==TRUE) {
			armynum=P_NARMY;
			if(sct[XREAL][YREAL].owner==0
			&& P_ATYPE!=A_MARINES
			&& P_ATYPE!=A_SAILOR) {
				errormsg("Only sailors or marines may disembark in unowned land");
				return;
			} else if (sct[XREAL][YREAL].owner!=country
			&& sct[XREAL][YREAL].owner!=0
			&& P_ATYPE!=A_MARINES) {
				errormsg("Only marines may disembark in someone else's land");
				return;
			}
			P_ASTAT=DEFEND;
			P_NARMY=MAXARM;
			if (!((sct[XREAL][YREAL].designation==DCITY
			|| sct[XREAL][YREAL].designation==DCAPITOL)
			&& (sct[XREAL][YREAL].owner==country
			|| (!ntn[sct[XREAL][YREAL].owner].dstatus[country]!=UNMET
			&& ntn[sct[XREAL][YREAL].owner].dstatus[country]<=NEUTRAL)))
			|| P_NMOVE < N_CITYCOST) {
				P_NMOVE=0;
			} else {
				P_NMOVE-= N_CITYCOST;
			}
			NADJMOV;
			NADJHLD;
			AADJSTAT;
		} else if(doarmy==FALSE){
			if (sct[XREAL][YREAL].owner!=country) {
				mvaddstr(LINES-3,0,"Unload in a sector you don't own? (y or n)");
				clrtoeol();
				refresh();
				if (getch()!='y') {
					return;
				}
			}
			mvaddstr(LINES-2,0,"Unload how many people?");
			refresh();
			amount=get_number();
			if(amount > mhold*P_NPEOP) {
				errormsg("There are not that many on board");
			} else if (amount > 0) {
				sct[XREAL][YREAL].people += amount;
				P_NPEOP=(unsigned char)((mhold*P_NPEOP-amount)/mhold);
				NADJHLD;
				if ((sct[XREAL][YREAL].designation!=DCITY
				&& sct[XREAL][YREAL].designation!=DCAPITOL)
				|| P_NMOVE < N_CITYCOST) {
					P_NMOVE=0;
				} else {
					P_NMOVE-= N_CITYCOST;
				}
				NADJMOV;
				SADJCIV;
			}
		}
	} else {
		clear_bottom(0);
		mcargo = mhold*(SHIPHOLD-P_NPEOP);
		if(P_NARMY==MAXARM) {
			gcargo = ghold*SHIPHOLD;
			mvprintw(LINES-4,0,"Available Space:  %d soldiers  %d people", gcargo, mcargo);
		} else {
			gcargo = 0;
			mvprintw(LINES-4,0,"Available Space:  0 soldiers  %d people", mcargo);
		}
		if(gcargo==0) doarmy=FALSE;
		else if(mcargo==0) doarmy=TRUE;
		else {
			doarmy=get_cargo("Load");
		}
		if(doarmy==TRUE) {
			mvaddstr(LINES-2,0,"Load what army?");
			refresh();
			armynum = get_number();
			if(armynum<0) {
				;
			} else if((armynum>=MAXARM)||(P_ASOLD<=0)
			||(loadstat(P_ASTAT)==FALSE)) {
				errormsg("Invalid Army");
			} else if((P_AXLOC!=XREAL)||(P_AYLOC!=YREAL)) {
				errormsg("Army not in sector");
			} else if(P_ASOLD > gcargo &&
			(P_ATYPE<MINLEADER || P_ATYPE>=MINMONSTER)) {
				errormsg("Army too large for fleet");
			} else {
				P_ASTAT=ONBOARD;
				P_AMOVE=0;
				P_NARMY=armynum;
				if (!((sct[XREAL][YREAL].designation==DCITY
				|| sct[XREAL][YREAL].designation==DCAPITOL)
				&& (sct[XREAL][YREAL].owner==country
				|| (!ntn[sct[XREAL][YREAL].owner].dstatus[country]!=UNMET
				&& ntn[sct[XREAL][YREAL].owner].dstatus[country]<=NEUTRAL)))
				|| P_NMOVE < N_CITYCOST) {
					P_NMOVE=0;
				} else {
					P_NMOVE-= N_CITYCOST;
				}
				NADJMOV;
				NADJHLD;
				AADJMOV;
				AADJSTAT;
			}
		} else if(doarmy==FALSE && mcargo!=0){
			mvaddstr(LINES-2,0,"Load how many people?");
			refresh();
			amount=get_number();
			if(sct[XREAL][YREAL].owner!=country) {
				errormsg("The people refuse to board");
			} else if(amount > mcargo) {
				errormsg("Not enough room on fleet");
			} else if(sct[XREAL][YREAL].people < amount) {
				errormsg("Not enough people in sector");
			} else if (amount > 0) {
				sct[XREAL][YREAL].people -= amount;
				P_NPEOP += (unsigned char)(amount / mhold);
				SADJCIV;
				if ((sct[XREAL][YREAL].designation!=DCITY
				&& sct[XREAL][YREAL].designation!=DCAPITOL)
				|| P_NMOVE < N_CITYCOST) {
					P_NMOVE=0;
				} else {
					P_NMOVE-= N_CITYCOST;
				}
				NADJMOV;
				NADJHLD;
			}
		} else if (mcargo==0) {
			errormsg("No more room onboard fleet");
		}
	}
}
#endif /* CONQUER */
