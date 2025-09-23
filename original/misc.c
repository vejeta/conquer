/* Conquer: Copyright (c) 1988 by Edward M Barlow */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <strings.h>
#ifdef CONQUER
#include <sys/types.h>
#include <sys/stat.h>
#endif /*CONQUER*/
#include "header.h"
#include "data.h"

extern FILE *fnews;
extern short country,redraw;

extern char *HVegcost, *OVegcost, *EVegcost, *DVegcost, *FVegcost;
extern char *HElecost, *OElecost, *EElecost, *DElecost, *FElecost;

#ifdef SYSV
char    *memset();
#endif

#ifdef CONQUER
int
move_file( from, to )
register char	*from;
register char	*to;
{
	if( unlink( to ) < 0 ) {
		fprintf( stderr, "unlink( %s ) failed \n", to );
		sleep( 2 );
		return( -1 );
	}

	if( link( from, to ) < 0 ) {
		fprintf( stderr, "link( %s, %s ) failed \n", from, to );
		sleep( 2 );
		return( -1 );
	}

	if( unlink( from ) < 0 ) {
		fprintf( stderr, "unlink( %s ) failed \n", from );
		sleep( 2 );
		return( -1 );
	}

	return( 0 );
} /* move_file() */
#endif /* CONQUER */

/* returns integer input greater than zero or */
/* -1 for no input.                           */
long
get_number()
{
	long sum=0;
	char ch;
	int done=FALSE,count=0,xpos,ypos;
	/* this routine totally redone to allow deleting */
	while(!done) {
		ch=getch();
		if(isdigit(ch) && count<12) {
			/* only print numbers to the screen */
			addch(ch);
			refresh();
			sum *= 10L;
			count++;
			sum += (long)(ch-'0');
		} else if ((ch=='\b' || ch=='\177')&&(count)) {
			/* only delete what was printed */
			getyx(stdscr,ypos,xpos);
			move(ypos,--xpos);
			addch(' ');
			move(ypos,xpos);
			refresh();
			sum /= 10L;
			count--;
		} else if((ch=='\n')||(ch=='\r')) {
			done=TRUE;
		}
	}
	if (count==0) {
		return(-1);
	}
	return( sum );
}

#define INFINITE	1000

int		bx;		/* destination 'x' coordinate */
int		by;		/* destination 'y' coordinate */
int		moving_country;	/* country that is moving */

#define MAX_MOVE_UNITS	0x7f
unsigned char	**history_reachp;
int	level;

/*
 *	land_2reachp()
 */

int
land_2reachp( ax, ay, move_points )
int	ax;
int	ay;
int	move_points;
{
	register int	i = 0;
	int	delta_x, delta_y;
	int	x_abs_delta, y_abs_delta;
	int	own;
	int	dx[ 8 ];
	int	dy[ 8 ];

	delta_x = bx - ax;
	delta_y = by - ay;

	/* Have we got where we are going? */
	if( delta_x == 0 && delta_y == 0 ) {
		return( 1 );
	}

	/* Any move points left? (optimization) */
	if( move_points == 0 ) {
		return( 0 );
	}

	x_abs_delta = (delta_x < 0) ? -delta_x : delta_x;
	y_abs_delta = (delta_y < 0) ? -delta_y : delta_y;

	/* couldn't reach if all moves cost 1 (optimization) */
	if( max( x_abs_delta, y_abs_delta ) > move_points ) {
		return( 0 );
	}

	{
		register int	inc_x;
		register int	inc_y;

		inc_x = (delta_x < 0 ) ? -1 : 1;
		inc_y = (delta_y < 0 ) ? -1 : 1;

		/*I HAVE CHANGED THIS CODE FROM THE ORIGINAL TO OPTIMIZE IT*/
		/*I think it should work well*/
		if( y_abs_delta == 0) {
			/* try 'x' movements first */
			dx[i] = inc_x; dy[i++] = 0;
			dx[i] = inc_x; dy[i++] = inc_y;
			dx[i] = inc_x; dy[i++] = -inc_y;
			dx[i] = 0; dy[i++] = inc_y;
			dx[i] = 0; dy[i++] = -inc_y;
			dx[i] = -inc_x; dy[i++] = inc_y;
			dx[i] = -inc_x; dy[i++] = 0;
			dx[i] = -inc_x; dy[i++] = -inc_y;
		} else if( x_abs_delta == 0 ) {
			/* try 'y' movements first */
			dx[i] = 0; dy[i++] = inc_y;
			dx[i] = inc_x; dy[i++] = inc_y;
			dx[i] = -inc_x; dy[i++] = inc_y;
			dx[i] = inc_x; dy[i++] = 0;
			dx[i] = -inc_x; dy[i++] = 0;
			dx[i] = inc_x; dy[i++] = -inc_y;
			dx[i] = 0; dy[i++] = -inc_y;
			dx[i] = -inc_x; dy[i++] = -inc_y;
		} else {	/* x_abs_delta != 0, 0 != y_abs_delta */
			/* try diagonal movements first */
			dx[i] = inc_x; dy[i++] = inc_y;

			dx[i] = 0; dy[i++] = inc_y;
			dx[i] = inc_x; dy[i++] = 0;

			dx[i] = -inc_x; dy[i++] = inc_y;
			dx[i] = inc_x; dy[i++] = -inc_y;

			dx[i] = -inc_x; dy[i++] = 0;
			dx[i] = 0; dy[i++] = -inc_y;

			dx[i] = -inc_x; dy[i++] = -inc_y;
		} /* if */
	} /* block */

	{
		register int	x, y;
		register int	new_mp;

		for( i = 0; i < 8; i++ ) {
			if( (x = ax + dx[i]) < 0 || x >= MAPX )
				continue;
			if( (y = ay + dy[i]) < 0 || y >= MAPY )
				continue;

			if ( movecost[ x ][ y ] < 0 )	/* just in case */
				continue;
			if( sct[x][y].altitude == PEAK)
				continue;
			if( sct[x][y].altitude == WATER)
				continue;

			new_mp = move_points - movecost[ x ][ y ];
			if( new_mp < 0 )
				continue;

			/*
			*	If we have been to this sector before
			*	in fewer move points this path is not
			*	going to do any better.
			*/
			if( history_reachp[x][y] >= new_mp ) {
				continue;
			}
			history_reachp[x][y] = new_mp;

			/*
			*	Test for a hostile army
			*/
			/* BUG: should engage if army is hostile but does not own sector */
			/* BUG: take into account THE_VOID, HIDDEN, and NINJA */
			if( (own = sct[x][y].owner) > 0 &&
			ntn[own].dstatus[moving_country] >= WAR &&
			x != bx && y != by &&
			solds_in_sector( x, y, own ) > 0 ) {
				continue;	/* at war with the owner, may not pass */
			}

			if( own > 0 &&
			ntn[moving_country].dstatus[own] < WAR &&
			ntn[own].dstatus[moving_country] > ALLIED &&
			ntn[own].dstatus[moving_country] < WAR) {
				/* not at war with owner & owner is neutral,no passing*/
				continue;
			}

			level++;
			if( land_2reachp( x, y, new_mp ) ) {
				level--;
				return( 1 );
			} /* if */
			level--;
		} /* for */
	} /* block */
	return( 0 );
} /* land_2reachp() */

/*
 *	land_reachp()
 */
