/* conquer : Copyright (c) 1988 by Ed Barlow.
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

/*Create a world*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <ctype.h>
#include <pwd.h>
#include "header.h"
#include "data.h"

#define HALF 2
#define LAND 3

int pwater;		/* percent water in world (0-100) */
extern short	country;
extern int	numleaders,remake;
char datadir[FILELTH];
char **area_map;			/*Value Map of Areas*/
char **type;

#define TOHILL(x,y) if( nmountains != 0 ) { \
sct[(x)][(y)].altitude=HILL; \
nmountains--; \
}
#define TOMT(x,y) if( nmountains != 0 ) { \
sct[(x)][(y)].altitude=MOUNTAIN; \
nmountains--; \
}
#define TOPEAK(x,y) if( nmountains != 0 ) { \
sct[(x)][(y)].altitude=PEAK; \
nmountains--; \
}

void
zeroworld()
{
	int i,armynum,nvynum;

	/* initialize all countries */
	for (i = 0; i < NTOTAL; i++) {
		curntn = &ntn[i];
		for (armynum=0; armynum < MAXARM; armynum++) {
			P_ASOLD = (long)0;
			P_AXLOC = P_AYLOC = P_ATYPE = P_AMOVE = (unsigned char)0;
			P_ASTAT = DEFEND;
		}
		for (nvynum=0; nvynum < MAXNAVY; nvynum++) {
			P_NWSHP = P_NMSHP = P_NGSHP = (unsigned short)0;
			P_NCREW = P_NPEOP = P_NARMY = (unsigned char)0;
			P_NXLOC = P_NYLOC = P_NMOVE = (unsigned char)0;
		}
		curntn->active = INACTIVE;
		curntn->repro = (char)0;
		curntn->jewels = curntn->tgold = curntn->metals = 0L;
		curntn->powers = curntn->tciv = curntn->tmil = curntn->score = 0L;
		curntn->race = TUNKNOWN;
		curntn->maxmove = 0;
		curntn->spellpts = 0;
		curntn->class = curntn->aplus = curntn->dplus = (short)0;
		curntn->inflation = curntn->tsctrs = curntn->tships = (short)0;
	}
}

void
makeworld(rflag)
int	rflag;		/* TRUE if you wish to read in a map from mapfiles */
{
	char passwd[PASSLTH+1],*getpass();
	char newstring[BIGLTH],tempc[BIGLTH];
	FILE *fopen();
	struct passwd *getpwnam();
	int i,valid;

	/* conquer makeworld information */
	newinit();
	sprintf(newstring, "Datadir: %s", datadir);
	errorbar("World Generator", newstring);

	mvaddstr(0,COLS/2-9,"WELCOME TO CONQUER");
	mvaddstr(2,5,"Genesis begins...  Your super user login will be 'god'.");
	mvaddstr(3,0,"Non-player countries will be read in from the file 'nations',");
	mvaddstr(4,0,"and will have the same password as god, which you will soon set.");
	mvaddstr(5,0,"     To add players after world creation:  conqrun -a");
	if (strcmp(datadir,"[default]")!=0)
		printw(" -d %s", datadir);
	addch('.');

	newerror("..Zero out extraneous files from prior games");
	/* flush out beginning input */
	sprintf(newstring,"rm -f %s* %s* %s* %s* %s %s 2> /dev/null",
		exefile, msgfile, newsfile, isonfile, tradefile, timefile);
	system(newstring);
	newmsg("....Initialize the nation structures");
	zeroworld();
	newmsg("Initialization complete:  And there was light....");
	sleep(1);

	valid=FALSE;
	while(valid==FALSE) {			/* password routine */
		mvaddstr(7,0,"Enter Super-User Password: ");
		clrtoeol();
		refresh();
		i = get_pass(newstring);
		if (i < 4) {
			newerror("Password Too Short");
			continue;
		} else if (i > PASSLTH) {
			newerror("Password Too Long");
			continue;
		}
		mvaddstr(7,0,"Reenter Super-User Password: ");
		clrtoeol();
		refresh();
		i = get_pass(passwd);

		if((i<4)||(i>PASSLTH)||(strncmp(passwd,newstring,PASSLTH)!=0)){
			newerror("Invalid Password Match");
		} else valid=TRUE;
	}
	strncpy(ntn[0].passwd,crypt(passwd,SALT),PASSLTH);
	
	/* finally ask for the secondary administrator */
	mvaddstr(7,0,"You may designate an other user as an alternate \"god\" for this world.");
	mvaddstr(8,0,"Enter a System Login or \"god\" to have none.  [Return for default]");
#ifdef REMAKE
	if (remake==FALSE) {
		(void) strcpy(ntn[0].leader,"god");
	} else if (getpwnam(ntn[0].leader)==NULL) {
		(void) strcpy(ntn[0].leader,"god");
		remake=FALSE;
	}
#else
	(void) strcpy(ntn[0].leader,"god");
#endif /* REMAKE */
	while(TRUE) {
		mvprintw(9,0,"What demi-god shall co-rule this world? [%s]: ",ntn[0].leader);
		clrtoeol();
		refresh();
		get_nname( newstring );
		if ((strcmp(newstring,"GOD")==0)
		||(strcmp(newstring,"God")==0)
		||(strcmp(newstring,"god")==0)) {
			newmsg("God will personally rule this world!!!");
			sleep(1);
			(void) strcpy(ntn[0].leader,LOGIN);
			mvaddstr(7,0,"Demi-God: [none]");
			clrtoeol();
			break;
		}
		if (strlen(newstring)==0) {
#ifdef REMAKE
			if (remake==FALSE) {
#endif /*REMAKE*/
				newmsg("God will personally rule this world!!!");
				sleep(1);
				(void) strcpy(ntn[0].leader,LOGIN);
				mvaddstr(7,0,"Demi-God: [none]");
				clrtoeol();
				break;
#ifdef REMAKE
			} else {
				(void) sprintf(tempc,"The demi-god %s will continue to reign.",ntn[0].leader);
				newmsg(tempc);
				sleep(1);
				mvprintw(7,0,"Demi-God: [%s]",ntn[0].leader);
				clrtoeol();
				break;
			}
#endif /*REMAKE*/
		} else if (strlen(newstring) <= LEADERLTH) {
			if (getpwnam(newstring)!=NULL) {
				(void) sprintf(tempc,"The demi-god %s may administrate this new world.",newstring);
				newmsg(tempc);
				(void) strncpy(ntn[0].leader,newstring,LEADERLTH);
				mvprintw(7,0,"Demi-God: %s",ntn[0].leader);
				clrtoeol();
				break;
			} else {
				(void) sprintf(tempc,"Their is no mortal named %s on this system.",newstring);
				newerror(tempc);
			}
		} else {
			newerror("That Name is Too long.");
		}
	}
#ifdef CHECKUSER
	ntn[0].uid = getuid();
#endif
	mvaddstr(8,0,"Please Enter the Size of the World.  [Divisible by 8 and > 23]");
	clrtoeol();
	while(TRUE) {
		mvaddstr(9,0,"Enter number of X sectors: ");
		clrtoeol();
		refresh();
		world.mapx = get_number();
		if(((world.mapx % 8) != 0 ) || (world.mapx<24)){
			newerror("Invalid X Value Entered");
			continue;
		}
		break;
	}
	while (TRUE) {
		mvaddstr(10,0,"Enter number of Y sectors: ");
		clrtoeol();
		refresh();
		world.mapy = get_number();
		if(((world.mapy % 8) != 0 ) || (world.mapy<24)){
			newerror("Invalid Y Value Entered");
			continue;
		}
		break;
	}
	mvprintw(8,0,"Map Size: %dx%d", (int)world.mapx, (int)world.mapy);
	clrtoeol();
	move(10,0);
	clrtoeol();

	getspace();	/* malloc space for this world */

	/* get amount of water to have in the world */
	while(TRUE) {
		mvaddstr(9,0,"Enter percent water to have in world (0-100): ");
		clrtoeol();
		refresh();
		pwater = get_number();
		if((pwater<0) || (pwater>100 )){
			newerror("Invalid Percentage Entered");
			continue;
		}
		break;
	}
	mvprintw(9,0,"Percentage Water: %d", pwater);
	clrtoeol();
	refresh();

	if( rflag==FALSE ) createworld();
	else readmap();	/* read map in from mapfiles */
	rawmaterials();

	verifydata(__FILE__,__LINE__);
	writedata();

	/* initialize news file */
	sprintf(newstring,"%s0",newsfile);
	if( (fm=fopen(newstring,"w"))!=(FILE *)NULL ) {
		fprintf(fm,"1\tIMPORTANT WORLD NEWS\n");
		fprintf(fm,"5\tGLOBAL ANNOUNCEMENTS\n");
		fclose(fm);
	} else {
		sprintf(tempc,"error opening news file <%s>\n",newstring);
		newerror(tempc);
	}
	newreset();
}

