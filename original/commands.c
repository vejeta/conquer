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
#ifdef CONQUER
#include <sys/types.h>
#include <sys/stat.h>
#endif /*CONQUER*/
#include "header.h"
#include "data.h"

extern long conq_mail_size;

extern FILE *fexe;			/*execute file pointer*/
extern short country;
extern short selector;
extern short pager;
extern short xcurs,ycurs,xoffset,yoffset;
extern short redraw;

int roads_this_turn = 0;

/* routine to determine if the given designation is ok; TRUE for ok */
int
desg_ok(prtflag, desg, sptr)
	int prtflag;
	char desg;
	struct	s_sector	*sptr;
{
	/* check vegetation */
	if((desg!=DNODESIG)&&(desg!=DROAD)&&(desg!=DFORT)
	&&(desg!=DSTOCKADE)&&(tofood(sptr,country)<DESFOOD)) {
		if(prtflag) errormsg("vegetation too sparse");
		return(FALSE);
	}

	/* don't allow the same designation */
	if(desg==sptr->designation) {
		if(prtflag) errormsg("Hey, get your act together!  There is already one there.");
		return(FALSE);
	}

	/* check for city/capitol being made into something else */
	if((desg!=DRUIN)
	&&((desg!=DCAPITOL && sptr->designation==DCITY)
	||sptr->designation==DCAPITOL)) {
		if(prtflag) {
			char buf[LINELTH+1];
			sprintf(buf,"Must first burn down city/capitol (designate as '%c')",DRUIN);
			errormsg(buf);
		}
		return(FALSE);
	}

	/* check for proper population */
	if((sptr->people<500)
	&&(desg==DCAPITOL || desg==DCITY || desg==DTOWN)) {
		if(prtflag) errormsg("Need 500 people to build a city or town");
		return(FALSE);
	}

	/* only god may create pirate  */
	if(desg==DBASECAMP) {
		if(prtflag) errormsg("A Pirate Cove?? Are you serious?!");
		return(FALSE);
	}

	/* should not appear in display list */
	if (prtflag==FALSE &&
	((desg==DSPECIAL && sptr->tradegood!=TG_stones) || desg==DNODESIG))
		return(FALSE);

	if (desg==DRUIN) {
		if(sptr->designation!=DCITY&&sptr->designation!=DCAPITOL) {
			if(prtflag) errormsg("Ruins may only come from cities or capitols");
			return(FALSE);
		}
	}

	if (desg==DCAPITOL || desg==DCITY) {
		if((desg==DCAPITOL && sptr->designation!=DCITY)
		&& sptr->designation!=DTOWN && sptr->designation!=DRUIN) {
			if(prtflag) errormsg("You can't build a city from that!");
			return(FALSE);
		}
	}

	if (desg==DUNIVERSITY || desg==DLUMBERYD) {
		if((sptr->tradegood != TG_none)
		&&(*(tg_stype + sptr->tradegood) != desg)
		&&(*(tg_stype + sptr->tradegood) != 'x')) {
			if(prtflag) errormsg("You can't have one of those here!");
			return(FALSE);
		}
	}

	if(desg==DMINE || desg==DGOLDMINE) {
		if( !tg_ok( country, sptr )) {
			if(prtflag) errormsg("Your people refuse to be unemployed");
			return(FALSE);
		}
		if((desg==DMINE && sptr->metal==0 )
		||( desg==DGOLDMINE && sptr->jewels==0)) {
			if(prtflag) errormsg("Your people refuse to be unemployed");
			return(FALSE);
		}
	}

	if((desg==DSPECIAL)&&(magic(country,SUMMON)!=TRUE)) {
		if(prtflag) errormsg("You are gonna need SUMMON power to use those stones!");
		return(FALSE);
	}

	return(TRUE);
}

