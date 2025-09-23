/* Conquer: Copyright (c) 1988 by Edward M Barlow */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <pwd.h>
#include <ctype.h>
#include "header.h"
#include "data.h"

#ifdef RANEVENT
char	*names[] = {		/* must end in single character name */
	"groo","brok","vul","poin","srop","hoga","nobi","bonz","gail",
	"lynn","zorb","theed","urda","anima","bedlam","delos","quin",
	"xynd","putz","erde","clym","fanz","ilth","X"
};

extern FILE *fnews;
extern short country;

char eventstr[LINELTH+1];
int xpos,ypos;		/* saved x and y position */

char *randevents[] = {
/* 0 */  "a military rebellion",
/* 1 */  "a cult breaks with you",
/* 2 */  "a province rebels",
/* 3 */  "an evil wizard sets up",
/* 4 */  "a tax revolt occurs",
/* 5 */  "open rebellion flares",
/* 6 */  "an army revolts",
/* 7 */  "religions schism",
/* 8 */  "peasants revolt",
/* 9 */  "dragons raid",
/* 10 */ "a famine rages",
/* 11 */ "a hurricane blows over",
/* 12 */ "a tornado hits",
/* 13 */ "a volcano erupts",
/* 14 */ "a royal wedding occurs",
/* 15 */ "new alloy discovered",
/* 16 */ "royal advisor discovered to be spy",
/* 17 */ "gold strike in one sector",
/* 18 */ "gold strike in one sector",
/* 19 */ "gold vein runs out in one goldmine sector",
/* 20 */ "gold vein runs out in one goldmine sector",
/* 21 */ "a flood ravishes",
/* 22 */ "an earthquake quakes",
/* 23 */ "severe frost destroys crops",
/* 24 */ "feared dragon killed",
/* 25 */ "several nomad armies raid",
/* 26 */ "fire ravishes town",
/* 27 */ "black plague rages",
/* 28 */ "pirates raid",
/* 29 */ "savages raid",
/* 30 */ "wizard grants power",
/* 31 */ "magic item grants magic power",
/* 32 */ "ores in one mine run out",
/* 33 */ "new architect strengthens castle walls",
/* 34 */ "new ores discovered + 4-10 metal one sector",
/* 35 */ "skilled diplomacy obtains peace",
/* 36 */ "powerful magi curses nation",
/* 37 */ "severe winter hits",
/* 38 */ "tidal wave -- abandon all coastlands ",
/* 39 */ "ninja destroy general staff",
/* 40 */ "general found to be spy",
/* 41 */ "general prosperity +20% gold",
/* 42 */ "disease kills 20% of soldiers",
/* 43 */ "poor conditions kill 20% of soldiers"
};
#define MAXRANEVENT 43

/*finds unused nation and sets it up partially*/
int
findnew()
{
	int newntn=0,nationis;
	for ( nationis=NTOTAL-1; nationis >= 1; nationis--)
		if(ntn[nationis].active == INACTIVE) newntn=nationis;
	if (newntn == 0) return (0);
	strcpy(ntn[newntn].leader,"rebel");
	strcpy(ntn[newntn].passwd,ntn[0].passwd);
	ntn[newntn].score=0L;
	ntn[newntn].tsctrs=0;
	ntn[newntn].active=NEUTRAL_6FREE;
	return(newntn);
}

/* returns unused character for nation mark */
char
getnewmark()
{
	char tmpchr='A'-1;			/* cap letters first */
	while (TRUE) {
		tmpchr++;
		if( markok( tmpchr, FALSE )) break;
		if(tmpchr=='Z') tmpchr='a'-1;	/* then small letters */
		if(tmpchr=='z') break;	/* otherwise it will loop forever */
	}
	return(tmpchr);
}