#ifdef ADMIN
int
land_reachp( ax, ay, gx, gy, move_points, movee )
int	ax;
int	ay;
int	gx;
int	gy;
int	move_points;
int	movee;
{
	int	result;

	if( move_points >= MAX_MOVE_UNITS ) {
		fprintf( stderr, "land_reachp(): move_points = %d\n",
			move_points );

		abrt();
	}

	/* Are we starting or ending in the water or on a peak? */
	if( sct[ax][ay].altitude == WATER || sct[ax][ay].altitude == PEAK )
		return( 0 );
	if( sct[gx][gy].altitude == WATER || sct[gx][gy].altitude == PEAK )
		return( 0 );

	history_reachp = (unsigned char **) m2alloc(MAPX,MAPY,sizeof(char));
#ifdef BSD
	bzero((char *) *history_reachp,MAPX*MAPY);
#else
	memset((char *) *history_reachp, 0, MAPX*MAPY );
#endif

	history_reachp[ax][ay] = move_points;

	bx = gx;
	by = gy;
	moving_country = movee;

	level = 1;
	result = land_2reachp( ax, ay, move_points );
	free(history_reachp);
	return( result );
} /* land_reachp() */
#endif /* ADMIN */
#ifdef ADMIN
/*
 *	water_2reachp()
 */

int
water_2reachp( ax, ay, move_points )
int	ax;
int	ay;
int	move_points;
{
	register int	i = 0;
	int	delta_x;
	int	delta_y;
	int	dx[ 8 ];
	int	dy[ 8 ];

	/* this path uses too many move units */
	if( move_points < 0 )
		return( 0 );

	/*
	*	If we have been to this sector before in fewer move points
	*	this path is not going to do any better.
	*/
	if( history_reachp[ ax ][ ay ] <= move_points )
		return( 0 );

	history_reachp[ ax ][ ay ] = move_points;

	delta_x = ax - bx;
	delta_y = ay - by;

	/* Have we got where we are going? */
	if( delta_x == 0 && delta_y == 0 )
		return( 1 );

	/* Have we run into ground, but not reached our destination? */
	if( sct[ax][ay].altitude != WATER )
		return( 0 );

	/* Any move points left? (optimization) */
	if( move_points == 0 )
		return( 0 );

	/* couldn't reach if all moves cost 1 (optimization) */
	if( max( abs( delta_x ), abs( delta_y ) ) > move_points )
		return( 0 );

	/* BUG: test for an enemy navy */

	{
		register int	inc_x;
		register int	inc_y;

		inc_x = (delta_x < 0 ) ? -1 : (delta_x > 0) ? 1 : 0;
		inc_y = (delta_y < 0 ) ? -1 : (delta_y > 0) ? 1 : 0;

		if( abs(delta_x) > abs(delta_y) ) {
			/* try 'x' movements first */
			dx[i] = inc_x; dy[i++] = 0;
			dx[i] = inc_x; dy[i++] = inc_y;
			dx[i] = inc_x; dy[i++] = -inc_y;
			dx[i] = 0; dy[i++] = inc_y;
			dx[i] = 0; dy[i++] = -inc_y;
			dx[i] = -inc_x; dy[i++] = inc_y;
			dx[i] = -inc_x; dy[i++] = 0;
			dx[i] = -inc_x; dy[i++] = -inc_y;
		} else {	/* abs(delta_x) < abs(delta_y) */
			/* try 'y' movements first */
			dx[i] = 0; dy[i++] = inc_y;
			dx[i] = inc_x; dy[i++] = inc_y;
			dx[i] = -inc_x; dy[i++] = inc_y;
			dx[i] = inc_x; dy[i++] = 0;
			dx[i] = -inc_x; dy[i++] = 0;
			dx[i] = inc_x; dy[i++] = -inc_y;
			dx[i] = 0; dy[i++] = -inc_y;
			dx[i] = -inc_x; dy[i++] = -inc_y;
		} /* if */
	} /* block */

	{
		register int	x, y;
		register int	new_mp;

		for( i = 0; i < 8; i++ ) {
			if( (x = ax + dx[i]) < 0 || x >= MAPX )
				continue;
			if( (y = ay + dy[i]) < 0 || y >= MAPY )
				continue;

			new_mp = move_points - 1;
			if( new_mp < 0 )
				continue;

			if( water_2reachp( x, y, new_mp ) )
				return( 1 );
		} /* for */
	} /* block */

	return( 0 );
} /* water_2reachp() */
#endif /* ADMIN */
#ifdef XYZ	/* XYZ never is defined */
/*
 *	water_reachp()
 */

int
water_reachp( ax, ay, gx, gy, move_points, movee )
int	ax;
int	ay;
int	gx;
int	gy;
int	move_points;
int	movee;
{
	if( move_points >= MAX_MOVE_UNITS ) {
		fprintf( stderr, "water_reachp(): move_points = %d\n",
			move_points );

		abrt();
	}

#ifdef SYSV
	memset(history_reachp, MAX_MOVE_UNITS, MAPX*MAPY*sizeof(history_reachp));
#else
	{ register int i,j;
		for (i=0; i < MAPX ; i++)
		for (j=0; j < MAPY ; j++ )
			history_reachp [i] [j] = MAX_MOVE_UNITS ;
	}/* eof memset replacement block */
#endif

	history_reachp[ ax ][ ay ] = 0;

	bx = gx;
	by = gy;
	moving_country = movee;

	return( water_2reachp( ax, ay, move_points ) );
} /* water_reachp() */
#endif /* 0 */

/*
 *	solds_in_sector()
 */

long
solds_in_sector( x, y, nation )
int	x;
int	y;
int	nation;
{
	register struct s_nation	*nptr = &ntn[nation];
	register int	j;
	long	total = 0;

	for( j = 0; j < MAXARM; j++ ) {
		if( nptr->arm[j].sold == 0 )
			continue;

		if( nptr->arm[j].xloc == x && nptr->arm[j].yloc == y )
			total += nptr->arm[j].sold;
	}

	return( total );
} /* solds_in_sector() */
#ifdef ADMIN

/* score_one()	*/
struct wght {
	int	sectors;
	int	civilians;
	int	soldiers;
	int	gold;
	int	jewels;
	int	metal;
	int	magics;
	int	ships;
} weights[] = {
/*		Per 2  1000    1000      100K 100K   100K   Magic  10 */
/*	Races   Sector People  Soldiers  Gold Jewels Iron   Power Ship */
/* NPC */	{ 2,    1,     0,        0,    1,    1,     1,    0 },
/* kingdom */	{ 2,    1,     2,        3,    0,    0,     0,    0 },
/* empire */	{ 3,    0,     0,        1,    1,    0,     0,    0 },
/* wizard */	{ 0,    2,     1,        0,    3,    5,     7,   0 },
/* theocracy */	{ 2,    1,     0,        0,    3,    0,     3,    0 },
/* pirate */	{ 0,    0,     5,        0,    10,   10,    1,    5 },
/* trader */	{ 2,    1,     0,        0,    1,    1,     1,    8 },
/* warlord */	{ 2,    1,     2,        0,    1,    1,     1,    0 },
/* demon */	{ 2,    0,     1,        0,    1,    0,     5,   0 },
/* dragon */	{ 0,    0,     0,        10,   20,   0,     0,    0 },
/* shadow */	{ 2,    0,     0,        0,    0,    5,     0,    0 },
/* miner */	{ 0,    0,     5,        0,    10,   10,   1,    5 },
};