/*change current hex designation*/
void
redesignate()
{
	char	newdes;
	char	tgtype[NAMELTH+1];
	struct s_sector *sptr= &sct[XREAL][YREAL];
	short	x,y;
	long	metal=0;
	int	isgod=FALSE;

	if(country==0){
		isgod=TRUE;
		country=sptr->owner;
		curntn= &ntn[country];

		clear_bottom(0);
		mvaddstr(LINES-4,0,"SUPER USER: CHANGE (v)eg, (e)lev, (d)esig, (o)wner, (p)op, (t)radegood");
		refresh();
		switch(getch()){
		case 'd':
			/* fall into normal redesignation command */
			break;
		case 'e':
			/*simple contour map definitions*/
			mvprintw(LINES-3,7,"ELEVATIONS: change to %c, %c, %c, %c or %c?",WATER,PEAK,MOUNTAIN,HILL,CLEAR);
			refresh();
			newdes=getch();
			if(newdes!=WATER&&newdes!=PEAK&&newdes!=MOUNTAIN
			&&newdes!=HILL&&newdes!=CLEAR) {
				reset_god();
				return;
			}
			sptr->altitude=newdes;
			if((newdes==PEAK)||(newdes==WATER)) {
				sptr->owner=0;
				sptr->people=0;
				sptr->fortress=0;
			}
			/*will fall through as must change vegetation*/
		case 'v':
			/*vegetation types*/
			mvprintw(LINES-3,7,"VEGETATIONS: change to %c, %c, %c, %c, %c, %c, %c, %c, %c, %c, %c or %c?",
			VOLCANO,DESERT,TUNDRA,BARREN,LT_VEG,
			GOOD,WOOD,FOREST,JUNGLE,SWAMP,ICE,NONE);
			refresh();
			newdes=getch();
			if(newdes!=VOLCANO
			&&newdes!=DESERT&&newdes!=TUNDRA
			&&newdes!=BARREN&&newdes!=LT_VEG
			&&newdes!=NONE&&newdes!=GOOD
			&&newdes!=WOOD&&newdes!=FOREST&&newdes!=JUNGLE
			&&newdes!=SWAMP&&newdes!=ICE) {
				reset_god();
				return;
			}
			sptr->vegetation=newdes;
			if( tofood(sptr,0) < DESFOOD )
				sptr->designation=newdes;
			else sptr->designation=DNODESIG;
			reset_god();
			return;
		case 'o':
			mvaddstr(LINES-3,7,"What nation owner:");
			refresh();
			x = get_country();
			if (x>0 && x<NTOTAL) sptr->owner=x;
			reset_god();
			return;
		case 'p':
			if (sptr->altitude == WATER) {
				errormsg("Trying to build a colony of mermen?");
				reset_god();
				return;
			}
			mvaddstr(LINES-3,7,"new population for sector: ");
			refresh();
			metal = get_number();
			if (metal <= (-1)) return;
			sptr->people = metal;
			reset_god();
			return;
		case 't':
			x=TRUE;
			while(x==TRUE) {
				mvaddstr(LINES-2,7,"new sector tradegood type:");
				clrtoeol();
				refresh();
				get_nname(tgtype);
				if(strlen(tgtype)==0) {
					reset_god();
					return;
				}
				for(y=0;x==TRUE && y<=TG_none;y++)
				if(strcmp(tg_name[y],tgtype)==0) x=FALSE;
			}
			y--;
			if((y!=TG_none)&&(y>END_NORMAL)) {
				mvaddstr(LINES-1,7,"new sector value: ");
				refresh();
				x = get_number();
				if(x<100 && x>0) {
				if(y>END_MINE) {
					sptr->jewels = (char)x;
					sptr->metal = 0;
				} else {
					sptr->metal  = (char)x;
					sptr->jewels = 0;
				}
				} else {
					sptr->jewels = 0;
					sptr->metal = 0;
				}
			} else {
				sptr->jewels=0;
				sptr->metal=0;
			}
			sptr->tradegood= (char)y;
			reset_god();
			return;
		default:
			reset_god();
			return;
		}
	}

	clear_bottom(0);
	if((SOWN!=country)&&(isgod==FALSE)) {
		errormsg("Hey!  You don't own that sector!");
		return;
	}

	mvaddstr(LINES-4,0,"Possible sector designations: ");
	x = 30;
	y = LINES-4;
	for(newdes=0; *(des+newdes) != '0'; newdes++ ) {
		if((isgod==TRUE)||desg_ok(FALSE,*(des+newdes),sptr)) {
			mvprintw(y,x,"(%c)",*(des+newdes));
			x+=4;
			if(x>COLS-15) {
				x=5;
				y++;
			}
		}
	}

	if((sptr->tradegood != TG_none)
	&&( *(tg_stype+sptr->tradegood) != 'x')
	&&( isgod==TRUE || desg_ok(FALSE,*(tg_stype+sptr->tradegood),sptr)) )
	mvprintw(y,x,"(%c special=>%c)",DSPECIAL,*(tg_stype+sptr->tradegood));

	mvaddstr(++y,0,"<Any other key to return>  What new designation:");
	refresh();

	/*read answer*/
	if((newdes=getch())==DSPECIAL) {
		if((sptr->tradegood == TG_none)
		||( *(tg_stype+sptr->tradegood) == 'x')||(isgod==FALSE
		&& !desg_ok(FALSE,*(tg_stype+sptr->tradegood),sptr)) ) {
			errormsg("no special designation possible" );
			if(isgod==TRUE) reset_god();
			return;
		} else
			newdes = *(tg_stype + sptr->tradegood);
	}

	for(x=0; *(des+x) != '0'; x++ )
		if(newdes == *(des+x)) break;

	if(*(des+x) == '0') {
		if(isgod==TRUE) reset_god();
		return;
	}

	/* validate designation */
	if((isgod==FALSE)&&!desg_ok(TRUE,newdes,sptr)) {
		return;
	}

	if (newdes == DROAD && isgod==FALSE) {
		if( sptr->people < 100 ) {
			errormsg("Need 100+ people to build a road!");
			return;
		}
		roads_this_turn++;
		if (roads_this_turn>2) {
			roads_this_turn=2;
			errormsg("Only two roads a turn!  Let the road crews sleep!!");
			return;
		}
	}

	/* do not need metal to build a capitol from a city */
	if((newdes==DFORT)||(newdes == DTOWN)) metal=DESCOST;
	else if((newdes == DCITY)
	||(newdes == DCAPITOL && sptr->designation!=DCITY)) metal=5*DESCOST;

	if((newdes!=DTOWN)&&(newdes!=DFORT)&&(newdes!=DCITY)&&(newdes!=DCAPITOL)){
		/*decrement treasury*/
		if(newdes==DRUIN) {
			if (sptr->fortress>4) {
				sptr->fortress-=4;
			} else {
				sptr->fortress=0;
			}
		} else if (sptr->designation==DRUIN) {
			curntn->tgold-=REBUILDCOST;
		}
		sptr->designation=newdes;

		SADJDES;
		if(newdes == DSTOCKADE)
			curntn->tgold-=STOCKCOST*(1-isgod);
		else
			curntn->tgold-=DESCOST*(1-isgod);

	} else if((isgod==FALSE)&&(curntn->metals<metal)) {
		errormsg("Not enough metal for city, town, or fort");
	} else if((newdes==DCITY)||(newdes==DCAPITOL)) {

		if(sptr->designation==DRUIN){
			curntn->tgold-=10*DESCOST*(1-isgod);
			curntn->metals-=metal/2*(1-isgod);
		} else {
			curntn->tgold-=20*DESCOST*(1-isgod);
			if(newdes==DCITY || sptr->designation!=DCITY)
				curntn->metals-=metal*(1-isgod);
		}
		x=curntn->capx;	/* need this for SADJDES2 */
		y=curntn->capy;
		/* can only have one capitol */
		if(newdes==DCAPITOL){
			if (sct[x][y].owner==country) {
				sct[x][y].designation=DCITY;
				SADJDES2;
			}
			curntn->capx=XREAL;
			curntn->capy=YREAL;
		}
		sptr->designation=newdes;
		SADJDES;
	} else if((newdes==DFORT)||(newdes==DTOWN)){
		curntn->tgold-=10*DESCOST*(1-isgod);
		curntn->metals-=metal*(1-isgod);
		sptr->designation=newdes;
		SADJDES;
	} else	errormsg("Serious error: What designation are you?");
	if(isgod==TRUE) reset_god();
}