/* disolve 'percent' of nation 'target; returns index of new nation */
int
disolve(percent, target, ispsnt)
int target;
int percent;
int ispsnt;		/* true/false */
{
	int new;	/* new nation number */
	int split;	/* number of sectors split */
	int defaultx=(-1), defaulty=(-1), realx=(-1), realy=(-1), dist;
	int i=0,j,armynum,narmynum;

	split =  ntn[target].tsctrs * percent / 100;
	if((split<=7)&&(!ispsnt)) {
		strcpy(eventstr,"nation too small->no sectors will be split");
		return(0);
	}
	/* find starting town */
	if( ispsnt ) {
		for( new=0; new<NTOTAL; new++ ) {
			if((ntn[new].race==ntn[target].race )
			&&( ntn[new].active==NPC_PEASANT )) {
				printf("\tntn %s peasants of same type as target %d\n",ntn[new].name,target);
				sprintf(eventstr,"rebellion joins nation %s",ntn[new].name);
				return(new);
			}
		}
		new = country;
		country=target;
		xpos = ypos = (-1);
		while( i++ < 300 ) {
			rand_sector();
			if(sct[xpos][ypos].people>=300) break;
		} 
		if( i==300 ) {
			strcpy(eventstr,"no sectors available");
			return(0);
		}
		printf("TMP peasant centered on %d,%d\n",xpos,ypos);
		country = new;
		realx = xpos;
		realy = ypos;
	} else for(i=0; i<MAPX; i++) for(j=0; j<MAPY; j++) {
		if(sct[i][j].owner == target){
			if((sct[i][j].designation==DCITY)
		 	||(sct[i][j].designation==DTOWN)
			||((sct[i][j].people>=2000)&&(sct[i][j].designation!=DCAPITOL))){
				if( rand()%3 == 0 ) {
					realx = i;
					realy = j;
				} else {
					defaultx = i;
					defaulty = j;
				}
			} 
		}
	}
	if(realx == (-1)){
		realx = defaultx;
		realy = defaulty;
	}
	if(realx == (-1)) {
		strcpy(eventstr,"no cities available");
		return(0);
	}
	if((new=findnew()) == 0) {
		strcpy(eventstr,"no nations available");
		return(0);
	}
	if(getnewname(new) == 0) {
		strcpy(eventstr,"no names available");
		return(0);
	}

#ifdef HIDELOC
	sprintf(eventstr,"new nation %s created",ntn[new].name);
#else
	sprintf(eventstr,"new nation %s created at %d,%d",ntn[new].name,realx,realy);
#endif /* HIDELOC */
	printf("TMP new nation %s created at %d,%d",ntn[new].name,realx,realy);
	sct[realx][realy].owner=new;
#ifdef CHECKUSER
	ntn[new].uid = getpwnam(LOGIN)->pw_uid;
#endif /* CHECKUSER */
	ntn[new].capx=realx;
	ntn[new].capy=realy;
	sct[realx][realy].designation=DCAPITOL;
	ntn[new].class=ntn[target].class;
	ntn[new].race= ntn[target].race;
	if( !ispsnt ) {
		ntn[new].tgold= ntn[target].tgold* percent / 100;
		ntn[new].tfood= ntn[target].tfood* percent / 100;
		ntn[new].jewels= ntn[target].jewels* percent / 100;
		ntn[new].metals= ntn[target].metals* percent / 100;
		ntn[target].tgold -= ntn[new].tgold;
		ntn[target].tfood -= ntn[new].tfood;
		ntn[target].jewels -= ntn[new].jewels;
		ntn[target].metals -= ntn[new].metals;
	}
	ntn[new].tciv= ntn[target].tciv* percent / 100;
	ntn[new].tmil= ntn[target].tmil* percent / 100;
	ntn[target].tciv -= ntn[new].tciv;
	ntn[target].tmil -= ntn[new].tmil;
	ntn[new].repro= ntn[target].repro;
	ntn[new].maxmove= ntn[target].maxmove;
	if( ispsnt ) {
		ntn[new].aplus= ntn[new].dplus= 0;
	} else {
		ntn[new].aplus= ntn[target].aplus - 10;
		ntn[new].dplus= ntn[target].dplus - 10;
	}
	ntn[new].location= ntn[target].location;
	ntn[new].powers= ntn[target].powers;
	ntn[new].tships= 0;
	ntn[new].tsctrs = split;
	ntn[new].tax_rate = 10;

	/* first check first letter of name */
	if( markok(toupper(ntn[new].name[0]),FALSE) )
		ntn[new].mark = toupper(ntn[new].name[0]);
	else
		ntn[new].mark = getnewmark();

	for ( dist=1 ; dist < 10; dist++) if (split > 0)
	for (i=realx-dist; i<realx+dist; i++)
		for (j=realy-dist; j<realy+dist; j++){
			if(ONMAP(i,j)
			&&( split>0 )
			&&( sct[i][j].designation != DCAPITOL )
			&&( sct[i][j].owner == target)){
				split--;
				if( sct[i][j].people > 0 )
					sct[i][j].owner=new;
			}
		}

	narmynum=1;
	for (armynum=0;armynum<MAXNAVY;armynum++) {
		ntn[new].nvy[armynum].warships = 0;
		ntn[new].nvy[armynum].merchant = 0;
	}
	if(!ispsnt) for (armynum=narmynum; armynum<MAXARM; armynum++) {
		ntn[new].arm[armynum].sold = 0;
		if((ntn[target].arm[armynum].sold>0)
		&&(sct[ntn[target].arm[armynum].xloc][ntn[target].arm[armynum].yloc].owner==new)){
			ntn[new].arm[narmynum].sold
				=ntn[target].arm[armynum].sold;
			ntn[new].arm[narmynum].unittyp
				=ntn[target].arm[armynum].unittyp;
			ntn[new].arm[narmynum].xloc
				=ntn[target].arm[armynum].xloc;
			ntn[new].arm[narmynum].yloc
				=ntn[target].arm[armynum].yloc;
			ntn[new].arm[narmynum].stat
				=ntn[target].arm[armynum].stat;
			ntn[new].arm[narmynum].smove
				=ntn[target].arm[armynum].smove;
			ntn[target].arm[armynum].sold = 0;
			narmynum++;
		}
	}
	if( !ispsnt ) {
		armynum=0;
		ntn[new].arm[0].sold = 300;
		ntn[new].arm[0].unittyp = A_INFANTRY;
		ntn[new].arm[0].xloc = realx;
		ntn[new].arm[0].yloc = realy;
		ntn[new].arm[0].stat = GARRISON;
		ntn[new].arm[0].smove = 0;
	}
	for(dist=0;dist<NTOTAL;dist++) if(dist!=new) {
		/* create realistic diplomatic status */
		if( ispsnt || ismonst( ntn[dist].active ) ) {
			ntn[new].dstatus[dist]=WAR;
			ntn[dist].dstatus[new]=WAR;
		} else if(ntn[target].dstatus[dist]==UNMET) {
			ntn[new].dstatus[dist]=UNMET;
			ntn[dist].dstatus[new]=UNMET;
		} else {
			ntn[new].dstatus[dist]=NEUTRAL;
			ntn[dist].dstatus[new]=NEUTRAL;
		}
	}
	ntn[new].dstatus[target]=WAR;
	ntn[target].dstatus[new]=WAR;
	return(new);
}

