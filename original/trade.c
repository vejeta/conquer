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

/*
 *  The following file "trade.c" was written by Adam Bryant who
 *  gives all rights to this code to Ed Barlow provided that this
 *  message remains intact.
 */

/* thanks adam -- Ed */

/*	trade.c		*/

/*include files*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include "header.h"
#include "data.h"

#ifdef TRADE		

/* possible commodities */
#define TDGOLD   0
#define TDFOOD   1
#define TDMETAL  2
#define TDJEWL   3
#define TDLAND   4
#define TDARMY   5
#define TDSHIP   6

/* constants and indicators */
#define	NUMPRODUCTS	7
#define	MAXITM	30
#define	SELL	0
#define	BUY	1
#define	NODEAL	2
#define	NOSALE	3
#define TRADECOST(cost)	(100-cost)/100	/* twenty percent cost is normal*/

extern short country;

char *commodities[NUMPRODUCTS] = { "Gold", "Food", "Metal", "Jewels",
		"Land", "Soldiers", "Ships"};
#ifdef ADMIN
char *tradefail[NUMPRODUCTS] = { "lack of gold", "lack of food",
	"lack of metal", "lack of jewels", "land not owned",
	"unavailable or destroyed armies",
	"unavailable or destoryed navies"};
#endif /* ADMIN */
#ifdef CONQUER
/* Use this when you wish to bid something */
char *buylist[NUMPRODUCTS] = { "Bid how much gold? ", "Bid how much food? ",
	"Bid how much metal? ", "Bid how many jewels? ", "What X location? ",
	"Bid what army? ", "Bid what navy? "};