/*build fort or ship-type */
void
construct()
{
	int	tmpvar,tmpvar2,onboard;
	long	cost;
	int	armbonus;
	int	x,y;
	short	nvynum=0;
	short	shipsize,amount;
	short	isgod=FALSE;
	char	type;

	clear_bottom(0);
	if(country==0){
		isgod=TRUE;
		country=sct[XREAL][YREAL].owner;
		curntn= &ntn[country];
	}

	if(sct[XREAL][YREAL].owner!=country) {
		errormsg("You do not own");
		if(isgod==TRUE) reset_god();
		return;
	}
	if((isgod==FALSE)&&(sct[XREAL][YREAL].people<=500)) {
		errormsg("You need over 500 people to construct");
		return;
	}

	if((isgod==FALSE) && (curntn->tgold < 0 )) {
		errormsg("You are broke");
		return;
	}
	tmpvar=FALSE;
	for(x=XREAL-1;x<=XREAL+1;x++)
		for(y=YREAL-1;y<=YREAL+1;y++)
			if(ONMAP(x,y) && sct[x][y].altitude==WATER)
				tmpvar=TRUE;

	if((sct[XREAL][YREAL].designation==DTOWN)
	||(sct[XREAL][YREAL].designation==DFORT)
	||(sct[XREAL][YREAL].designation==DCAPITOL)
	||(sct[XREAL][YREAL].designation==DCITY)) {
		/*calculate cost for fort*/
		cost=FORTCOST;
		if(isgod==TRUE) cost=0;
		else for(x=1;x<=sct[XREAL][YREAL].fortress;x++)
			cost*=2;

		if(sct[XREAL][YREAL].designation==DTOWN) armbonus=TOWNSTR;
		else if(sct[XREAL][YREAL].designation==DFORT) armbonus=FORTSTR;
		else if(sct[XREAL][YREAL].designation==DCAPITOL) armbonus=CITYSTR;
		else if(sct[XREAL][YREAL].designation==DCITY) armbonus=CITYSTR;
		if(magic(country,ARCHITECT)==TRUE) armbonus*=2;

		if(tmpvar)
		mvprintw(LINES-4,0,"<f>ortify sector (+%d%%/%ld talons); <b>uild or <r>epair ships?: ",armbonus,cost);
		else
		mvprintw(LINES-4,0,"<f>ortify sector (+%d%% - %ld talons):",armbonus,cost);
		refresh();
		type=getch();
	} else {
		errormsg("Must construct in town, city, or fortress");
		if (isgod==TRUE) reset_god();
		return;
	}

	/* construct ships*/
	if((type=='b')||(type=='r')) {
		/*check if next to sea*/
		if((tmpvar==FALSE)||(sct[XREAL][YREAL].designation==DFORT)){
			errormsg("not in a harbor");
			if(isgod==TRUE) reset_god();
			return;
		}

		nvynum=getselunit()-MAXARM;
		if(type=='r') {
			if((nvynum>=MAXNAVY)||(nvynum<0)){
				errormsg("INVALID NAVY");
				if(isgod==TRUE) reset_god();
				return;
			}

			if (P_NCREW==SHIPCREW) {
				errormsg("You may only &^#$! repair damaged fleets!!!");
				return;
			}
			clear_bottom(0);
			shipsize = flthold(nvynum);
			mvprintw(LINES-4,0,"Repairing Fleet (%d)",nvynum);
			mvprintw(LINES-4,30,"Storage Units [%d]",shipsize);
			mvprintw(LINES-3,0,"crew per unit = %d",(int)P_NCREW);
			mvprintw(LINES-3,30,"max crew per unit = %d",SHIPCREW);
			mvaddstr(LINES-2,0,"How many crew per unit do you wish to add:");
			clrtoeol();
			refresh();
			amount = (short) get_number();
			if (amount<0) {
				if(isgod==TRUE) reset_god();
				return;
			}
				    

			/* find cost of repairs on all ships */
			cost = 0;
			for(tmpvar=N_LIGHT;tmpvar<=N_HEAVY;tmpvar++) {
				cost += (tmpvar+1)*WARSHPCOST*P_NWAR(tmpvar);
				cost += (tmpvar+1)*MERSHPCOST*P_NMER(tmpvar);
				cost += (tmpvar+1)*GALSHPCOST*P_NGAL(tmpvar);
			}
			cost *= amount / SHIPCREW;

			if(isgod==TRUE)				cost=0;
			else if(magic(country,SAILOR)==TRUE)	cost/=2L;

			if( curntn->tgold < cost ) {
				errormsg("NOT ENOUGH GOLD");
				if(isgod==TRUE) reset_god();
				return;
			}
			if( sct[XREAL][YREAL].people < amount * shipsize) {
				errormsg("NOT ENOUGH CIVILIANS IN SECTOR");
				if(isgod==TRUE) reset_god();
				return;
			}
			if( SHIPCREW < P_NCREW + amount ){
				errormsg("THAT WOULD EXCEED MAXIMUM SHIP CREW");
				if(isgod==TRUE) reset_god();
				return;
			}

			curntn->tgold -= cost;
			sct[XREAL][YREAL].people -= amount*shipsize;
			P_NCREW += (unsigned char) amount;
			NADJCRW;
			SADJCIV;

			if(isgod==TRUE) reset_god();
			return;
		}

		mvaddstr(LINES-3,0,"Do you wish to raise a new fleet? (y or n)");
		clrtoeol();
		refresh();
		if(getch()=='y') nvynum=(-1);

		clear_bottom(0);
		if(nvynum<0) {
			nvynum=0;
			x=(-1);
			while((x==(-1))&&(nvynum<MAXNAVY)) {
				if(P_NWSHP==0 && P_NMSHP==0 && P_NGSHP==0) {
					x=nvynum;
					P_NWSHP=0;
					P_NMSHP=0;
					P_NGSHP=0;
					P_NCREW=0;
					P_NARMY=MAXARM;
					P_NPEOP=0;
					NADJHLD;
					NADJCRW;
					NADJWAR;
					NADJMER;
					NADJGAL;
				}
				nvynum++;
			}
			nvynum=x;
			if(nvynum<0){
				errormsg("NO FREE NAVIES");
				if(isgod==TRUE) reset_god();
				return;
			} else
			mvprintw(LINES-1,0,"Raising New Fleet (%d)",nvynum);
		} else mvprintw(LINES-1,0,"Adding to Fleet (%d)",nvynum);

		/* process type first to be consistant with drafting */
		mvaddstr(LINES-4,0,"What ship type to construct: (W)arship (M)erchant (G)alley?");
		refresh();
		/*
		 *  Note:  3 and 6 hard-coded for ship sizes based on
		 *   0 = light   1 = medium  2 = heavy
		 *         Bad technique, but it is not worth saying
		 *   (N_HEAVY-N_LIGHT+1) everywhere.
		 */
		switch(getch()) {
		case 'w':
		case 'W':
			shipsize=0;
			break;
		case 'g':
		case 'G':
			shipsize=3;
			break;
		case 'm':
		case 'M':
			shipsize=6;
			break;
		case ' ':
			if(isgod==TRUE) reset_god();
			return;
		default:
			errormsg("Invalid Ship Type");
			if(isgod==TRUE) reset_god();
			return;
		}

		mvaddstr(LINES-3,0,"What ship class to construct: (L)ight  (M)edium");
		if(sct[XREAL][YREAL].designation!=DTOWN) addstr("  (H)eavy?");
		else addstr("?");
		refresh();
		switch(getch()) {
		case 'l':
		case 'L':
			shipsize+=N_LIGHT;
			break;
		case 'm':
		case 'M':
			shipsize+=N_MEDIUM;
			break;
		case 'h':
		case 'H':
			if(sct[XREAL][YREAL].designation==DTOWN) {
				errormsg("Towns cannot construct heavy ships");
				if(isgod==TRUE) reset_god();
				return;
			}
			shipsize+=N_HEAVY;
			break;
		case ' ':
			if(isgod==TRUE) reset_god();
			return;
		default:
			errormsg("Invalid Ship Class");
			if(isgod==TRUE) reset_god();
			return;
		}

		mvaddstr(LINES-2,0,"How many ships to construct?");
		refresh();
		amount = (short) get_number();

		/*sanity checks*/
		if((amount>N_MASK)) amount=0;
		if (amount<=0)
		{
			if(isgod==TRUE) reset_god();
			return;
		}

		/* 6 and 3 hard coded ... see above comment */
		cost = (long) amount * ( shipsize%3 + 1 );
		if (shipsize>=6) {
			cost *= MERSHPCOST;
		} else if (shipsize>=3) {
			cost *= GALSHPCOST;
		} else {
			cost *= WARSHPCOST;
		}

		if(isgod==TRUE)				cost=0;
		else if(magic(country,SAILOR)==TRUE)	cost/=2L;

		if((cost > curntn->tgold) && (cost > 0)) {
			errormsg("sorry - not enough talons");
			return;
		}

		if( sct[XREAL][YREAL].people < amount * (shipsize+1) * SHIPCREW ){
			errormsg("NOT ENOUGH CIVILIANS IN SECTOR");
			if(isgod==TRUE) reset_god();
			return;
		}

		if((nvynum>=0)&&(nvynum<MAXNAVY)) {
			clear_bottom(0);
			tmpvar = amount*SHIPCREW*(shipsize%3+1)
				+ flthold(nvynum)*P_NCREW;
			onboard = P_NPEOP*fltmhold(nvynum);

			if (shipsize>=6) {
				shipsize %= 3;
				tmpvar2 = NADD_MER(amount);
			} else if (shipsize>=3) {
				shipsize %= 3;
				tmpvar2 = NADD_GAL(amount);
			} else {
				shipsize %= 3;
				tmpvar2 = NADD_WAR(amount);
			}

			/* check for bad build */
			if (tmpvar2==FALSE) {
				errormsg("Too many such ships in fleet.");
				if(isgod==TRUE) reset_god();
				return;
			}

			/* crew average based on number of holding units */
			P_NCREW = (unsigned char)( tmpvar / flthold(nvynum) );

			sct[XREAL][YREAL].people-=amount*(shipsize+1)*SHIPCREW;
			curntn->tgold -= cost;

			P_NXLOC =XREAL;
			P_NYLOC =YREAL;
			P_NMOVE=0;

			mvprintw(LINES-4,0,"Fleet (%2d):     Warships  = [Light %2hd/Medium %2hd/Heavy %2hd]",nvynum,P_NWAR(N_LIGHT),P_NWAR(N_MEDIUM),P_NWAR(N_HEAVY));
			mvprintw(LINES-3,0,"                Merchants = [Light %2hd/Medium %2hd/Heavy %2hd]",P_NMER(N_LIGHT),P_NMER(N_MEDIUM),P_NMER(N_HEAVY));
			mvprintw(LINES-2,0,"Avg Crew[%3d]   Galleys   = [Light %2hd/Medium %2hd/Heavy %2hd]",(int)P_NCREW,P_NGAL(N_LIGHT),P_NGAL(N_MEDIUM),P_NGAL(N_HEAVY));

			if(fltmhold(nvynum)>0)
			P_NPEOP = (unsigned char) (onboard/fltmhold(nvynum));
			else P_NPEOP = 0;
			SADJCIV;
			NADJCRW;
			NADJWAR;
			NADJMER;
			NADJGAL;
			NADJHLD;
			NADJLOC;
			NADJMOV;
			mvaddstr(LINES-2,65,"HIT ANY KEY");
			refresh();
			getch();

		} else errormsg("ERROR!!!!!!!!!!!!!");
	}
	/* construct fortification points*/
	else if(type=='f'){
		/* can only go into debt as much as the nation has jewels */
		if (sct[XREAL][YREAL].fortress>11) {
			errormsg("That sector is as impregnable as you can make it");
		} else if ((curntn->tgold - cost) >= ((-1)*10*curntn->jewels)) {
			mvprintw(LINES-2,5,"you build +%d%% fort points for %ld gold",armbonus,cost);
			curntn->tgold-=cost;
			sct[XREAL][YREAL].fortress++;
			INCFORT;
			errormsg("");
		} else errormsg("you may not spend that much");
	}
	else errormsg("invalid input error");

	if(isgod==TRUE) reset_god();
	refresh();
}