/* get new npc nation name from list at start of this file */
int
getnewname(new)
int	new;
{
	int count,i=0;

	for( i=0;strlen(*(names+i)) > 1;i++ ){
		for(count=0;count<NTOTAL;count++)
			if(strcmp(ntn[count].name, *(names+i))==0) break;
		if( count==NTOTAL ) {
			strcpy(ntn[new].name,*(names+i));
			return(1);
		}
	}
	return(0);
}

void
randomevent()
{
	int percent,count, event, newnation, i, j, armynum,x,y;
	int done,holdval;	/*if 1 then event happened */
	long longval;
	long newpower;
	struct s_sector *sptr;

	printf("Random Events\n");

#ifdef VULCANIZE
/* have a volcano erupt on the map based on percent chance of PVULCAN */
	if(rand()%100<PVULCAN) erupt();
#endif

	/* decide what nations get random event */
	for(country=0;country<NTOTAL;country++) {
		curntn = &ntn[country];
		if(( !isntn(curntn->active))
		||(curntn->score<=20L)
		||(curntn->tsctrs<=20))
			continue;

		/* clear the event string */
		strcpy(eventstr,"");
		
		x = 10*curntn->tax_rate - curntn->popularity- curntn->terror - 3*curntn->charity;
#ifdef DEBUG
printf("TEMP: %s chance of peasant revolt is %d (tax=%d pop=%d terror=%d)\n",
		curntn->name, x, curntn->tax_rate, curntn->popularity,
		curntn->terror );
#endif /* DEBUG */

		if((rand()%100)<x) {
			if(rand()%100<PREVOLT){
				holdval=0;
				peasant_revolt( &holdval );
				if( holdval != 0 )
				wdisaster(country,ntn[holdval].capx,ntn[holdval].capy,0,"peasant revolt");
				else printf("revolt in %s fails because:\n\t%s\n",curntn->name,eventstr);
			}
		} 

		x = 5 * curntn->tax_rate - curntn->prestige;
#ifdef DEBUG
printf("TEMP: %s chance of revolt is %d (tax=%d prest=%d)\n",
		curntn->name, x, curntn->tax_rate, curntn->prestige );
#endif /* DEBUG */

		if(( rand()%100)< x ){
			if(rand()%100<PREVOLT){
				event = other_revolt( &holdval );
				if( event != -1 )
				wdisaster(country,ntn[holdval].capx,ntn[holdval].capy,0,randevents[event]);
				else printf("revolt in %s fails because:\n\t%s\n",curntn->name,eventstr);
			}
		}

		if( (rand()%100) < PWEATHER )	weather();

		/* clear the event string for other random events */
		strcpy(eventstr,"");

		/* do truely random events */
		if((rand()%100)* WORLDSCORE < RANEVENT * WORLDNTN * curntn->score){

		percent=0;
		event = rand()%(MAXRANEVENT-9) + 9;
		done=TRUE;
		xpos = ypos = -1;
		/* do the event */
		switch(event) {
		case 9: /*dragon raid -- lose 30% of food*/
			strcpy(eventstr,"lose 30% of food");
			curntn->tfood *= 7L;
			curntn->tfood /= 10L;
			break;
		case 10: /*famine -- food=0 10% starve*/
			curntn->tfood /= 4L;
			strcpy(eventstr,"lose 3/4ths of food & 10% starve");
			for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
			if(sct[i][j].owner==country)
				sct[i][j].people -= sct[i][j].people / 10;
			break;
		case 11: /*hurricane*/
			sptr = rand_sector();
			percent = 10 + rand()%20;
			/* one hex radius */
			for (x=xpos-1;x<=xpos+1;x++)
			for (y=ypos-1;y<=ypos+1;y++) if(ONMAP(x,y)) {
				sptr = &sct[x][y];
				if( !(ISCITY( sptr->designation ))
				&& (sptr->designation != DRUIN))
					sptr->designation = DNODESIG;
				if (sptr->fortress>=1) sptr->fortress--;
				reduce(x, y, percent);
			}
			break;
		case 12:
			/*tornado*/
			sptr = rand_sector();
			if( !(ISCITY( sptr->designation ))
			&& (sptr->designation != DRUIN))
				sptr->designation = DNODESIG;
			else {
				if (sptr->fortress < 2) sptr->fortress = 0;
				else sptr->fortress -= 2;
			}
			percent = 10 + rand()%25;
			reduce(xpos, ypos, percent);
			break;
		case 13:
			/*volcano -- all flee around one mountain -- 30% die*/
			holdval=0;		/* holdval is # of mountains */
			for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
			if(sct[i][j].owner==country && sct[i][j].altitude==MOUNTAIN)
				holdval++;

			if (holdval > 0) count = (rand()%holdval) + 1;
			else {
				done = FALSE;
				break;
			}

			sprintf(eventstr,"all flee, 30%% die in 1 sector range");
			for (xpos=0; count && (xpos<MAPX); xpos++)
			for (ypos=0;count && (ypos<MAPY); ypos++)
			if (( sct[xpos][ypos].owner == country)
			&& (sct[xpos][ypos].altitude == MOUNTAIN)) {
				count--;
				if (count == 0) blowup(xpos,ypos);
			}
#ifdef HIDELOC
			/* hide nation of eruption if HIDELOC */
			done = FALSE;
#endif /* HIDELOC */
			break;
		case 14:
			/*royal wedding (absorb neighbor nation)*/
			/*	takeover ( 100, 0 ); */  
			/* sprintf(eventstr,"absorb neighbor nation %s");*/
			/* something not right.... */
			done=FALSE;
			break;
		case 15:
			/*new alloy +10% combat (WARRIOR...)*/
			if(magic(country,WARRIOR)!=1){
				curntn->powers|=WARRIOR;
				exenewmgk(WARRIOR);
				strcpy(eventstr,"gives WARRIOR power");
			}
			else if(magic(country,CAPTAIN)!=1){
				curntn->powers|=CAPTAIN;
				exenewmgk(CAPTAIN);
				strcpy(eventstr,"gives CAPTAIN power");
			}
			else if(magic(country,WARLORD)!=1){
				curntn->powers|=WARLORD;
				exenewmgk(WARLORD);
				strcpy(eventstr,"gives WARLORD power");
			}
			else {	/* have all three powers... oh well */
				done=FALSE;
			}
			break;
		case 16:
			/*royal advisor is spy -- lose power*/
			/* sprintf(eventstr,"nation loses power %s");*/
			done=FALSE;
			break;
		case 17:
		case 18:
			/*gold/jewel strike one sector */
			holdval = 0;
			for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
				if((sct[i][j].owner == country)
				&& (sct[i][j].jewels == 0)
				&& (is_habitable(i,j))) {
					holdval++;
				}
			if (holdval > 0) count = (rand()%holdval) + 1;
			else {
				done = FALSE;
				break;
			}
			for (xpos=0; count && (xpos<MAPX); xpos++)
			for (ypos=0; count && (ypos<MAPY); ypos++)
			if(( sct[xpos][ypos].owner == country)
			&& (sct[xpos][ypos].jewels == 0)
			&& (is_habitable(xpos,ypos))) {
				if (count-- <=0) {
					getjewel( &sct[xpos][ypos] );
					done=TRUE;
				}
			}
			/* decrement to contain proper value */
			xpos--;
			ypos--;
			break;
		case 19:
		case 20:
			/*gold vein runs out one goldmine sector */
			holdval = 0;
			for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
			if(( sct[i][j].owner == country)
			&& (sct[i][j].designation == DGOLDMINE)){
				holdval+=sct[i][j].jewels;
			}
			if (holdval > 0) count = (rand()%holdval) + 1;
			else count = done = FALSE;
			if (count == FALSE ) break;
			for (xpos=0; count && (xpos<MAPX); xpos++)
			for (ypos=0; count && (ypos<MAPY); ypos++)
			if(( sct[xpos][ypos].owner == country)
			&& (sct[xpos][ypos].designation == DGOLDMINE)){
				count-=sct[xpos][ypos].jewels;
				if (count<=0) {
					count=0;
					done=TRUE;
					sct[xpos][ypos].designation = DNODESIG;
					sct[xpos][ypos].tradegood = TG_none;
					sct[xpos][ypos].jewels = 0;
				}
			}
			/* decrement to contain proper value */
			xpos--;
			ypos--;
			break;
		case 21:
			/*flood*/
			done=FALSE;
			break;
		case 22:	/*earthquake*/
			/* get epicenter */
			sptr = rand_sector();
			percent = 30+rand()%40;
			/* 10% damage in 3 sectors, 25 in 1, 50 in */
			for (x=xpos-3;x<=xpos+3;x++)
			for (y=ypos-3;y<=ypos+3;y++) if(ONMAP(x,y)) {
				reduce(x, y, percent/5 );
			}

			for (x=xpos-1;x<=xpos+1;x++)
			for (y=ypos-1;y<=ypos+1;y++) if(ONMAP(x,y)) {
				reduce(x,y,percent/5);	/* ADDITIONAL % */
				if((rand()%2) == 0)
					DEVASTATE(x,y);
			}

			if(sptr->fortress < 2) sptr->fortress = 0;
			else sptr->fortress -= 2;
			reduce(xpos, ypos, (percent*3)/5); /* ADDITIONAL % */
			break;
		case 23:
			/*frost -- crops ruined*/
			done=FALSE;
			break;
		case 24:
			/*dragon killed + 50000 jewels*/
			longval = rand()%10 * 10000;
			sprintf(eventstr,"you gain %ld jewels",longval);
			curntn->jewels+=longval;
			break;
		case 25:
#ifdef MONSTER
			/*nomad raid -- put large nomad army in area*/
			for( holdval=1;holdval<NTOTAL;holdval++ )
				if( ntn[holdval].active==NPC_NOMAD ) break;

			if( holdval==NTOTAL ) break;
			done=FALSE;
			if(holdval==NTOTAL) break;
			for(count=0; count < 100; count++) if(done <= 3){
			xpos=(rand()%(MAPX-8))+4;
			ypos=(rand()%(MAPY-8))+4;
			/* get army number */
			armynum = -1;
			for(newpower=0; newpower<MAXARM; newpower++)
				if (ntn[holdval].arm[newpower].sold == 0)
					armynum=newpower;
			if(armynum == -1) done=4;
			else if((is_habitable(xpos,ypos))
			&& ( sct[xpos][ypos].owner == country)) {
				ntn[holdval].arm[armynum].xloc =xpos;
				ntn[holdval].arm[armynum].yloc =ypos;
				if(curntn->tmil > 10000)	/* 800-4800 */
				ntn[holdval].arm[armynum].sold =800+50*(rand()%80);
				else if(curntn->tmil > 5000) /* 500-2500 */
				ntn[holdval].arm[armynum].sold =500+50*(rand()%40);
				else if(curntn->tmil > 1000) /* 400-1400 */
				ntn[holdval].arm[armynum].sold =400+20*(rand()%50);
				else	/* 200-600 */
				ntn[holdval].arm[armynum].sold =200+20*(rand()%20);
				ntn[holdval].arm[armynum].unittyp = A_LT_CAV;
				ntn[holdval].arm[armynum].stat =ATTACK;
				done++;
			}
			}
			done=TRUE;
			break;
#endif /* MONSTER */
		case 26:
			/*town burns -- reduce fort and redesignate*/
			holdval=0;
			for (xpos=0; xpos<MAPX; xpos++) 
			for (ypos=0; ypos<MAPY; ypos++)
			if(( sct[xpos][ypos].owner == country)
			&& ( sct[xpos][ypos].designation == DTOWN)){
				holdval++;
			}
			done = FALSE;
			if (holdval > 0) count = (rand()%holdval) + 1;
			else	break;

			percent = 25 + rand()%50;
			for (xpos=0; count && (xpos<MAPX); xpos++)
			for (ypos=0; count && (ypos<MAPY); ypos++)
			if(( sct[xpos][ypos].owner == country)
			&& ( done==FALSE )
			&& ( sct[xpos][ypos].designation == DTOWN)){
				count--;
				if (count<=0) {
					count=0;
					sct[xpos][ypos].designation = DNODESIG;
					reduce(xpos,ypos,percent);
					done=TRUE;
					break;
				}
			}
			/* decrement to contain proper value */
			xpos--;
			ypos--;
			break;
		case 27:
			/*plague -- 40% of populace in cities dies*/
			strcpy(eventstr,"40% of populace & armies in towns die");
			for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
			if( sct[i][j].owner == country ){
				sptr = &sct[i][j];
				if( ISCITY( sptr->designation )){
					sct[i][j].people *= 6;
					sct[i][j].people /= 10;
				}
			}
			for (armynum=0; armynum<MAXARM; armynum++)
			if((P_ASOLD > 0)&&(P_ATYPE<MINLEADER)){
				sptr = &sct[P_AXLOC][P_AYLOC];
				if( ISCITY( sptr->designation )){
					P_ASOLD *= (6);
					P_ASOLD /= (10);
				}
			}
			break;
		case 28: /*pirate raid on harbor*/
			done=FALSE;
			break;
		case 29: /*savages raid*/
			done=FALSE;
			break;
		case 30: /*new magician + RANDOM POWER*/
			/*buy new powers and/or new weapons*/
			if((newpower=getmagic(M_CIV))!=0L){
				for(i=S_CIV;i<S_CIV+E_CIV;i++) 
				if(powers[i]==newpower){
				sprintf(eventstr,"nation %s gets civilian power %s",curntn->name,pwrname[i]);
				}
				exenewmgk(newpower);
			}
			else done=FALSE;
			break;
		case 31: /*new magic item + RANDOM POWER*/
			/*buy new powers and/or new weapons*/
			if((newpower=getmagic(M_MIL))!=0){
				for(i=S_MIL;i<S_MIL+E_MIL;i++) 
				if(powers[i]==newpower)
				sprintf(eventstr,"nation %s gets military power %s",curntn->name,pwrname[i]);
				exenewmgk(newpower);
			}
			else done=FALSE;
			break;
		case 32:
			/* ores run out */
			holdval=0;
			for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
			if(( sct[i][j].owner == country)
			&& (sct[i][j].designation == DMINE)){
				holdval+= sct[i][j].metal;
			}

			done = FALSE;
			if (holdval > 0) count = (rand()%holdval) + 1;
			else break;

			for (xpos=0; count && (xpos<MAPX); xpos++)
			for (ypos=0; count && (ypos<MAPY); ypos++)
			if(( sct[xpos][ypos].owner == country)
			&& (done == FALSE )
			&& (sct[xpos][ypos].designation == DMINE)){
				count-=sct[xpos][ypos].metal;
				if (count<=0) {
					sct[xpos][ypos].metal =0;
					sct[xpos][ypos].tradegood =TG_none;
					sct[xpos][ypos].designation = DNODESIG;
					done=TRUE;
				}
			}
			/* decrement to contain proper value */
			xpos--;
			ypos--;
			break;
		case 33:
			/*new architect strengthens castle walls */
			for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
				if( sct[i][j].owner == country ){
					sptr = &sct[i][j];
					if( ISCITY( sptr->designation ))
						sct[i][j].fortress += 2;
				}
			break;
		case 34: /* new ores discovered */
			holdval=0;
			for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
			if(( sct[i][j].owner == country)
			&& (is_habitable(i,j))
			&&(sct[i][j].metal == 0)) holdval++;

			done = FALSE;
			if (holdval > 0) count = (rand()%holdval)+1;
			else break;

			for (xpos=0; count && (xpos<MAPX); xpos++)
			for (ypos=0; count && (ypos<MAPY); ypos++)
			if(( sct[xpos][ypos].owner == country)
			&& ( done == FALSE )
			&& (is_habitable(xpos,ypos))
			&& (sct[xpos][ypos].metal == 0)){
				if(count-- <= 0) {
					getmetal( &(sct[xpos][ypos]) );
					done=TRUE;
					break;
				}
			}
			/* decrement to contain proper value */
			xpos--;
			ypos--;
			break;
		case 35:
			/*diplomat sets up peace*/
			for(newnation=0;newnation<NTOTAL;newnation++) 
			if(( country!=newnation ) 
			&&( isntn( ntn[newnation].active ))
			&&( ntn[newnation].dstatus[country]>NEUTRAL )){
				ntn[newnation].dstatus[country]=NEUTRAL;
				curntn->dstatus[newnation]=NEUTRAL;
			}
			break;
		case 36:
			/*powerful magi curses nation; lose a power*/
			done=TRUE;
			i=holdval=0;
			while( powers[i] != 0 ){
				if(magic(country,powers[i])==TRUE) holdval++;
				i++;
			}
			if (holdval!=0) holdval = rand()%holdval+1;
			newpower=0L;
			for(i=0;powers[i]!=0 && holdval>0;i++) {
				if (magic(country,powers[i])==TRUE) holdval--;
				if (holdval==0) {
					newpower=powers[i];
					break;
				}
			}
			if(newpower==0L) {
				done=FALSE;
				break;
			}
			/* upgrade powers */
			switch(newpower) {
			case MI_MONST:
				if(magic(country,AV_MONST)==TRUE) {
					newpower=AV_MONST;
					i++;
				}
			case AV_MONST:
				if(magic(country,MA_MONST)==TRUE) {
					newpower=MA_MONST;
					i++;
				}
				break;
			case WARRIOR:
				if(magic(country,CAPTAIN)==TRUE) {
					newpower=CAPTAIN;
					i++;
				}
			case CAPTAIN:
				if(magic(country,WARLORD)==TRUE) {
					newpower=WARLORD;
					i++;
				}
				break;
			case SUMMON:
				if(magic(country,WYZARD)==TRUE) {
					newpower=WYZARD;
					i++;
				}
			case WYZARD:
				if(magic(country,SORCERER)==TRUE) {
					newpower=SORCERER;
					i++;
				}
				break;
			default:
				break;
			}
			/* may not remove racial magics */
			switch(curntn->race) {
			case ORC:
				if(newpower==MI_MONST) done=FALSE;
				break;
			case ELF:
				if(newpower==THE_VOID) done=FALSE;
				break;
			case DWARF:
				if(newpower==MINER) done=FALSE;
				break;
			case HUMAN:
				if(newpower==WARRIOR) done=FALSE;
				break;
			}
			switch(curntn->class) {
			case C_WIZARD:
				if(newpower==SUMMON) done=FALSE;
				if(newpower==WYZARD) done=FALSE;
				break;
			case C_PRIEST:
				if(newpower==RELIGION) done=FALSE;
				break;
			case C_PIRATE:
				if(newpower==SAILOR) done=FALSE;
				break;
			case C_TRADER:
				if(newpower==URBAN) done=FALSE;
				break;
			case C_WARLORD:
				if(newpower==WARLORD) done=FALSE;
				break;
			case C_DEMON:
				if(newpower==DESTROYER) done=FALSE;
				break;
			case C_DRAGON:
				if(newpower==MA_MONST) done=FALSE;
				break;
			case C_SHADOW:
				if(newpower==THE_VOID) done=FALSE;
				break;
			default:
				break;
			}
			/* remove the magic */
			if(done==TRUE) {
				if(magic(country,newpower)==TRUE) {
					curntn->powers ^= newpower;
					removemgk(newpower);
					sprintf(eventstr,"nation %s loses %s power",curntn->name,pwrname[i]);
				} else {
					done=FALSE;
					fprintf(stderr,"ERROR: removing no-magic\n");
				}
			}
			break;
		case 37:
			/*severe winter*/
			done=FALSE;
			break;
		case 38:
			/*tidal wave -- abandon all coastlands */
			done=FALSE;
			break;
		case 39: /*ninja attack paralyzes half your armys P_AMOVE=0*/
		case 40: /*general found to be spy P_AMOVE=0*/
 			strcpy(eventstr,"1/2 nations armies are paralyzed");
			for(armynum=0;armynum<MAXARM;armynum++) if(rand()%2==0)
				P_AMOVE = 0;
			break;
		case 41:
			/*general prosperity +20% gold*/
			if (curntn->tgold > 0l) {
				curntn->tgold += curntn->tgold / 5;
			}
			else 
				curntn->tgold += 50000L;
			break;
		case 42:
		case 43:
			/*kill 20% of armies*/
			for (armynum=0; armynum<MAXARM; armynum++)
				if((P_ASOLD > 0) && (P_ATYPE<MINLEADER)){
					P_ASOLD *= (8);
					P_ASOLD /= (10);
				}
			break;
		default:
			break;
		}
		if(done) wdisaster(country,xpos,ypos,percent,randevents[event]);
		}
	}
}