void
trade()
{
	FILE *tfile;
	int count, done=FALSE, notopen=FALSE;
	int buysell, holdint, holdint2, extint, inloop;
	int type1[MAXITM], type2[MAXITM], deal[MAXITM], extra[MAXITM];
	int natn[MAXITM], itemnum, getland(), gettrade(), checkland();
	int tradable();
	long lvar1[MAXITM], lvar2[MAXITM], holdlong, holdlong2, armyvalue();
	void tradeerr(), setaside(), takeback();
	
	clear();
	while (done==FALSE) {
		itemnum=0;
		done=TRUE;
		/* open trading file */
		if ((tfile=fopen(tradefile,"r")) == NULL ) {
			notopen=TRUE;
		}
		/* read in all of the data */
		while (notopen==FALSE && !feof(tfile)) {
			holdint = fscanf(tfile,"%d %d %d %d %ld %ld %d\n",
				&deal[itemnum],&natn[itemnum],&type1[itemnum],
				&type2[itemnum],&lvar1[itemnum],
				&lvar2[itemnum],&extra[itemnum]);
			if (holdint == 7) {
				if (deal[itemnum]==NOSALE) {
					/* remove item from sales list */
					deal[type1[itemnum]]=NOSALE;
				} else if (deal[itemnum]==SELL) {
					itemnum++;
				}
			}
		}
		if (notopen==FALSE) fclose(tfile);
		move(0,0);
		clrtobot();
		/* display header */
		standout();
		mvaddstr(0,27,"COMMODITIES EXCHANGE");
		mvaddstr(1,20,"No Guarantees... All Trades Final");
		standend();
		count=3;
		mvaddstr(count,0,"    Nation                       Item            Price");
		/* give some minor items for purchase */
		mvprintw(++count,0,"%2d) %-20s",GETFOOD,"Merchants Guild");
		mvprintw(count,30,"%9ld %s",GODFOOD,commodities[TDFOOD]);
		mvprintw(count,50,"%9ld %s",GODPRICE,commodities[TDGOLD]);
		mvprintw(++count,0,"%2d) %-20s",GETMETAL,"Merchants Guild");
		mvprintw(count,30,"%9ld %s",GODMETAL,commodities[TDMETAL]);
		mvprintw(count,50,"%9ld %s",GODPRICE,commodities[TDGOLD]);
		mvprintw(++count,0,"%2d) %-20s",GETJEWL,"Merchants Guild");
		mvprintw(count,30,"%9ld %s",GODJEWL,commodities[TDJEWL]);
		mvprintw(count,50,"%9ld %s",GODPRICE,commodities[TDGOLD]);

		/* go through list of commodities */
		for (holdint=0;holdint<itemnum;holdint++) {
			if ((deal[holdint]==SELL) && ((country==0) ||
			(ntn[natn[holdint]].dstatus[country] != UNMET)
			|| (natn[holdint] == country))) {
				count++;
				mvprintw(count,0,"%2d) %-20s",
					holdint+1,
					ntn[natn[holdint]].name);

				if (type1[holdint]==TDLAND) {
					holdlong = (long) tofood( &sct[(int)lvar1[holdint]][extra[holdint]],0);
					mvprintw(count,30,"(food=%2ld) %s",
						holdlong,
						commodities[type1[holdint]]);
				} else {
					holdlong = lvar1[holdint];
					mvprintw(count,30,"%9ld %s",
						holdlong,
						commodities[type1[holdint]]);
				}
				if (type2[holdint]==TDLAND) {
					mvprintw(count,50,"(food=%2ld) %s",
						lvar2[holdint],
						commodities[type2[holdint]]);
				} else {
				mvprintw(count,50,"%9ld %s",
					lvar2[holdint],
					commodities[type2[holdint]]);
				}
				if (count>LINES-8) {
					standout();
					mvaddstr(LINES-2,30,"Hit Any Key to Continue");
					standend();
					refresh();
					getch();
					clear();
					standout();
					mvaddstr(0,27,"COMMODITIES EXCHANGE");
					standend();
			    		mvaddstr(2,0,"    Nation                       Item            Minimum Price");
					count=2;
				}
			}
		}
		standout();
		count++;
		count++;
		mvaddstr(count++,0,"Do you wish to (B)uy, (S)ell, or (U)nsell?");
		standend();
		refresh();
		inloop=TRUE;
		while (inloop==TRUE) switch(getch()) {
		case 'b':
		case 'B':
			buysell=BUY;
			mvaddstr(count++,0,"What item number do you want to purchase? ");
			refresh();
			holdint = get_number();
			if (holdint<0) break;
			/* check for minor sales */
			if (holdint==GETFOOD || holdint==GETMETAL || holdint==GETJEWL) {
				/* strange flow but less control needed */
				mvaddstr(count++,0,"Spend how much gold? ");
				refresh();
				holdlong = get_number();
				curntn->tgold-=holdlong;
				if (curntn->tgold<0L) {
					curntn->tgold+=holdlong;
					tradeerr("You do not have enough gold");
				}
				else switch (holdint) {
				case GETFOOD:
					curntn->tfood+= (long)(GODFOOD * ((double)holdlong / GODPRICE));
					if ( (tfile = fopen(tradefile,"a+"))==NULL) {
						tradeerr("Error opening file for trading");
						abrt();
					}
					fprintf(tfile, "%d %d %d %d %ld %ld %d\n",BUY, country, GETFOOD, 0, curntn->tfood, 0, 0);
					fclose(tfile);
					break;
				case GETMETAL:
					curntn->metals+=(long)(GODMETAL * ((double)holdlong / GODPRICE));
					break;
				case GETJEWL:
					curntn->jewels+=(long)(GODMETAL * ((double)holdlong / GODPRICE));
					break;
				}
				inloop=FALSE;
				done=FALSE;
				break;
			} else if (holdint<1 || holdint>itemnum
				|| ntn[natn[holdint - 1]].dstatus[country]==UNMET) {
				tradeerr("Invalid Item Number");
				return;
			}
			holdint--;
			if (deal[holdint]!=SELL) {
				tradeerr("Sorry, that item is not on the market.");
				return;
			}
			if (ntn[natn[holdint]].dstatus[country]==UNMET) {
				tradeerr("That nation has not been met by you");
				return;
			}
			if (ntn[natn[holdint]].dstatus[country]>HOSTILE) {
				tradeerr("That nation is not doing business with you");
				return;
			}
			/* obtain bid */
			mvprintw(count++,0,"%s",buylist[type2[holdint]]);
			refresh();
			holdlong2 = 0L;
			holdlong = (long) get_number();
			if (holdlong< 0L) break;
			/* check for valid bid */
			switch(type2[holdint]) {
			case TDGOLD:
				if (holdlong < lvar2[holdint]) {
					tradeerr("You underbid the minimum.");
					buysell=NODEAL;
				} else if (holdlong > curntn->tgold) {
					tradeerr("Not Enough Gold");
					buysell=NODEAL;
				}
				break;
			case TDFOOD:
				if (holdlong < lvar2[holdint]) {
					tradeerr("You underbid the minimum.");
					buysell=NODEAL;
				} else if (holdlong > curntn->tfood) {
					tradeerr("Not Enough Food");
					buysell=NODEAL;
				}
				break;
			case TDMETAL:
				if (holdlong < lvar2[holdint]) {
					tradeerr("You underbid the minimum.");
					buysell=NODEAL;
				} else if (holdlong > curntn->metals) {
					tradeerr("Not Enough Iron");
					buysell=NODEAL;
				}
				break;
			case TDJEWL:
				if (holdlong < lvar2[holdint]) {
					tradeerr("You underbid the minimum.");
					buysell=NODEAL;
				} else if (holdlong > curntn->jewels) {
					tradeerr("Not Enough Jewels");
					buysell=NODEAL;
				}
				break;
			case TDLAND:
				mvaddstr(count++,0,"What Y position? ");
				refresh();
				holdlong2 = (long) get_number();
				if (holdlong2 < 0L) break;
				if (checkland(BUY,(int)(holdlong),(int)(holdlong2))==NODEAL) {
					buysell=NODEAL;
				} else if (tofood( &sct[(int)holdlong][(int)holdlong2],natn[holdint]) < lvar2[holdint]) {
					tradeerr("You underbid the minimum");
					buysell=NODEAL;
				}
				break;
			case TDARMY:
				if ((int)holdlong > MAXARM) {
					tradeerr("Invalid Unit");
					buysell=NODEAL;
				} else if (tradable(country,(int)holdlong)==FALSE) {
					tradeerr("That unit type is non-tradable.");
					buysell=NODEAL;
				} else if (armyvalue(country,(int)holdlong) < lvar2[holdint]) {
					tradeerr("You underbid the minimum.");
					buysell=NODEAL;
				}
				holdlong2=holdlong;
				break;
			case TDSHIP:
				if ((int)holdlong >= MAXNAVY) {
					tradeerr("Invalid Navy");
					buysell=NODEAL;
				} else if (flthold((int)holdlong) < (int)lvar2[holdint]) {
					tradeerr("You underbid the minimum.");
					buysell=NODEAL;
				} else if((curntn->nvy[(int)holdlong].armynum!=MAXARM)||(curntn->nvy[(int)holdlong].people!=0)) {
					tradeerr("Navy must be unloaded first.");
					buysell=NODEAL;
				}
				holdlong2=holdlong;
				break;
			default:
				tradeerr("Invalid Commodity");
				buysell=NODEAL;
				break;
			}
			if (buysell==BUY) {
				if ( (tfile = fopen(tradefile,"a+"))==NULL) {
					tradeerr("Error opening file for trading");
					abrt();
				}
				setaside(country,type2[holdint],holdlong,(int)holdlong,FALSE);
				fprintf(tfile, "%d %d %d %d %ld %ld %d\n",BUY, country, holdint, 0, holdlong, holdlong2, 0);
				fclose(tfile);
			}
			return;
		case 's':
		case 'S':
			/* sell an item */
			/* only allow MAXITM on market */
			if (itemnum>=MAXITM) {
				standout();
				if (itemnum==0) mvaddstr(count++,0,"Market Congested.  Hit any key to continue");
				else mvaddstr(count++,0,"Market Congested.  (B)uy or any key to continue");
				standend();
				refresh();
				break;
			}
			buysell=SELL;
			holdint = gettrade("Selling",&count);
			if (holdint==(-1)) {
				tradeerr("Invalid Option");
				return;
			}

			mvprintw(count++,0,"%s",buylist[holdint]);
			refresh();
			/* find out how much commodities */
			holdlong = (long) get_number();
			if (holdlong < 0) return;
			extint = 0;
			if (holdint< TDLAND && holdlong==0L)
				return;

			/* check for valid items */
			switch(holdint) {
			case TDGOLD:
				if (holdlong > curntn->tgold) {
					tradeerr("Not Enough Gold");
					buysell=NODEAL;
				}
				break;
			case TDFOOD:
				if (holdlong > curntn->tfood) {
					tradeerr("Not Enough Food");
					buysell=NODEAL;
				}
				break;
			case TDMETAL:
				if (holdlong > curntn->metals) {
					tradeerr("Not Enough Iron");
					buysell=NODEAL;
				}
				break;
			case TDJEWL:
				if (holdlong > curntn->jewels) {
					tradeerr("Not Enough Jewels");
					buysell=NODEAL;
				}
				break;
			case TDLAND:
				mvaddstr(count++,0,"What Y position? ");
				refresh();
				extint = get_number();
				if (extint < 0) {
					buysell=NODEAL;
					break;
				}
				buysell = checkland(SELL,(int)holdlong,extint);
				break;
			case TDARMY:
				if (holdlong>=MAXARM || curntn->arm[(int)holdlong].sold <= 0) {
					tradeerr("Invalid Army");
					buysell=NODEAL;
				} else if (tradable(country,(int)holdlong)==FALSE) {
					tradeerr("That unit is non-tradable.");
					buysell=NODEAL;
				}
				break;
			case TDSHIP:
				if (holdlong>=MAXNAVY || flthold((int)holdlong) <= 0) {
					tradeerr("Invalid Navy");
					buysell=NODEAL;
				} else if((curntn->nvy[(int)holdlong].armynum!=MAXARM)||(curntn->nvy[(int)holdlong].people!=0)) {
					tradeerr("Navy must be unloaded first.");
					buysell=NODEAL;
				}
				break;
			default:
				tradeerr("Invalid Commodity");
				buysell=NODEAL;
				break;
			}
			/* invalid commodity */
			if (buysell==NODEAL) return;

			/* find out what they want in trade */
			holdint2 = gettrade("In Trade For",&count);
			if (holdint2==(-1)) {
				tradeerr("Invalid Option");
				return;
			}
			if (holdint2==TDLAND) {
				holdlong2 = (long) getland(&count);
				if (holdlong2==(-1L)) {
					tradeerr("Invalid Vegetation");
					return;
				}
			} else {
				/* find out for what value */
				mvprintw(count++,0,"Minimum Amount of %s? "
					,commodities[holdint2]);
				refresh();
				holdlong2 = (long) get_number();
				if (holdlong2 <= 0L) return;
			}

			/* make sure what was bid is unusable */
			setaside(country,holdint,holdlong,(int)holdlong,FALSE);

			/* set up output properly */
			if (holdint==TDARMY) {
				extint = (int) holdlong;
				holdlong = armyvalue(country,(int)holdlong);
			}
			else if (holdint==TDSHIP) {
				extint = (int)holdlong;
				holdlong = (long)flthold(extint);
			}

			/* send it out */
			if ( (tfile = fopen(tradefile,"a+"))==NULL) {
				tradeerr("Error opening file for trading");
				abrt();
			}
			fprintf(tfile, "%d %d %d %d %ld %ld %d\n", SELL, country, holdint, holdint2, holdlong, holdlong2, extint);
			fclose(tfile);
			inloop=FALSE;
			done=FALSE;
			break;
		case 'u':
		case 'U':
			/* unsell an item */
			if (itemnum==0) break;
			mvaddstr(count++,0,"What item number to remove? ");
			refresh();
			holdint = get_number();
			if (holdint < 0) return;
			if (holdint==0 || holdint>itemnum) {
				tradeerr("Invalid Item Number");
				return;
			}
			holdint--;
#ifdef OGOD
			/* allow god to remove commodities */
			if (country!=0 && country!=natn[holdint]) 
#else 
			if (country != natn[holdint]) 
#endif /* OGOD */
			{
				tradeerr("That is not your item");
				return;
			}

			if (deal[holdint]!=SELL) {
				tradeerr("That item is not up for sale");
				return;
			}
			
			/* remove it from market */
			if ( (tfile = fopen(tradefile,"a+"))==NULL) {
				tradeerr("Error opening file for trading");
				abrt();
			}
			fprintf(tfile, "%d %d %d %d %ld %ld %d\n", NOSALE, natn[holdint], holdint, 0, 0L, 0L, 0);
			fclose(tfile);
			takeback(natn[holdint],type1[holdint],lvar1[holdint],extra[holdint],FALSE);
			/*redraw the commodities board so removal is seen*/
			inloop=FALSE;
			done=FALSE;
			break;
		default:
			/* return on no choice */
			return;
		}
	}
}