/*DRAFT IF IN A CITY*/
void
draft()
{
	short	armynum,x,y,i;
	long	men=0,mercs;
	short	army=(-1), isgod=FALSE, newtype=0;
	long	i_cost, e_cost;
	char	ch;

	clear_bottom(0);
	if(country==0) {
		isgod=TRUE;
		country=sct[XREAL][YREAL].owner;
		curntn= &ntn[country];
	} else if(sct[XREAL][YREAL].owner!=country) {
		errormsg("You do not own");
		return;
	}

	if((sct[XREAL][YREAL].designation!=DTOWN)
	&&(sct[XREAL][YREAL].designation!=DCAPITOL)
	&&(sct[XREAL][YREAL].designation!=DCITY)) {
		errormsg("must raise in towns/cities/capitols");
		if(isgod==TRUE) reset_god();
		return;
	}
	if(curntn->tgold <= 0){
		errormsg("You are broke");
		if(isgod==TRUE) reset_god();
		return;
	}

	if(ISCITY(sct[XREAL][YREAL].designation)
	&&(sct[XREAL][YREAL].people*(3*CITYLIMIT+(curntn->tsctrs/2))<curntn->tciv)){
		mvprintw(LINES-1,0,"Need %d people in sector: hit any key",curntn->tciv/(3*CITYLIMIT+(curntn->tsctrs/2)));
		refresh();
		getch();
		if(isgod==TRUE) reset_god();
		return;
	}

	/*ask what type of unit*/
	y=LINES-2;
	mvaddstr(y,0,"options: 1) spy 2) scout");
	clrtoeol();
	x=25;
	for(i=0;i<=NOUNITTYPES;i++){
		if(unitvalid(i)==TRUE) {
			mvprintw(y,x+2,"%s",*(shunittype+i));
			mvprintw(y,x,"(%c)",*(shunittype+i)[0]);
			x+= strlen( *(shunittype+i) ) +3;
			if(x>COLS-10){
				x=0;
				y++;
			}
		}
	}
	move(y,x);
	clrtoeol();
	if((magic(country,WARRIOR)==TRUE)
	||(magic(country,WARLORD)==TRUE)
	||(magic(country,CAPTAIN)==TRUE))
	mvaddstr(LINES-3,0,"(Warrior = 1/2 enlist cost) what type of unit do you want:");
	else
	mvaddstr(LINES-3,0,"what type of unit do you want to raise:");

	clrtoeol();
	refresh();
	ch = getch();
	for(newtype=0;newtype<=NOUNITTYPES;newtype++)
		if(ch == *(shunittype+newtype)[0]) break;

	if((newtype == NOUNITTYPES+1 )||(unitvalid(newtype)==FALSE)) {
		if( ch == '1' ) newtype=A_SPY;
		else if( ch == '2' ) newtype=A_SCOUT;
		else {
			errormsg("Invalid type");
			if (isgod==TRUE) reset_god();
			return;
		}
	}
	clear_bottom(0);

	/* marines and sailors may only be drafted in harbors */
	if(newtype==A_MARINES || newtype==A_SAILOR) {
		i=FALSE;
		for (x=XREAL-1;x<=XREAL+1;x++)
		for (y=YREAL-1;y<=YREAL+1;y++)
		  if (sct[x][y].altitude==WATER) i=TRUE;

		/* not a harbor */
		if (i==FALSE) {
			if (newtype==A_MARINES)
			errormsg("Huh?  What would marines do without the water?");
			else errormsg("You gotta be kinding!?  Sailors on land?");
			if (isgod==TRUE) reset_god();
			return;
		}
	}

	/*raise an untrained army */
	i = FALSE;
	if( newtype==A_SPY || newtype==A_SCOUT ) {
		men=1;
	} else {
		mvprintw(LINES-3,0,"how many %s do you wish to raise:",unittype[newtype]);
		clrtoeol();
		refresh();
		men = get_number();
		if(men<=0) {
			if (isgod==TRUE) reset_god();
			return;
		}
	}

	/* i_people*256 is initial people -> can  draft up to following */
  	/*	draftable = max_draft - already drafted */
  	/*		  = imen/4 - ( imen - people)   */
  	/*		  = -3/4 * imen + people)  	*/
	/*	192 comes from 3*256/4 			*/
	if( (newtype != A_MERCENARY && (men > sct[XREAL][YREAL].people - (sct[XREAL][YREAL].i_people*192) ) )
  	||(sct[XREAL][YREAL].i_people < 0)) {
  		if(sct[XREAL][YREAL].i_people < 0)
  		errormsg("error: sector wasn't city at beginning of turn");
		else errormsg("error: raising too many soldiers");
		if(isgod==TRUE) reset_god();
		return;
	}

	/* check that you dont have too many mercenaries */
	mercs=0;
	if(newtype == A_MERCENARY){
		int totalsolds=0;
		for(armynum=0;armynum<MAXARM;armynum++){
			if(P_ATYPE<MINLEADER) {
				if(P_ATYPE==A_MERCENARY) mercs+=P_ASOLD;
				totalsolds+=P_ASOLD;
			}
		}
		if(men+mercs > (totalsolds+men)/2) {
			errormsg("you would then have more than 50%% mercenaries");
			if(isgod==TRUE) reset_god();
			return;
		}
		if(mercgot+men > MERCMEN/NTOTAL) {
			errormsg("there are not that many mercanaries available");
			if(isgod==TRUE) reset_god();
			return;
		}
	}

	e_cost= (long) *(u_encost+newtype) * men;
	i_cost= (long) *(u_enmetal+newtype) * men;

	/*magiced get 1/2 enlistment costs*/
	if((magic(country,WARRIOR)==TRUE)
	||(magic(country,WARLORD)==TRUE)
	||(magic(country,CAPTAIN)==TRUE))
		e_cost/=2;

	if((magic(country,SAPPER)==TRUE)
	&&((newtype==A_SIEGE)||(newtype==A_CATAPULT))){
		e_cost/=2;
		i_cost/=2;
	}

	/*check to see if enough gold*/
	if(e_cost >  curntn->tgold) {
		errormsg("You don't have enough talons");
		if(isgod==TRUE) reset_god();
		return;
	} else if(i_cost > curntn->metals) {
		mvprintw(LINES-1,0,"You don't have %ld metal",i_cost);
		mvaddstr(LINES-1,COLS-20,"PRESS ANY KEY");
		clrtoeol();
		refresh();
		getch();
		if(isgod==TRUE) reset_god();
		return;
	} else {
		move(LINES-2,0);
		clrtoeol();
	}

	/*count is order of that army in sector*/
	/*armynum is number of that army*/
	if((armynum=getselunit())>=0){
		if(armynum>=MAXARM || newtype==A_SPY || newtype==A_SCOUT) {
			army = -1;
		} else {
			/*if different types, must raise new army*/
			if((newtype == P_ATYPE)&&(P_ASTAT!=ONBOARD)) {
			mvaddstr(LINES-1,0,"Do you wish to raise a new army:");
			clrtoeol();
			refresh();
			if(getch()!='y') army=armynum;
			else army= -1;
			}
			else army=(-1);
		}
	}
	if(army==(-1)) {
		mvprintw(LINES-2,0,"(%s, gold talons=%ld, metal=%ld) raising a new army",*(unittype+newtype),e_cost,i_cost);
		clrtoeol();
		refresh();
		sleep(1);

		armynum=0;
		while((army==(-1))&&(armynum<MAXARM)) {
			if(P_ASOLD<=0) {
				army=armynum;
				P_ASOLD=0;
				if( newtype==A_MILITIA )
				P_ASTAT=MILITIA;/* new militia units=MILITIA */
				else
				P_ASTAT=DEFEND; /* set new armies to DEFEND */
				AADJSTAT;
				AADJMEN;
			}
			armynum++;
		}
		if(army==(-1)){
			errormsg("NO FREE ARMIES");
			if(isgod==TRUE) reset_god();
			return;
		}
		armynum=army;
	} else {
		mvprintw(LINES-2,0,"(%s, gold talons=%ld, metal=%ld) adding to existing army",*(unittype+newtype),e_cost,i_cost);
		clrtoeol();
		refresh();
		sleep(2);
	}

	if( newtype == A_SPY ) {
		while(TRUE){
			clear_bottom(0);
			mvaddstr(LINES-3,0,"Spy Against What Nation: ");
			refresh();
			if((i = get_country())==(-1)) {
				if (isgod==TRUE) reset_god();
				return;
			}
			if(i==country) {
				errormsg("What?  You don't even trust yourself?");
				i=NTOTAL;
			}
			if(!(isntn(ntn[i].active))) {
				errormsg("You can't spy against them");
				i=NTOTAL;
			}
			if( i<NTOTAL && isactive(i)) break;
		}
		if(curntn->dstatus[i]!=UNMET) {
			P_AYLOC = ntn[i].capy;
			P_AXLOC = ntn[i].capx;
			mvprintw(LINES-2,0,"The Spy Starts in %s's Capitol (%d,%d)",
				ntn[i].name,(int)P_AXLOC,(int)P_AYLOC);
			clrtoeol();
		} else {
			clear_bottom(0);
			mvprintw(LINES-4,0,"You do not yet know where %s is",ntn[i].name);
			mvaddstr(LINES-3,0,"Have the Spy start from this sector? [y or n]");
			refresh();
			if(getch()!='y') {
				if(isgod==TRUE) reset_god();
				return;
			}
			P_AYLOC = YREAL;
			P_AXLOC = XREAL;
			mvprintw(LINES-2,0,"Intelligence indicates that %s lies to the %s",ntn[i].name,
				*(directions+compass((int)P_AXLOC,(int)P_AYLOC,
				  (int)ntn[i].capx,(int)ntn[i].capy)));
			clrtoeol();
		}
		errormsg("");
		redraw=PART;
	} else {
		P_AYLOC=YREAL;
		P_AXLOC=XREAL;
	}
	if( newtype == A_SPY || newtype == A_SCOUT ) {
		P_ASTAT = SCOUT;
		AADJSTAT;
	}
	P_ATYPE=newtype;
	if(P_ATYPE != A_MERCENARY){
		sct[XREAL][YREAL].people -= men;
		SADJCIV;
	} else {
		mercgot += men;
		AADJMERC;
	}
	AADJLOC;
	P_AMOVE=0;
	AADJMOV;
	P_ASOLD+=men;
	AADJMEN;
	if(P_ASTAT>=NUMSTATUS) {
		P_ASTAT=ATTACK;
		AADJSTAT;
	}
	curntn->metals -= i_cost;
	curntn->tgold -= e_cost;
	makemap(); /* if display 'y' is set, this will show new army */
	if(isgod==TRUE) reset_god();
}