/* print a report to the appropriate places */
void
wdisaster( cntry,xloc,yloc,prcnt,event )
int cntry,xloc,yloc,prcnt;
char *event;
{
	fprintf(fnews,"1. \t%s in %s\n",event,ntn[cntry].name);
	printf("\t%s in %s\n",event,ntn[cntry].name);

	/*send a message to the country if it is a PC*/
	if(ispc(ntn[cntry].active)) {
		if(mailopen( cntry )!=(-1)) {
		fprintf(fm,"MESSAGE FROM CONQUER\n\n");
		fprintf(fm,"An event occurs within your nation (%s)\n",ntn[cntry].name);
		fprintf(fm,"%s during the %s of Year %d,\n",event,PSEASON(TURN),YEAR(TURN));
		if(xloc != -1)
		fprintf(fm," centered around location %d, %d.\n",xloc,yloc);
		if(prcnt>0) {
			fprintf(fm,"Damage was estimated at about %d%% in severity.\n",prcnt);
		}
		}
	}

	if(strlen(eventstr)>5) {
#ifdef HIDELOC
		/* make sure that volcano locations are not revealed */
		if(strcmp(eventstr,"all flee, 30%% die in 1 sector range")!=0)
#endif /* HIDELOC */
		fprintf(fnews,"1. \tevent in %s -->%s\n",ntn[cntry].name,eventstr);
#ifndef HIDELOC
		if(xloc != -1)
		fprintf(fnews,"1. \tevent in %s -->centered around location %d, %d.\n",ntn[cntry].name,xloc,yloc);
#endif /* HIDELOC */
		printf("\t\t->%s\n",eventstr);
		if(ispc(ntn[cntry].active))
			fprintf(fm,"\t%s\n",eventstr);
	}
	if(ispc(ntn[cntry].active)) mailclose(cntry);
}

