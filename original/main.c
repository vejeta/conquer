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
#include <errno.h>
#include <signal.h>
#include <pwd.h>
#include <time.h>
#include "header.h"
#include "data.h"
#include "patchlevel.h"

extern	int armornvy,roads_this_turn,terror_adj;

char	fison[FILELTH];
char	*getpass();
struct	s_sector **sct;
struct	s_nation ntn[NTOTAL];	/* player nation stats */
struct	s_world	world;		
char	**occ;	/*is sector occupied by an army?*/
short	**movecost;
long	startgold=0;
long	mercgot=0;

short	xoffset=0,yoffset=0;	/*offset of upper left hand corner*/
/* current cursor postion (relative to 00 in upper corner) */
/*	position is 2*x,y*/
short	xcurs=0,ycurs=0;
short	redraw=FULL;	/* if !DONE: redraw map		*/
int	done=FALSE;	/* if TRUE: you are done	*/
short	hilmode=HI_OWN;	/* hilight mode */
short	dismode=DI_DESI;/* display mode			*/
short	otherdismode= -(DI_MOVE);
short	otherhilmode= HI_OWN;
short	selector=0;	/* selector (y vbl) for which army/navy... is "picked"*/
short	pager=0;	/* pager for selector 0,1,2,3*/
short	country=0;	/* nation id of owner*/
struct	s_nation	*curntn;
short	Gaudy=FALSE;
int	owneruid;

FILE *fexe, *fopen();