/*go through msgfile not rewriting to temp messages you discard*/
/* then move temp to msgfile*/
void
rmessage()
{
	int unlink();
	FILE *mesgfp;
	FILE *fptemp;
	int i;
	int count,msglen;
	int contd;
	int done=FALSE;
	char tempfile[FILELTH];
	char mesgfile[FILELTH];
	char line[LINELTH+1], inpch;
	char save[LINELTH][LINELTH+1];
	struct stat fst;

	/*open file; used in mailopen() as well */
	sprintf(tempfile,"%s%hd.tmp",msgfile,country);
	if( (fptemp = fopen(tempfile,"w")) == NULL ) {
		clear_bottom(0);
		sprintf(mesgfile,"error: %s open",tempfile);
		errormsg(mesgfile);
		redraw=DONE;
		makebottom();
		return;
	}

	sprintf(mesgfile,"%s%d",msgfile,country);
	if ((mesgfp=fopen(mesgfile,"r"))==NULL) {
		(void) unlink (tempfile) ;
		clear_bottom(0);
		errormsg("No Messages");
		makebottom();
		redraw=DONE;
		return;
	}

	/* check for people sending mail */
	sprintf(line,"send.%s%hd",msgfile,country);
	if (stat(line,&fst)==0) {
		long now;
		now = time(0);
		if (now - fst.st_mtime < TIME_DEAD) {
			/* someone is sending mail to the country */
			(void) unlink (tempfile) ;
			clear_bottom(0);
			errormsg("Someone is sending you mail... please wait.");
			makebottom();
			redraw=DONE;
			return;
		} else {
			/* remove useless file */
			(void) unlink(line);
		}
	}

	/*read in file a line at at time*/
	if(fgets(line,LINELTH,mesgfp)==NULL) {
		done=TRUE;
		redraw=DONE;
		clear_bottom(0);
		errormsg("No messages");
		makebottom();
	}
	if (done==FALSE) clear();
	while(done==FALSE) {
		contd=FALSE;
		msglen=0;
		count=3;
		clear();
		standout();
		/*print to end of message*/
		while(contd==FALSE) {
			if(msglen<LINELTH) strcpy(save[msglen],line);
			if(count==LINES-3) {
				standout();
				mvaddstr(LINES-3,(COLS/2)-8,"--- more ---");
				standend();
				refresh();
				getch();
				clear();
				count=3;
			}
			mvprintw(count,0,"%s",line);
			standend();
			count++;
			msglen++;
			if(fgets(line,LINELTH,mesgfp)==NULL) contd=TRUE;
			if(strncmp(line,"END",3)==0) contd=TRUE;
		}
		standout();
		mvaddstr(LINES-3,(COLS/2)-13,"HIT ANY KEY TO CONTINUE");
		mvaddstr(LINES-2,(COLS/2)-16,"HIT RETURN TO DELETE MESSAGE");
		standend();
		refresh();
		inpch=getch();
		if((inpch!='\n' && inpch!='\r')) {
			for(i=0;i<msglen;i++) fputs(save[i],fptemp);
			strcpy(line,"END\n");
			fputs(line,fptemp);
		}
		if(fgets(line,LINELTH,mesgfp)==NULL) done=TRUE;
	}
	fclose(mesgfp);
	fclose(fptemp);

	/*IMPLEMENT A MOVE BETWEEN TMP FILE AND REAL FILE HERE*/
	move_file( tempfile, mesgfile );
#ifdef SYSMAIL
	/* increase size to ensure mail check works */
	conq_mail_size++;
#endif /* SYSMAIL */
}