int
peasant_revolt(newnation)	/* peasant revolt */
int	*newnation;		/* return nation id */
{
	register int i,j;
	int	armynum;

	/* a little cheating for now */
	if(isnpc( ntn[country].active ) && (rand()%2==0)) {
		strcpy(eventstr,"npc cheating");
		return;
	}
	if((*newnation=disolve(10, country, TRUE)) == 0 ) return;

	curntn = &ntn[*newnation];	/* beware - curntn & country not same */
	curntn->active = NPC_PEASANT;
	curntn->class = 0;

	for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++) {
		if(( sct[i][j].owner == country)
		&&( rand()%2==0 ) 
		&&( sct[i][j].designation != DCAPITOL )
		&&( sct[i][j].people > 0 )
		&&( solds_in_sector(i,j,country)==0)){
			sct[i][j].owner = *newnation;
			for(armynum=0;armynum<MAXARM;armynum++)
			if(P_ASOLD == 0) {
				P_ASOLD = sct[i][j].people/5;
				P_ATYPE = A_MILITIA;
				P_ASTAT = MILITIA;
				P_AXLOC = i;
				P_AYLOC = j;
				sct[i][j].people -= P_ASOLD;
				break;
			}
		}
	}
	curntn->popularity=99;	/* so it dont happen too often */
	curntn = &ntn[country];	/* fix above beware comment */
	return;
}
int
other_revolt( new )	/* return reason and new nation number*/
int	*new;
{
	short	reason = rand()%8;
	switch( reason ) {
	case 0: /* general */
		if((*new=disolve(10, country, FALSE))!=0) return(reason);
		break;
	case 1: /* cult */
		if((*new=disolve(10, country, FALSE))!=0) return(reason);
		break;
	case 2: /* general/province defects*/
		if((*new=disolve(10, country, FALSE))!=0) return(reason);
		break;
	case 3: /* evil wizard sets up */
		if((*new=disolve(10, country, FALSE))!=0) return(reason);
		break;
	case 4: /* tax revolt */
		if((*new=disolve(20, country, FALSE))!=0) return(reason);
		break;
	case 5: /* open rebellion */
		if((*new=disolve(30, country, FALSE))!=0) return(reason);
		break;
	case 6: /* general takes over province*/
		if((*new=disolve(30, country, FALSE))!=0) return(reason);
		break;
	case 7: /*religious schism*/
		if((*new=disolve(30, country, FALSE))!=0) return(reason);
		break;
	}
	return(-1);
}

