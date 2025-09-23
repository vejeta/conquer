/*Print and io subroutines for interactive game*/

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

/*include files*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <curses.h>
#include "header.h"
#include "data.h"

/*offset of upper left hand corner*/
extern short xoffset;
extern short yoffset;
/*current cursor postion (relative to 00 in upper corner)*/
/*	position is 2*x,y*/
extern short xcurs;
extern short ycurs;
/*redraw map in this turn if redraw is a 1*/
extern short redraw;
/*display state SEE data.h FOR CURRENT VALUES OF THESE */
extern short hilmode;
extern short dismode;
extern short otherdismode;
extern short otherhilmode;

/* nation id of owner*/
extern short country;

static char *hasseen;

/* allocate space for the hasseen array based on the actual screen size */
void
init_hasseen()
{
	hasseen = (char *)malloc(((COLS-10)/2) * (LINES-5));
#ifdef BSD
	bzero(hasseen,((COLS-10)/2) * (LINES-5));
#else
	memset( hasseen, 0, ((COLS-10)/2) * (LINES-5));
#endif
	if (hasseen == (char *)NULL) {
		errormsg("Cannot allocate memory.");
		bye(FALSE);
	}
}

/*make a map*/
void
makemap()
{
	register int x,y;

	for(x=0;x<SCREEN_X_SIZE;x++) for(y=0;y<SCREEN_Y_SIZE;y++)
	if( HAS_SEEN(x,y) ) {
		highlight(x,y,hilmode);
		see(x,y);
	}
	move(ycurs,2*xcurs);
}


void
get_display_mode(dmode,hmode,odmode,ohmode)
	short *dmode, *hmode, *odmode, *ohmode;
{
	short temp;
	
	mvaddstr(LINES-4,0,"viewing options:  (d)esignation, (r)ace, (M)ove cost, (p)eople, (D)efense");
	clrtoeol();
	mvaddstr(LINES-3,0,"   (f)ood, (c)ontour, (v)egetation, (m)etal, (n)ation mark, (j)ewels, (i)tems");
	clrtoeol();
	mvaddstr(LINES-2,0,"highlight option: (o)wned, (a)rmy, (y)our Army, move (l)eft, (s)pecial,(x)=none");
	clrtoeol();
	if ( odmode !=NULL) {
		mvaddstr(LINES-1,0,"Toggle second display: '/'; set display: '1' or '2'");
	} else {
		mvprintw(LINES-1,0,"     choose selection for the %s display",
			(*ohmode == 1) ? "primary" : "secondary");
	}
	clrtoeol();
	standout();
	mvaddstr(LINES-1,COLS-25,odmode!=NULL?"what display?:":"display?:");
	standend();
	refresh();
	redraw=PART;
	switch(getch()) {
	case '/':
		if (odmode !=NULL)
			*odmode = - *odmode;
		else
			goto error;
		break;
	case '2':
		temp = 2;
		if (odmode !=NULL)
			get_display_mode(odmode,ohmode,NULL,&temp);
		else
			goto error;
		break;
	case '1':
		temp = 1;
		if (odmode !=NULL)
			get_display_mode(dmode,hmode,NULL,&temp);
		else
			goto error;
		break;
	case 'f':
		*dmode=DI_FOOD;
		goto changed_display;
	case 'v':	/* vegetation map*/
		*dmode=DI_VEGE;
		goto changed_display;
	case 'd':	/* designations map*/
		*dmode=DI_DESI;
		goto changed_display;
	case 'c':	/* contour map of world */
		*dmode=DI_CONT;
		goto changed_display;
	case 'n':	/* nations map*/
		*dmode=DI_NATI;
		goto changed_display;
	case 'r':	/* race map*/
		*dmode=DI_RACE;
		goto changed_display;
	case 'M': /* move cost map */
		*dmode=DI_MOVE;
		goto changed_display;
	case 'D':
		*dmode=DI_DEFE;
		goto changed_display;
	case 'p':
		*dmode=DI_PEOP;
		goto changed_display;
	case 'j':
		*dmode=DI_GOLD;
		goto changed_display;
	case 'm':
		*dmode=DI_METAL;
		goto changed_display;
	case 'i':
		*dmode=DI_ITEMS;
	changed_display:
		if ( odmode !=NULL && *odmode > 0) /* just 'd' not 'd1 or 'd2' */
			*odmode= - *odmode;
		break;
	case 'a':	/* armies hilighted map*/
		prep(country,FALSE);
		*hmode=HI_ARMY;
		goto changed_highlight;
	case 'o':	/* owners hilighted map*/
		*hmode=HI_OWN;
		goto changed_highlight;
	case 's':	/* hilight tradegoods */
		*hmode=HI_GOOD;
		goto changed_highlight;
	case 'x':	/*no highlighting*/
		*hmode=HI_NONE;
		goto changed_highlight;
	case 'y':	/* your armies hilighted map*/
		prep(country,FALSE);
		*hmode=HI_YARM;
		goto changed_highlight;
	case 'l':	/* your armies with moves left hilighted map*/
		prep(country,FALSE);
		*hmode=HI_MOVE;
	changed_highlight:
		if ( odmode !=NULL ) /* just 'd' not 'd1 or 'd2' */
			*ohmode= *hmode;
		break;
	default:
	error:
		beep();
		redraw=DONE;
	}
}