void
wmessage()
{
	int x,y;
	int done=FALSE;
	char ch;
	char name[NAMELTH+1];
	int temp=(-1);
	int linedone,dotitles=TRUE;
	char line[BIGLTH];

	/*what nation to send to*/
	clear_bottom(0);
	mvaddstr(LINES-4,0,"The Conquer Administrator is 'god'; To send to the News use 'news';");
	mvaddstr(LINES-3,0,"Send mail to what nation? ");
	refresh();
	temp=get_country();

	if( temp == NEWSMAIL ) {
		strcpy(name,"news");
	} else {
		/* quick return on bad input */
		if(temp==(-1) || temp>=NTOTAL 
		|| (!isntn(ntn[temp].active) && temp!=0)) {
			makebottom();
			return;
		}
		strcpy(name,ntn[temp].name);	/* find nation name */
	}

	if(mailopen( temp )==(-1)) {
		makebottom();
		return;
	}
	redraw=FULL;

	if(temp != -2) {
		if (country==0)
		fprintf(fm,"Message to %s from GOD (%s of year %d)\n\n",name,PSEASON(TURN),YEAR(TURN));
		else	fprintf(fm,"Message to %s from %s (%s of year %d)\n\n",name,curntn->name,PSEASON(TURN),YEAR(TURN));
	} else fprintf(fm,"5.----------\n");
	strcpy(line,"");

	while(done==FALSE) {
		if (dotitles==TRUE) {
			move(0,0);
			clrtobot();
			standout();
			if(temp != -2)
				mvprintw(3,(COLS-25)/2,"Message to Nation %s",name);
			else
				mvaddstr(3,(COLS-25)/2,"Message to All Players");
			mvaddstr(LINES-2,(COLS-37)/2,"End with a <Control-D> on a New Line");
			mvaddstr(LINES-1,(COLS-28)/2,"Hit ESC to Abort the Message");
			standend();
			mvaddstr(5,0,line);
			y=6;
			x=0;
			refresh();
			dotitles=FALSE;
		}
		linedone=FALSE;
		ch=' ';
		/*read line*/
		while(linedone==FALSE){
			/* check for delete or backspace */
			switch(ch) {
			case '\b':
			case '\177':
				/* backspace or delete */
				if(x>1) x--;
				mvaddch(y,x,' ');
				move(y,x);
				line[x]=' ';
				refresh();
				ch=getch();
				break;
			case '\n':
			case '\r':
				/* newline or carriage return */
				linedone=TRUE;
				break;
			case '\004':
				/* a control-d was hit */
				if (x==1) {
					linedone=TRUE;
					done=TRUE;
				} else {
					standout();
					mvaddstr(LINES-3,(COLS-37)/2,"Hit [RETURN] Control-D to End Message");
					standend();
					move(y,x);
					refresh();
					ch = getch();
					move(LINES-3,0);
					clrtoeol();
					refresh();
				}
				break;
			case '\033':
				/* escape key was hit */
				mvaddstr(LINES-3,0,"Abort Message? ");
				refresh();
				if(getch()=='y') {
					linedone=TRUE;
					done=TRUE;
					temp=ABORTMAIL;
				} else {
					move(LINES-3,0);
					clrtoeol();
					move(y,x);
					refresh();
					ch = getch();
				}
				break;
			case '':
				/* new page -- end of form */
				wrefresh(stdscr);
				ch=getch();
				break;
			default:
				/* any remaining possibilities */
				if(isprint(ch)&&(x<65)){
					/*concatonate to end*/
					line[x]=ch;
					mvaddch(y,x,ch);
					x++;
					refresh();
				}
				ch=getch();
				break;
			}
		}
		if((ch!='\n')&&(ch!='\r')&&(ch!='\033')) {
			mvaddch(y,x,ch);
			line[x]=ch;
			x++;
		}
		line[x]='\0';

		/*check for single period */
		if (strcmp(line," .")==0) done=TRUE;

		/*write to file*/
		if (done==FALSE) {
			if (temp != -2) fprintf(fm,"%s\n",line);
			else {
				if (country!=0) fprintf(fm,"5.%-9s:%s\n",curntn->name,line);
				else fprintf(fm,"5.God      :%s\n",line);
			}
			x=0;
			y++;
			if (y==LINES-3) {
				standout();
				mvaddstr(LINES-3,0,"Continuing...");
				standend();
				refresh();
				sleep(2);
				dotitles=TRUE;
			}
		}
	}
	mailclose(temp);
}

