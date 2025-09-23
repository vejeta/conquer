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
#include <curses.h>
#include <pwd.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h>
#include <pwd.h>
#include "header.h"
#include "data.h"

/*Declarations*/
char	*getpass();
struct	s_sector **sct;
struct	s_nation ntn[NTOTAL];   /* player nation stats */
struct	s_world	world;
/*is sector occupied by an army?*/
char	**occ;
short	**movecost;
long	startgold=0;

/*offset of upper left hand corner*/
short	xoffset=0,yoffset=0;
/*current cursor postion (relative to 00 in upper corner)*/
/*	position is 2*x,y*/
short	xcurs=0,ycurs=0;
/*display state*/
short	dismode=2;
/* nation id of owner*/
short	country=0;
struct	s_nation	*curntn;
extern char datadir[];
#ifdef REMAKE
int	remake=FALSE;
#endif /*REMAKE*/

FILE *fexe, *fopen();

void
main(argc,argv)
int argc;
char **argv;
{
	int geteuid(), getuid(), setuid(), realuser, l;
	register int i,j;
	char *name;
#ifndef __STDC__
	void srand();
#endif
	int getopt();
	long time();
	/* mflag = make world, a=add player, x=execute, p=print */
	/* rflag = make world from read in files */
	int mflag, aflag, xflag, rflag;
	char string[FILELTH];
	extern char *optarg;
	char defaultdir[BIGLTH],cq_opts[BIGLTH];
	struct passwd *getpwnam(), *pwent;

	umask (MASK);
	mflag = aflag = xflag = rflag = 0;
	srand((unsigned) time((long *) 0));
	strcpy(datadir,"");
	strcpy(cq_opts,"");
	name = string;
	*name = 0;

	/* check conquer options */
	if (getenv(ENVIRON_OPTS)!=NULL) {
		strncpy(cq_opts, getenv(ENVIRON_OPTS), BIGLTH);
	}
	if (cq_opts[0] != '\0') {
		l = strlen(cq_opts);
		for(i=0; i<l; i++) {
			switch(cq_opts[i]) {
			case 'G':
				/* ignore Gaudy display */
				break;
			case 'N':
			case 'n':
				/* ignore nation name */
				for (;i<l && cq_opts[i]!=',';i++);
				break;
			case 'D':
			case 'd':
				/* check for data directory */
				if (strncmp(cq_opts+i+1,"ata=",4)==0) {
					i += 5;
				} else if (strncmp(cq_opts+i+1,"atadir=",7)==0) {
					i += 8;
				} else if (strncmp(cq_opts+i+1,"irectory=",9)==0) {
					i += 10;
				} else if (strncmp(cq_opts+i+1,"ir=",3)==0) {
					i += 4;
				} else {
					fprintf(stderr,"conquer: invalid environment\n");
					fprintf(stderr,"\t%s = %s\n",ENVIRON_OPTS,cq_opts);
					fprintf(stderr,"\texpected <data=NAME>\n");
					exit(FAIL);
				}
				if (i<l) {
					/* grab the data directory */
					for (j=0; j<l-i && cq_opts[i+j]!=',';j++) {
						datadir[j] = cq_opts[i+j];
					}
					datadir[j]='\0';
					i += j-1;
				}
				break;
			case ' ':
			case ',':
				/* ignore commas and spaces */
				break;
			default:
				/* complain */
				fprintf(stderr,"conquer: invalid environment\n");
				fprintf(stderr,"\t%s = %s\n",ENVIRON_OPTS,cq_opts);
				fprintf(stderr,"\tunexpected option <%c>\n",cq_opts[i]);
				exit(FAIL);
				break;
			}
	     }
	}

	/* process the command line arguments */
	while((i=getopt(argc,argv,"maxr:d:"))!=EOF) switch(i){
	/* process the command line arguments */
	case 'm':  /* make a new world*/
		mflag++;
		break;
	case 'a': /* anyone with password can add player*/
		aflag++;
		break;
	case 'x': /* execute (update) program*/
		xflag++;
		break;
	case 'r': /* read map file */
		rflag++;
		if(strlen(optarg) > NAMELTH){
			fprintf(stderr,"ERROR: MAPFILE STEM LONGER THAN %d\n",NAMELTH);
			exit(FAIL);
		}
		strcpy(scenario, optarg);
		break;
	case 'd':
		strcpy(datadir, optarg);
		break;
	case '?': /*  print out command line arguments */
		printf("Command line format: %s [-max -dDIR -rSCENARIO]\n",argv[0]);
		printf("\t-m          make a world\n");
		printf("\t-a          add new player\n");
		printf("\t-x          execute (update) program\n");
		printf("\t-d DIR      to use play different game\n");
		/* printf("\t-r SCENARIO read map while making a new world\n\t\t\tuses SCENARIO.ele, SCENARIO.veg, &  SCENARIO.nat\n"); */
		exit(SUCCESS);
	};

	realuser = getuid();
	/* may now replace user identity */
	(void) setuid (geteuid ()) ;

	/* set proper defaultdir */
	if (datadir[0] != '/') {
		if (strlen(datadir) > 0) {
			sprintf(defaultdir, "%s/%s", DEFAULTDIR, datadir);
		} else {
			strcpy(defaultdir,DEFAULTDIR);
			strcpy(datadir,"[default]");
		}
	} else {
		strcpy(defaultdir,datadir);
	}

	/* now that we have parsed the args, we can got to the
	 * dir where the files are kept and do some work.
	 */
	if (chdir(defaultdir)) {
		printf("unable to change dir to %s\n",defaultdir);
		exit(FAIL);
	}
	if((mflag)||(rflag)) {
#ifdef REMAKE
		/* check if datafile currently exists*/
		if(access(datafile,00) == 0) {
			/* read in the data*/
			readdata();
			verifydata( __FILE__, __LINE__ );

			/* verify ability to remake the world */
			if ((realuser != (getpwnam(LOGIN))->pw_uid ) &&
			    ((pwent=getpwnam(ntn[0].leader)) == NULL ||
				realuser != pwent->pw_uid )) {
				printf("Sorry -- you can not create a world\n");
				printf("you need to be logged in as %s",LOGIN);
				if (strcmp(LOGIN, ntn[0].leader)!=0) {
					printf(" or %s",ntn[0].leader);
				}
				printf(".\n");
				exit(FAIL);
			}
			remake=TRUE;
			printf("************* WARNING!!!! *******************\n\n");
			printf("    There is already a game in progress.\n\n");
			printf("*********************************************\n\n");
			printf("Do you wish to destroy the current game? ");
			scanf("%s",string);
			if (strcmp(string,"yes")!=0 && strcmp(string,"y")!=0) {
				printf("Okay... the world is left intact\n");
				exit(FAIL);
			}
			printf("Are you absolutely certain? ");
			scanf("%s",string);
			if (strcmp(string,"yes")!=0 && strcmp(string,"y")!=0) {
				printf("Okay... the world is left intact\n");
				exit(FAIL);
			}
			printf("The re-destruction of the world has begun...\n");
			sleep(1);
		}
#else
		/* check for god permissions */
		if(realuser!=(getpwnam(LOGIN))->pw_uid) {
			printf("Sorry -- you can not create a world\n");
			printf("you need to be logged in as %s.\n",LOGIN);
			exit(FAIL);
		}

		/* check if datafile already exists*/
		if(access(datafile,00) == 0) {
			printf("ABORTING: File %s exists\n",datafile);
			printf("\tthis means that a game is in progress. To proceed, you must remove \n");
			printf("\tthe existing data file. This will, of course, destroy that game.\n\n");
			exit(FAIL);
		}
#endif /* REMAKE */

		makeworld(rflag);
		sprintf(string,"%sup",isonfile);
		unlink(string);
		exit(SUCCESS);
	}

	/* read data*/
	readdata();
	verifydata( __FILE__, __LINE__ );

	if (aflag) { /* a new player */
		sprintf(string,"%sup",isonfile);
		if(check_lock(string,FALSE)==TRUE) {
			printf("Conquer is updating\n");
			printf("Please try again later.\n");
			exit(FAIL);
		}

		sprintf(string,"%s0",isonfile);
		if(check_lock(string,FALSE)==TRUE) {
			printf("God is currently logged in.\n");
			printf("Please try again later.\n");
			exit(FAIL);
		}

		printf("\n********************************************");
		printf("\n*      PREPARING TO ADD NEW PLAYERS        *");
		printf("\n********************************************\n");
		if( TURN > LASTADD ){
			printf("more than %d turns have passed since game start!\n", LASTADD);
			printf("permission of game administrator required\n");
			if(strncmp(crypt(getpass("\nwhat is conquer super user password:"),SALT),ntn[0].passwd,PASSLTH)!=0)
			{
				printf("sorry...\n");
				exit(FAIL);
			}
		}
		/* prevent more than one addition */
		sprintf(string,"%sadd",isonfile);
		if(check_lock(string,TRUE)==TRUE) {
			printf("Someone else is adding\n");
			printf("Please try again later.\n");
			exit(FAIL);
		}

		/* disable interrupts */
		signal(SIGINT,SIG_IGN);
		signal(SIGQUIT,SIG_IGN);
		newlogin(realuser);
		unlink(string);
		exit(SUCCESS);
	}

#ifdef OGOD
	if ((realuser != (getpwnam(LOGIN)->pw_uid) ) &&
	  ((pwent=getpwnam(ntn[0].leader)) == NULL ||
	  realuser != pwent->pw_uid )) {
		printf("Sorry -- you can not administrate conquer\n");
		printf("you need to be logged in as %s",LOGIN);
		if (strcmp(LOGIN, ntn[0].leader)!=0) {
			printf(" or %s",ntn[0].leader);
		}
		printf("\n");
		exit(FAIL);
	}
#endif /* OGOD */

	if (xflag) {	/* update the game */
#ifndef OGOD
		if ((realuser != (getpwnam(LOGIN))->pw_uid ) &&
		  ((pwent=getpwnam(ntn[0].leader)) == NULL ||
		  realuser != pwent->pw_uid )) {
			printf("sorry -- your uid is invalid for updating\n");
			printf("you need to be logged in as %s",LOGIN);
			if (strcmp(LOGIN, ntn[0].leader)!=0) {
				printf(" or %s",ntn[0].leader);
			}
			printf("\n");
			exit(FAIL);
		}
#endif /* OGOD */
#ifdef RUNSTOP
		/* check if any players are on */
		for (i=0;i<NTOTAL;i++) {
			sprintf(string,"%s%d",isonfile,i);
			if(check_lock(string,FALSE)==TRUE) {
				printf("Nation %d is still in the game.\n",i);
				printf("Update aborted.\n");
				exit(FAIL);
			}
		}
#endif /* RUNSTOP */
		sprintf(string,"%sup",isonfile);
		if(check_lock(string,TRUE)==TRUE) {
			printf("Another update is still executing.\n");
			printf("Update aborted.\n");
			exit(FAIL);
		}
		update();
		writedata();
		unlink(string);
#ifdef TIMELOG
		sprintf(string, "date > %s", timefile);
		system(string);
#endif /* TIMELOG */
		exit(SUCCESS);
	}
	printf("error: must specify an option\n");

	/*  print out command line arguments */
	printf("Command line format: %s [-max -dDIR]\n",argv[0]);
	printf("\t-a       add new player\n");
	printf("\t-d DIR   to use play different game\n");
	printf("\t-m       make a world\n");
	printf("\t-x       execute (update) program\n");
	exit(SUCCESS);
}