void
createworld()	/* create world */
{
	int	i,j;
	register int x,y;
	int	n;		/*count used in string searches*/
	float	avvalue;	/*Average water tvalue of sectors*/
	int	number[5];	/*Number of sectors with type=[0-4]*/
	int	alloc = NUMAREAS * 2;	/*Value Allocated*/
	char	**tplace;	/*Temp: to see if an area is placed*/
	int	X, Y, chance;
	int	X1,Y1;
	int	X2,Y2;
	int	valid;
	int	rnd;
	long	nmountains;

	tplace = (char **) m2alloc(MAPX,MAPY,sizeof(char));
	area_map = (char **) m2alloc(MAPX,MAPY,sizeof(char));
	type = (char **) m2alloc(MAPX,MAPY,sizeof(char));

	mvaddstr(11,0,"Creating world");
	mvaddstr(12,5,"In the beginning, the world was a set of bits ordered in");
	mvaddstr(13,0,"a random way.  Then the conquer game administrator (hereafter");
	mvaddstr(14,0,"known as god) decreed 'conqrun -m'!!!");

	/*initialize variables */
	newmsg("Day 1... And the variables were initialized.");
	sleep(1);
	move(11,0);
	clrtoeol();
	move(12,0);
	clrtoeol();
	move(13,0);
	clrtoeol();
	move(14,0);
	clrtoeol();
	avvalue = (((float) (100-pwater)/25.0)); /*Average water tvalue of sectors*/
	for(i=0;i<MAXX;i++) for(j=0;j<MAXY;j++)
		tplace[i][j] = area_map[i][j] = 0;

	for(i=0;i<MAPX;i++) for(j=0;j<MAPY;j++) sct[i][j].vegetation=NONE;

	for(i=0;i<5;i++) number[i] = NUMAREAS/5;  /*areas with type=[i]*/
	number[2]=NUMAREAS - 4*number[0]; /*correct for roundoff*/

	/*determine # of area types to areas*/
	for(i=0;i<250;i++) {
		if((avvalue*NUMAREAS)>alloc) {
			/*have not allocated high enough so Allocate more */
			x = rand()%4;   /*0 to 3*/
			if(number[x]>0) {
				number[x] = number[x] - 1;
				number[x+1] = number[x+1] + 1;
				alloc = alloc + 1;
			}
		}
		else	{
			/*have allocated too much  Allocate less */
			x = (rand()%4) +1; /*1 to 4*/
			if(number[x]>0) {
				number[x] = number[x] - 1;
				number[x-1] = number[x-1] + 1;
				alloc = alloc - 1;
			}
		}
	}

	i=0;
	while((number[4]>0)&&(i<500)) {
		i++;
		/*place a full land sector anywhere but on edge*/
		X = ((rand()%(MAXX-2))+1); /*1 to MAXX-2)*/
		Y = ((rand()%(MAXY-2))+1); /*1 to MAXY-2)*/
		if(tplace[X][Y] == 0) {
			tplace[X][Y]=1;
			area_map[X][Y]=4;
			number[4]=number[4] - 1;
			/*place surrounding sectors*/
			if(tplace[X+1][Y] == 0) {
				rnd = rand()%100 + 1; /*1 to 100*/
				if((rnd<25) && (number[4]>0)) {
					area_map[X+1][Y]=4;
					number[4]=number[4]-1;
					tplace[X+1][Y]=1;
				}
				if(rnd>25 && number[3]>0) {
					area_map[X+1][Y]=3;
					number[3]=number[3]-1;
					tplace[X+1][Y]=1;
				}
			}
			if(tplace[X-1][Y] == 0) {
				rnd = rand()%100 + 1 ; /*(1 to 100)*/
				if(rnd<25 && number[4]>0) {
					area_map[X-1][Y]=4;
					number[4]=number[4]-1;
					tplace[X-1][Y]=1;
				}
				if(rnd>25 && number[3]>0) {
					area_map[X-1][Y]=3;
					number[3]=number[3]-1;
					tplace[X-1][Y]=1;
				}
			}
			if(tplace[X][Y+1] == 0) {
				rnd = rand()%100 + 1 ; /*(1 to 100)*/
				if(rnd<25 && number[4]>0) {
					area_map[X][Y+1]=4;
					number[4]=number[4]-1;
					tplace[X][Y+1]=1;
				}
				if(rnd>25 && number[3]>0) {
					area_map[X][Y+1]=3;
					number[3]=number[3]-1;
					tplace[X][Y+1]=1;
				}
			}
			if(tplace[X][Y-1] == 0) {
				rnd = rand()%100 + 1 ; /*(1 to 100)*/
				if(rnd<25 && number[4]>0) {
					area_map[X][Y-1]=4;
					number[4]=number[4]-1;
					tplace[X][Y-1]=1;
				}
				if(rnd>25 && number[3]>0) {
					area_map[X][Y-1]=3;
					number[3]=number[3]-1;
					tplace[X][Y-1]=1;
				}
			}
		}
	}

	/* place all other areas*/
	for(X=0;X<MAXX;X++) for(Y=0;Y<MAXY;Y++) {
		while(tplace[X][Y] == 0) {
			rnd = rand()%5; /*(0 to 4)*/
			if(number[rnd]>0) {
				area_map[X][Y]=rnd;
				number[rnd]=number[rnd]-1;
				tplace[X][Y]=1;
			}
		}
	}

	/*ALL AREAS PLACED, NOW DETERMINE SECTOR TYPE
 *fill in each area with sectors
 *      1)   water
 *      2)   water with major islands (25% land)
 *      3)   50/50 water/land
 *      4)   land with major water (75% Land)
 *      5)   land
 */
	for(Y=0;Y<MAXY;Y++) for(X=0;X<MAXX;X++) {
		/*fill in edges*/
		fill_edge(X,Y);
		/*fill in center*/
		for(i=1;i<7;i++) {
			for(j=1;j<7;j++) switch(area_map[X][Y]){

			case 0:
				if((rand()%100) < 95) type[X*8+i][Y*8+j]=WATER;
				else type[X*8+i][Y*8+j]=HALF;
				break;
			case 1:
				if(rand()%2 == 0) type[X*8+i][Y*8+j]=WATER;
				else type[X*8+i][Y*8+j]=HALF;
				break;
			case 2:
				if (rand()%2 == 0) type[X*8+i][Y*8+j]=WATER;
				else type[X*8+i][Y*8+j]=LAND;
				break;
			case 3:
				if (rand()%2 == 0) type[X*8+i][Y*8+j]=LAND;
				else type[X*8+i][Y*8+j]=HALF;
				break;
			case 4:
				if ((rand()%100) < 95) type[X*8+i][Y*8+j]=LAND;
				else type[X*8+i][Y*8+j]=HALF;
				break;
			default:
				newerror("Uh oh!!!  The world has gone wacky.");
				newreset();
				abrt();
			}
		}
	}

	/*calculate all 50% areas*/
	for(X=0;X<MAPX;X++) for(Y=0;Y<MAPY;Y++) {
		if(type[X][Y] == HALF)
			if(rand()%100 >= (100-pwater)) {
				type[X][Y] = LAND;
			}
			else type[X][Y] = WATER;
	}

	chance=0;
	for(X=0;X<MAPX;X++) for(Y=0;Y<MAPY;Y++)
		if(type[X][Y] == WATER) chance++;

	mvprintw(10,0,"Water .................  %d out of %d sectors",chance,NUMSECTS);
	clrtoeol();
	newmsg("Day 2... God added water to the world");
	sleep(1);

	/*Newly added code to smooth the world out*/
	for(X=1;X<MAPX-1;X++) for(Y=1;Y<MAPY-1;Y++) {
		chance = 0;
		/*count # of land and sea sides*/
		for(i=X-1;i<=X+1;i++) for(j=Y-1;j<=Y+1;j++)
			if(type[i][j] == LAND) chance++;
		if(rand()%9 < chance) type[X][Y] = LAND;
		else type[X][Y] = WATER;
	}
	chance=0;
	for(X=0;X<MAPX;X++) for(Y=0;Y<MAPY;Y++)
		if(type[X][Y] == WATER) chance++;

	mvprintw(10,0,"Water .................  %d out of %d sectors",chance,NUMSECTS);
	clrtoeol();
	newmsg("But God was not pleased... and smoothed the oceans.");
	sleep(1);

	/*Adjust world given sectors as land or sea, place vegetation,
	designation, and altitude */

	for(i=0;i<MAPX;i++) for(j=0;j<MAPY;j++)
		if(type[i][j]==LAND) sct[i][j].altitude = CLEAR;
		else sct[i][j].altitude = WATER;

	/* place  */
	avvalue	= PMOUNT * (100-pwater);
	avvalue	/= 10000;
	nmountains	= NUMSECTS * avvalue;
	
	mvprintw(11,0,"Hills and Mountains....  %d out of %d sectors",nmountains,NUMSECTS);
	newmsg("Day 3... God created hills and mountains");
	sleep(1);

	/* heuristic says that 5 is cutoff number to stop placing ranges */
	/* and 1 third of mountains are placed as random hills		*/
	while(nmountains>5 && nmountains> (NUMSECTS*avvalue)/3) {
		X1 = rand()%(MAPX-8);	/* Place one endpoint of range */
		Y1 = rand()%(MAPY-8);
		X2 = (rand()%8) + X1;	/* Place second endpoint */
		Y2 = (rand()%8) + Y1;

		valid = FALSE;
		for(x=X1;x<=X2;x++){
			if(X1<X2)	y=((Y2-Y1)*(x-X1)/(X2-X1))+Y1;
			else		y=Y1;
			for(j=y-2;j<=y+2;j++) if(j>=0)
			if( type[x][j] != LAND && rand()%2==0 ) {
				valid = TRUE;
				break;
			}
		}
		if( valid==TRUE) continue;

		/*fill in mountain range:
			 	y-2	y-1	y	y+1	y+2
			%peak	0	10	20	10	0
			%mtn	10	40	80	40	10
			%hill	40	30	0	30	40
		*/
		for(x=X1;x<=X2;x++) {
			if(X1<X2)	y=((Y2-Y1)*(x-X1)/(X2-X1))+Y1;
			else		y=Y1;

			if(type[x][y] == LAND){
				if(rand()%100>80) {	TOPEAK(x,y);
				} else {		TOMT(x,y) 
				}
			}

			if((y < MAPY - 1) && type[x][y+1] == LAND) {
				rnd=rand()%100+1;
				if(rnd>90) {		TOPEAK(x,y+1);
				} else if(rnd>50) {	TOMT(x,y+1);
				} else if(rnd>20) {	TOHILL(x,y+1);
				}
			}
			if((y!=0) && type[x][y-1] == LAND ) {
				rnd=rand()%100+1;
				if(rnd>90) {		TOPEAK(x,y-1);
				} else if(rnd>50) {	TOMT(x,y-1);
				} else if(rnd>20) {	TOHILL(x,y-1);
				}
			}
			if((y>=2)
			&&(type[x][y-2] == LAND )) {
				rnd=rand()%100+1;
				if(rnd>90) { 		TOMT(x,y-2);
				} else if(rnd>50){	TOHILL(x,y-2);
				}
			}
			if((y < MAPY - 2)
			&&(type[x][y+2] == LAND )) {
				rnd=rand()%100+1;
				if(rnd>90) {		TOMT(x,y+2);
				} else if(rnd>50) {	TOHILL(x,y+2);
				}
			}
		}
	}

	/*fill in random hills to work out,not to left of to water*/
	while(nmountains>0) {
		x = rand()%(MAPX-1);
		y = rand()%(MAPY-1);
		if((type[x][y]==LAND)&&(type[x+1][y]==LAND))
			TOHILL(x,y);
	}

	/*make sure no peak or mountain is next to water*/
	for(y=1;y<MAPY-1;y++) for(x=1;x<MAPX-1;x++)
		if((sct[x][y].altitude==PEAK)
		||(sct[x][y].altitude==MOUNTAIN))
			for(i=0;i<=2;i++) for(j=0;j<=2;j++)
				if(sct[x+i-1][y+j-1].altitude==WATER)
					sct[x][y].altitude=HILL;

	/*FIGURE OUT SECTOR VEGETATION TYPE
 *use sector.altitude, and sector to determine vegetation
 *from water is distance from nearest water
 */

	for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++)
		if(type[x][y]==LAND)
		{
			sct[x][y].vegetation=(*(veg+3+rand()%5));
			/*if hill then decrement vegetation*/
			if(sct[x][y].altitude==HILL) {
				for(n=3;n<9;n++)
					if(sct[x][y].vegetation==(*(veg+n)))
						sct[x][y].vegetation=(*(veg+n-1));
				if(area_map[x/8][y/8]<=1) sct[x][y].vegetation=VOLCANO;
			}
			else if(sct[x][y].altitude==MOUNTAIN)
				if((rand()%6==4)&&((y>MAPY/2+8)||(y<MAPY/2-8)))
					sct[x][y].vegetation=ICE;
				else sct[x][y].vegetation=(*(veg+2+rand()%3));
			else if(sct[x][y].altitude==PEAK)
				if((rand()%3==0)&&((y>MAPY/2+8)||(y<MAPY/2-8)))
					sct[x][y].vegetation=ICE;
				else sct[x][y].vegetation=VOLCANO;
		}

	/*REWORK POLEAR/EQUATORIAL sector.vegetation*/
	/*Determine which areas are North Pole and Equatorial*/
	/*polar work*/
	for(x=0;x<MAPX;x++)
	{
		for(y=0;y<6;y++) if(type[x][y]==LAND)
		{
			if(rand()%4 == 0) sct[x][y].vegetation = ICE;
			else for(n=3;n<10;n++)
				if(sct[x][y].vegetation==(*(veg+n)))
					sct[x][y].vegetation=(*(veg+(n-1)));
		}
		for(y=MAPY-7;y<MAPY;y++) if(type[x][y]==LAND)
		{
			if(rand()%4 == 0) sct[x][y].vegetation = ICE;
			else for(n=3;n<10;n++)
				if(sct[x][y].vegetation==(*(veg+n)) )
					sct[x][y].vegetation=(*(veg+(n-1)));
		}
	}

	/*insert equator*/
	for(y=(MAPY/2)-8;y<=(MAPY/2)+8;y++) for(x=0;x<MAPX;x++)
		if(type[x][y]==LAND)
			if(rand()%10 ==0) sct[x][y].vegetation=DESERT;
			/*increment vegetation if between Waste and Jungle*/
			else for(n=2;n<9;n++)
				if((sct[x][y].vegetation==(*(veg+n)))
				&&(sct[x][y].altitude==CLEAR)
				&&(rand()%4==0))
				sct[x][y].vegetation=(*(veg+(n+1)));

	for(y=((MAPY/2)-2);y<=((MAPY/2)+2);y++) for(x=0;x<MAPX;x++)
		if((type[x][y]==LAND)&&(sct[x][y].altitude==CLEAR))
			if(rand()%10 == 0) sct[x][y].vegetation=DESERT;
			else if(rand()%10 == 0) sct[x][y].vegetation=JUNGLE;
			else if(rand()%10 == 0) sct[x][y].vegetation=SWAMP;
			/*increment vegetation again, but only Waste to Light*/
			else for(n=2;n<4;n++)
				if(sct[x][y].vegetation==(*(veg+n)))
					sct[x][y].vegetation=(*(veg+(n+1)));

	/*expand swamps*/
	for(y=2;y<MAPY;y++) for(x=2;x<MAPX;x++)
		if(sct[x][y].vegetation==SWAMP)
			for(i=0;i<2;i++) for(j=0;j<2;j++)
				if((type[x-i][y-j]==LAND)&&((rand()%3)==0))
					sct[x-i][y-j].vegetation=SWAMP;
	/*expand deserts*/
	for(y=2;y<MAPY;y++) for(x=2;x<MAPX;x++)
		if(sct[x][y].vegetation==DESERT)
			for(i=0;i<2;i++) for(j=0;j<2;j++)
				if((type[x-i][y-j]==LAND)&&((rand()%3)==0))
					sct[x-i][y-j].vegetation=DESERT;

	/*change all volcanos to peaks */
	for(y=1;y<MAPY;y++) for(x=1;x<MAPX;x++)
		if(sct[x][y].vegetation==VOLCANO) sct[x][y].altitude=PEAK;

	/*make sure no desert is next to water*/
	for(y=1;y<MAPY-1;y++) for(x=1;x<MAPX-1;x++)
		if(sct[x][y].vegetation==DESERT)
			for(i=0;i<=2;i++) for(j=0;j<=2;j++)
				if(sct[x+i-1][y+j-1].altitude==WATER)
					sct[x][y].vegetation=LT_VEG;

	free(tplace);
	free(type);
	free(area_map);
}