void
tradeerr(mesg)
char *mesg;
{
	clear_bottom(0);
	standout();
	mvaddstr(21,0,mesg);
	standend();
	mvaddstr(22,0,"Hit any key to continue");
	refresh();
	getch();
}

int
checkland(tradestat,xspot,yspot)
int tradestat,xspot,yspot;
{
	int newstat=tradestat;
	if (!ONMAP(xspot,yspot)) {
		tradeerr("That is off the map");
		newstat=NODEAL;
	} 
	else if (sct[xspot][yspot].owner != country) {
		tradeerr("You don't own it");
		newstat=NODEAL;
	}
	else if (curntn->capx==xspot && curntn->capy==yspot) {
		tradeerr("That is your capitol");
		newstat=NODEAL;
	}
	else if (sct[xspot][yspot].designation == DTOWN) {
		tradeerr("Towns may not be sold");
		newstat=NODEAL;
	}
	else if (sct[xspot][yspot].designation == DCITY) {
		tradeerr("Cities may not be sold");
		newstat=NODEAL;
	}
	return(newstat);
}

/* get minimum foodvalue for land */
int
getland(count)
int *count;
{
	int	temp;
	int	i,j;
	char	entered;

	mvprintw((*count)++,0,"MINIMUM VEGETATION: %c, %c, %c, %c, %c, %c, %c, %c, %c, %c, %c or %c: ",
		VOLCANO,DESERT,TUNDRA,BARREN,LT_VEG,
		GOOD,WOOD,FOREST,JUNGLE,SWAMP,ICE,NONE);

	refresh();
	entered=getch();
	if(entered!=VOLCANO       &&entered!=JUNGLE
		&&entered!=DESERT &&entered!=TUNDRA
		&&entered!=BARREN &&entered!=LT_VEG
		&&entered!=NONE   &&entered!=GOOD
		&&entered!=WOOD   &&entered!=FOREST
		&&entered!=SWAMP  &&entered!=ICE) temp=(-1);
	else {
		for(i=0;i<MAPX;i++)
		for(j=0;j<MAPY;j++) if(sct[i][j].vegetation == entered) {
			temp = tofood( &sct[i][j],country );
			i=MAPX+1;
			j=MAPY+1;
		}
	}
	mvprintw((*count)++,0," JUST ENTERED %c so food value is %d", entered,temp);
	refresh();
	return(temp);
}