/*strategic move of civilians...once only*/
void
moveciv()
{
	long	people;
	short	i,j;

	clear_bottom(0);
#ifdef OGOD
	if(sct[XREAL][YREAL].owner!=country && country!=0)
#else
	if(sct[XREAL][YREAL].owner!=country)
#endif
	{
		errormsg("Sorry, you don't own that sector.");
		return;
	}
	else if(sct[XREAL][YREAL].people==0){
		errormsg("Nobody lives there!!!");
		return;
	}

	mvprintw(LINES-4,0,"Sector contains %d people [cost 50 per civilian]",sct[XREAL][YREAL].people);
	mvaddstr(LINES-3,0,"How many people to move?");
	clrtoeol();
	refresh();
	people = get_number();
	if (people <= 0) {
		return;
	}
	if (people>sct[XREAL][YREAL].people) {
		errormsg("Sorry, not that many people live there.");
		return;
	}
	if (people*50>curntn->tgold) {
		errormsg("Sorry, you do not have enough gold talons.");
		return;
	}

	mvprintw(LINES-4,0,"Sector location is x=%d, y=%d",XREAL,YREAL);
	clrtoeol();
	mvaddstr(LINES-3,0,"What X location to move to?");
	clrtoeol();
	refresh();
	i = get_number();
	if (i < 0) {
		return;
	}

	if((i-(XREAL))>2||(i-(XREAL))<-2) {
		errormsg("Sorry, your people refuse to move more than two sectors.");
		return;
	}

	mvaddstr(LINES-2,0,"What Y location to move to?");
	clrtoeol();
	refresh();
	j = get_number();
	if (j < 0) {
		return;
	}
	if((j-(YREAL)>2)||((YREAL)-j>2)) {
		errormsg("Sorry, your people refuse to move more than two sectors.");
	}
#ifdef OGOD
	else if(sct[i][j].owner!=country && country!=0)
#else
	else if(sct[i][j].owner!=country)
#endif /*OGOD*/
	{
		errormsg("Sorry, you don't own that sector.");
	} else if(movecost[i][j]<0){
		/*need to check move cost > 0 for sector*/
		errormsg("Sorry, your people refuse to enter that sector.");
	} else if ((i!=XREAL)||(j!=YREAL)){
		curntn->tgold-=50*people;
		sct[XREAL][YREAL].people-=people;
		SADJCIV;
		sct[i][j].people+=people;
		SADJCIV2;
	}
}