void
rawmaterials() 		 /*PLACE EACH SECTOR'S RAW MATERIALS */
{
	int i,j;
	register int x,y;
	int X1,Y1;
	int valid;
	int nmountains;
	struct	s_sector	*sptr;
	char newstring[BIGLTH];

	TURN=1;

	nmountains = 10 * (END_NORMAL+1);
	for(i=0;i<=END_NORMAL;i++) nmountains -= ( *(tg_value+i) - '0');

	newmsg("Day 4... God placed the world's raw materials");
	sleep(1);
	for(y=0;y<MAPY;y++) for(x=0;x<MAPX;x++) {

		sptr = &sct[x][y];

		/*default designations*/
		sptr->designation=DNODESIG;
		sptr->tradegood = TG_none;

		/* default setup of sector */
		sptr->owner = sptr->metal = sptr->jewels = sptr->fortress = 0;
		sptr->people = 0l;

		if( is_habitable(x,y)==FALSE ) continue;

		/* exotic trade goods !!! */
		if(rand()%100 < TRADEPCT) {
			valid = FALSE;
			if((rand()%100 < METALPCT )||sptr->altitude==MOUNTAIN){
				getmetal( sptr );	/* metal */
			} else if((rand()%(100-METALPCT))<JEWELPCT ) {
				getjewel( sptr );	/* jewel */
			} else while( valid == FALSE ) { /* random good */
				j=rand()%nmountains;
				for(i=0;i<=END_NORMAL;i++) {
					j -= (10 - ( *(tg_value+i) - '0'));
					if(j <= 0) break;
				}	/* 'i' now random trade good type */

				/* fish	- next to water */
				if(i==TG_fish){
				for(X1=x-1;X1<=x+1;X1++)for(Y1=y-1;Y1<=y+1;Y1++)
				if((ONMAP(X1,Y1))
				&&(sct[X1][Y1].altitude==WATER)){
					valid=TRUE;
					break;
				}

				if(valid==FALSE) continue;
				}

				/* corn,fruit - should be arable land */
				if(((i==TG_corn)||(i==TG_fruit))
				&&(tofood(sptr,0)<6)) continue;

				/* timber,pine,oak - wood/forest */
				if(((i==TG_timber)||(i==TG_pine)||(i==TG_oak))
				&&(sptr->vegetation!=FOREST)
				&&(sptr->vegetation!=WOOD)) continue;

				valid = TRUE;
				sptr->tradegood = i;
			}
			if(sptr->tradegood == TG_none)
				newerror("??? DEBUG -tradegood==NONE");
		}
	}

	mvaddstr(13,5,"All manner of creatures were created: big ones, little ones,");
	mvaddstr(14,0,"fat ones, skinny ones, orange ones, turquois ones, bright blue ones.");
	mvaddstr(15,0,"WAIT!!!  God has suddenly realized that smurfs were taking things");
	mvaddstr(16,0,"too far and stopped creating new ones to place everybody on the map...");
	newmsg("Day 5... God decreed that world would be populated");
	sleep(1);
	move(14,0);
	clrtoeol();
	move(15,0);
	clrtoeol();
	move(16,0);
	clrtoeol();
	populate();
	MERCMEN = ST_MMEN;
	MERCATT = ST_MATT;
	MERCDEF = ST_MDEF;
	newmsg("Day 6... God, believing in long weekends, went and got smashed");
	sleep(1);
	newmsg("Day 7... God rested (to get rid of that stupid hangover)");
	sleep(1);
	sprintf(newstring," ...Log in via 'conquer -n god");
	if (strcmp(datadir,"[default]")!=0) {
		strcat(newstring," -d ");
		strcat(newstring,datadir);
	}
	strcat(newstring,"'");
	newerror(newstring);
	sprintf(newstring," ...Players may be added via 'conqrun -a");
	if (strcmp(datadir,"[default]")!=0) {
		strcat(newstring," -d ");
		strcat(newstring,datadir);
	}
	strcat(newstring,"'");
	newerror(newstring);
}