long
score_one( nation )
int	nation;
{
	struct	s_nation	*nptr = &ntn[ nation ];
	long	total = 0;
	int	bonus;
	struct	wght	*wght = &weights[ nptr->class ];

	total += wght->sectors * nptr->tsctrs / 2L;
	total += wght->civilians * nptr->tciv / 1000L;
	total += wght->soldiers * nptr->tmil / 1000L;
	if(nptr->tgold > 0 ) total += wght->gold * (nptr->tgold / 100000L);
	total += wght->jewels * nptr->jewels / 100000L;
	total += wght->metal * nptr->metals / 100000L;
	total += wght->magics * num_powers(nation,M_MIL);
	total += wght->magics * num_powers(nation,M_CIV);
	total += wght->magics * num_powers(nation,M_MGK);
	total += wght->ships * nptr->tships / 10L;
	switch( nptr->class ) {
	case 	C_KING:
			bonus=(nptr->popularity+nptr->prestige-nptr->poverty);
			break;
	case	C_EMPEROR:
			bonus=(nptr->power+nptr->prestige-nptr->poverty);
			break;
	case	C_WIZARD:
			bonus=(nptr->knowledge+nptr->power-50);
			break;
	case	C_PRIEST:
			bonus=(nptr->wealth+nptr->terror-nptr->poverty);
			break;
	case	C_PIRATE:
			bonus=(nptr->reputation+nptr->wealth-50);
			break;
	case	C_TRADER:
			bonus=(nptr->wealth+nptr->prestige-nptr->tax_rate*5);
			break;
	case	C_WARLORD:
			bonus=(nptr->reputation+nptr->prestige-50);
			break;
	case	C_DEMON	:
			bonus=(nptr->knowledge+nptr->terror-50);
			break;
	case	C_DRAGON:
			bonus=(nptr->wealth+nptr->terror-50);
			break;
	case	C_SHADOW:
			bonus=(nptr->power+nptr->terror-50);
			break;
	default:	bonus=0;
	}
	total += bonus/10;
	return( total );
} /* score_one() */
#endif /* ADMIN */
/*
 *	print_accum()
 */

	/* max number of print_accum() calls in one printf() */
#define MAX_BUFFER	4
#define BUFFER_SIZE	20

/* is_habitable() - returns TRUE/FALSE if habitable */
int
is_habitable( x, y )
int	x;
int	y;
{
	char	temp;

	if(( (temp=sct[x][y].altitude)==WATER )||( temp==PEAK )) return(FALSE);

	if(((temp=sct[x][y].vegetation)==BARREN )
	|| ( temp==LT_VEG )
	|| ( temp==GOOD )
	|| ( temp==WOOD )
	|| ( temp==FOREST )) return( TRUE );

	return( FALSE );
}

#ifdef CONQUER
int
units_in_sector(x,y,nation)
int	x;
int	y;
{
	int count=0, armynum, nvynum;
	struct	s_nation  *nptr = curntn;

	curntn = &ntn[nation];
	for(armynum=0;armynum<MAXARM;armynum++)
		if((P_ASOLD>0)&&(P_AXLOC==x)&&(P_AYLOC==y)) count++;
	for(nvynum=0;nvynum<MAXNAVY;nvynum++)
		if(((P_NWSHP+P_NMSHP+P_NGSHP)!=0)&&
		(P_NXLOC==x)&&(P_NYLOC==y)) count++;

	curntn = nptr;
	return(count);
}
#endif /* CONQUER */

int
num_powers(nation,type)
int nation,type;
{
	int	count_magic=0;
	int	try;
	long	start, end;
	switch(type){
		case M_MGK:
			start=S_MGK;
			end=E_MGK;
			break;
		case M_CIV:
			start=S_CIV;
			end=E_CIV;
			break;
		case M_MIL:
			start=S_MIL;
			end=E_MIL;
			break;
		case M_ALL:
			start=S_MIL;
			end=E_MGK;
			break;
		default:
			fprintf(stderr,"fatal error in num_powers");
			abrt();
	}
	for( try = start; try < start+end; try++ )
		if( magic(nation, powers[try] ) == 1 ) count_magic++;
	return(count_magic);
}

/* returns food value of sector */
/* 4 is limit of livable land */
int
tofood(sptr,cntry)
struct s_sector *sptr;
int	cntry;
{
	register int i=0;
	register int foodvalue;
	while( sptr->vegetation != *(veg+i) ) i++;
	foodvalue = *(vegfood+i) - '0';
	if( cntry != 0 ) {
		if(foodvalue == 0) {
#ifdef DERVDESG
			if ((magic(cntry,DERVISH)||magic(cntry,DESTROYER))
			&&(sptr->vegetation==DESERT || sptr->vegetation==ICE))
				return(6);
#endif /* DERVDESG */
			return( 0 );
		}
		if(ntn[cntry].race == ELF){
			if(sptr->vegetation == FOREST) foodvalue+=3;
			else if(sptr->vegetation == BARREN) foodvalue--;
		}
	}
	if(( sptr->tradegood <= END_EATRATE )
	&&( sptr->tradegood > END_COMMUNICATION ))
		foodvalue += *(tg_value+sptr->tradegood) - '0';
	return( foodvalue );
}

/*jewel cost for civilian power = Base * 2**( #mgk/2 + #civ + #mil/2 )	*/
/*race			magical		civilian	military	*/
/*	elves -		50K		50K		50K		*/
/*	dwarves -	80K		40K		40K		*/
/*	humans -	100K		25K		50K		*/
/*	orcs -		100K		50K		25K		*/

/* returns cost of magic power - returns -1 if invalid */
long
getmgkcost(type,nation)
int type, nation;
{
	int i;
	long cost;
	long base=BASEMAGIC;
	int npowers;
	switch(type) {
	case M_MGK:
		if(ntn[nation].race==DWARF)		base=DWFMAGIC;
		else if(ntn[nation].race==HUMAN)	base=HUMMAGIC;
		else if(ntn[nation].race==ORC)		base=ORCMAGIC;
		npowers=num_powers(nation,M_CIV)+num_powers(nation,M_MIL)+1
		+2*num_powers(nation,M_MGK);
		npowers/=2;
		break;
	case M_CIV:
		if(ntn[nation].race==DWARF)		base=DWFCIVIL;
		else if(ntn[nation].race==HUMAN)	base=HUMCIVIL;
		else if(ntn[nation].race==ORC)		base=ORCCIVIL;
		npowers=num_powers(nation,M_MGK)+num_powers(nation,M_MIL)+1
		+2*num_powers(nation,M_CIV);
		npowers/=2;
		break;
	case M_MIL:
		if(ntn[nation].race==DWARF)		base=DWFMILIT;
		else if(ntn[nation].race==ORC)		base=ORCMILIT;
		npowers=num_powers(nation,M_CIV)+num_powers(nation,M_MGK)+1
		+2*num_powers(nation,M_MIL);
		npowers/=2;
		break;
	default:
		return(-1);
	}
	cost = base;
	for (i=1; i<npowers; i++) {
		cost <<= 1;
		if (cost > BIG)
			return(BIG/2L);
	}
	return(cost);
}

int
todigit(character)
register int	character;
{
	if( character >= '0' && character <= '9' )
		return( character - '0' );
	return( -1 );
}

/* set up occ[][] for country.
 * if leader==true, only for leader sectors plus ntn.communicatins range
 * if leader==(-1), do not include ships on the sector search
 */
void
prep(nation,leader)
int nation,leader;
{
	short armynum,nvynum;
	int save,i,j,x,y,start,end,com;

	/*set occ to 0*/
	for(i=0;i<MAPX;i++) for(j=0;j<MAPY;j++) occ[i][j]=0;

	save=nation;
	if(leader==TRUE) {
		/* only do the given country */
		start=save;
		end=save+1;
	} else {
		/* go through all countries */
		start=0;
		end=NTOTAL;
	}

	/*set occ to country of occupant army*/
	for(nation=start;nation<end;nation++) if(ntn[nation].active!=INACTIVE) {
		curntn = &ntn[nation];
		for(armynum=0;armynum<MAXARM;armynum++){
			if( leader==TRUE ) {
				if((P_ATYPE<MINLEADER)
				||(P_ATYPE>=MINMONSTER)
				||(P_ASOLD<=0)) continue;
				i=P_AXLOC;
				j=P_AYLOC;
				com = P_NTNCOM; /* do communications radius */
				for(x=i-com;x<=i+com;x++)
				for(y=j-com;y<=j+com;y++)
					if(ONMAP(x,y)) occ[x][y]=nation;
			} else if((P_ASOLD>0)&&(P_ASTAT!=SCOUT)){
				i=P_AXLOC;
				j=P_AYLOC;
				if((occ[i][j]== 0)||(occ[i][j]== nation))
					occ[i][j]= nation;
				else occ[i][j]= NTOTAL;
			}
		}
		if( leader==FALSE ) for(nvynum=0;nvynum<MAXNAVY;nvynum++){
			if((P_NWSHP!=0)||(P_NGSHP!=0)||(P_NMSHP!=0)){
				i=P_NXLOC;
				j=P_NYLOC;
				if((occ[i][j]== 0)||(occ[i][j]== nation))
					occ[i][j]= nation;
				else occ[i][j]= NTOTAL;
			}
		}
	}

	nation=save;
	curntn = &ntn[nation];
}