int
armygoto()
{
	short armynum=0,loop=0;
	armynum=getselunit();
	if((armynum<0)||(armynum>MAXARM)) armynum=0;
	else armynum++;
	/* move to next army with > 0 soldiers or army not in group */
	while((armynum < MAXARM) && ((P_ASOLD <= 0) || (P_ASTAT>=NUMSTATUS)))
		armynum++;
	if(armynum >= MAXARM)  {
		armynum=0;
		if(P_ASOLD <= 0) return(0);
	}
	/*move to correct location*/
	xcurs = (int)P_AXLOC - xoffset;
	ycurs = (int)P_AYLOC - yoffset;
	coffmap();

	/*select correct unit*/
	selector=0;
	pager=0;
	while((getselunit() != armynum) && (loop++ < 500)) {
		selector+=2;
		if(selector>=10) {
			selector=0;
			pager+=1;
		}
	}
	return(1);
}

int
navygoto()
{
	short nvynum=0,loop=0;
	nvynum=getselunit()-MAXARM;
	if((nvynum<0)||(nvynum>MAXNAVY)) nvynum=0;
	else nvynum++;
	/* move to next nvy with > 0 soldiers*/
	while ((nvynum < MAXNAVY) && (P_NMSHP==0) && (P_NWSHP==0)
	&& (P_NGSHP == 0)) nvynum++;
	if(nvynum >= MAXNAVY) {
		nvynum=0;
		if((P_NMSHP==0)&&(P_NWSHP==0)&&(P_NGSHP == 0)) return(0);
	}
	/*move to correct location*/
	xcurs = P_NXLOC - xoffset;
	ycurs = P_NYLOC - yoffset;
	coffmap();

	/*select correct unit*/
	selector=0;
	pager=0;
	while(((getselunit()-MAXARM) != nvynum) && (loop++ < 500)) {
		selector+=2;
		if(selector>=10) {
			selector=0;
			pager+=1;
		}
	}
	return(1);
}