/*fill: subroutine to fill in a square edges with land or sea*/
void
fill_edge(AX,AY)
{
/*      1)   water
 *      2)   water with major islands (25% land)
 *      3)   50/50 water/land
 *      4)   land with major water (75% Land)
 *      5)   land
 */
	register int i;
	int edgearea, X0, Y0, X1, Y1, X2, Y2, X3, Y3, X4, Y4;
	int area;

	X0=AX;
	Y0=AY;
	X1=X0-1;
	Y1=Y0;
	X2=X0+1;
	Y2=Y0;
	X3=X0;
	Y3=Y0-1;
	X4=X0;
	Y4=Y0+1;

	/*NORMALIZE FOR EDGE OF WORLD*/
	if( X1 < 0 ) X1 = MAXX - 1;
	if( X2 >= MAXX ) X2 = 0;
	if( Y3 < 0 ) Y3 = MAXY - 1;
	if( Y4 >= MAXY ) Y4 = 0;

	area=area_map[X0][Y0];
	/*fill in south*/
	edgearea=area_map[X4][Y4];
	if(area + edgearea > 6) for(i=0;i<8;i++) type[X0*8+i][Y0*8+7] = LAND;
	else if((area + edgearea)>3) {
		for(i=0;i<8;i++)
			if(rand()%2 == 0) type[(X0*8)+i][Y0*8+7] = LAND;
			else type[(X0*8)+i][Y0*8+7] = WATER;
	}
	else for(i=0;i<8;i++) type[(X0*8)+i][Y0*8+7] = WATER;
	/*fill in east*/
	edgearea=area_map[X2][Y2];
	if(area + edgearea > 6) for(i=0;i<8;i++) type[X0*8+7][Y0*8+i] = LAND;
	else if((area + edgearea)>3) {
		for(i=0;i<8;i++)
			if(rand()%2==0) type[X0*8+7][Y0*8+i] = LAND;
			else type[X0*8+7][Y0*8+i] = WATER;
	}
	else for(i=0;i<8;i++) type[X0*8+7][Y0*8+i] = WATER;
	/*fill in west*/
	edgearea=area_map[X1][Y1];
	if(area + edgearea > 6) for(i=0;i<=7;i++) type[X0*8][Y0*8+i] = LAND;
	else if((area + edgearea)>3) {
		for(i=0;i<8 ;i++)
			if(rand()%2==0) type[X0*8][Y0*8+i] = LAND;
			else type[X0*8][Y0*8+i] = WATER;
	}
	else for(i=0;i<8 ;i++) type[X0*8][Y0*8+i] = WATER;
	/*fill in north*/
	edgearea=area_map[X3][Y3];
	if(area + edgearea > 6) for(i=0;i<8 ;i++) type[X0*8+i][Y0*8] = LAND;
	else if((area + edgearea)>3) {
		for(i=0;i<8 ;i++)
			if(rand()%2==0) type[(X0*8)+i][Y0*8] = LAND;
			else type[(X0*8)+i][Y0*8] = WATER;
	}
	else for(i=0;i<8 ;i++) type[(X0*8)+i][Y0*8] = WATER;
}