/* if parameter == 0 do for all nations */
void
att_setup(cntry)
int	cntry;
{
	int	nat;
	for( nat= 0; nat<NTOTAL;nat++) if( isntn( ntn[nat].active ))
	if( (cntry==0) || (nat==cntry) ){
		ntn[nat].farm_ability = 10;
		ntn[nat].poverty = 95;
		ntn[nat].popularity=ntn[nat].reputation=ntn[nat].prestige = 50;
		ntn[nat].eatrate = 25;
		ntn[nat].tax_rate = 10;
		if( magic(nat,MINER) )
			ntn[nat].mine_ability = 25;
		else	ntn[nat].mine_ability = 10;
		ntn[nat].knowledge = 10;
		ntn[nat].charity = 0;
	}
}

/* calculates a nations base values in each of its attributes */
/* includes bonuses for magic powers, but not trade goods */
void
att_base()
{
	long	cityfolk,townfolk,scholars,foodpts,minepts,roads,clerics,ngrain;
	long	blksmths;
	long	mercs,armynum,ncities;
	long	temp;
	int	x,y;
	char	d;

	/* set to one so NEVER will have a divide by 0 */
	WORLDJEWELS=1; WORLDGOLD=1; WORLDMETAL=1;
	WORLDFOOD=1; WORLDSCORE=1; WORLDCIV=1; WORLDSCT=1;
	WORLDMIL=1; WORLDNTN=0;
	for(country=1;country<NTOTAL;country++) {
		curntn= &ntn[country];
		if( !isntn( curntn->active )) continue;
		WORLDNTN++;
		WORLDJEWELS+=curntn->jewels;
		if(curntn->tgold>0) WORLDGOLD+=curntn->tgold;
		WORLDMETAL+=curntn->metals;
		WORLDFOOD+=curntn->tfood;
		WORLDSCORE+=curntn->score;
		WORLDCIV+=curntn->tciv;
		WORLDSCT+=curntn->tsctrs;
		WORLDMIL+=curntn->tmil;
	}
	if (WORLDGOLD==0) WORLDGOLD=1;
	
	/* count the number of sectors */
	for(country=1;country<NTOTAL;country++) {
		if(!isntn(ntn[country].active)) continue;
		curntn= &ntn[country];
		cityfolk=townfolk=scholars=foodpts=minepts=roads=clerics=ngrain=ncities=blksmths=0;

		for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++)
		if(sct[x][y].owner == country) {
			d = sct[x][y].designation;
			if( d==DTOWN)		townfolk+=sct[x][y].people;
			else if( d==DCITY ){
				cityfolk+=sct[x][y].people;
				ncities++;
			} else if( d==DMINE )	{
				if( tg_ok( country, &sct[x][y] ))
					minepts+=sct[x][y].metal;
			} else if( d==DFARM )
				foodpts += sct[x][y].people * tofood( &sct[x][y],country);
			else if( d==DCAPITOL ){
				ncities+=3;
				cityfolk+=sct[x][y].people;
			} else if( d==DUNIVERSITY)scholars+=sct[x][y].people;
			else if( d==DROAD )	roads++;
			else if( d==DCHURCH )	clerics+=sct[x][y].people;
			else if( d==DGRANARY )	ngrain++;
			else if( d==DBLKSMITH )	blksmths+=sct[x][y].people;
		}
		cityfolk /= 167;
		townfolk /= 167;
		scholars /=167;
		clerics /= 167;
		blksmths /= 167;

		/* use prior eatrate a weighting factor */
		if (curntn->eatrate<25) curntn->eatrate=25;
		if( TURN!= 1) {	/* remember eatrate is scaled by 25 */
			switch(SEASON(TURN)) {
			case WINTER:	/* 7 food/person for each eatrate */
				temp = 180L * curntn->tfood/(curntn->eatrate+25L);
				break;
			case SPRING:	/* 6 food/person for each eatrate */
				temp = 204L * curntn->tfood/(curntn->eatrate+25L);
				break;
			case SUMMER:	/* 5 food/person for each eatrate */
				temp = 250L * curntn->tfood/(curntn->eatrate+25L);
				break;
			case FALL:	/* 4 food/person for each eatrate */
				temp = 312L * curntn->tfood/(curntn->eatrate+25L);
				break;
			}
			if(curntn->tciv>0) x = curntn->eatrate/2 + temp/curntn->tciv;
			else x = 25;
			if( x < MAXTGVAL ) curntn->eatrate = (char)x;
			else curntn->eatrate=MAXTGVAL;
			if ( curntn->eatrate < 25 ) curntn->eatrate=25;

		} else {
			cityfolk = 10;		/* Aproximate steady state */
		}

		if( 30 <= 1+ngrain+ncities ) curntn->spoilrate=1;
		else curntn->spoilrate = 30-ngrain-ncities;
		if( curntn->tfood > curntn->tciv * 10 )
			curntn->spoilrate = 30;

		/* get number of mercenaries */
		mercs=0;
		for(armynum=0;armynum<MAXARM;armynum++)
			if(P_ATYPE==A_MERCENARY) mercs+=P_ASOLD;
		if(curntn->tmil>0 && curntn->tciv>0) temp=(1000*curntn->tmil)/curntn->tciv+(1000*mercs)/curntn->tmil;
		else temp=0;
		curntn->terror = min( temp/5, MAXTGVAL );

		temp = (5*townfolk/2+5*cityfolk) + roads*5;
		curntn->communications = min( temp,2*MAXTGVAL );

		temp=1000*curntn->score/WORLDSCORE + 1000*curntn->tmil/WORLDMIL;
		curntn->power = min(temp/5,MAXTGVAL);

		/* calculate national wealth */
		temp = curntn->tgold;
		if(temp<0) temp=0;
		temp = (long)(1000.0*temp/WORLDGOLD + 1000.0*curntn->jewels/WORLDJEWELS + 1000.0*curntn->metals/WORLDMETAL) + cityfolk*5/3 + townfolk*5/6;
		if (temp >= curntn->wealth) {
			curntn->wealth = min( temp/10,MAXTGVAL );
		} else {
			curntn->wealth -= (curntn->wealth - temp)/4;
		}

		if( TURN!= 1) {
		curntn->reputation += rand()%8-3;
		curntn->reputation = min( curntn->reputation,MAXTGVAL );

		temp = (curntn->prestige + curntn->power + curntn->wealth) / 3;
		curntn->prestige = min( temp,MAXTGVAL );

		if(curntn->tciv>0) temp = foodpts*10 / curntn->tciv;
		else temp = 0;
		curntn->farm_ability = min( temp,MAXTGVAL );
		}

		/* calcualte mining ability */
		temp = (minepts/3 + cityfolk/2 + townfolk/2 + blksmths);
		if( magic(country,MINER) )
			temp += 15;
		if( magic(country,STEEL) )
			temp += 15;
		if (temp >= curntn->mine_ability) {
			curntn->mine_ability = min( temp,MAXTGVAL );
		} else {
			curntn->mine_ability -= (curntn->mine_ability - temp)/4;
		}

		/* calculate knowledge */
		temp = cityfolk/2 + townfolk/6 + scholars/2;
		curntn->knowledge = min( temp,MAXTGVAL );

		/* find national popularity */
		temp = (curntn->wealth + 10*P_EATRATE + clerics + curntn->popularity)/2;
		curntn->popularity = min( temp,MAXTGVAL );

		if(magic(country,SLAVER))	curntn->terror+=PWR_NA;
		if(magic(country,RELIGION))	curntn->popularity+=PWR_NA;
		if(magic(country,URBAN)) {
			if(curntn->popularity > PWR_NA)
				curntn->popularity-=PWR_NA;
			else	curntn->popularity=0;
		}
		if(magic(country,DEMOCRACY)) {
			curntn->eatrate += 25;
			if(curntn->terror > PWR_NA)
				curntn->terror-=PWR_NA;
			else	curntn->terror=0;
			if (curntn->charity < 15) {
				/* it creeps up */
				curntn->charity+=2;
			}
		}
		if(magic(country,KNOWALL))	curntn->knowledge+=PWR_NA;
		if(magic(country,ARCHITECT)){
			if(curntn->spoilrate>=PWR_NA)
				curntn->spoilrate-=PWR_NA;
			else curntn->spoilrate=1;
		}
		if(magic(country,ROADS))	curntn->communications+=50;
		if(magic(country,DESTROYER))	curntn->terror+=PWR_NA;
		if(magic(country,ROADS))	curntn->terror+=PWR_NA;
		if(magic(country,VAMPIRE))	curntn->terror+=PWR_NA;

		switch(curntn->class){
		case C_NPC:	curntn->popularity+=CLA_NA;
				curntn->terror+=CLA_NA;		break;
		case C_KING:	curntn->popularity+=CLA_NA;	break;
		case C_TRADER:	curntn->wealth+=CLA_NA;
				curntn->popularity+=CLA_NA;
				curntn->prestige+=CLA_NA/3;	break;
		case C_EMPEROR:	curntn->wealth+=CLA_NA;
				curntn->popularity+=CLA_NA;
				curntn->prestige+=CLA_NA/3;	break;
		case C_WIZARD:	curntn->knowledge+=CLA_NA;	break;
		case C_PRIEST:	curntn->popularity+=CLA_NA;	break;
		case C_PIRATE:	curntn->terror+=CLA_NA;		break;
			/* for warlord remember it is recursive */
		case C_WARLORD:	curntn->prestige+=CLA_NA*2/3;	break;
		case C_DEMON:	curntn->terror+=CLA_NA;		break;
		case C_DRAGON:	curntn->terror+=CLA_NA;		break;
		case C_SHADOW:	curntn->terror+=CLA_NA;		break;
		}

		curntn->prestige = min( curntn->prestige, MAXTGVAL );
		curntn->popularity = min( curntn->popularity, MAXTGVAL );
		curntn->power = min( curntn->power, MAXTGVAL );
		curntn->communications = min( curntn->communications, 2*MAXTGVAL);
		curntn->wealth = min( curntn->wealth, MAXTGVAL );
		curntn->eatrate = min( curntn->eatrate, MAXTGVAL );
		curntn->knowledge = min( curntn->knowledge, MAXTGVAL );
		curntn->farm_ability = min( curntn->farm_ability, MAXTGVAL );
		curntn->mine_ability = min( curntn->mine_ability, MAXTGVAL );
		curntn->terror = min( curntn->terror, MAXTGVAL );
		curntn->reputation = min( curntn->reputation, MAXTGVAL );
	}
}