void
newdisplay()
{
	get_display_mode(&dismode,&hilmode,&otherdismode,&otherhilmode);
	makebottom();
}

/*see what is in xy as per display modes*/
char
get_display_for(x,y,dmode)
	int x,y;
	short dmode;
{
	int armbonus;

	char ch= '{';
	if((magic(sct[x+xoffset][y+yoffset].owner,THE_VOID)==TRUE)
	   &&((dmode==DI_DEFE)||(dmode==DI_GOLD)||(dmode==DI_METAL)
		 ||(dmode==DI_PEOP)||(dmode==DI_FOOD)||(dmode==DI_ITEMS))
	   &&(country!=sct[x+xoffset][y+yoffset].owner)
	   &&(magic(country,NINJA)!=TRUE)
	   &&(country!=0)) {
		ch='?';
	} else {
		switch(dmode){
		case DI_FOOD:	/*food */
			if(tofood( &sct[x+xoffset][y+yoffset],country)==0)
				ch=sct[x+xoffset][y+yoffset].vegetation;
			else if (tofood( &sct[x+xoffset][y+yoffset],country)<10)
				ch=tofood( &sct[x+xoffset][y+yoffset],country)+'0';
			else ch='+';
			break;
		case DI_VEGE: /*vegetation*/
			ch=sct[x+xoffset][y+yoffset].vegetation;
			break;
		case DI_DESI: /*designation*/
			if(sct[x+xoffset][y+yoffset].owner==0){
				if(tofood( &sct[x+xoffset][y+yoffset],country)!=0)
					ch=sct[x+xoffset][y+yoffset].altitude;
				else ch=sct[x+xoffset][y+yoffset].vegetation;
			}
			else if((country==0)
				   ||(sct[x+xoffset][y+yoffset].owner==country))
				ch=sct[x+xoffset][y+yoffset].designation;
			else ch=ntn[sct[x+xoffset][y+yoffset].owner].mark;
			break;
		case DI_CONT: /*contour*/
			ch=sct[x+xoffset][y+yoffset].altitude;
			break;
		case DI_NATI: /*ownership*/
			if(sct[x+xoffset][y+yoffset].owner==0)
				ch=sct[x+xoffset][y+yoffset].altitude;
			else ch=ntn[sct[x+xoffset][y+yoffset].owner].mark;
			break;
		case DI_RACE: /*race*/
			if(sct[x+xoffset][y+yoffset].owner==0)
				ch=sct[x+xoffset][y+yoffset].altitude;
			else ch=ntn[sct[x+xoffset][y+yoffset].owner].race;
			break;
		case DI_MOVE:	/*movement cost map*/
			if(movecost[x+xoffset][y+yoffset]>=0) {
				if(movecost[x+xoffset][y+yoffset]>=10)
					ch='+';
				else ch=movecost[x+xoffset][y+yoffset]+'0';
			} else if(sct[x+xoffset][y+yoffset].altitude==WATER)
				ch=WATER;
			else
				ch='X';
			break;
		case DI_DEFE:	 /*Defence*/
			if (sct[x+xoffset][y+yoffset].altitude==WATER)
				ch=WATER;
			else if(movecost[x+xoffset][y+yoffset]<0)
				ch='X';
			else {
				/*Racial combat bonus due to terrain (the faster you move the better)*/
				armbonus=0;
				armbonus+=5*(9-movecost[x+xoffset][y+yoffset]);
				
				if(sct[x+xoffset][y+yoffset].altitude==MOUNTAIN)
					armbonus+=40;
				else if(sct[x+xoffset][y+yoffset].altitude==HILL)
					armbonus+=20;

				if(sct[x+xoffset][y+yoffset].vegetation==JUNGLE)
					armbonus+=30;
				else if(sct[x+xoffset][y+yoffset].vegetation==FOREST)
					armbonus+=20;
				else if(sct[x+xoffset][y+yoffset].vegetation==WOOD)
					armbonus+=10;

				armbonus+=fort_val(&sct[x+xoffset][y+yoffset]);
				
				if(armbonus<200) ch=armbonus/20+'0';
				else ch='+';
			}
			break;
		case DI_PEOP:	 /*People*/
			if (sct[x+xoffset][y+yoffset].altitude==WATER)
				ch=WATER;
			else if (sct[x+xoffset][y+yoffset].people>=9950)
				ch='X';
			else if (sct[x+xoffset][y+yoffset].people>=4950)
				ch='V';
			else if (sct[x+xoffset][y+yoffset].people>=950)
				ch='I';
			else
				ch=(50+sct[x+xoffset][y+yoffset].people)/100+'0';
			break;
		case DI_GOLD:	/*Gold*/
			if (sct[x+xoffset][y+yoffset].altitude==WATER)
				ch=WATER;
			else if(tofood( &sct[x+xoffset][y+yoffset],country)==0)
				ch='X';
			else if (tg_ok(country,&sct[x+xoffset][y+yoffset])){
				if (sct[x+xoffset][y+yoffset].jewels>=10)
					ch='+';
				else
					ch=sct[x+xoffset][y+yoffset].jewels+'0';
			} else ch='0';
			break;
		case DI_METAL:	 /*Metal*/
			if (sct[x+xoffset][y+yoffset].altitude==WATER)
				ch=WATER;
			else if(tofood( &sct[x+xoffset][y+yoffset],country)==0)
				ch='X';
			else if (tg_ok(country,&sct[x+xoffset][y+yoffset])){
				if (sct[x+xoffset][y+yoffset].metal>=10)
					ch='+';
				else
					ch=sct[x+xoffset][y+yoffset].metal+'0';
			} else ch='0';
			break;
		case DI_ITEMS:	/* designations needed for tradegoods */
			if (sct[x+xoffset][y+yoffset].altitude==WATER)
				ch=WATER;
			else if(tofood( &sct[x+xoffset][y+yoffset],country)==0)
				ch='X';
			else if (sct[x+xoffset][y+yoffset].tradegood!=TG_none
			&& (*(tg_stype+sct[x+xoffset][y+yoffset].tradegood)!='x')
				&& tg_ok(country,&sct[x+xoffset][y+yoffset]))
				ch= *(tg_stype+sct[x+xoffset][y+yoffset].tradegood);
			else ch='-';
			break;
		default:
			break;
		}
	}
	return ch;
}