int
gettrade(saletype,count)
char *saletype;
int *count;
{
	int hold=(-1);

	mvprintw((*count)++,0,"%s: (G)old, (F)ood, (I)ron, (J)ewels, (L)and, (A)rmy, (S)hips?",saletype);
	refresh();
	switch(getch()) {
	case 'g':
	case 'G':
		hold=TDGOLD;
		break;
	case 'f':
	case 'F':
		hold=TDFOOD;
		break;
	case 'i':
	case 'I':
		hold=TDMETAL;
		break;
	case 'j':
	case 'J':
		hold=TDJEWL;
		break;
	case 'l':
	case 'L':
		hold=TDLAND;
		break;
	case 'a':
	case 'A':
		hold=TDARMY;
		break;
	case 's':
	case 'S':
		hold=TDSHIP;
		break;
	default:
		break;
	}
	return(hold);
}
#endif /* CONQUER */

/* set aside things that are up for bid */
void
setaside(cntry,item,longval,extint,isup)
int cntry,item,isup,extint;
long longval;
{
	switch(item)
	{
	case TDGOLD:
		if (isup==FALSE) ntn[cntry].tgold -= longval;
		break;
	case TDFOOD:
		ntn[cntry].tfood -= longval;
		break;
	case TDMETAL:
		if (isup==FALSE) ntn[cntry].metals -= longval;
		break;
	case TDJEWL:
		if (isup==FALSE) ntn[cntry].jewels -= longval;
		break;
	case TDLAND:
		break;
	case TDARMY:
		ntn[cntry].arm[extint].smove = 0;
		ntn[cntry].arm[extint].stat = TRADED;
		break;
	case TDSHIP:
		/* use commodity to hold indicator */
		ntn[cntry].nvy[extint].smove = 0;
		ntn[cntry].nvy[extint].commodity = TRADED;
		break;
	}
}