/* calculates a nations bonuses due to trade goods */
void
att_bonus()
{
	short	x,y,nation,good;
	struct	s_sector	*sptr;
	printf("working on exotic trade goods\n");
	for(x=0;x<MAPX;x++) for(y=0;y<MAPY;y++) {
		if(!isntn(ntn[sct[x][y].owner].active)) continue;

		sptr = &sct[x][y];
		nation = sptr->owner;
		curntn = &ntn[nation];

		if( !tg_ok( nation, sptr) ) continue;

		good = sptr->tradegood;

		if(( *(tg_stype+good)== sptr->designation )
		||(( *(tg_stype+good)== DTOWN )&&(sptr->designation==DCITY))
		||(( *(tg_stype+good)== DTOWN )&&(sptr->designation==DCAPITOL))
		||(( *(tg_stype+good)== DCITY )&&(sptr->designation==DCAPITOL))
		||(( *(tg_stype+good)== DUNIVERSITY )&&(sptr->designation==DCITY))
		||(( *(tg_stype+good)== DUNIVERSITY )&&(sptr->designation==DCAPITOL))
		||( *(tg_stype+good)== 'x' ))
		if( good <= END_POPULARITY ) {
			curntn->popularity += ( *(tg_value+good) - '0');
			curntn->popularity = min( MAXTGVAL, curntn->popularity );
		} else if( good <= END_COMMUNICATION ) {
			if(curntn->communications + (*(tg_value+good) - '0')<2*MAXTGVAL)
				curntn->communications += (*(tg_value+good) - '0');
			else curntn->communications = 2*MAXTGVAL;
		} else if( good <= END_EATRATE ) { /* eatrate scaled already */
			/* no tradegoods for eatrate */
			curntn->eatrate = min( MAXTGVAL, curntn->eatrate );
		} else if( good <= END_SPOILRATE ) {
			if(curntn->spoilrate > (*(tg_value+good) - '0'))
				curntn->spoilrate -= (*(tg_value+good)-'0');
			else curntn->spoilrate = 1;
		} else if( good <= END_KNOWLEDGE ) {
			if(curntn->knowledge + (*(tg_value+good)-'0') < MAXTGVAL)
				curntn->knowledge += (*(tg_value+good) - '0');
			else curntn->knowledge = MAXTGVAL;
		} else if( good <= END_FARM ) {
			if(curntn->farm_ability + (*(tg_value+good) - '0') < MAXTGVAL)
				curntn->farm_ability += (*(tg_value+good)-'0');
			else curntn->farm_ability = MAXTGVAL;
		} else if( good <= END_SPELL ) {
			curntn->spellpts += sptr->people/1000 +1;
		} else if( good <= END_TERROR ) {
			if(curntn->terror + (*(tg_value+good)-'0')< MAXTGVAL)
				curntn->terror += (*(tg_value+good)-'0');
			else curntn->terror = MAXTGVAL;
		}
	}
}