void
see(x,y)
{
	char ch;
	if((x<0)||(y<0)||(x>=SCREEN_X_SIZE)||(y>=SCREEN_Y_SIZE)
	||((y+yoffset)>=MAPY)||((x+xoffset)>=MAPX)) return;
	if(((y+yoffset)<MAPY)&&((x+xoffset)<MAPX)) {
		ch=get_display_for(x,y,dismode);
		mvaddch(y,2*x,ch);
		if ( otherdismode > 0 ) {
			highlight(x,y,otherhilmode);
			ch=get_display_for(x,y,otherdismode);
			mvaddch(y,2*x+1,ch);
		}
	}
	else {
		mvaddch(y,2*x,' ');
		mvaddch(y,2*x+1,' ');
	}
}

/*highlight what is in xy as per highlight mode*/
void
highlight(x,y,hmode)
	short hmode;
{
	int	armynum;
	if((x<0)||(y<0)||(x>=SCREEN_X_SIZE)||(y>=SCREEN_Y_SIZE)
	||((y+yoffset)>=MAPY)||((x+xoffset)>=MAPX)) return;
	standend();
	switch(hmode){
	case HI_MOVE:	/* your armies w/ move left */
		for(armynum=0;armynum<MAXARM;armynum++)
			if(( P_ASOLD != 0 )
			&&( P_AMOVE != 0 )
			&&( P_AXLOC==(x+xoffset ))
			&&( P_AYLOC==(y+yoffset ))) break;
		if(armynum<MAXARM) standout();
		break;
	case HI_YARM:	/* your armies */
		for(armynum=0;armynum<MAXARM;armynum++)
			if(( P_ASOLD != 0)
			&&( P_AXLOC==x+xoffset)
			&&( P_AYLOC==y+yoffset)) break;
		if(armynum<MAXARM) standout();
		break;
	case HI_GOOD:	/* trade goods */
		if(tg_ok( country, &sct[x+xoffset][y+yoffset])
		   &&(sct[x+xoffset][y+yoffset].tradegood != TG_none)
		   &&(magic(sct[x+xoffset][y+yoffset].owner,THE_VOID)!=TRUE
		    || country==0
		    || country==sct[x+xoffset][y+yoffset].owner
		    || magic(country,NINJA)==TRUE)
		   &&(sct[x+xoffset][y+yoffset].altitude!=WATER)) standout();
		break;
	case HI_OWN: /* ownership */
		if(country==0) {
			if(sct[x+xoffset][y+yoffset].owner!=0)
				standout();
		} else if(sct[x+xoffset][y+yoffset].owner==country)
			standout();
		break;
	case HI_ARMY: /* any armies */
		if(occ[x+xoffset][y+yoffset]!=0) standout();
		break;
	default:
		break;
	}
}