/* regain things that are up for bid */
void
takeback(cntry,item,longval,extint,isup)
int cntry,item,isup,extint;
long longval;
{
	if (cntry == -1) return;
	switch(item)
	{
	case TDGOLD:
		if (isup==FALSE) ntn[cntry].tgold += longval;
		break;
	case TDFOOD:
		ntn[cntry].tfood += longval;
		break;
	case TDMETAL:
		if (isup==FALSE) ntn[cntry].metals += longval;
		break;
	case TDJEWL:
		if (isup==FALSE) ntn[cntry].jewels += longval;
		break;
	case TDLAND:
		break;
	case TDARMY:
		ntn[cntry].arm[extint].stat = DEFEND;
		break;
	case TDSHIP:
		/* use commodity to hold indicator */
		ntn[cntry].nvy[extint].commodity = 0;
		break;
	}
}

#ifdef ADMIN
/* give things that were purchased from cntry1 to cntry2 */
long
tradeit(cntry1,cntry2,item,longval,extra)
int cntry1,cntry2,item,extra;
long longval;
{
	int unitnum=(-1),unitcount=0;
	/* error for -1 returned */
	long returnval=(-1);
	switch(item)
	{
	case TDGOLD:
		returnval = longval;
		ntn[cntry2].tgold += longval * TRADECOST(20);
		break;
	case TDFOOD:
		returnval = longval;
		ntn[cntry2].tfood += longval * TRADECOST(20);
		break;
	case TDMETAL:
		ntn[cntry2].metals += longval * TRADECOST(20);
		returnval = longval;
		break;
	case TDJEWL:
		ntn[cntry2].jewels += longval * TRADECOST(20);
		returnval = longval;
		break;
	case TDLAND:
		if (sct[(int)longval][extra].owner==cntry1) {
			sct[(int)longval][extra].owner = cntry2;
			returnval = longval;
		}
		break;
	case TDARMY:
		/* find army number for cntry2 */
		/* give army to cntry2 */
		if(ntn[cntry1].arm[extra].sold <= 0) return -1;
		while(unitnum==(-1)&&unitcount<MAXARM) {
			if (ntn[cntry2].arm[unitcount].sold<=0) {
				/* give army to cntry2 */
				ntn[cntry2].arm[unitcount].sold = ntn[cntry1].arm[extra].sold;
				ntn[cntry2].arm[unitcount].unittyp = ntn[cntry1].arm[extra].unittyp;
				ntn[cntry2].arm[unitcount].xloc = ntn[cntry2].capx;
				ntn[cntry2].arm[unitcount].yloc = ntn[cntry2].capy;
				ntn[cntry2].arm[unitcount].stat = DEFEND;
				ntn[cntry2].arm[unitcount].smove = 0;
				/* remove army from cntry1 */
				ntn[cntry1].arm[extra].sold = 0;
				ntn[cntry1].arm[extra].smove = 0;
				ntn[cntry1].arm[extra].stat = DEFEND;
				unitnum=unitcount;
			}
			unitcount++;
		}
		returnval=(long)unitnum;
		break;
	case TDSHIP:
		/* give navy to cntry1 */
		if(ntn[cntry1].nvy[extra].merchant==0
		   && ntn[cntry1].nvy[extra].warships==0
		   && ntn[cntry1].nvy[extra].galleys==0) return -1;
		while(unitnum==(-1)&&unitcount<MAXARM){
			if ((int)ntn[cntry2].nvy[unitcount].merchant+ntn[cntry2].nvy[unitcount].warships+ntn[cntry2].nvy[unitcount].galleys == 0) {
				/* give navy to cntry2 */
				ntn[cntry2].nvy[unitcount].warships = ntn[cntry1].nvy[extra].warships;
				ntn[cntry2].nvy[unitcount].merchant = ntn[cntry1].nvy[extra].merchant;
				ntn[cntry2].nvy[unitcount].galleys = ntn[cntry1].nvy[extra].galleys;
				ntn[cntry2].nvy[unitcount].crew = ntn[cntry1].nvy[extra].crew;
				ntn[cntry2].nvy[unitcount].xloc = ntn[cntry1].nvy[extra].xloc;
				ntn[cntry2].nvy[unitcount].yloc = ntn[cntry1].nvy[extra].yloc;
				ntn[cntry2].nvy[unitcount].commodity = 0;
				ntn[cntry2].nvy[unitcount].smove = 0;
				/* remove navy from cntry1 */
				ntn[cntry1].nvy[extra].smove = 0;
				ntn[cntry1].nvy[extra].merchant = 0;
				ntn[cntry1].nvy[extra].warships = 0;
				ntn[cntry1].nvy[extra].galleys = 0;
				ntn[cntry1].nvy[extra].crew = 0;
				ntn[cntry1].nvy[extra].commodity = 0;
				unitnum=unitcount;
			}
			unitcount++;
		}
		returnval=(long)unitnum;
		break;
	}
	return(returnval);
}