/* ALLOCATE POPULATIONS OF THE WORLD*/
void
populate()
{
	int	i=0,x=0,y=0,j=0,xloc,yloc,xpos,ypos;
	int	nvynum=0,armynum=0,points,shipsize,temp,cnum;
	short	short1,short2;			/*temporary short variables */
	short	class;

	int	loopcnt=0;	
	int	pirarmy=0,barbarmy=0,nomadarmy=0,lizarmy=0; 
	short	npirates=0,nbarbarians=0,nnomads=0,nlizards=0;

	FILE *fp, *fopen();
	char line[LINELTH+1],allign;
	char fname[FILELTH];

	/*set up god but dont place -- do not change leader name*/
	curntn = &ntn[0];
	strcpy(curntn->name,"unowned");
	curntn->race=GOD;
	curntn->location=GOD;
	curntn->powers=KNOWALL;	/* so god can see the map */
	curntn->powers|=NINJA;	/* so god can see exact unit numbers */
	curntn->mark='-';

	for( country=1; country<NTOTAL; country++ ) {
		curntn = &ntn[country];
		if( isactive( curntn->active )) {
			newerror("THIS SHOULDNT HAPPEN");
			continue;
		}
		strcpy(curntn->passwd,ntn[0].passwd);
		curntn->powers=0;
		curntn->repro=0;
		curntn->active=INACTIVE;
		curntn->maxmove=0;
		curntn->mark='-';
	}

	for( country=0; country<NTOTAL; country++ ) {
		for(i=country;i<NTOTAL;i++) {
			ntn[country].dstatus[i]=UNMET;
			ntn[i].dstatus[country]=UNMET;
		}
	}

#ifdef MONSTER
	for( country=NTOTAL-1; country>=NTOTAL-4; country-- ) {
		curntn = &ntn[country];
		if( country==NTOTAL-1 ) {
			strncpy(curntn->name,"lizard",10);
			strncpy(curntn->leader,"dragon",10);
			curntn->active=NPC_LIZARD;
			curntn->race=LIZARD;
		} else if( country==NTOTAL-2 ) {
			strcpy(curntn->name,"savages");
			strcpy(curntn->leader,"shaman");
			curntn->active=NPC_SAVAGE;
			curntn->race=SAVAGE;
		} else if( country==NTOTAL-3 ) {
			strcpy(curntn->name,"nomad");
			strcpy(curntn->leader,"khan");
			curntn->active=NPC_NOMAD;
			curntn->race=NOMAD;
		} else if( country==NTOTAL-4 ) {
			strcpy(curntn->name,"pirate");
			strcpy(curntn->leader,"captain");
			curntn->active=NPC_PIRATE;
			curntn->race=PIRATE;
		}

		curntn->aplus=0;
		curntn->dplus=0;
		curntn->powers=KNOWALL;
		curntn->mark='*';
		curntn->maxmove=12;
		curntn->repro=5;

		/* everybody hates monster nations */
		if(ismonst(ntn[country].active)) 
		for(i=1;i<NTOTAL;i++) {
			ntn[country].dstatus[i]=WAR;
			ntn[i].dstatus[country]=WAR;
		}
	}

	/* create one inhabitant per MONSTER sectors */
	temp = (NUMSECTS)/MONSTER;
	while( temp-- >0 ) switch( rand()%4 ) {
		case 0: if(npirates < MAXNAVY )		npirates++;
			break;
		case 1: if(nbarbarians < MAXARM )	nbarbarians++;
			break;
		case 2: if(nnomads < MAXARM )		nnomads++;
			break;
		case 3: if(rand()%3==0) {
				if(nlizards < MAXARM/2 ) nlizards++;
			} else	if(nnomads < MAXARM )	nnomads++;
			break;
	}
	mvprintw(13,0,"Placing %d lizard cities, %d pirates, %d savages, and %d nomad tribes",nlizards,npirates,nbarbarians,nnomads);
	clrtoeol();
	refresh();

	while((nlizards+npirates+nbarbarians+nnomads > 0 )&&(loopcnt++ <5000)) {
		if( nlizards>0 ) {
			for(country=1;country<NTOTAL;country++)
				if( ntn[country].active == NPC_LIZARD ) break;
			armynum=lizarmy;
		} else	if( nbarbarians>0 ) {	
			for(country=1;country<NTOTAL;country++)
				if( ntn[country].active==NPC_SAVAGE ) break;
			armynum=barbarmy;
		} else	if( nnomads>0 )	 {	
			for(country=1;country<NTOTAL;country++)
				if( ntn[country].active == NPC_NOMAD ) break;
			armynum=nomadarmy;
		} else	if( npirates>0 ) {
			for(country=1;country<NTOTAL;country++)
				if( ntn[country].active == NPC_PIRATE ) break;
			armynum=pirarmy;
		}

		curntn = &ntn[country];

		/*50% chance will be close to map edge, else anywhere*/
		if((rand()%2==0)&&(curntn->active!=NPC_LIZARD)){
			if(rand()%2==0) {
				x=(rand()%20);
				y=(rand()%20);
			} else {
				x=(MAPX-(rand()%20)-1);
				y=(MAPY-(rand()%20)-1);
			}
			if(rand()%2==0) x = rand()%MAPX;
			else y = rand()%MAPY;
		} else {
			x = rand()%MAPX;
			y = rand()%MAPY;
		}

		if(sct[x][y].owner!=0)	continue;
		if(!is_habitable(x,y))	continue;

		sct[x][y].owner = country;

		/* now place people*/
		switch( curntn->active ) {
		case NPC_LIZARD:
			nlizards--;
			sct[x][y].designation = DFORT;
			sct[x][y].metal = 0;
			sct[x][y].jewels = 8 + (i=rand()%30);
			sct[x][y].tradegood = TG_platinum;
			/* make fortificaton consistant with gold */
			sct[x][y].fortress = 6+i/5;
			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if((ONMAP(i,j))&&(sct[i][j].altitude!=WATER))
					sct[i][j].owner = country;
			P_AMOVE=0;
			P_AXLOC=x;
			P_AYLOC=y;
			P_ASTAT=GARRISON;
			P_ASOLD=750+100*(rand()%10);
			P_ATYPE=defaultunit(country);
			armynum++;
			lizarmy++;
			P_AMOVE=8;
			P_AXLOC=x;
			P_AYLOC=y;
			P_ASTAT=ATTACK;
			P_ASOLD=750+100*(rand()%10);
			P_ATYPE=defaultunit(country);
			lizarmy++;
			break;
		case NPC_PIRATE:
			/* pirates must be on islands */
			temp=TRUE;
			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++) 
				if((ONMAP(i,j)) && ( x!=i || y!=j)
				&&( sct[i][j].altitude!=WATER)) {
					/* fifty percent chance of failure */
					/* for every non-land sector       */
					if (sct[i][j].owner!=0 || rand()%2==0)
						temp=FALSE;
				}

			if(temp==FALSE) {
				sct[x][y].owner = 0;
				break;
			}

			/* build the island */
			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
			if((ONMAP(i,j)) && ( x!=i || y!=j))
			if( sct[i][j].altitude!=WATER ) {
				sct[i][j].altitude=WATER;
				sct[i][j].vegetation=WATER;
				sct[i][j].tradegood=TG_none;
				sct[i][j].jewels=0;
				sct[i][j].metal=0;
			}

			npirates--;
			sct[x][y].designation = DBASECAMP;
			P_AMOVE=8;
			P_AXLOC=x;
			P_AYLOC=y;
			P_ASTAT=ATTACK;
			P_ASOLD=150+100*(rand()%3);
			P_ATYPE=defaultunit(country);
			pirarmy++;
			P_NXLOC=x;
			P_NYLOC=y;
			P_NPEOP=0;
			P_NARMY=MAXARM;
			shipsize = N_LIGHT;
			(void) NADD_WAR( rand()%5+2 );
			shipsize = N_MEDIUM;
			(void) NADD_WAR( rand()%3+1 );
			shipsize = N_HEAVY;
			(void) NADD_WAR( rand()%2 );
			P_NCREW=SHIPCREW;
			nvynum++;
			break;
		case NPC_NOMAD:
			nnomads--;
			P_AXLOC=x;
			P_AYLOC=y;
			P_ASTAT=ATTACK;
			P_ASOLD=100+100*(rand()%8);
			P_ATYPE=defaultunit(country);
			nomadarmy++;
			break;
		case NPC_SAVAGE:
			nbarbarians--;
			P_AXLOC=x;
			P_AYLOC=y;
			P_ASTAT=ATTACK;
			P_ASOLD=100+100*(rand()%4);
			P_ATYPE=defaultunit(country);
			barbarmy++;
			break;
		}
	}

	/* put random monsters around the world */
	for(country=1;country<NTOTAL;country++) {
		if( ntn[country].active != NPC_SAVAGE ) continue;
		curntn = &ntn[country];
		armynum=barbarmy;
		while(armynum<MAXARM) {
			x = rand()%MAPX;
			y = rand()%MAPY;
			if (is_habitable(x,y)&&sct[x][y].owner==0) {
				sct[x][y].owner = country;
				if( sct[x][y].jewels==0 ) 
					getjewel( &(sct[x][y]) );
				P_AXLOC= x;
				P_AYLOC= y;
				P_ASTAT= ATTACK;
				P_ATYPE= MINMONSTER + rand()%(MAXMONSTER-MINMONSTER+1);
				P_ASOLD= *(unitminsth+(P_ATYPE%UTYPE));
				P_AMOVE= 10;
				armynum++;
			}
		}
	}

	newmsg("... All random population and monsters placed");
	sleep(1);