/* check if cursor is out of bounds*/
void
coffmap()
{
	if( xcurs<0 || (XREAL!=0 && xcurs==0) || (ycurs<0) ||
	   (YREAL!=0 && ycurs==0) || xcurs>=SCREEN_X_SIZE-1 ||
	   ycurs>=SCREEN_Y_SIZE-1 || XREAL>=MAPX || YREAL>=MAPY) {
		centermap();
		redraw=PART;
	}
	
	if(redraw!=DONE) {
		if (redraw==FULL) {
			clear();	/* clear real screen */
		} else {
			move(0,0);	/* clear curses screen but not real screen */
			clrtobot();
		}
		makemap();	/* update map */
		makebottom();
		redraw=DONE;
	}
	move(ycurs,2*xcurs);
	makeside(FALSE);  /*update side*/
	move(ycurs,2*xcurs);
	refresh();
}

int
canbeseen(x,y)
int	x,y;
{
	if(!ONMAP(x,y)) return(FALSE);
	return( (int) HAS_SEEN(x-xoffset,y-yoffset) );
}

/** CANSEE() fills seen[SCREEN_X_SIZE][SCREEN_Y_SIZE] */
void
whatcansee()
{
	register int x,y;
	int	i,j;
	short	armynum,nvynum;

	if((magic(country,KNOWALL)==TRUE)||(country==0)) {
		for(x=0;x<SCREEN_X_SIZE;x++) for(y=0;y<SCREEN_Y_SIZE;y++)
			HAS_SEEN(x,y)=TRUE;
		return;
	}
	for(x=0;x<SCREEN_X_SIZE;x++) for(y=0;y<SCREEN_Y_SIZE;y++)
		HAS_SEEN(x,y)=FALSE;

	for(x=(-LANDSEE);(x<SCREEN_X_SIZE+LANDSEE);x++)
	for(y=(-LANDSEE);(y<SCREEN_Y_SIZE+LANDSEE);y++)
	if((ONMAP(x+xoffset,y+yoffset))
	&&(sct[x+xoffset][y+yoffset].owner==country)){
		for(i=x-LANDSEE;i<=x+LANDSEE;i++)
		for(j=y-LANDSEE;j<=y+LANDSEE;j++)
		if(i>=0 && j>=0 && i<SCREEN_X_SIZE && j<SCREEN_Y_SIZE)
			if (ONMAP(i+xoffset,j+yoffset))
				HAS_SEEN(i,j)=TRUE;
	}

	for(nvynum=0;nvynum<MAXNAVY;nvynum++)
	if((P_NMSHP!=0)||(P_NWSHP!=0)||(P_NGSHP!=0))
		for(i=(int)P_NXLOC-xoffset-NAVYSEE;i!=1+(int)P_NXLOC-xoffset+NAVYSEE;i++)
		for(j=(int)P_NYLOC-yoffset-NAVYSEE;j!=1+(int)P_NYLOC-yoffset+NAVYSEE;j++)
			if(ONMAP(i+xoffset,j+yoffset) && i>=0 && j>=0
			&& i<SCREEN_X_SIZE && j<SCREEN_Y_SIZE)
				HAS_SEEN(i,j)=TRUE;

	for(armynum=0;armynum<MAXARM;armynum++)
		if(P_ASOLD>0)
		for(i=(int)P_AXLOC-xoffset-ARMYSEE;i!=1+(int)P_AXLOC-xoffset+ARMYSEE;i++)
		for(j=(int)P_AYLOC-yoffset-ARMYSEE;j!=1+(int)P_AYLOC-yoffset+ARMYSEE;j++)
		if(ONMAP(i+xoffset,j+yoffset) && i>=0 && j>=0
		&& i<SCREEN_X_SIZE && j<SCREEN_Y_SIZE)
			HAS_SEEN(i,j)=TRUE;

	return;
}