long
gettval(cntry1,cntry2,type,longval,extint)
int cntry1, cntry2, extint;
long longval;
{
	int returnval=(-1);
	long armyvalue();

	switch(type) {
	case TDGOLD:
	case TDFOOD:
	case TDMETAL:
	case TDJEWL:
		returnval=longval;
		break;
	case TDLAND:
		if (cntry2 == sct[(int)longval][extint].owner)
		returnval=(long)tofood( &sct[(int)longval][extint],cntry1);
		break;
	case TDARMY:
		if (armyvalue(cntry2,extint)>0)
		returnval=armyvalue(cntry2,extint);
		break;
	case TDSHIP:
		curntn = &ntn[cntry2];
		if (flthold(extint)>0)
		returnval = (long)flthold(extint);
		break;
	}
	return(returnval);
}

/* this function sends detailed message to players */
/* upon completion of a trade */
void
trademail(cntry1,cntry2,item1,item2,lvar1,lvar2,lvar3,lvar4)
int cntry1,cntry2,item1,item2;
long lvar1,lvar2,lvar3,lvar4;
{
	FILE *fp[2];
	int count;
	char cname[2][NAMELTH+1],filename[2][FILELTH];

	sprintf(filename[0],"%s%d",msgfile,cntry1);
	sprintf(filename[1],"%s%d",msgfile,cntry2);
	strcpy(cname[0],ntn[cntry1].name);
	strcpy(cname[1],ntn[cntry2].name);

	if ((fp[0]=fopen(filename[0],"a+"))==NULL) {
		printf("error opening <%s>\n",filename[0]);
		abrt();
	}
	if ((fp[1]=fopen(filename[1],"a+"))==NULL) {
		printf("error opening <%s>\n",filename[1]);
		abrt();
	}

	for (count=0;count<2;count++) {
		fprintf(fp[count],"Message to %s from Conquer Commerce Commision\n",cname[count]);
		fprintf(fp[count],"Dated: %s of Year %d\n",PSEASON(TURN),YEAR(TURN));
		fprintf(fp[count],"\n");
		fprintf(fp[count],"   Trade transaction between %s and %s completed.\n",cname[1],cname[0]);
		if (item1<=TDJEWL)
		fprintf(fp[count],"       Nation %s receives %ld %s\n",cname[1],lvar1,commodities[item1]);
		else if (item1==TDLAND)
		fprintf(fp[count],"       Nation %s receives sector %ld, %ld\n",cname[1],lvar1,lvar2);
		else if (item1==TDARMY)
		fprintf(fp[count],"       Nation %s receives army #%ld\n",cname[1],lvar1);
		else if (item1==TDSHIP)
		fprintf(fp[count],"       Nation %s receives navy #%ld\n",cname[1],lvar1);
		if (item2<=TDJEWL)
		fprintf(fp[count],"       Nation %s receives %ld %s\n",cname[0],lvar3,commodities[item2]);
		else if (item2==TDLAND)
		fprintf(fp[count],"       Nation %s receives sector %ld, %ld\n",cname[0],lvar3,lvar4);
		else if (item2==TDARMY)
		fprintf(fp[count],"       Nation %s receives army #%ld\n",cname[0],lvar3);
		else if (item2==TDSHIP)
		fprintf(fp[count],"       Nation %s receives navy #%ld\n",cname[0],lvar3);
		fprintf(fp[count],"END\n");
		fclose(fp[count]);
	}
}
#endif /* ADMIN */