#ifdef ADMIN
/*routine to depelete a nation without a capitol */
void
deplete(nation)
int nation;
{
	struct s_nation *saventn=curntn;
	int i,j,x,y,armynum;

	x = ntn[nation].capx;
	y = ntn[nation].capy;
	if((sct[x][y].designation==DCAPITOL)&&((sct[x][y].owner==nation)
	||(sct[x][y].owner==0)||(!isntn(ntn[sct[x][y].owner].active))))
		return;

	curntn = &ntn[nation];
	fprintf(fnews,"1.\tNation %s is depleted by the lack of a Capitol\n",ntn[nation].name);

	for(armynum=0;armynum<MAXARM;armynum++) if (P_ASOLD>0) {
		/* first disband PDEPLETE% of the military */
		if (P_ATYPE<MINLEADER &&
		(rand()%100<PDEPLETE||P_ATYPE==A_MERCENARY)) {
			if(P_ATYPE==A_MERCENARY) {
				MERCMEN += P_ASOLD;
			} else if(ntn[sct[AXLOC][AYLOC].owner].race==ntn[nation].race) {
				sct[P_AXLOC][P_AYLOC].people += P_ASOLD;
			}
			P_ASOLD=0;
			if(ispc(curntn->active)) {
				if (mailopen(nation)!=(-1)) {
					fprintf(fm,"Message to %s from Conquer\n\n",curntn->name);
					fprintf(fm,"\tYour %s Army %d disperses into the population\n",*(unittype+(P_ATYPE%UTYPE)),armynum);
					mailclose(nation);
				}
			}
		} else if(P_ATYPE>=MINMONSTER) {
			/* disbanding of ALL monsters should take place */
			P_ASOLD=0;
			if(ispc(curntn->active)) {
				if (mailopen(nation)!=(-1)) {
					fprintf(fm,"Message to %s from Conquer\n\n",curntn->name);
					fprintf(fm,"\tYour %s (unit %d) leaves due to the loss of your jewels.\n",*(unittype+(P_ATYPE%UTYPE)),armynum);
					mailclose(nation);
				}
			}
		}
	}

	/* check for sectors breaking away -- not capx, capy */
	if(ispc(curntn->active)) {
		/* create a summarized mail message of sectors effected */
		if (mailopen(nation)!=(1)) {
			fprintf(fm,"Message to %s from Conquer\n\n",curntn->name);
			fprintf(fm,"Riots and Rebellion flourish:\n");
		}
	}
	for(i=0;i<MAPX;i++) for(j=0;j<MAPY;j++)
	if(sct[i][j].owner==nation && (i!=x || j!=y) ) {
		if(rand()%100 < PDEPLETE && sct[x][y].people>0) {
			if(rand()%100 < PDEPLETE) {
				/* sector riots */
				flee(i,j,TRUE,FALSE);
				DEVASTATE(i,j);
				if(ispc(curntn->active)) {
					/* add to listing */
					fprintf(fm,"\tsector %d, %d has massive riots\n",i,j);
				}
			} else {
				/* sector becomes owned by another nation */
#ifdef NOTDONE
				/* must work on this still */
				giveaway(i,j,&nation);
				if(ispc(curntn->active)) {
					fprintf(fm,"\tsector %d, %d joins nation %s\n",ntn[nation].name);
				}
#endif /* NOTDONE */
			}
		}
	}
	if(ispc(curntn->active)) {
		mailclose(nation);
	} else if(isnpc(curntn->active)) {
		if(sct[curntn->capx][curntn->capy].owner==nation) {
			/* reset capitol for npcs */
			sct[curntn->capx][curntn->capy].designation=DCAPITOL;
			if(sct[curntn->capx][curntn->capy].fortress<1)
				sct[curntn->capx][curntn->capy].fortress=1;
		}
	}
	/* restore */
	curntn = saventn;
}


/*routine to sack a nation's captiol */
void
sackem(cntry)
	int cntry;
{
	struct s_nation *saventn=curntn;
	int x,y,i,j,foundcap,nation;

	/* hail the conquerer */
	curntn = &ntn[cntry];
	x = curntn->capx;
	y = curntn->capy;
	nation = sct[x][y].owner;
	if(nation==cntry || nation==0) return;

	/* advertise */
	fprintf(fnews,"1.\tCapitol of %s sacked by %s\n",ntn[cntry].name,ntn[nation].name);

        /* Remove goods from trade board */
	fixtrade(cntry);

	/* first give all prizes to the conquerer */
	if(curntn->tgold > 0) {			/* all gold */
		ntn[nation].tgold += curntn->tgold;
		curntn->tgold=0;
	}
	ntn[nation].jewels += curntn->jewels;	/* all jewels */
	curntn->jewels=0;
	ntn[nation].metals += curntn->metals;	/* all metals */
	curntn->metals=0;
	ntn[nation].tfood += curntn->tfood/5L;	/* 20% of food */
	curntn->tfood -= curntn->tfood/5L;

	/* fix the designation */
	if(sct[x][y].designation==DCAPITOL) {
		if(isntn(ntn[nation].active)) {
			sct[x][y].designation = DCITY;
		} else {
			DEVASTATE(x,y);
			sct[x][y].owner=cntry;
		}
	}

	/* set another sector to the capx, capy to make sure that */
	/* sacking does not occur next update for same sacking.   */
	foundcap=FALSE;
	for(i=0;foundcap==FALSE && i<MAPX;i++)
	for(j=0;foundcap==FALSE && j<MAPY;j++) if(sct[i][j].owner==cntry) {
		if(sct[i][j].designation==DCITY) {
			x = i; y = j;
			foundcap=TRUE;
		} else if((sct[i][j].designation==DTOWN)
		&&(((x==curntn->capx)&&(y==curntn->capy))
		  ||(sct[x][y].designation!=DTOWN))) {
			x = i; y = j;
		} else if((x==curntn->capx)&&(y==curntn->capy)) {
			x = i; y = j;
		}
	}

	if ((x!=curntn->capx)||(y!=curntn->capy)) {
		/* assign new pseudo capitol */
		if(ispc(curntn->active)) {
			if(mailopen(cntry)!=(-1)) {
			fprintf(fm,"Message to %s from Conquer\n\n",ntn[cntry].name);
			fprintf(fm,"\tYour Capitol at sector location %d,%d\n",curntn->capx,curntn->capy);
			fprintf(fm,"\t was overrun by nation %s.\n\n",ntn[nation].name);
			fprintf(fm,"\tA temporary headquarters is now in sector %d,%d,\n",x,y);
			fprintf(fm,"\t but designation of a new Capitol is recommended.\n");
			mailclose(cntry);
			}
		}
		curntn->capx=x;
		curntn->capy=y;
	} else {
		/* no new capitol assignment */
		if(ispc(curntn->active)) {
			if(mailopen(cntry)!=(-1)) {
			fprintf(fm,"Message to %s from Conquer\n\n",ntn[cntry].name);
			fprintf(fm,"\tYour Capitol at sector location %d,%d\n",curntn->capx,curntn->capy);
			fprintf(fm,"\t was overrun by nation %s.\n\n",ntn[nation].name);
			fprintf(fm,"\tNo other land remains.  The destruction\n");
			fprintf(fm,"\t of your nation seems imminent.\n");
			mailclose(cntry);
			}
		}
	}
	/* restore */
	curntn = saventn;
}
#endif /* ADMIN */