#ifdef VULCANIZE
/*volcano erupts --- causes devastation in surrounding sectors */
void
erupt()
{
	int i, j, nvolcanos=0, volhold;

	printf("checking for volcanic eruptions\n");
	/* count all of the volcanos */
	for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
		if(sct[i][j].vegetation == VOLCANO) nvolcanos++;

	if(nvolcanos==0) {
		printf("no volcano's found\n");
		return;
	}
	/* choose a random one to erupt */
	volhold = rand()%nvolcanos;
	for (i=0; i<MAPX; i++) for (j=0; j<MAPY; j++)
	if ( sct[i][j].vegetation == VOLCANO ) {
		volhold--;
		if (volhold == 0) blowup(i,j);
	}
}
#endif /* VULCANIZE */

/* blowup a volcano in sector i,j */
void
blowup(i,j)
register int i,j;
{
	register int x,y;
	wdisaster(sct[i][j].owner,i,j,100,"volcano erupted");
	printf("\tvolcano at %d, %d erupts; devastates surounding area\n",i,j);
	fprintf(fnews,"1. \tevent in sector %d, %d->volcanic eruption causes devastation\n",i,j);
	sct[i][j].vegetation = VOLCANO;
	sct[i][j].jewels = 0;
	sct[i][j].metal = 0;
	reduce(i,j,100);
	DEVASTATE(i,j);
	sct[i][j].fortress = 0;
	/* decrease neighboring population and armies 30% */
	for(x=i-1; x<=i+1; x++) for(y=j-1; y<=j+1; y++)
	if((ONMAP(x,y))&&(sct[x][y].altitude != WATER)) {
		reduce(x,y,30);
		DEVASTATE(i,j);
		sct[x][y].fortress = 0;
	}
}