#ifdef CONQUER
/* routine to determine whether or not an army type is tradable */
int
tradable(cntry,armynum)
int cntry,armynum;
{
	int oldcntry=country,returnval=FALSE;
	country=cntry;
	if ( (ASTAT!=TRADED) && (ASTAT!=ONBOARD) && (ATYPE==A_MERCENARY
		|| ATYPE==A_SIEGE || ATYPE==A_CATAPULT
		|| ATYPE==A_ELEPHANT || ATYPE>=MINMONSTER) ) returnval=TRUE;
	country=oldcntry;
	return(returnval);
}
#endif /* CONQUER */

/* routine to determine commercial value of army */
long armyvalue(cntry,unit)
int cntry,unit;
{
	long returnval;
	
	returnval = ntn[cntry].arm[unit].sold*100 +
		ntn[cntry].arm[unit].sold * unitattack[ntn[cntry].arm[unit].unittyp%UTYPE];
	if (ntn[cntry].arm[unit].unittyp >= MINMONSTER) returnval+=ntn[cntry].arm[unit].sold*10;
	returnval/=100;
	return(returnval);
}

void
checktrade()
{
	FILE *tfile;
	int count, itemnum=0, natn[MAXITM];
	int type1[MAXITM], type2[MAXITM], deal[MAXITM], extra[MAXITM];
	long lvar1[MAXITM], lvar2[MAXITM];
	void takeback();
	void setaside();
	/* initialize purchase list */
	for (count=0; count<MAXITM; count++) {
		deal[count]=(-1);
	}
	/* open trading file */
	if ((tfile = fopen(tradefile,"r")) == NULL) {
		/* no commodities - no transactions */
		return;
	}
	/* read in all of the transactions */
	while(!feof(tfile)) {
		if (7 != fscanf(tfile,"%d %d %d %d %ld %ld %d\n",&deal[itemnum],
			&natn[itemnum],&type1[itemnum],&type2[itemnum],&lvar1[itemnum],&lvar2[itemnum],&extra[itemnum])) break;
		if (deal[itemnum]==NOSALE) {
			if (natn[itemnum]==country)
			takeback(country,type1[type1[itemnum]],lvar1[type1[itemnum]],extra[type1[itemnum]],TRUE);
		} else if (deal[itemnum]==SELL) {
			if (natn[itemnum]==country)
			setaside(country,type1[itemnum],lvar1[itemnum],extra[itemnum],TRUE);
			itemnum++;
		} else if (deal[itemnum]==BUY) {
			if (natn[itemnum]==country) {
				if (type1[itemnum]==GETFOOD) ntn[country].tfood=lvar1[itemnum];
				else setaside(country,type2[type1[itemnum]],lvar1[itemnum],(int)lvar1[itemnum],TRUE);
			}
		}
	}
	fclose(tfile);
}