/*destroy nation--special case if capitol not owned by other nation*/
void
destroy(cntry)
int cntry;
{
	short armynum, nvynum;
	int i, x, y;
	char buf[LINELTH];
	struct s_nation	*nptr;

	nptr = &ntn[cntry];
	if( !isactive(nptr->active) ) return;

	if( !ismonst(nptr->active) ) {
		fprintf(fnews,"1.\tNation %s was destroyed ",nptr->name);
		if(cntry!=sct[nptr->capx][nptr->capy].owner){
			fprintf(fnews,"(their capitol is now owned by %s)\n",ntn[sct[nptr->capx][nptr->capy].owner].name);
			/*get +5% to combat skill*/
			ntn[sct[nptr->capx][nptr->capy].owner].aplus+=5;
		}
		else fprintf(fnews,"(they owned their capitol)\n");
	}

	nptr->active=INACTIVE;
	nptr->score=0;
	sprintf(buf,"%s%d",msgfile,cntry);
	unlink(buf);

	for(armynum=0;armynum<MAXARM;armynum++) if(ASOLD>0) {
		if(ntn[sct[AXLOC][AYLOC].owner].race==nptr->race)
			sct[AXLOC][AYLOC].people+=ASOLD;
		ASOLD=0;
	}
	for(nvynum=0;nvynum<MAXNAVY;nvynum++) {
		NMSHP=0;
		NWSHP=0;
		NGSHP=0;
	}
	for(i=0;i<NTOTAL;i++) {
		if (ntn[i].active < NPC_PEASANT ) {
			ntn[i].dstatus[cntry]=UNMET;
			nptr->dstatus[i]=UNMET;
		} else {
			ntn[i].dstatus[cntry]=WAR;
			nptr->dstatus[i]=WAR;
		}
	}
	
	/*if take them you get their gold*/
	if(cntry!=sct[nptr->capx][nptr->capy].owner){
		if(nptr->tgold>0) ntn[sct[nptr->capx][nptr->capy].owner].tgold+=nptr->tgold;
		if(nptr->jewels>0) ntn[sct[nptr->capx][nptr->capy].owner].jewels+=nptr->jewels;
		if(nptr->metals>0) ntn[sct[nptr->capx][nptr->capy].owner].metals+=nptr->metals;
		if(nptr->tfood>0) ntn[sct[nptr->capx][nptr->capy].owner].tfood+=nptr->tfood;
		sct[nptr->capx][nptr->capy].designation=DCITY;
	}

	/*if god destroys then kill all population*/
	if(cntry==sct[nptr->capx][nptr->capy].owner){
		for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++)
		if(sct[x][y].owner==cntry) {
			sct[x][y].people=0;
			sct[x][y].owner=0;
			sct[x][y].designation=DNODESIG;
		}
	}
	/*slowly take over and all people flee*/
	else if(ntn[sct[nptr->capx][nptr->capy].owner].race!=nptr->race){
		for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++)
		if(sct[x][y].owner==cntry) {
			/*all kinds of refugees to neighboring countries*/
			flee(x,y,TRUE,FALSE);
			sct[x][y].people=0;
			sct[x][y].owner=0;
			if(tofood( &sct[x][y],0 )<DESFOOD)
				sct[x][y].designation=DNODESIG;
			else	sct[x][y].designation=sct[x][y].vegetation;
		}
	}
	/*else same race, so give all land to conqueror*/
	else {
		for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++)
		if(sct[x][y].owner==cntry){
			sct[x][y].owner=sct[nptr->capx][nptr->capy].owner;
			if( !ISCITY( sct[x][y].designation )) {
			if(tofood( &sct[x][y],0)<DESFOOD)
				sct[x][y].designation=DNODESIG;
			else	sct[x][y].designation=DFARM;
			}
		}
	}
}

#define ALPHA_SIZE	128

/*movecost contains movement cost unless water  -1 or unenterable land (-2)*/
/* if water and not ajacent to land will cost -4*/
void
updmove(race,cntry)
int cntry;
char race;
{
	register struct s_sector	*sptr;
	register int	i,j;
	int x,y;
	short	veg_cost[ ALPHA_SIZE ];
	short	ele_cost[ ALPHA_SIZE ];

	if( race==GOD ) {
		for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) movecost[x][y] = 0;
		return;
	}

	for( j = 0; veg[j] != '0'; j++ ) {
		switch( race ) {
		case ELF:
			veg_cost[ veg[j] ] = EVegcost[j] - '0';
			break;
		case DWARF:
			veg_cost[ veg[j] ] = DVegcost[j] - '0';
			break;
		case ORC:
			veg_cost[ veg[j] ] = OVegcost[j] - '0';
			break;
		case HUMAN:
		default:
			veg_cost[ veg[j] ] = HVegcost[j] - '0';
			break;
		} /* switch */
	} /* for */

	if(magic(cntry,DERVISH) || magic(cntry,DESTROYER)) {
		veg_cost[ ICE ] = 0;
		veg_cost[ DESERT ] = 0;
	}

	for( j = 0; ele[j] != '0'; j++ ) {
		switch( race ) {
		case ELF:
			ele_cost[ ele[j] ] = EElecost[j] - '0';
			break;
		case DWARF:
			ele_cost[ ele[j] ] = DElecost[j] - '0';
			break;
		case ORC:
			ele_cost[ ele[j] ] = OElecost[j] - '0';
			break;
		case HUMAN:
		default:
			ele_cost[ ele[j] ] = HElecost[j] - '0';
			break;
		} /* switch */
	} /* for */

	for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++){
		sptr = &sct[x][y];

		if(sptr->altitude==WATER) {
			movecost[x][y] = -4;
			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
			if( ONMAP(i,j) )
				if( sct[i][j].altitude != WATER) {
					movecost[x][y] = -1;
					i=x+2;
					j=y+2;
				}
		} else {
			if( veg_cost[ sptr->vegetation ] == -1
			|| ele_cost[ sptr->altitude ] == -1 )
				movecost[x][y] = -2;
			else
				movecost[x][y] = veg_cost[ sptr->vegetation ] + ele_cost[ sptr->altitude ];
		} /* if */
		if (sptr->designation == DROAD)
			movecost[x][y] = (movecost[x][y] + 1) / 2;
	} /* for */
} /* updmove() */

#ifdef CONQUER
/* calculations for cost of movement during flight */
int
flightcost(i,j)
int i,j;
{
	int cnt,hold=(-1),hold2=(-1);

	for (cnt=0; ele[cnt]!='0'; cnt++ ) {
		if (sct[i][j].altitude==ele[cnt])
			hold = (FElecost[cnt] - '0');
	}
	for (cnt=0; veg[cnt]!='0'; cnt++ ) {
		if (sct[i][j].vegetation==veg[cnt])
			hold2 = (FVegcost[cnt] - '0');
	}
	if (hold==(-1) || hold2==(-1)) {
		hold=(-1);
	}
	else hold+=hold2;

	return(hold);
}
#endif /* CONQUER */
#ifdef ADMIN
/* determines whether or not a unit has the ability to fly */
int
avian(typ)
unsigned char typ;
{
	switch(typ) {
	case A_ROC:
	case A_GRIFFON:
	case SPIRIT:
	case DJINNI:
	case DEMON:
	case DRAGON:
		return(TRUE);
	default:
		return(FALSE);
	}
}
#endif /* ADMIN */