#endif /* MONSTER */

	for (i=0;i<MAXHELP;i++) {
		sprintf(fname,"%s/%s%d",DEFAULTDIR,helpfile,i);
		if ((fp=fopen(fname,"r"))==NULL) {
			char tempc[BIGLTH];
			sprintf(tempc,"cannot find helpfile <%s>.",fname);
			newerror(tempc);
		}
	}

#ifdef NPC
	mvaddstr(14,0,"Do you want NPC nations in this campaign? (y or n)");
	clrtoeol();
	refresh();
	while( ((i=getch()) != 'y')&&(i != 'n') ) ;
	if( i!='y' ) {
		newmsg("OK; no NPC nations used");
		sleep(1);
		return;
	}
	if((fp=fopen(npcsfile,"r"))==NULL) {
		mvaddstr(14,0,"Do you wish to use default NPC nations file (y or n)?");
		clrtoeol();
		refresh();
		while( ((i=getchar()) != 'y')&&(i != 'n') ) ;
		if( i=='y'){
			sprintf(line,"%s/%s",DEFAULTDIR,npcsfile);
			if ((fp=fopen(line,"r"))==NULL) {
				newerror("Cannot read nation file... no NPCs added");
				return;
			} else newmsg("OK; default nations used");
		} else {
			newmsg("OK; no NPC nations used");
			sleep(1);
			return;
		}
	}

	cnum=1;
	mvaddstr(14,0,"ADDING NATIONS:");
	clrtoeol();
	refresh();
	xpos = 16;
	ypos = 14;
	while(fgets(line,LINELTH,fp)!=NULL) {
		/*read and parse a new line*/
		if(line[0]!='#') {
			xloc = yloc = -1;
			sscanf(line,"%s %s %c %c %c %hd %hd %hd %ld %ld %d %hd %c %d %d %hd",
			ntn[cnum].name,ntn[cnum].leader,&ntn[cnum].race,
			&ntn[cnum].mark,&ntn[cnum].location,&ntn[cnum].aplus,
			&ntn[cnum].dplus,&short1,&ntn[cnum].tgold,
			&ntn[cnum].tmil,&points,&short2,&allign,&xloc,&yloc,
			&class);

			country=cnum;
			curntn = &ntn[country];
			curntn->class = (short)class;
			sprintf(line," %s (%s)",curntn->name,*(Class+curntn->class));
			mvaddstr(ypos,xpos,line);
			xpos += strlen(line);
			if (xpos > COLS-20) {
				xpos = 5;
				ypos++;
			}
			refresh();
			if( cnum > MAPX*MAPY/NPC*(100-pwater)/100 ) {
				sprintf(line,"World too small to add npc nation %d %s",cnum,curntn->name);
				newerror(line);
				continue;
			} 
			if( isactive(ntn[cnum].active) ) {
				sprintf(line,"Not enough available nations to add npc nation %d %s",cnum,curntn->name);
				newerror(line);
				continue;
			}
			curntn->maxmove = short1;
			curntn->repro = short2;
			if( allign == 'G' )
				curntn->active = PC_GOOD;
			else if( allign == 'N' )
				curntn->active = PC_NEUTRAL;
			else if( allign == 'E' )
				curntn->active = PC_EVIL;
			else if( allign == 'g' )
				curntn->active = GOOD_6FREE;
			else if( allign == 'n' )
				curntn->active = NEUTRAL_6FREE;
			else if( allign == 'e' )
				curntn->active = EVIL_6FREE;
			else if( allign == 'i' )
				curntn->active = ISOLATIONIST;
			else {
				sprintf(line,"invalid nation alignment (%c)");
				newerror(line);
				newreset();
				abrt();
			}
			strcpy(curntn->passwd,ntn[0].passwd);

			points -= doclass( class, FALSE );
			points -= startcost();
			if(points < 10 ) {
				newerror("ERROR: nation doesn't have enough points left for 10000 civilians");
				newreset();
				abrt();
			}
			curntn->tciv = 1000L * points;

			if(curntn->race==HUMAN){
				curntn->powers|=WARRIOR;
				exenewmgk(WARRIOR);
			} else if(curntn->race==DWARF){
				curntn->powers|=MINER;
				exenewmgk(MINER);
			} else if(curntn->race==ELF){
				curntn->powers|=THE_VOID;
				exenewmgk(THE_VOID);
			} else if(curntn->race==ORC){
				curntn->powers|=MI_MONST;
				exenewmgk(MI_MONST);
			} else {
				curntn->powers|=WARRIOR;
				exenewmgk(WARRIOR);
			}
			curntn->tfood= curntn->tciv * 3;
			curntn->metals=10000L;
			curntn->jewels=10000L;
			if (cnum < NTOTAL) cnum++;
			place(xloc,yloc);
			att_setup(country);	/* nation attributes */
		}
	}
	att_base();	/* get nation attributes */
	newmsg("All NPC nations placed");
	sleep(1);
#endif /* NPC */
}