#ifdef ADMIN
void
uptrade()
{
	FILE *tfile;
	int count, itemnum=0, natn[MAXITM];
	int type1[MAXITM], type2[MAXITM], deal[MAXITM], extra[MAXITM];
	extern FILE *fnews;
	void trademail();
	int whobuy[MAXITM];
	long tradeit(), buy1[MAXITM], buy2[MAXITM];
	long price[MAXITM], gettval(), longval1, longval2;
	long lvar1[MAXITM], lvar2[MAXITM];
	void takeback();

	/* initialize purchase list */
	for (count=0; count<MAXITM; count++) {
		deal[count]=(-1);
		whobuy[count]=(-1);
		price[count]=(-1);
	}
	/* open trading file */
	if ((tfile = fopen(tradefile,"r")) == NULL) {
		/* no commodities - no transactions */
		return;
	}
	/* read in all of the transactions */
	while(!feof(tfile)) {
		if (7 != fscanf(tfile,"%d %d %d %d %ld %ld %d\n",&deal[itemnum],
			&natn[itemnum],&type1[itemnum],&type2[itemnum],&lvar1[itemnum],&lvar2[itemnum],&extra[itemnum])) break;
		if (deal[itemnum]==NOSALE) {
			/* remove item from sales list */
			deal[type1[itemnum]]=NOSALE;
		} else if (deal[itemnum]==SELL) {
			itemnum++;
		} else if (deal[itemnum]==BUY) {
			if (type1[itemnum]==GETFOOD) /* just ignore food */;
			else if (deal[type1[itemnum]]==SELL &&
			(price[type1[itemnum]] <
			gettval(natn[type1[itemnum]],natn[itemnum],
			type2[type1[itemnum]],lvar1[itemnum],(int)lvar2[itemnum]))) {
				deal[type1[itemnum]]=BUY;
				/* highest bid so far */
				price[type1[itemnum]]=gettval(natn[type1[itemnum]],natn[itemnum],type2[type1[itemnum]],lvar1[itemnum],(int)lvar2[itemnum]);
				/* return bid to loser */
				takeback(whobuy[type1[itemnum]],
					type2[type1[itemnum]],
					buy1[type1[itemnum]],(int)buy2[type1[itemnum]],FALSE);
				/* record details of trade */
				buy1[type1[itemnum]]=lvar1[itemnum];
				buy2[type1[itemnum]]=lvar2[itemnum];
				whobuy[type1[itemnum]]=natn[itemnum];
			} else {
				/* return bid */
				takeback(natn[itemnum],
					type2[type1[itemnum]],
					lvar1[itemnum],(int)lvar2[itemnum],FALSE);
			}
		}
	}
	fclose(tfile);
	unlink(tradefile);
	if (itemnum == 0) return;
	/* reopen the file for unsold commodities */
	if ((tfile=fopen(tradefile,"w")) == NULL) {
		/* error on opening file */
		printf("Error opening <%s> for trade update\n",tradefile);
		abrt();
	}

	/* compute the trading */
	for (count=0;count<itemnum;count++) {
		if (deal[count]==SELL) {
			/* adjust the displayed value */
			if (type1[count] != TDLAND)
			lvar1[count]=gettval(0,natn[count],type1[count],lvar1[count],extra[count]);
			/* keep unsold items up for sale */
			if(lvar1[count]>=0) fprintf(tfile,"%d %d %d %d %ld %ld %d\n", deal[count], natn[count],type1[count],type2[count],lvar1[count],lvar2[count],extra[count]);
		} else if (deal[count]==BUY) {
			/* do the trading */
			longval1 = tradeit(natn[count],whobuy[count],type1[count],lvar1[count],extra[count]);
			if (longval1!=(-1)) longval2 = tradeit(whobuy[count],natn[count],type2[count],buy1[count],(int)buy2[count]);
			if (longval1==(-1)) {
				/* abort due to seller */
				fprintf(fnews,"2.\tTrade between %s and %s breaks down due to %s.\n",
					ntn[natn[count]].name,ntn[whobuy[count]].name,tradefail[type1[count]]);
			} else if (longval2==(-1)) {
				/* abort due to buyer */
				tradeit(whobuy[count],natn[count],type1[count],longval1,extra[count]);
				fprintf(fnews,"2.\tTrade between %s and %s breaks down due to %s.\n",
					ntn[whobuy[count]].name,ntn[natn[count]].name,tradefail[type2[count]]);
				/* place it on the list for next turn */
				/* adjust the displayed value */
				/* let land fail during trade */
				if (type1[count] != TDLAND)
				lvar1[count]=gettval(0,natn[count],type1[count],lvar1[count],extra[count]);
				if(lvar1[count]>=0) fprintf(tfile,"%d %d %d %d %ld %ld %d\n", SELL, natn[count],type1[count],type2[count],lvar1[count],lvar2[count],extra[count]);
			} else {
				/* trade completed send mail */
				fprintf(fnews,"2.\tNation %s sells %s to %s for %s\n"
					,ntn[natn[count]].name,commodities[type1[count]],ntn[whobuy[count]].name,commodities[type2[count]]);
				trademail(natn[count],whobuy[count],type1[count],
					type2[count],longval1,(long)extra[count],
					longval2,buy2[count]);
			}
		}
	}
	fclose(tfile);
}

/* remove a nations items from the trading board */
void 
fixtrade (cntry)
int cntry;
{
	FILE *tfile;
	int holdint, notopen=FALSE;
	int type1[MAXITM], type2[MAXITM], deal[MAXITM], extra[MAXITM];
	int natn[MAXITM], itemnum, getland(), gettrade(), checkland();
	long lvar1[MAXITM], lvar2[MAXITM], armyvalue();
	void  setaside(), takeback();

	/* open trading file */
	if ((tfile=fopen(tradefile,"r")) == NULL ) {
		notopen=TRUE;
	}
	itemnum = 0;

	/* read in all of the data */
	while (notopen==FALSE && !feof(tfile)) 
	{
		if (fscanf(tfile,"%d %d %d %d %ld %ld %d\n",&deal[itemnum],
			&natn[itemnum],&type1[itemnum],&type2[itemnum],
			&lvar1[itemnum],&lvar2[itemnum],&extra[itemnum]) == 7){
			if (deal[itemnum]==NOSALE) {
				/* remove item from sales list */
				deal[type1[itemnum]]=NOSALE;
			} else if (deal[itemnum]==SELL) {
				itemnum++;
			}
		}
	}
	if (notopen==FALSE) fclose(tfile);

	/* go through list of commodities */

	for ( holdint=0; holdint<itemnum; holdint++) 
	{
		if ( deal[holdint]==SELL ) 
		{

			if ( natn[holdint] == cntry )
			{
				/* remove it from market */

				if ( (tfile = fopen(tradefile,"a+"))==NULL)
				{
					printf("Error opening file for trading");
					abrt();
				}

				fprintf(tfile, "%d %d %d %d %ld %ld %d\n", 
				 NOSALE, natn[holdint], holdint, 0, 0L, 0L, 0);

				fclose(tfile);

				takeback( natn[holdint], type1[holdint], lvar1[holdint],
					extra[holdint], FALSE);

			}  /* natn == cntry */

		} /* SELL order */

	} /* loop through commodities */
}
#endif /* ADMIN */
#endif /* TRADE */