void
spreadsheet(nation)
int nation;
{
	register struct s_sector	*sptr;
	register struct s_nation	*nptr;
	long	product;
	long	city_pop, cap_pop;
	int x,y,i,j,foundmill;

	nptr = &ntn[ nation ];

	spread.revothr = spread.revfood = spread.revjewels = spread.revmetal = spread.revcap = spread.revcity = 0L;
	spread.inothr = spread.incity = spread.incap = spread.ingold = spread.infarm = spread.inmetal = 0;
	spread.food = nptr->tfood;
	spread.gold = nptr->tgold;
	spread.metal = nptr->metals;
	spread.jewels = nptr->jewels;
	spread.sectors = 0;
	spread.civilians = 0L;

	for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) {
		sptr = &sct[x][y];
		if(sptr->owner!=nation) continue;

		/*update nation file for owner*/
		spread.sectors++;
		spread.civilians += sptr->people;

		product = 0;
		/*PRODUCE*/
		/*increase tmin based on mined stuff...*/
		if(sptr->designation==DMINE) {
			if( !tg_ok( nation, sptr )) continue;
			spread.inmetal += sptr->people;
			if(sptr->people>TOMANYPEOPLE) {
				product = sptr->metal * TOMANYPEOPLE;
				product += sptr->metal * (sptr->people-TOMANYPEOPLE)/2L;
			} else product = sptr->metal *  sptr->people;

			if(magic(sptr->owner,MINER)==1) product*=2L;
			if(magic(sptr->owner,STEEL)==1) product*=2L;
			spread.metal += product;
			spread.revmetal += product*TAXMETAL*nptr->tax_rate/100L;
		}
		/*harvest food*/
		else if(sptr->designation==DFARM) {
			spread.infarm += sptr->people;
			if(sptr->people>TOMANYPEOPLE) {
				product = (long)tofood(sptr,sptr->owner) * TOMANYPEOPLE;
				product += (long)tofood(sptr,sptr->owner) * (sptr->people-TOMANYPEOPLE)/2L;
			} else product = (long)tofood(sptr,sptr->owner) *  sptr->people;

			switch(SEASON(TURN)) {
			case SPRING:
				product/=2;
				break;
			case SUMMER:
				break;
			case FALL:
				product*=5;
				product/=2;
				break;
			case WINTER:
				product=0;
				break;
			}
			/* search for neighboring mills */
			foundmill=FALSE;
			for(i=x-1;foundmill==FALSE && i<=x+1;i++)
			for(j=y-1;foundmill==FALSE && j<=y+1;j++)
			if((ONMAP(i,j))
			&&(sct[i][j].owner==sptr->owner)
			&&(sct[i][j].designation==DMILL)
			&&(sct[i][j].people>=MILLSIZE)) {
				product *= 12L;
				product /= 10L;
				/* must break this way... two for() loops */
				foundmill=TRUE;
			}
			spread.food += product;
			spread.revfood += product*TAXFOOD*nptr->tax_rate/100L;
		}
		/*gold mines produce gold*/
		else if(sptr->designation==DGOLDMINE) {
			if( !tg_ok( nation, sptr )) continue;
			spread.ingold += sptr->people;
			if(sptr->people>TOMANYPEOPLE) {
				product = sptr->jewels * TOMANYPEOPLE;
				product += sptr->jewels * (sptr->people-TOMANYPEOPLE)/2L;
			} else product = sptr->jewels *  sptr->people;

			if(magic(sptr->owner,MINER)==1) product*=2;

			spread.jewels += product;
			spread.revjewels += product*TAXGOLD*nptr->tax_rate/100L;
		}
		else if((sptr->designation==DCITY)
		||(sptr->designation==DCAPITOL)) {
			cap_pop  = sptr->people;
			spread.incap += cap_pop;

			if( magic(sptr->owner, ARCHITECT ) ) {
				cap_pop *= 2L;
			}

			spread.revcap +=  cap_pop * TAXCITY*nptr->tax_rate / 100L;
		} else if(sptr->designation==DTOWN) {
			spread.incity += sptr->people;
			city_pop = sptr->people;
			if( magic(sptr->owner, ARCHITECT ) )
				city_pop *= 2L;

			spread.revcity +=  city_pop*TAXTOWN*nptr->tax_rate/100L;
		}
#ifndef DERVDESG
		else if(((magic(sptr->owner,DERVISH)==1)
			||(magic(sptr->owner,DESTROYER)==1))
		&&((sptr->vegetation==ICE) ||(sptr->vegetation==DESERT))
		&&(sptr->people>0)) {
			if(sptr->people>TOMANYPEOPLE) {
				product = 6L * TOMANYPEOPLE;
				product += 3L * (sptr->people-TOMANYPEOPLE);
			} else product = 6L *  sptr->people;

			spread.food += product;
			/* desert food production mostly static */
			if (sptr->vegetation==DESERT) {
				/* harsh summer in desert; good winter */
				if (SEASON(TURN)==SUMMER) product/=2;
				else if (SEASON(TURN)==WINTER) {
					product*=5;
					product/=4;
				}
			} else {
				/* opposite in ice */
				if (SEASON(TURN)==WINTER) product/=2;
				else if (SEASON(TURN)==SUMMER) {
					product*=5;
					product/=4;
				}
			}
			spread.revfood += product*TAXFOOD*nptr->tax_rate/100L;
		}
#endif /* DERVDESG */
		else {	/* other sectors */
			spread.inothr += sptr->people;
			if(sptr->people>TOMANYPEOPLE) {
				product = (long)tofood(sptr,sptr->owner) * TOMANYPEOPLE;
				product += (long)tofood(sptr,sptr->owner) * (sptr->people-TOMANYPEOPLE)/2L;
			} else product = (long)tofood(sptr,sptr->owner) *  sptr->people;

			spread.revothr += product*TAXOTHR*nptr->tax_rate/100L;
		}
	}
	spread.gold += spread.revfood + spread.revjewels + spread.revmetal + spread.revcity + spread.revcap + spread.revothr;
}

/* string inputing routine to allow deleting */
void
get_nname(str)
char str[];
{
	char ch;
	int done=0,count=0,xpos,ypos;

	while(!done) {
		ch=getch();
		if (isprint(ch)) {
			if (count<NAMELTH) {
				/* only input displayable characters */
				addch(ch);
				refresh();
				str[count++] = ch;
			}
		}
		else if ((ch=='\b' || ch=='\177')&&(count))
		{
			/* only delete what was printed */
			getyx(stdscr,ypos,xpos);
			move(ypos,--xpos);
			addch(' ');
			move(ypos,xpos);
			refresh();
			count--;
		} else if((ch=='\n')||(ch=='\r')) {
			done=TRUE;
		}
	}
	str[count] = '\0';
}


#ifdef CONQUER
/* routine to find a nation number using name or number  */
/* returns NTOTAL+1 if input is invalid; -1 for no input */
int
get_country()
{
	char name[NAMELTH+1],ch;
	int i,l,hold;

	/* get name and check through list */
	get_nname(name);

	/* return on no entry */
	if ((l=strlen(name))==0) {
		return(-1);
	}

	for(hold=0;hold<NTOTAL;hold++)
		if(strcmp(ntn[hold].name,name)==0) break;

	/* check for 'god' */
	if (strcmp("god",name)==0) hold=0;
	if (strcmp("news",name)==0) hold= NEWSMAIL;

	/* check for numbers if name too long */
	if (hold==NTOTAL) {
		hold=0;
		for (i=0;i<l;i++) {
			ch=name[i];
			if(ch < '0' || ch > '9' ) {
				errormsg("There is no nation by that name.");
				return(NTOTAL);
			} else {
				hold *= 10;
				hold += (ch-'0');
			}
		}
		if (hold>NTOTAL) hold=NTOTAL;
	}
	/* send back result */
	return(hold);
}