/** reduce will drop armies & and civilians in sector by percent **/
void
reduce(x,y,percent)
int x,y,percent;
{
	long temp;	/* used to avoid overflow problems */
	int armynum,ctry;

	percent = 100 - percent;	/* invert percent so math works */

	/* work on people */
	temp = sct[x][y].people;
	temp *= percent;
	temp /= 100;
	sct[x][y].people = temp;

	/* work on armies */
	for(ctry=1;ctry<NTOTAL;ctry++) {
		for(armynum=0;armynum<MAXARM;armynum++)
		if((ntn[ctry].arm[armynum].xloc==x)
		&&(ntn[ctry].arm[armynum].unittyp<MINLEADER)
		&&(ntn[ctry].arm[armynum].yloc==y)) {
			temp = ntn[ctry].arm[armynum].sold;
			temp *= percent;
			temp /= 100;
			ntn[ctry].arm[armynum].sold = temp;
		}
	}
}

/* returns pointer to random sector in country */
struct s_sector
*rand_sector()
{
	int count=0;
	for(xpos=0;xpos<MAPX;xpos++) for(ypos=0;ypos<MAPY;ypos++)
		if(sct[xpos][ypos].owner == country) count++;
	count = rand()%count;
	for(xpos=0;xpos<MAPX;xpos++) for(ypos=0;ypos<MAPY;ypos++){
		if(sct[xpos][ypos].owner == country) count--;
		if(count==0) return(&sct[xpos][ypos]);
	}
	fprintf(stderr,"could find no location for country %d\n",country);
	abrt();
	return(NULL);	/* stop lint from complaining */
}

void
weather()
{
}
#endif /* RANEVENT */