/************************************************************************/
/*	MAIN() - main loop for conquer					*/
/************************************************************************/
void
main(argc,argv)
int	argc;
char	**argv;
{
#ifdef  USERLOG
	FILE *userlog;
#endif
#ifdef  CHECKUSER
	int checkuser_mod = FALSE;
	int checkuser_uid = 0;
	int checkuser_list = FALSE;
#endif
	int geteuid(), getuid(), setuid();
	register int i,j;
	char name[NAMELTH+1],filename[FILELTH];
#ifndef __STDC__
	void srand();
#endif
	void init_hasseen(),mapprep();
	int getopt();
	char passwd[PASSLTH+1];
	long time();
	extern char *optarg, conqmail[];
#ifdef SYSMAIL
	extern char sysmail[];
#endif /* SYSMAIL */
	int sflag=FALSE,pflag=FALSE,l,in_ch,old_ch=' ';

	char defaultdir[BIGLTH],tmppass[PASSLTH+1];
	char cq_opts[BIGLTH];
	struct passwd *getpwnam(), *pwent;

	owneruid=getuid();
	srand((unsigned) time((long *) 0));
	strcpy(name,"");
	strcpy(defaultdir,"");
	strcpy(cq_opts,"");

	/* check conquer options */
	if (getenv(ENVIRON_OPTS)!=NULL) {
		strncpy(cq_opts, getenv(ENVIRON_OPTS), BIGLTH);
	}
	if (cq_opts[0] != '\0') {
		l = strlen(cq_opts);
		for(i=0; i<l; i++) {
			switch(cq_opts[i]) {
			case 'G':
				/* set Gaudy display */
				Gaudy = TRUE;
				break;
			case 'N':
			case 'n':
				/* check for nation name */
				if (strncmp(cq_opts+i+1,"ation=",6)==0) {
					i += 7;
				} else if (strncmp(cq_opts+i+1,"ame=",4)==0) {
					i += 5;
				} else {
					fprintf(stderr,"conquer: invalid environment\n");
					fprintf(stderr,"\t%s = %s\n",ENVIRON_OPTS,cq_opts);
					fprintf(stderr,"\texpected <nation=NAME>\n");
					exit(FAIL);
				}
				if (i<l) {
					/* grab the nation name */
					for (j=0;j<NAMELTH&&j<l-i&&cq_opts[i+j]!=',';j++) {
						name[j] = cq_opts[i+j];
					}
					name[j]='\0';
					
					/* end the parse properly */
					i += j-1;
					if (j==NAMELTH) {
						for (;i<l && cq_opts[i]!=',';i++);
					}
				}
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
						defaultdir[j] = cq_opts[i+j];
					}
					defaultdir[j]='\0';
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

	/* set the default data directory */
	if (defaultdir[0] == '\0') {
		strcpy(defaultdir, DEFAULTDIR);
	}
	if (defaultdir[0] != '/') {
		strcpy(cq_opts, defaultdir);
		sprintf(defaultdir, "%s/%s", DEFAULTDIR, cq_opts);
	}

	/* process the command line arguments */
#ifdef CHECKUSER
	while((i=getopt(argc,argv,"Ghpln:u:d:s"))!=EOF) switch(i){
#else
	while((i=getopt(argc,argv,"Ghpn:d:s"))!=EOF) switch(i){
#endif
	/* process the command line arguments */
	case 'h': /* execute help program*/
		initscr();
		savetty();
		noecho();
		crmode();			/* cbreak mode */
		signal(SIGINT,SIG_IGN);		/* disable keyboard signals */
		signal(SIGQUIT,SIG_IGN);
		help();
		endwin();
		putchar('\n');
		exit(SUCCESS);
	case 'p': /* print the map*/
		pflag++;
		break;
	case 'G':
		Gaudy = TRUE;
		break;
	case 'd':
		if(optarg[0]!='/') {
			sprintf(defaultdir, "%s/%s", DEFAULTDIR, optarg);
		} else {
			strcpy(defaultdir, optarg);
		}
		break;
	case 'n':
		strcpy(name, optarg);
		break;
#ifdef CHECKUSER
	case 'l':
		checkuser_list = TRUE;
		break;
	case 'u':
		checkuser_mod = TRUE;	/* check for god later */
		checkuser_uid = owneruid;
		if (strlen (optarg) > 0)
		   if (getpwnam(optarg))
		      checkuser_uid = getpwnam(optarg)->pw_uid;
#endif
	case 's': /*print the score*/
		sflag++;
		break;
	case '?': /*  print out command line arguments */
#ifdef CHECKUSER
		fprintf(stderr,"Command line format: %s [-Ghpls -d DIR -nNAT -u USER]\n",argv[0]);
		fprintf(stderr,"\t-u USER  change nation username\n");
		fprintf(stderr,"\t-l       list nation owners\n");
#else
		fprintf(stderr,"Command line format: %s [-Ghps -d DIR -nNAT]\n",argv[0]);
#endif
		fprintf(stderr,"\t-n NAT   play as nation NAT\n");
		fprintf(stderr,"\t-d DIR   to use play different game\n");
		fprintf(stderr,"\t-G       gaudily highlight nation in news\n");
		fprintf(stderr,"\t-h       print help text\n");
		fprintf(stderr,"\t-p       print a map\n");
		fprintf(stderr,"\t-s       print scores\n");
		exit(SUCCESS);
	};

	/* now that we have parsed the args, we can go to the
	 * dir where the files are kept and do some work.
	 */
	if (chdir(defaultdir)) {
		fprintf(stderr,"unable to change dir to %s\n",defaultdir);
		exit(FAIL);
	}

	readdata();				/* read data*/
	verifydata( __FILE__, __LINE__ );	/* verify data */

	/* now print the scores */
	if(sflag){
		printscore();
		exit(SUCCESS);
	}

	/*
	*  Set the real uid to the effective.  This will avoid a
	*  number of problems involving file protection if the
	*  executable is setuid.
	*/
	if (getuid() != geteuid()) { /* we are running suid */
		(void) umask(MASK);	/* nobody else can read files */
		(void) setuid (geteuid ()) ;
	}

	fprintf(stderr,"conquer %s.%s: Copyright (c) 1988 by Edward M Barlow\n",VERSION,PATCHLEVEL);

	/* check for update in progress */
	sprintf(filename,"%sup",isonfile);
	if(check_lock(filename,FALSE)==TRUE) {
		fprintf(stderr,"Conquer is updating\n");
		fprintf(stderr,"Please try again later.\n");
		exit(FAIL);
	}

	/* identify player and country represented */
	/* get nation name from command line or by asking user.
	*     if you fail give name of administrator of game
	*/
	if (name[0] == '\0') {
		if (pflag != FALSE)
			fprintf(stderr,"Display map for what nation: ");
		else fprintf(stderr,"What nation would you like to be: ");
		gets(name);
	}
#ifdef OGOD
	if(strcmp(name,"god")==0 || strcmp(name,"unowned")==0) {
		if ((owneruid != (getpwnam(LOGIN))->pw_uid ) &&
		  ((pwent=getpwnam(ntn[0].leader)) == NULL ||
		  owneruid != pwent->pw_uid )) {
			fprintf(stderr,"Sorry -- you can not login as god\n");
			fprintf(stderr,"you need to be logged in as %s",LOGIN);
			if (strcmp(LOGIN, ntn[0].leader)!=0) {
				fprintf(stderr," or %s",ntn[0].leader);
			}
			fprintf(stderr,"\n");
			exit(FAIL);
		}
		strcpy(name,"unowned");
		hilmode = HI_NONE;
	}
#else
	if(strcmp(name,"god")==0) strcpy(name,"unowned");
#endif /* OGOD */

#ifdef CHECKUSER
	if ((checkuser_mod) || (checkuser_list)) {
		if ((owneruid == (getpwnam(LOGIN))->pw_uid) ||
        	    (owneruid == (getpwnam(ntn[0].leader))->pw_uid)) {
			/* don't change - already set */	
		}
		else {
			checkuser_mod = FALSE;
			checkuser_list = FALSE;
			fprintf (stderr, "Sorry -- only god may modify/list a nation's uid\n");
		      	exit(FAIL);
		}
	}
#else
		fprintf (stderr,"CHECKUSER not on\n");
		exit (FAIL);
#endif
	country=(-1);
	for(i=0;i<NTOTAL;i++)
	if(strcmp(name,ntn[i].name)==0) {
		country=i;
		break;
	}

	if(country==(-1)) {
		fprintf(stderr,"Sorry, name <%s> not found\n",name);
		fprintf(stderr,"\nFor rules type <conquer -h>");
		fprintf(stderr,"\nFor information on conquer please contact %s.",OWNER);
		fprintf(stderr,"\nTo enter this campaign please send mail to %s", LOGIN);
		if (strcmp(LOGIN, ntn[0].leader)!=0) {
			fprintf(stderr," or %s",ntn[0].leader);
		}
		fprintf(stderr,".\n");
		return;
	} else if(country==0 && !pflag) {
		sprintf(filename,"%sadd",isonfile);
		if(check_lock(filename,FALSE)==TRUE) {
			fprintf(stderr,"A new player is being added.\n");
			fprintf(stderr,"Continue anyway? [y or n]");
			while(((i=getchar())!='y')&&(i!='n')) ;
			if(i!='y') exit(FAIL);
		}
	}
	curntn = &ntn[country];

	/*get encrypted password*/
	fprintf(stderr,"\nWhat is your Nation's Password: ");
	strncpy(tmppass,getpass(""),PASSLTH+1);
	strncpy(passwd,crypt(tmppass,SALT),PASSLTH+1);
	if((strncmp(passwd,curntn->passwd,PASSLTH)!=0)
	&&(strncmp(passwd,ntn[0].passwd,PASSLTH)!=0)) {
		fprintf(stderr,"\nError: Reenter your Nation's Password: ");
		strncpy(tmppass,getpass(""),PASSLTH+1);
		strncpy(passwd,crypt(tmppass,SALT),PASSLTH+1);
		if((strncmp(passwd,curntn->passwd,PASSLTH)!=0)
		&&(strncmp(passwd,ntn[0].passwd,PASSLTH)!=0)) {
			fprintf(stderr,"\nSorry:");
			fprintf(stderr,"\nFor rules type <conquer -h>");
			fprintf(stderr,"\nFor information on conquer please contact %s.",
				OWNER);
			fprintf(stderr,"\nTo enter this campaign please send mail to %s",
				LOGIN);
			if (strcmp(LOGIN, ntn[0].leader)!=0) {
				fprintf(stderr," or %s",ntn[0].leader);
			}
			fprintf(stderr,".\n");
			exit(FAIL);
		}
	}

	/* now print the maps */
	if (pflag) {	/* print a map of the game */
		fprintf(stderr,"\nFor convenience, this output is to stderr,\n");
		fprintf(stderr,"while the maps will be sent to stdout.\n\n");
		fprintf(stderr,"\tThe valid options are,\n");
		fprintf(stderr,"\t\t1) altitudes\n\t\t2) vegetations\n");
		fprintf(stderr,"\t\t3) nations\n");
		fprintf(stderr,"\t\t4) designations\n\n");
		fprintf(stderr,"\tWhat type of map? ");
		scanf("%hd", &dismode);
		fprintf(stderr,"\n");
		switch(dismode) {
		case 1:
			mapprep();
			printele();
			break;
		case 2:
			mapprep();
			printveg();
			break;
		case 3:
			mapprep();
			pr_ntns();
			break;
		case 4:
			mapprep();
			pr_desg();
			break;
		default:
			fprintf(stderr,"Invalid Choice\n");
		     exit(FAIL);
			break;
		}
		exit(SUCCESS);
	}

#ifdef CHECKUSER
        /* New code by "spide" */
	if (checkuser_mod)
	   {
		fprintf (stderr, "Nation:  %s\n", curntn->name);
                fprintf (stderr, "   Current player = %s\n", 
			getpwuid(curntn->uid)->pw_name);;
		curntn->uid = checkuser_uid;
                fprintf (stderr, "   New player = %s\n", 
			getpwuid(curntn->uid)->pw_name);
		writedata();
		exit (SUCCESS);
	   }
	if (checkuser_list)
	   {
	      	for (i=0; i < NTOTAL; i++) 
			if (ntn[i].active != INACTIVE) 
				fprintf (stderr, "%3d %15s %d %-15s\n",
					i, ntn[i].name, 
					ntn[i].uid,
					getpwuid(ntn[i].uid)->pw_name);
		exit (SUCCESS);
	   }
        if ((curntn->uid != owneruid) &&
	    (owneruid != (getpwnam(LOGIN))->pw_uid) && 
            (owneruid != (getpwnam(ntn[0].leader))->pw_uid) &&
	    (curntn->uid != (getpwnam(LOGIN))->pw_uid))
           {
              fprintf (stderr,"\nSorry -- you are not the owner of %s",curntn->name);
	      fprintf(stderr,"\nFor information on conquer please contact %s.",OWNER);
              fprintf(stderr,"\nor send mail to %s", LOGIN);
	      fprintf(stderr,"\n");
	      exit(FAIL);
           }
#endif

	initscr();		/* SET UP THE SCREEN */
	/* check terminal size */
	if (COLS<80 || LINES<24) {
		fprintf(stderr,"%s: terminal should be at least 80x24\n",argv[0]);
		fprintf(stderr,"please try again with a different setup\n");
		beep();
		getch();
		bye(FALSE);
	}

	copyscreen();		/* copyright screen */
				/* note the getch() later - everything between
					now and then is non-interactive */
	init_hasseen();		/* now we know how big the screen is, 
					we can init that array!	*/

	strcpy(fison,"START");	/* just in case you abort early */
	crmode();		/* cbreak mode */

	/* check if user is super-user nation[0] */
	/*	else setup cursor to capitol*/
	if((country==0)||(ismonst(ntn[country].active))) {
		xcurs=MAPX/2-1;
		ycurs=MAPY/2-1;
		redraw=FULL;
		/* create gods lock file but do not limit access */
		(void) aretheyon();
	} else {
		if(curntn->active==INACTIVE) {
			standout(); 
			mvaddstr(LINES-2,0,"Sorry, for some reason, your country no longer exists.");
			mvprintw(LINES-1,0,"If there is a problem, please send mail to %s", LOGIN);
			if (strcmp(LOGIN, ntn[0].leader)!=0) {
				printw(" or %s",ntn[0].leader);
			}
			printw(".");
			standend();
			beep();
			refresh();
			getch();
			bye(TRUE);
		}
		if(aretheyon()==TRUE) {
			mvaddstr(LINES-2,0,"Sorry, country is already logged in.");
			mvaddstr(LINES-1,0,"Please try again later.");
			beep();
			refresh();
			getch();
			bye(FALSE);
		}
		execute(FALSE);
#ifdef TRADE
		checktrade();
#endif /* TRADE */
		xcurs = curntn->capx;
		ycurs = curntn->capy;
	}
	xoffset = 0;
	yoffset = 0;
#ifdef USERLOG
	userlog = fopen (".userlog", "a");
        fprintf (userlog, "%3d %15s %30s %15s\n", 
   		TURN, getpwuid(owneruid)->pw_name, defaultdir, curntn->name);
        fclose (userlog);
#endif
	centermap();
	updmove(curntn->race,country);

	/* open output for future printing*/
	sprintf(filename,"%s%d",exefile,country);
	if ((fexe=fopen(filename,"a"))==NULL) {
		beep();
		mvprintw(LINES-2,0,"error opening %s",filename);
		refresh();
		getch();
		bye(TRUE);
	}


	signal(SIGINT,SIG_IGN);		/* disable keyboard signals */
	signal(SIGQUIT,SIG_IGN);
	signal(SIGHUP,hangup);		/* must catch hangups */
	signal(SIGTERM,hangup);		/* likewise for cheats!! */

	noecho();
	prep(country,FALSE);	/* initialize prep array */
	whatcansee();			/* what can they see */

	/* initialize mail files */
	(void) sprintf(conqmail,"%s%d",msgfile,country);
#ifdef SYSMAIL
	if (getenv("MAIL")==0) {
		(void) sprintf(sysmail,"%s/%s",SPOOLDIR,getenv("USER"));
	} else {
		(void) strcpy(sysmail,getenv("MAIL"));
	}
#endif /* SYSMAIL */
	mvaddstr(LINES-1, COLS-20, "PRESS ANY KEY");
	refresh();
	getch();		/* get response from copyscreen */

	while(done==FALSE) {			/*main while routine*/
		coffmap(); 	/* check if cursor is out of bounds*/
		check_mail();	/* check for new mail */
		in_ch = getch();
		/* get commands */
		if (in_ch=='!') {
			parse(old_ch);
		} else {
			if (parse(in_ch)) old_ch=in_ch;
		}
	}

	if(country==0) writedata();
	else {
		fprintf(fexe,"L_NGOLD\t%d \t%d \t%ld \t0 \t0 \t%s\n",
			XNAGOLD ,country,curntn->tgold,"null");
		fprintf(fexe,"L_NMETAL\t%d \t%d \t%ld \t0 \t0 \t%s\n",
			XNAMETAL ,country,curntn->metals,"null");
		fprintf(fexe,"L_NJWLS\t%d \t%d \t%ld \t0 \t0 \t%s\n",
			XNARGOLD ,country,curntn->jewels,"null");
	}
	bye(TRUE);	 		/* done so quit */
}

/************************************************************************/
/* MAKEBOTTOM() - make the bottom of the screen				*/
/************************************************************************/
void
makebottom()
{
	standend();
	move(LINES-4,0);
	clrtoeol();
	mvprintw(LINES-3,0,"Conquer: %s.%s Turn %d",VERSION,PATCHLEVEL,TURN);
	clrtoeol();
	mvaddstr(LINES-1,0,"  type ? for help");
	clrtoeol();
	mvaddstr(LINES-2,0,"  type Q to save & quit");
	clrtoeol();

	if(country==0) {
		mvaddstr(LINES-3,COLS-20,"nation...GOD");
	} else {
		mvprintw(LINES-3,COLS-20,"nation...%s",curntn->name);
		mvprintw(LINES-2,COLS-20,"treasury.%ld",curntn->tgold);
	}
	mvprintw(LINES-1,COLS-20,"%s of Year %d",PSEASON(TURN),YEAR(TURN));

	/* mail status */
#ifdef SYSMAIL
	/* display mail information */
	if (sys_mail_status==NEW_MAIL) {
		mvaddstr(LINES-3,COLS/2-6,"You have System Mail");
	}
	if (conq_mail_status==NEW_MAIL) {
		mvaddstr(LINES-2,COLS/2-6,"You have Conquer Mail");
	}
#else
	/* display mail information */
	if (conq_mail_status==NEW_MAIL) {
		mvaddstr(LINES-3,COLS/2-6,"You have Conquer Mail");
	}
#endif /* SYSMAIL */
}

/************************************************************************/
/*	PARSE() - interpret entered character				*/
/*	  return TRUE if command is repeatable FALSE otherwise		*/
/************************************************************************/
int
parse(ch)
	int ch;
{
	char	name[LINELTH+1];
	char	passwd[PASSLTH+1];
	struct passwd *getpwnam(), *pwent;
#ifdef DEBUG
	void sect_info();
#endif /* DEBUG */
	int	ocountry;

	switch(ch) {
	case EXT_CMD:	/* extended command */
		ext_cmd( -1 );
		makebottom();
		refresh();
		curntn->tgold -= MOVECOST;
		return(TRUE);
		break;
	case '':	/* redraw the screen */
		centermap();
		redraw=FULL;
		break;
#ifdef DEBUG
	case '\t':	/* debugging information for god and demi-god */
		if ((owneruid != (getpwnam(LOGIN))->pw_uid ) &&
		    ((pwent=getpwnam(ntn[0].leader))==NULL || owneruid != pwent->pw_uid ))
			break;
		sect_info();
		break;
#endif /* DEBUG */
	case 'a':	/*army report*/
		redraw=FULL;
		armyrpt(0);
		curntn->tgold -= MOVECOST;
		break;
	case '1':
	case 'b':	/*move south west*/
		pager=0;
		selector=0;
		if (XREAL > 0) xcurs--;
		if (YREAL < MAPY - 1) ycurs++;
		break;
	case 'B':	/*budget*/
		redraw=FULL;
		budget();
		curntn->tgold -= MOVECOST;
		break;
	case 'c':	/*change nation stats*/
		redraw=FULL;
		change();
		curntn->tgold -= MOVECOST;
		break;
	case 'C':	/*construct*/
		construct();
		makebottom();
		curntn->tgold -= MOVECOST;
		break;
	case 'd':	/*change display*/
		newdisplay();
		break;
	case 'D':	/*draft*/
		draft();
		curntn->tgold -= MOVECOST;
		makebottom();
		return(TRUE);
		break;
	case 'f': /*report on ships and load/unload*/
		redraw=PART;
		curntn->tgold -= MOVECOST;
		fleetrpt();
		break;
	case 'F':	/*go to next army*/
		navygoto();
		break;
	case 'g':	/*group report*/
		redraw=PART;
		curntn->tgold -= MOVECOST;
		armyrpt(1);
		break;
	case 'G':	/*go to next army*/
		armygoto();
		break;
	case 'H':	/*scroll west*/
		pager=0;
		selector=0;
		if (XREAL > (COLS-22)/4) xcurs-=((COLS-22)/4);
		else xcurs = -xoffset;
		break;
	case '4':
	case 'h':	/*move west*/
		pager=0;
		selector=0;
		if (XREAL > 0) xcurs--;
		break;
	case 'I':	/*campaign information*/
		camp_info();
		redraw=FULL;
		break;
	case 'J':	/*scroll down*/
		pager=0;
		selector=0;
		if (YREAL + (SCREEN_Y_SIZE)/2 < MAPY)
			ycurs+=((SCREEN_Y_SIZE)/2);
		else ycurs = MAPY - yoffset - 1;
		break;
	case '2':
	case 'j':	/*move down*/
		pager=0;
		selector=0;
		if (YREAL < MAPY - 1) ycurs++;
		break;
	case '8':
	case 'k':	/*move up*/
		pager=0;
		selector=0;
		if (YREAL > 0) ycurs--;
		break;
	case 'K':	/*scroll up*/
		pager=0;
		selector=0;
		if (YREAL > (SCREEN_Y_SIZE)/2) ycurs-=((SCREEN_Y_SIZE)/2);
		else ycurs = -yoffset;
		break;
	case '6':
	case 'l':	/*move east*/
		pager=0;
		selector=0;
		if (XREAL < MAPX - 1) xcurs++;
		break;
	case 'L':	/*scroll east*/
		pager=0;
		selector=0;
		if (XREAL + (COLS-22)/4 < MAPX) xcurs+=((COLS-22)/4);
		else xcurs = MAPX - xoffset - 1;
		break;
	case 'm':	/*move selected item to new x,y */
		mymove();
		makebottom();
		curntn->tgold -= MOVECOST;
		return(TRUE);
		break;
	case 'M':	/*magic*/
		redraw=FULL;
		curntn->tgold -= MOVECOST;
		domagic();
		break;
	case '3':
	case 'n':	/*move south-east*/
		pager=0;
		selector=0;
		if (YREAL < MAPY - 1) ycurs++;
		if (XREAL < MAPX - 1) xcurs++;
		break;
	case 'N':	/*read newspaper */
		redraw=PART;
		curntn->tgold -= MOVECOST;
		newspaper();
		break;
	case 'o':	/*pick (crsr up)*/
		selector-=2;
		if(selector<0) {
			selector=SCRARM*2-2;
			pager--;
		}
		/*move to last army in current sector*/
		if (pager<0) {
			pager=(units_in_sector(XREAL,YREAL,country)-1)/SCRARM;
			selector=((units_in_sector(XREAL,YREAL,country)-1)%SCRARM)*2;
		}
		break;
	case 'p':	/*pick*/
		selector+=2;
		if(selector>=SCRARM*2) {
			selector=0;
			pager+=1;
		}
		/*current selected unit is selector/2+SCRARM*pager*/
		if((selector/2)+(pager*SCRARM)>=units_in_sector(XREAL,YREAL,country)) {
			pager=0;
			selector=0;
		}
		break;
	case 'P':	/*production*/
		redraw=FULL;
		curntn->tgold -= MOVECOST;
		produce();
		break;
	case 'Q':	/*quit*/
	case 'q':	/*quit*/
		done=TRUE;
		break;
	case 'r':	/*redesignate*/
		redesignate();
		curntn->tgold -= MOVECOST;
		makemap();
		makebottom();
		break;
		/*list*/
	case 'R':	/*Read Messages*/
		redraw=PART;
		curntn->tgold -= MOVECOST;
		rmessage();
		refresh();
		break;
	case 's':	/*score*/
		redraw=FULL;
		curntn->tgold -= MOVECOST;
		showscore();
		break;
	case 'S':	/*diplomacy screens*/
		diploscrn();
		curntn->tgold -= MOVECOST;
		redraw=FULL;
		break;
	case 't':	/*fleet loading*/
		loadfleet();
		curntn->tgold -= MOVECOST;
		makeside(FALSE);
		makebottom();
		return(TRUE);
		break;
#ifdef TRADE
	case 'T':	/*go to commerce section*/
		trade();
		curntn->tgold -= MOVECOST;
		redraw=FULL;
		break;
#endif /* TRADE */
	case '9':
	case 'u':	/*move north-east*/
		pager=0;
		selector=0;
		if (YREAL > 0) ycurs--;
		if (XREAL < MAPX - 1) xcurs++;
		break;
    	case 'U':	/* scroll north-east */
		pager=0;
		selector=0;
		if (XREAL + (COLS-22)/4 < MAPX) xcurs+=((COLS-22)/4);
		else xcurs = MAPX - xoffset - 1;
		if (YREAL > (SCREEN_Y_SIZE)/2) ycurs-=((SCREEN_Y_SIZE)/2);
		else ycurs = -yoffset;
		break;
    	case 'v':	/* version credits */
		credits();
		redraw=FULL;
		break;
	case 'w':	/* spell casting */
		wizardry();
		curntn->tgold -= MOVECOST;
		return(TRUE);
		break;
	case 'W':	/*message*/
		curntn->tgold -= MOVECOST;
		wmessage();
		break;
	case 'X': /*jump to capitol*/
		redraw = PART;
		pager=0;
		selector=0;
		jump_to(TRUE);
		break;
	case 'x': /*jump to a location*/
		redraw=PART;
		pager=0;
		selector=0;
		jump_to(FALSE);
		makebottom();
		break;
	case '7':
	case 'y':	/*move north-west*/
		pager=0;
		selector=0;
		if (YREAL > 0) ycurs--;
		if (XREAL > 0) xcurs--;
		break;
	case 'Y':	/* scroll north-west */
		pager=0;
		selector=0;
		if (XREAL < (COLS-22)/4) xcurs = -xoffset;
		else xcurs-=((COLS-22)/4);
		if (YREAL < (SCREEN_Y_SIZE)/2) ycurs = -yoffset;
		else ycurs-=((SCREEN_Y_SIZE)/2);
		break;
	case 'Z':	/*move civilians up to 2 spaces*/
		moveciv();
		makebottom();
		curntn->tgold -= MOVECOST;
		break;
	case 'z':	/*login as new user */
#ifdef OGOD
		if ((owneruid != (getpwnam(LOGIN))->pw_uid ) &&
		    ((pwent=getpwnam(ntn[0].leader))==NULL || owneruid != pwent->pw_uid )) break;
#endif
		clear_bottom(0);
		if(country != 0) {
		fprintf(fexe,"L_NGOLD\t%d \t%d \t%ld \t0 \t0 \t%s\n",
			XNAGOLD ,country,curntn->tgold,"null");
		fprintf(fexe,"L_NMETAL\t%d \t%d \t%ld \t0 \t0 \t%s\n",
			XNAMETAL ,country,curntn->metals,"null");
		fprintf(fexe,"L_NJWLS\t%d \t%d \t%ld \t0 \t0 \t%s\n",
			XNARGOLD ,country,curntn->jewels,"null");
		} else
		mvaddstr(LINES-4,0,"SUPER-USER: YOUR CHANGES WILL NOT BE SAVED IF YOU DO THIS!!!");
		standout();
		mvaddstr(LINES-3,0,"Change login to:");
		standend();
		addch(' ');
		refresh();

		ocountry=country;
 		country=get_country();

		/* check validity of country choice */
		if( country==(-1) || country>=NTOTAL
		|| ( !isactive(ntn[country].active) && country!=0 )) {
			country=ocountry;
			makebottom();
			break;
		} 
		if(country==ocountry){
			errormsg("What?  You are already logged into that nation.");
			makebottom();
			break;
		}

		/*get password*/
		clear_bottom(0);
		mvaddstr(LINES-4,0,"What is your Nation's Password: ");
		refresh();
		(void) get_pass(passwd);
		strcpy(name,crypt(passwd,SALT));

		if((strncmp(name,ntn[country].passwd,PASSLTH)!=0)
		&&(strncmp(name,ntn[0].passwd,PASSLTH)!=0)){
			errormsg("Sorry, Password Invalid.");
			country=ocountry;
			makebottom();
			break;
		}
		if(aretheyon()==TRUE) {
			errormsg("Sorry, that Nation is already logged in.");
			country=ocountry;
			makebottom();
			break;
		}

		/* remove old lock file -- new one already made */
		sprintf(fison,"%s%d",isonfile,ocountry);
		unlink(fison);

		fclose(fexe);
		/* open output for future printing*/
		sprintf(fison,"%s%d",isonfile,country);
	 	sprintf(name,"%s%d",exefile,country);
	 	if ((fexe=fopen(name,"a"))==NULL) {
			beep();
			fprintf(stderr,"error opening %s\n",name);
			unlink(fison);
			exit(FAIL);
	 	}
		curntn = &ntn[country];

		fprintf(stderr,"\n");
		roads_this_turn=0;
		terror_adj=0;
		move(LINES-3,0);
		readdata();
		execute(FALSE);

		(void) sprintf(conqmail,"%s%d",msgfile,country);
		updmove(curntn->race,country);
		/*go to that nations capitol*/
		if((country==0)||(!isntn(ntn[country].active))) {
			xcurs=MAPX/2-1;
			ycurs=MAPY/2-1;
		} else {
			xcurs=curntn->capx;
			ycurs=curntn->capy;
		}
		xoffset = yoffset = 0;
		centermap();
		redraw=FULL;
		break;
	case '?':	/*display help screen*/
		redraw=PART;
		help();
		break;
	case ' ': /*ignore, and don't beep*/
		break;
	default:
		beep();
		break;
	}
	return(FALSE);
}

#ifdef DEBUG
/************************************************************************/
/*	SECT_INFO() - display sector debugging information		*/
/************************************************************************/
void
sect_info()
{
	int i,j,acnt1=0,acnt2=0,ncnt1=0,ncnt2=0,x,y;

	/* erase prior information */
	for(i=0;i<LINES-13;i++) {
		move(i,COLS-21);
		clrtoeol();
	}
	
	standout();
	mvaddstr(0,COLS-20,"Sector Information");
	mvprintw(1,COLS-20,"  x = %2d, y = %2d  ",(int)XREAL,(int)YREAL);
	standend();
	
	/* find units in the sector */
	for (i=0;i<NTOTAL;i++) if (ntn[i].active != INACTIVE) {
		x = 0;
		for (j=0;j<MAXARM;j++) {
			if (ntn[i].arm[j].sold > 0 && ntn[i].arm[j].xloc == XREAL
			    && ntn[i].arm[j].yloc == YREAL ) x++;
		}
		y = 0;
		for (j=0;j<MAXNAVY;j++) {
			if (ntn[i].nvy[j].xloc != XREAL ||
			    ntn[i].nvy[j].yloc != YREAL ) continue;
			if (ntn[i].nvy[j].warships!=0 ||
			    ntn[i].nvy[j].merchant!=0 ||
			    ntn[i].nvy[j].galleys!=0) y++;
		}
		if (i!=country) {
			acnt2 += x;
			ncnt2 += y;
		} else {
			acnt1 = x;
			ncnt1 = y;
		}
	}

	mvprintw(3,COLS-20,"Own A_Units: %d", acnt1);
	mvprintw(4,COLS-20,"Own N_Units: %d", ncnt1);
	mvprintw(5,COLS-20,"Other A_Units: %d", acnt2);
	mvprintw(6,COLS-20,"Other N_Units: %d", ncnt2);

	mvprintw(8,COLS-20,"Occval: %d", occ[XREAL][YREAL]);

	/* let them look at the information */
	errormsg("");

	/* fix the display */
	makeside(FALSE);
	makebottom();
}
#endif /* DEBUG */

/************************************************************************/
/*	MAKESIDE() -	make the right hand side display		*/
/************************************************************************/
void
makeside(alwayssee)
int	alwayssee;	/* see even if cant really see sector */
{
	int	i;
	int	armbonus;
	int	found=0,nvyfnd=0;
	long	enemy;
	int	y;
	short	armynum;
	short	nvynum;
	int	count;
	int	nfound=0,nfound2=0;
	register struct s_sector	*sptr = &sct[XREAL][YREAL];

	if( !alwayssee )
	if( !canbeseen((int) XREAL,(int) YREAL) ) {
		for(i=0;i<LINES-3;i++){
			move(i,COLS-21);
			clrtoeol();
		}
		return;
	}

	for(count=0;count<LINES-13;count++){	/*clear top right hand side */
		move(count,COLS-21);
		clrtoeol();
	}

	/*check for your armies*/
	count=units_in_sector(XREAL,YREAL,country);
	if(pager*SCRARM>count) pager=0;

	/*first army found is #0*/
	/*show armies / navies in range pager*SCRARM to pager*SCRARM + SCRARM*/
	/*so if pager=0 show 0 to 5 (SCRARM), pager=2 show 10 to 15*/
	/*current selected unit is selector/2+4*pager*/

	if(count>(SCRARM+(pager*SCRARM))) mvaddstr(LINES-14,COLS-20,"MORE...");

	nfound=0;
	for(armynum=0;armynum<MAXARM;armynum++){
		if((P_ASOLD>0)&&(P_AXLOC==XREAL)&&(P_AYLOC==YREAL)) {
			if((nfound>=pager*SCRARM)&&(nfound<SCRARM+(pager*SCRARM))) {
				/*print that army to nfound%SCRARM*/
				/* patch by rob mayoff */
				if(selector==(nfound%SCRARM)*2) {
					mvaddch((nfound%SCRARM)*2,COLS-21,'*');
					standout();
				} else	mvaddch((nfound%SCRARM)*2,COLS-21,'>');

				if(P_ATYPE<MINLEADER)
				mvprintw((nfound%SCRARM)*2,COLS-20,"army %d: %ld %s",armynum,P_ASOLD,*(shunittype+(P_ATYPE%UTYPE)));
				else 
				mvprintw((nfound%SCRARM)*2,COLS-20,"%s %d: str=%d",*(unittype+(P_ATYPE%UTYPE)),armynum,P_ASOLD);
				clrtoeol();

				if(P_ASTAT >= NUMSTATUS )
				mvprintw((nfound%SCRARM)*2+1,COLS-20," member group %d",P_ASTAT-NUMSTATUS);
				else
				mvprintw((nfound%SCRARM)*2+1,COLS-20," mv:%d st:%s",P_AMOVE,*(soldname+P_ASTAT));
				standend();
			}
			nfound++;
		}
		if((occ[XREAL][YREAL]!=0)
		&&(occ[XREAL][YREAL]!=country)
		&&((sptr->owner==country)||((P_ASOLD>0)&&(P_AXLOC<=XREAL+1)
		&&(P_AXLOC>=XREAL-1)&&(P_AYLOC<=YREAL+1)&&(P_AYLOC>=YREAL-1))))
			found=1;
		if((occ[XREAL][YREAL]!=0)&&(country==0)) found=1;
	}

	if(nfound<SCRARM+(pager*SCRARM)) for(nvynum=0;nvynum<MAXNAVY;nvynum++){
		if(((P_NWSHP!=0)||(P_NMSHP!=0)||(P_NGSHP!=0))
		&&(P_NXLOC==XREAL)&&(P_NYLOC==YREAL)) {
			if((nfound>=pager*SCRARM)&&(nfound<SCRARM+(pager*SCRARM))) {
				/*print a navy*/
				if(selector==(nfound%SCRARM)*2) {
					if((P_NARMY!=MAXARM)||(P_NPEOP!=0))
					mvaddch((nfound%SCRARM)*2,COLS-21,'+');
					else
					mvaddch((nfound%SCRARM)*2,COLS-21,'*');
					standout();
				} else	mvaddch((nfound%SCRARM)*2,COLS-21,'>');
	
				mvprintw((nfound%SCRARM)*2,COLS-20,"nvy %d: mv:%hd cw:%hd",nvynum,P_NMOVE,P_NCREW);
				mvprintw((nfound%SCRARM)*2+1,COLS-20,"war:%2hd mer:%2hd gal:%2hd",
					P_NWAR(N_LIGHT)+P_NWAR(N_MEDIUM)+P_NWAR(N_HEAVY),
					P_NMER(N_LIGHT)+P_NMER(N_MEDIUM)+P_NMER(N_HEAVY),
					P_NGAL(N_LIGHT)+P_NGAL(N_MEDIUM)+P_NGAL(N_HEAVY));
				standend();
			}
			nfound++;
		}
		if((occ[XREAL][YREAL]!=0)&&(occ[XREAL][YREAL]!=country)
		&&(P_NWSHP!=0||P_NMSHP!=0||P_NGSHP!=0)&&(P_NXLOC<=XREAL+1)
		&&(P_NXLOC>=XREAL-1)&&(P_NYLOC<=YREAL+1)&&(P_NYLOC>=YREAL-1))
			nvyfnd=1;
		if((occ[XREAL][YREAL]!=0)&&(country==0)) nvyfnd=1;
	}

	count=0;
	nfound2=nfound;
	if((found==1)||(nvyfnd==1)) for(i=0;i<NTOTAL;i++) {
		if( !magic(i,HIDDEN) || country == 0 ){
			enemy=0;
			for(armynum=0;armynum<MAXARM;armynum++){
				if((i!=country)
				&&(ntn[i].arm[armynum].xloc==XREAL)
				&&(ntn[i].arm[armynum].yloc==YREAL)
				&&(ntn[i].arm[armynum].sold>0)){
				if(nfound2>SCRARM) nfound2=SCRARM;
				if( ntn[i].arm[armynum].unittyp>=MINMONSTER ){
					mvprintw(nfound2*2+count,COLS-20,"%s: str=%d",*(unittype+(ntn[i].arm[armynum].unittyp%UTYPE)),ntn[i].arm[armynum].sold);
					count++;
				} else enemy += ntn[i].arm[armynum].sold;
				}
			}
			if(enemy>0) {
				if((magic(country,NINJA)==TRUE) || country == 0 )
					mvprintw(nfound2*2+count,COLS-20,"%s: %d men  ",ntn[i].name,enemy);
				else if(magic(i,THE_VOID)==TRUE)
				mvprintw(nfound2*2+count,COLS-20,"%s: ?? men  ",ntn[i].name);
				else {
					srand(i*17+enemy+TURN*3);
					mvprintw(nfound2*2+count,COLS-20,"%s: %ld men  ",ntn[i].name,(enemy*(rand()%60+70)/100));
					srand((unsigned) time((long *) 0));
				}
				count++;
			}
			enemy=0;
			for(nvynum=0;nvynum<MAXNAVY;nvynum++){
				if((i!=country)
				&&(ntn[i].nvy[nvynum].xloc==XREAL)
				&&(ntn[i].nvy[nvynum].yloc==YREAL)
				&&(ntn[i].nvy[nvynum].warships
				+ntn[i].nvy[nvynum].merchant
				+(int)ntn[i].nvy[nvynum].galleys!=0))
					enemy += fltships(i,nvynum);
				}
			if(enemy>0) {
				if((magic(country,NINJA)==TRUE) || country == 0 )
					mvprintw(nfound2*2+count,COLS-20,"%s: %d ships",ntn[i].name,enemy);
				else if(magic(i,THE_VOID)==TRUE)
				mvprintw(nfound2*2+count,COLS-20,"%s: ?? ships",ntn[i].name);
				else {
					srand(i*17+enemy+TURN*3);
					mvprintw(nfound2*2+count,COLS-20,"%s: %ld ships",ntn[i].name,(enemy*(rand()%60+70)/100));
					srand((unsigned) time((long *) 0));
				}
				count++;
			}
		}
	}

	standend();
	mvprintw(LINES-13,COLS-20,"x is %d",XREAL);
	clrtoeol();
	mvprintw(LINES-13,COLS-11,"y is %d",YREAL);
	clrtoeol();

	if((country!=0)&&(sptr->altitude==WATER)){
		for(y=LINES-12;y<=LINES-4;y++) {	move(y,COLS-20); clrtoeol();}
		mvaddstr(LINES-10,COLS-9,"WATER");
	} else {
	if((country!=0)&&(country!=sptr->owner)
	&&(magic(sptr->owner,THE_VOID)==TRUE)){
		for(y=LINES-11;y<=LINES-4;y++) { 
			move(y,COLS-20);
			clrtoeol();
		}
	} else {

		for(y=LINES-11;y<=LINES-10;y++) {
			move(y,COLS-20);
			clrtoeol();
		}

		if( sptr->designation!=DNODESIG ) standout();
		for(i=0;*(des+i)!='0';i++)
			if(sptr->designation== *(des+i)){
			mvprintw(LINES-11,COLS-20,"%s",*(desname+i));
			clrtoeol();
			break;
		}
		standend();

		if((sptr->owner==country)||(country==0)||(magic(country,NINJA)==TRUE))
		mvprintw(LINES-9,COLS-20,"people: %6d",sptr->people);
		else {
			srand(country*17+TURN*3+sptr->people);
			mvprintw(LINES-9,COLS-20,"people: %6d",sptr->people*(rand()%60+70)/100);
			srand((unsigned) time((long *) 0));
		}
		clrtoeol();
		if((sptr->owner==country)
		||(sptr->owner==0)
		||(country == 0)
		||(!isntn(ntn[sptr->owner].active))){
			/* exotic trade goods */
			if( sptr->tradegood != TG_none && tg_ok(country,sptr) ) {
				standout();
				mvprintw(LINES-7,COLS-20,"item: %s",tg_name[sptr->tradegood]);
				clrtoeol();
				if( *(tg_stype+sptr->tradegood) == 'x' )
					mvaddstr(LINES-7,COLS-4,"ANY");
				else
					mvprintw(LINES-7,COLS-4,"(%c)",*(tg_stype+sptr->tradegood));
				standend();
			} else {
				mvaddstr(LINES-7,COLS-20,"item: none");
				clrtoeol();
			}

			if( sptr->jewels != 0 && tg_ok(country,sptr)) {
				standout();
				mvprintw(LINES-6,COLS-20,"gold: %2d",sptr->jewels);
				standend();
			} else mvaddstr(LINES-6,COLS-20,"gold:  0");
			if( sptr->metal != 0 && tg_ok(country,sptr)) {
				standout();
				mvprintw(LINES-6,COLS-10,"metal: %2d",sptr->metal);
				standend();
			} else mvaddstr(LINES-6,COLS-10,"metal:  0");

			armbonus = fort_val(sptr);
			if(armbonus>0)
			mvprintw(LINES-5,COLS-20,"fortress: +%d%%",armbonus);
			else move(LINES-5,COLS-20);
			clrtoeol();
		}
		else {
			for(y=LINES-7;y<=LINES-5;y++) {
				move(y,COLS-20);
				clrtoeol();
			}
		}
	}

	standout();
	if((sptr->owner==0)||(ntn[sptr->owner].active==NPC_SAVAGE))
		mvaddstr(LINES-12,COLS-20,"unowned");
	else mvprintw(LINES-12,COLS-20,"owner: %s",ntn[sptr->owner].name);
	standend();
	clrtoeol();

	for(i=0;*(veg+i)!='0';i++)
		if(sptr->vegetation==*(veg+i))
		mvprintw(LINES-11,COLS-10,"%s",*(vegname+i));

	if(((i=tofood(sptr,country)) != 0)
	&&((magic(sptr->owner,THE_VOID)!=TRUE)
	||(sptr->owner==country))){
		if(i>6) standout();
#ifndef HPUX
		if(i<10)	mvprintw(LINES-11,COLS-1,"%d",i);
		else		mvprintw(LINES-11,COLS-2,"%d",i);
#else
		if(i<10)	mvprintw(LINES-11,COLS-2,"%d",i);
		else		mvprintw(LINES-11,COLS-3,"%d",i);
#endif /* HPUX */
		standend();
	}

	if(sptr->owner!=0) for(i=1;i<=8;i++)
		if(ntn[sptr->owner].race==*(races+i)[0]){
		mvprintw(LINES-10,COLS-20,"%s",*(races+i));
		clrtoeol();
		}

	for(i=0;(*(ele+i) != '0');i++)
		if( sptr->altitude == *(ele+i) ){
			mvprintw(LINES-10,COLS-10,"%s",*(elename+i));
			break;
		}
	}

	if(movecost[XREAL][YREAL]<0)
	mvaddstr(LINES-8,COLS-20,"YOU CAN'T ENTER HERE");
	else
	mvprintw(LINES-8,COLS-20,"move cost:  %2d      ",movecost[XREAL][YREAL]);
}

/************************************************************************/
/* 	ARETHEYON() - returns TRUE if 'country' is logged on, else FALSE */
/************************************************************************/
int
aretheyon()
{
	/* return file descriptor for lock file */
	sprintf(fison,"%s%d",isonfile,country);
	return(check_lock(fison,TRUE));
}

/************************************************************************/
/*	COPYSCREEN() -	print copyright notice to screen		*/
/* THIS SUBROUTINE MAY NOT BE ALTERED, AND THE MESSAGE CONTAINED HEREIN	*/
/* MUST BE SHOWN TO EACH AND EVERY PLAYER, EVERY TIME THEY LOG IN	*/
/************************************************************************/
void
copyscreen()
{
#ifdef TIMELOG
	FILE *timefp, *fopen();
	char string[LINELTH+1];
#endif /* TIMELOG */

	clear();
	standout();
	mvprintw(8,COLS/2-12,"Conquer %s.%s",VERSION,PATCHLEVEL);
	standend();
	mvaddstr(10,COLS/2-21, "Copyright (c) 1988 by Edward M Barlow");
	mvaddstr(11,COLS/2-22,"Written Edward M Barlow and Adam Bryant");
	mvaddstr(12,COLS/2-12,"All Rights Reserved");
	mvaddstr(LINES-8,COLS/2-21,"This version is for personal use only");
	mvaddstr(LINES-6,COLS/2-32,"It is expressly forbidden port this software to any form of");
	mvaddstr(LINES-5,COLS/2-32,"Personal Computer or to redistribute this software without");
	mvaddstr(LINES-4,COLS/2-26,"the permission of Edward Barlow or Adam Bryant");
#ifdef TIMELOG
	if ((timefp=fopen(timefile,"r"))!=NULL) {
		fgets(string, 50, timefp);
		mvprintw(LINES-1, 0, "Last Update: %s", string);
		fclose(timefp);
	}
#endif /* TIMELOG */
	mvaddstr(LINES-1, COLS-20, "PLEASE WAIT");
	refresh();
}

/************************************************************************/
/*	BYE()	-	exit gracefully from curses			*/
/************************************************************************/
void
bye(dounlink)
int	dounlink;	/* TRUE if want to do unlink */
{
	if( dounlink ) if(strcmp(fison,"START")!=0) unlink(fison);
	clear();
	refresh();
	nocrmode();
	endwin();
	if (fexe!=NULL) fclose(fexe);
	fprintf(stderr,"quit & save\n");
	exit(SUCCESS);
}

/************************************************************************/
/*	CREDITS() -	print credits notice to screen			*/
/************************************************************************/
void
credits()
{
	clear();
	mvprintw(4,0,"Conquer %s.%s",VERSION,PATCHLEVEL);
	mvaddstr(5,0,"Copyright (c) 1988 by Edward M Barlow");
	mvaddstr(6,0,"written Edward M Barlow and Adam Bryant");
	mvaddstr(12,0,"I would like to thank the following for comments,");
	mvaddstr(13,0,"   patches, and playtesting:");
	mvaddstr(15,0,"Derick Hirasawa    Brian Rauchfuss      Joe E. Powell");
	mvaddstr(16,0,"Andrew Collins     Joe Nolet");
	mvaddstr(17,0,"Kenneth Moyle      Brian Bresnahan");
	mvaddstr(18,0,"Paul Davison       Robert Deroy");
	mvaddstr(20,0,"Also thanks to the many playtesters at Boston University");
	mvaddstr(21,0,"and at the Communications Hex");
	errormsg("");
}

/************************************************************************/
/*	CAMP_INFO() -	display information about current data file	*/
/************************************************************************/
void
camp_info()
{
	int mercs=0,solds=0,armynum,nvynum,nontn=0;
	int numarm=0,numnvy=0,numlead=0;

	clear();
	standout();
	mvaddstr(2,COLS/2-16," CONQUER CAMPAIGN INFORMATION ");
	mvaddstr(5,0,"World Information");
	mvaddstr(5,COLS-40,"Player Information");
	standend();

	/* quick statistics */
	for(armynum=0;armynum<MAXARM;armynum++) {
		if (P_ASOLD!=0) {
			numarm++;
			if (P_ATYPE<MINLEADER) {
				solds+=P_ASOLD;
				if (P_ATYPE==A_MERCENARY) mercs+=P_ASOLD;
			} else if (P_ATYPE<MINMONSTER) {
				numlead++;
			}
		}
	}
	for(nvynum=0;nvynum<MAXNAVY;nvynum++) {
		if (P_NWSHP!=0||P_NGSHP!=0||P_NMSHP!=0) numnvy++;
	}
	for(armynum=1;armynum<NTOTAL;armynum++) {
		if (ismonst(ntn[armynum].active)) nontn++;
	}

	/* global information */
	mvprintw(7,0,"World Map Size............. %dx%d", MAPX, MAPY);
	mvprintw(8,0,"Currently Active Nations... %d", WORLDNTN);
	mvprintw(9,0,"Maximum Active Nations..... %d", NTOTAL-nontn-1);
	mvprintw(10,0,"Number of Monster Nations.. %d", nontn);
	mvprintw(11,0,"Maximum Number of Armies... %d", MAXARM);
	mvprintw(12,0,"Maximum Number of Navies... %d", MAXNAVY);
	mvprintw(13,0,"Land displacement to meet.. %d", MEETNTN);
	mvprintw(14,0,"Chance of Scout Capture.... %d%%", PFINDSCOUT);

	/* user information */
	mvprintw(7,COLS-40,"Number of Leaders........... %d",numlead);
	mvprintw(8,COLS-40,"Men Needed To Take Land..... %d",TAKESECTOR);
	mvprintw(9,COLS-40,"Mercenaries in Nation....... %d",mercs);
	mvprintw(10,COLS-40,"Total Soldiers in Nation.... %d",solds);
	mvprintw(11,COLS-40,"Current Number of Armies.... %d",numarm);
	mvprintw(12,COLS-40,"Current Number of Navies.... %d",numnvy);

	/* other information */
	mvprintw(LINES-6,0,"The Diety: %s", LOGIN);
	if (strcmp(LOGIN,ntn[0].leader)==0) {
		mvaddstr(LINES-5,0,"The Demi-God: [none]");
	} else {
		mvprintw(LINES-5,0,"The Demi-God: %s", ntn[0].leader);
	}

	standout();
	mvaddstr(LINES-2,COLS/2-13," HIT ANY KEY TO CONTINUE");
	standend();
	refresh();

	getch();
}