/* finds a nation for god to be, returns 1 on failure */
int
get_god()
{
	clear_bottom(0);
	mvaddstr(LINES-4,0,"Super User; For what nation? ");
	refresh();

	/* return on no entry or bad entry */
	if ((country=get_country())==(-1) || country==NTOTAL) {
		country = 0;
		redraw=DONE;
		makebottom();
		return(1);
	}

	curntn = &ntn[country];
	return(0);
}

/* quick routine to reassign god and gods nations */
void
reset_god()
{
	/* simple routine; but improves readibility */
	country=0;
	curntn= &ntn[country];
}
#endif /* CONQUER */

#ifdef ADMIN
int
getleader(class)
int	class;
{
	switch(class){
	case C_NPC:
	case C_KING:
	case C_TRADER:	return(L_BARON);
	case C_EMPEROR:	return(L_PRINCE);
	case C_WIZARD:	return(L_MAGI);
	case C_PRIEST:	return(L_BISHOP);
	case C_PIRATE:	return(L_CAPTAIN);
	case C_WARLORD:	return(L_LORD);
	case C_DEMON:	return(L_DEVIL);
	case C_DRAGON:	return(L_WYRM);
	case C_SHADOW:	return(L_NAZGUL);
	default:
		printf("ERROR-national class (%d) undefined\n",class);
		exit(0);
	}
	return(-1);	/* shut lint up */
}
#endif /* ADMIN */

/* name of the currently open mail file */
char tmp_mail_name[LINELTH];

int
mailopen(to)
{
#ifdef CONQUER
	char	line[LINELTH];
#endif /*CONQUER*/
	if(mailok != DONEMAIL) mailclose(ABORTMAIL);

	if (to != NEWSMAIL) {
#ifdef CONQUER
		struct stat fst;
		/* check if the player is currently reading messages */
		sprintf(line,"%s%hd.tmp",msgfile,to);
		if (stat(line,&fst)==0) {
			long now;
			now = time(0);
			if (now - fst.st_mtime < TIME_DEAD) {
				if (to>=0 && to<NTOTAL) {
					sprintf(line,"Nation %s is reading their mail... try again later.", ntn[to].name);
					errormsg(line);
				}
				return(-1);
			} else {
				/* remove useless file */
				unlink(line);
			}
		}

		/* otherwise continue; checking for others */
		/* this file name is also used in rmessages() */
		sprintf(tmp_mail_name,"send.%s%hd",msgfile,to);
		if (stat(tmp_mail_name,&fst)==0) {
			long now;
			now = time(0);
			if (now - fst.st_mtime < TIME_DEAD) {
				if (to>=0 && to<NTOTAL) {
					sprintf(line,"Someone is already mailing Nation %s... try again later.", ntn[to].name);
					errormsg(line);
				}
				return(-1);
			} else {
				/* remove useless file */
				unlink(tmp_mail_name);
			}
		}
#endif /*CONQUER*/
#ifdef ADMIN
		sprintf(tmp_mail_name,"%s%hd",msgfile,to);
#endif /*ADMIN*/
	} else {
		/* send to a location marked by the current player */
		sprintf(tmp_mail_name,"send.news%d", country);
	}
	if ((fm=fopen(tmp_mail_name,"a+"))==NULL) {
		fprintf(stderr,"error opening %s",tmp_mail_name);
		return(-1);
	}
	mailok=to;
	return(0);
}

void
mailclose(to)
{
	if(mailok==DONEMAIL) return;

	if(to >= 0) {
		fputs("END\n",fm);
	}
	fclose(fm);

#ifdef CONQUER
	if((to!=ABORTMAIL)&&(to==mailok)) {
		char line[BIGLTH];
		if (to==NEWSMAIL) {
			/* send to the current newspaper */
			sprintf(line,"cat %s >> news%d",tmp_mail_name,TURN-1);
		} else {
			/* send to the player now */
			sprintf(line,"cat %s >> %s%d",tmp_mail_name,msgfile,to);
		}
		system(line);
	}
	if (tmp_mail_name!=NULL) {
		(void) unlink(tmp_mail_name);
	}
#endif /*CONQUER*/
	mailok=DONEMAIL;
}

#ifdef ADMIN
/* markok returns TRUE if mark is ok as a nation mark */
int
markok(mark,prtflag)
char mark;
int prtflag;	/* if true printf reason */
{
	register int i;
	char temp[LINELTH];

	if((isprint(mark)==0)||(isspace(mark)!=0)) {
		if(prtflag) {
			sprintf(temp,"%c is white space",mark);
			newerror(temp);
		}
		return(FALSE);
	}

	for(i=0;ele[i]!='0';i++) if(mark==(*(ele+i))) {
		if(prtflag) {
			sprintf(temp,"%c is an elevation character",mark);
			newerror(temp);
		}
		return(FALSE);
	}

	for(i=0;veg[i]!='0';i++) if(mark==(*(veg+i))) {
		if(prtflag) {
			sprintf(temp,"%c is a vegetation character",mark);
			newerror(temp);
		}
		return(FALSE);
	}

	for(i=1;i<NTOTAL;i++) if(isactive(ntn[i].active) && ntn[i].mark==mark) {
		if(prtflag) {
			sprintf(temp,"%c is already used",mark);
			newerror(temp);
		}
		return(FALSE);
	}

	if(mark=='*') {
		if(prtflag) {
			sprintf(temp,"%c is used by Monsters",mark);
			newerror(temp);
		}
		return(FALSE);
	}

	if(!isalpha(mark)) {
		if(prtflag) {
			sprintf(temp,"%c is not an alpha character",mark);
			newerror(temp);
		}
		return(FALSE);
	}
	return(TRUE);
}
#endif /* ADMIN */

/*******************************************************************/
/* DEFAULTUNIT() returns the default army type for a given country */
/* this is mostly used by npc's to take advantage of their powers  */
/*******************************************************************/
long
defaultunit( nation )
int	nation;
{
	if(magic(nation,VAMPIRE)) return(A_ZOMBIE);
	if(magic(nation,AV_MONST)) {
		if(magic(nation,BREEDER))
			return(A_OLOG);
		else	return(A_URUK);
	}
	if(magic(nation,ARCHER)) return(A_ARCHER);
	if(magic(nation,MI_MONST)) return(A_ORC);	/* if race = orc */
	if( ntn[nation].active==NPC_NOMAD ) return(A_LT_CAV);
	return(A_INFANTRY);
}

#ifdef ADMIN
void
getmetal( sptr )
struct s_sector *sptr;
{
	int randval;
	randval = rand()%100;
	if((sptr->tradegood != TG_none)&&(sptr->tradegood != 0)) return;
	if( randval < 20 ) {
		sptr->tradegood = TG_copper;
		sptr->metal = rand()%2 + 1;
	} else if( randval < 30 ) {
		sptr->tradegood = TG_lead;
		sptr->metal = rand()%4 + 1;
	} else if( randval < 40 ) {
		sptr->tradegood = TG_tin;
		sptr->metal = rand()%4 + 2;
	} else if( randval < 55 ) {
		sptr->tradegood = TG_bronze;
		sptr->metal = rand()%4 + 2;
	} else if( randval < 80 ) {
		sptr->tradegood = TG_iron;
		sptr->metal = rand()%7 + 2;
	} else if( randval < 95 ) {
		sptr->tradegood = TG_steel;
		sptr->metal = rand()%8 + 3;
	} else if( randval < 99 ) {
		sptr->tradegood = TG_mithral;
		sptr->metal = rand()%11 + 5;
	} else {
		sptr->tradegood = TG_adamantine;
		sptr->metal = rand()%13 + 8;
	}
}

void
getjewel( sptr )
struct s_sector *sptr;
{
	int randval;
	if((sptr->tradegood != TG_none)&&(sptr->tradegood != 0)) return;
	randval = rand()%100;
	if( randval < 20 ) {
		sptr->tradegood = TG_spice;
		sptr->jewels = rand()%2 + 1;
	} else if( randval < 40 ) {
		sptr->tradegood = TG_silver;
		sptr->jewels = rand()%3 + 1;
	} else if( randval < 48 ) {
		sptr->tradegood = TG_pearls;
		sptr->jewels = rand()%3 + 1;
	} else if( randval < 56 ) {
		sptr->tradegood = TG_dye;
		sptr->jewels = rand()%5 + 1;
	} else if( randval < 64 ) {
		sptr->tradegood = TG_silk;
		sptr->jewels = rand()%5 + 1;
	} else if( randval < 84 ) {
		sptr->tradegood = TG_gold;
		sptr->jewels = rand()%6 + 1;
	} else if( randval < 91 ) {
		sptr->tradegood = TG_rubys;
		sptr->jewels = rand()%6 + 1;
	} else if( randval < 96 ) {
		sptr->tradegood = TG_ivory;
		sptr->jewels = rand()%7 + 2;
	} else if( randval < 99 ) {
		sptr->tradegood = TG_diamonds;
		sptr->jewels = rand()%11 + 2;
	 } else {
		sptr->tradegood = TG_platinum;
		sptr->jewels = rand()%17 + 4;
	}
}
#endif /* ADMIN */

/* tg_ok returns true if a trade good can be seen by the owner of sector */
int
tg_ok( nation, sptr )
int	nation;
struct	s_sector	*sptr;
{
	if(( nation == 0)||(nation>=NTOTAL)) return(TRUE);

	switch( sptr->tradegood ) {
	case TG_lead:	if(ntn[nation].mine_ability < 8) return(0); break;
	case TG_tin:	if(ntn[nation].mine_ability < 11) return(0); break;
	case TG_bronze:	if(ntn[nation].mine_ability < 15) return(0); break;
	case TG_iron:	if(ntn[nation].mine_ability < 25) return(0); break;
	case TG_steel:	if(ntn[nation].mine_ability < 30) return(0); break;
	case TG_mithral: if(ntn[nation].mine_ability < 30) return(0); break;
	case TG_adamantine: if(ntn[nation].mine_ability < 40) return(0); break;
	case TG_spice:
	case TG_silver:
	case TG_pearls:	break;
	case TG_dye:
	case TG_silk:	if(ntn[nation].wealth < 5) return(0); break;
	case TG_gold:
	case TG_rubys:	if(ntn[nation].wealth < 8) return(0); break;
	case TG_ivory:	if(ntn[nation].wealth < 15) return(0); break;
	case TG_diamonds:	if(ntn[nation].wealth < 20) return(0); break;
	case TG_platinum:	if(ntn[nation].wealth < 25) return(0); break;
	default:		break;
	};

	if(tofood(sptr,nation) >= DESFOOD) return(TRUE);
	return(FALSE);
}

/* this routine computes the fortification value of a sector */
int
fort_val(sptr)
	struct s_sector *sptr;
{
	if(sptr->designation==DSTOCKADE) {
		return(DEF_BASE);
	}
	if(sptr->designation==DFORT){
		if(magic(sptr->owner,ARCHITECT)==1){
			return(DEF_BASE + 2*FORTSTR * sptr->fortress);
		}
		else return(DEF_BASE + FORTSTR * sptr->fortress);
	}
	if (sptr->designation==DTOWN){
		if(magic(sptr->owner,ARCHITECT)==1){
			return(DEF_BASE + 2*TOWNSTR * sptr->fortress);
		} else return(DEF_BASE + TOWNSTR * sptr->fortress);
	}
	if((sptr->designation==DCAPITOL)
	||(sptr->designation==DCITY)){
		if(magic(sptr->owner,ARCHITECT)==1){
			return(2*DEF_BASE + 2*CITYSTR * sptr->fortress);
		}
		else return(2*DEF_BASE + CITYSTR * sptr->fortress);
	}
	return(0);
}

/* routine to determine compass direction of x1,y1 from x0,y0 */
int
compass(x0,y0,x1,y1)
	int x0,y0,x1,y1;
{
	int dx=x1-x0, dy=y1-y0;	/* diplacements */
	int hold;

	if(10*abs(dx) > abs(dy)) {
		if(10*abs(dy) > abs(dx)) {
			/* four off-quadrants */
			if(dx>0) {
				if(dy<0) hold=NORTHEAST;
				else hold=SOUTHEAST;
			} else {
				if(dy<0) hold=NORTHWEST;
				else hold=SOUTHWEST;
			}
		} else {
			/* east or west */
			if(dx>0) hold=EAST;
			else hold=WEST;
		}
	} else {
		/* north or south or same point */
		if(dy==0) hold=CENTERED;
		else if(dy<0) hold=NORTH;
		else hold=SOUTH;
	}
	return(hold);
}

#ifdef CONQUER
extern short xcurs;
extern short ycurs;
off_t conq_mail_size=0;
#ifdef SYSMAIL
static off_t sys_mail_size=0;
#endif /* SYSMAIL */
void
check_mail()
{
	struct stat info;
#ifdef SYSMAIL
	int osys_mail=sys_mail_status;
#endif
	int oconq_mail=conq_mail_status;

	/* check conquer mail box */
	if (stat(conqmail,&info)==(-1)) {
		conq_mail_status=NO_MAIL;
		conq_mail_size=0;
	} else {
		if (info.st_size > conq_mail_size) {
			conq_mail_status=NEW_MAIL;
			conq_mail_size=info.st_size;
		} else if (info.st_size < conq_mail_size) {
			conq_mail_status=NO_MAIL;
			conq_mail_size=info.st_size;
		}
	}

#ifdef SYSMAIL
	/* check system mail box */
	if (stat(sysmail,&info)==(-1)) {
		sys_mail_status=NO_MAIL;
		sys_mail_size=0;
	} else {
		if(info.st_atime>info.st_mtime) {
			sys_mail_status=NO_MAIL;
			sys_mail_size=info.st_size;
		} else if (info.st_size > sys_mail_size) {
			sys_mail_status=NEW_MAIL;
			sys_mail_size=info.st_size;
		} else if (info.st_size < sys_mail_size) {
			sys_mail_status=NO_MAIL;
			sys_mail_size=info.st_size;
		}
	}

	/* display mail information */
	if(sys_mail_status!=osys_mail) {
		if (sys_mail_status==NEW_MAIL) {
			mvaddstr(LINES-3,COLS/2-6,"You have System Mail");
		} else {
			mvaddstr(LINES-3,COLS/2-6,"                    ");
		}
		move(ycurs,2*xcurs);
		refresh();
	}
	if (conq_mail_status!=oconq_mail) {
		if (conq_mail_status==NEW_MAIL) {
			mvaddstr(LINES-2,COLS/2-6,"You have Conquer Mail");
		} else {
			mvaddstr(LINES-2,COLS/2-6,"                     ");
		}
		move(ycurs,2*xcurs);
		refresh();
	}
#else
	/* display mail information */
	if (conq_mail_status!=oconq_mail) {
		if (conq_mail_status==NEW_MAIL) {
			mvaddstr(LINES-3,COLS/2-6,"You have Conquer Mail");
		} else {
			mvaddstr(LINES-3,COLS/2-6,"                     ");
		}
		move(ycurs,2*xcurs);
		refresh();
	}
#endif /* SYSMAIL */
}
#endif /* CONQUER */
