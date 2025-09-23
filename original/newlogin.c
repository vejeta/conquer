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

/*create a new login for a new player*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <ctype.h>
#include <pwd.h>
#include "patchlevel.h"
#include "header.h"
#include "data.h"
#include "newlogin.h"

/* information about national classes */
char *Classwho[]= { "DEHO", "DEH", "DEH", "EH", "H", "DHO", "HE", "DHO",
	"O", "O", "O" };
char *CPowlist[]= { "None", "None", "None", "SUMMON", "RELIGION", "SAILOR",
	"URBAN", "WARLORD", "DESTROYER", "MA_MONST", "THE_VOID"};
int  Classcost[]= { 0, 0, 0, 4, 2, 2, 2, 6, 4, 4, 2 };
long Classpow[]= { 0x0L, 0x0L, 0x0L, SUMMON, RELIGION, SAILOR,
	URBAN, 0x000000007L, DESTROYER, 0x00000700L, THE_VOID };

char *Mprompt[]= { "<ADD", "SUB>" };
char *LType[]={ "Random", "Fair", "Great" };

char *Mlabels[]= { "Population", "Treasury", "Location",
	"Military", "Attack Bonus", "Defense Bonus", "Reproduction",
	"Movement", "Magic Powers", "Leaders", "Raw Materials" };

char *Mitems[]= { "people", "gold talons", "location", "soldiers",
	"percent", "percent", "percent", "move points", "powers",
	"nation leaders", "units of food" };

char *Mhelp[]= { "Population: Amount of citizens in your nation",
	"Treasury: Amount of monetary wealth in your nation",
	"Location: Relative value of nation placement in world",
	"Soldiers: Number of men in the national army, not counting leaders",
	"Attack Bonus: Skill level of your troops when attacking",
	"Defense Bonus: Skill level of your troops when defending",
	"Reproduction: Yearly rate of civilian population increase",
	"Movement: Number of movement points per normal army unit",
	"Magic Powers: Randomly obtain a new magical power",
	"Leaders: Number of leader units, including national ruler",
	"Raw Materials: Starting values for jewels, metal, and food" };

extern int pwater;		/* percent water in world (0-100) */
extern FILE *fexe, *fopen();
extern short country;
int	numleaders;
int spent[CH_NUMBER];

/* Teraform the area around somebodies capitol */
/* this gives everybody some chance of success */
void
teraform( x,y,range, chance )
int x,y;
int range,chance;
{
	int i,j;
	switch(curntn->race) {
	case DWARF:
		sct[x][y].altitude=MOUNTAIN;
		for(i=x-range;i<=x+range;i++)
		for(j=y-range;j<=y+range;j++)
		if((i!=x)&&(j!=y)&&(ONMAP(i,j))
		&&(sct[i][j].altitude!=WATER)){
			if (rand()%3==0)
				sct[i][j].altitude=MOUNTAIN;
			else	sct[i][j].altitude=HILL;

			if( rand()%100 < chance ) getmetal( &sct[i][j] );
		}
		return;
	case ELF:
		sct[x][y].vegetation = FOREST;
		for(i=x-range;i<=x+range;i++)
		for(j=y-range;j<=y+range;j++)
		if((i!=x)&&(j!=y)&&(ONMAP(i,j))
		&&(sct[i][j].altitude!=WATER)) {
			if (rand()%3==0)
				sct[i][j].vegetation=FOREST;
			else	sct[i][j].vegetation=WOOD;
			if( rand()%100 < chance ) getjewel( &sct[i][j] );
		}
		return;
	case ORC:
		sct[x][y].altitude=MOUNTAIN;
		for(i=x-range;i<=x+range;i++)
		for(j=y-range;j<=y+range;j++)
		if((i!=x)&&(j!=y)&&(ONMAP(i,j))
		&&(sct[i][j].altitude!=WATER)) {
			if (rand()%3==0)
				sct[i][j].altitude=MOUNTAIN;
			else	sct[i][j].altitude=HILL;

			if( rand()%100 < chance ) {
				if(rand()%2==0)
					getmetal( &sct[i][j] );
				else	getjewel( &sct[i][j] );
			}
		}
		return;
	case HUMAN:
		sct[x][y].altitude = CLEAR;
		for(i=x-range;i<=x+range;i++)
		for(j=y-range;j<=y+range;j++)
		if((i!=x)&&(j!=y)&&(ONMAP(i,j))
		&&(sct[i][j].altitude!=WATER)) {
			if (rand()%2==0)
			sct[i][j].altitude = CLEAR;

			if (rand()%2==0)
				sct[i][j].vegetation=WOOD;
			else	sct[i][j].vegetation=GOOD;

			if( rand()%100 < chance ) {
				if (rand()%2==0)
					getmetal( &sct[i][j] );
				else	getjewel( &sct[i][j] );
			}
		}
		return;
	}
}

void
mailtopc(string)
char	*string;
{
	int	ctry;
	for( ctry=0; ctry<NTOTAL; ctry++){
		if((ctry==0)||(ispc(ntn[ctry].active))){
			if (mailopen(ctry)!=(-1)) {
				fprintf(fm,"Message from Conquer\n\n");
				fprintf(fm,"%s\n",string);
				mailclose(ctry);
			}
		}
	}
}

/* function  to initialize the curses display */
void
newinit()
{
	initscr();
	/* check terminal size */
	if (COLS<80 || LINES<24) {
		mvaddstr(LINES-2,0,"conqrun: terminal should be at least 80x24");
		mvaddstr(LINES-1,0,"Please try again with a different setup");
		beep();
		getch();
		newbye(SUCCESS);
	}
	crmode();
	noecho();
}

/* function to end the curses display */
void
newreset()
{
	clear();
	refresh();
	nocrmode();
	endwin();
}

/* function to leave the program completely */
void
newbye(status)
	int status;
{
	newreset();
	exit(status);
}

/* message without wait for keystroke */
void
newmsg(str)
	char *str;
{
	mvaddstr(LINES-1,0,str);
	clrtoeol();
	refresh();
}

/* message with wait for keystroke */
void
newerror(str)
	char *str;
{
	mvaddstr(LINES-1, 0, str);
	clrtoeol();
	mvaddstr(LINES-1, COLS-16, "PRESS ANY KEY");
	beep();
	refresh();
	getch();
	move(LINES-1,0);
	clrtoeol();
	refresh();
}

/* function to check if a character is in a character array */
int
in_str(ch,str)
	char ch, *str;
{
	int i,l=strlen(str);

	for(i=0;i<l;i++)
		if (ch == str[i]) return(TRUE);
	return(FALSE);
}

/* function to display the comment and error window */
void
errorbar(str1,str2)
	char *str1,*str2;
{
	int i;
	move(LINES-4,0);
	standout();
	for(i=0;i<COLS-1;i++)
		addch(' ');
	standend();
	mvprintw(LINES-3,0," Conquer %s.%s: %s",VERSION,PATCHLEVEL,str1);
	mvaddstr(LINES-3,COLS-strlen(str2)-2,str2);
	move(LINES-2,0);
	for(i=0;i<COLS-1;i++)
		addch('-');
}

/* display amount string at current location */
void
dispitem(item, amount)
	int item;
	long amount;
{
	if (item == CH_LOCATE) {
		printw("%s %s", LType[amount], Mitems[item]);
	} else {
		printw("%ld %s", amount, Mitems[item]);
	}

	if (item != CH_RAWGOODS) {
		addch('.');
		return;
	}

	/* now show the extras for the Raw Materials */
	printw(", %ld jewels", (long) (amount *
		((float)NLJEWELS/Mvalues[CH_RAWGOODS])));
	printw(", and %ld metal.", (long) (amount *
		((float)NLMETAL/Mvalues[CH_RAWGOODS])));
}

/* show the current amount for country item */
void
showitem(line,item)
	int line, item;
{
	char tempc[LINELTH];
	
	move(line,15);
	if (item == CH_LOCATE) {
		sprintf(tempc,"%s %s", LType[spent[item]], Mitems[item]);
		printw("%23s",tempc);
	} else {
		sprintf(tempc,"%ld %s", spent[item]*Mvalues[item], Mitems[item]);
		printw("%23s",tempc);
	}
	
	if (item != CH_RAWGOODS) return;
	line++;

	/* now show the extras for the Raw Materials */
#if NLJEWELS==NLMETAL
	sprintf(tempc,"%ld jewels & metal",
		   spent[CH_RAWGOODS]*NLJEWELS);
	mvprintw(line,0,"%38s",tempc);
	mvprintw(line,COLS/2+13,"%ld jewels & metal",NLJEWELS);
#else
	sprintf(tempc,"%ld jewels",
		   spent[CH_RAWGOODS]*NLJEWELS);
	mvprintw(line,0,"%38s",tempc);
	mvprintw(line++,COLS/2+10,"%ld jewels",NLJEWELS);
	nsprintf(tempc,"%ld metal",
		    spent[CH_RAWGOODS]*NLMETAL);
	mvprintw(line,0,"%38s",tempc);
	mvprintw(line,COLS/2+13,"%ld metals",NLMETAL);
#endif
}

/* convert the stored information into the nation statistics */
void
convert()
{
	int i,loop;
	long x;

	curntn->tciv = Mvalues[CH_PEOPLE] * spent[CH_PEOPLE];
	curntn->tgold = Mvalues[CH_TREASURY] * spent[CH_TREASURY];
	switch(spent[CH_LOCATE]) {
	case 2:
		curntn->location = GREAT;
		break;
	case 1:
		curntn->location = FAIR;
		break;
	case 0:
	default:
		curntn->location = RANDOM;
		break;
	}
	curntn->tmil = Mvalues[CH_SOLDIERS] * spent[CH_SOLDIERS];
	curntn->aplus = (short) Mvalues[CH_ATTACK] * spent[CH_ATTACK];
	curntn->dplus = (short) Mvalues[CH_DEFEND] * spent[CH_DEFEND];
	curntn->repro = (char) Mvalues[CH_REPRO] * spent[CH_REPRO];
	curntn->maxmove = (unsigned char) Mvalues[CH_MOVEMENT]
		* spent[CH_MOVEMENT];
	for(i = 0; i < spent[CH_MAGIC]; i++) {
		/* purchase magic */
		loop = TRUE;
		while (loop == TRUE) {
			if((x=getmagic((rand()%M_MGK+M_MIL)))!=0L){
				CHGMGK;
				loop = FALSE;
			}
		}
	}
	numleaders = (int) Mvalues[CH_LEADERS] * spent[CH_LEADERS];
	curntn->tfood = Mvalues[CH_RAWGOODS] * spent[CH_RAWGOODS];
	curntn->jewels = NLJEWELS * spent[CH_RAWGOODS];
	curntn->metals = NLMETAL * spent[CH_RAWGOODS];
}

void
newlogin(realuser)
  int realuser;
{
	/* use points to create empire, add if late starter*/
	int points, clr;
	int pccount;
	int choice, direct;
	int valid=TRUE;  /*valid==FALSE means continue loop*/
	int temp,ypos,xpos;
	int more=TRUE;	/*0 if add another player*/
	long x;
	char tempc[LINELTH];
	char passwd[PASSLTH+1];
	register i;

	/* setup curses display */
	newinit();

	/*find valid nation number type*/
	country=0;
        pccount=0;
	for(i=1;i<NTOTAL;i++) 
		if(ntn[i].active==INACTIVE) 
		{
			country=i;
			curntn = &ntn[country];
			break;
		}
		else
		{
#ifdef CHECKUSER
			if ((ntn[i].uid == realuser) &&
			    (ntn[0].uid != realuser))
			{
			   newerror ("You may only have one nation in this world.");
			   newreset();
			   return;
			}
#endif
			pccount++;
		}

#ifdef MONSTER
	pccount = pccount + 4;	/* space taken by monster nations */
#endif
	while(more==TRUE) {
		clear();

		sprintf(tempc,"Country #%d", country);
		errorbar("Nation Builder",tempc);
		if((country==0)||(pccount+1>=NTOTAL-REVSPACE)) {
			newerror("No more nations available");
			newreset();
			return;
		}

		/* open output for future printing*/
		mvprintw(0,0,"Building Country Number %d",country);
		sprintf(tempc,"%s%d",exefile,i);
		if ((fexe=fopen(tempc,"w"))==NULL) {
			sprintf(tempc,"Error opening <%s>",tempc);
			newerror(tempc);
			newbye(FAIL);
		}

		/* begin purchasing */
		points=MAXPTS;
		for (i=0; i<CH_NUMBER; i++) {
			spent[i] = 0;
		}

		valid=FALSE;
		/*get name*/
		while(valid==FALSE) {
			valid=TRUE;
			mvaddstr(1,0,"Enter a Name for your Country: ");
			clrtoeol();
			refresh();
			get_nname(tempc);

			if((strlen(tempc)<=1)
			 ||(strlen(tempc)>NAMELTH)){
				newerror("Invalid Name Length");
				valid=FALSE;
			}

			/*check if already used*/
			if((strcmp(tempc,"god")==0)
			||(strcmp(tempc,"unowned")==0)){
				newerror("Name already used");
				valid=FALSE;
			}

			for(i=1;i<NTOTAL;i++)
			if((i!=country)&&(strcmp(ntn[i].name,tempc)==0)&&(isntn(ntn[i].active))) {
				newerror("Name already used");
				valid=FALSE;
			}
		}
		strcpy(curntn->name,tempc);
		move(0,0);
		clrtoeol();
		move(1,0);
		clrtoeol();
		standout();
		mvprintw(0,COLS/2-15-strlen(curntn->name)/2,
			"< Building Country %s >",curntn->name,country);
		standend();
		clrtoeol();

		valid=FALSE;
		while(valid==FALSE) {			/* password routine */
			mvaddstr(2,0,"Enter National Password: ");
			clrtoeol();
			refresh();
			i = get_pass(tempc);
			if (i < 2) {
				newerror("Password Too Short");
				continue;
			} else if (i > PASSLTH) {
				newerror("Password Too Long");
				continue;
			}
			mvaddstr(2,0,"Reenter National Password: ");
			clrtoeol();
			refresh();
			i = get_pass(passwd);

			if((i<2)||(i>PASSLTH)||(strncmp(passwd,tempc,PASSLTH)!=0)){
				newerror("Invalid Password Match");
			} else valid=TRUE;
		}
		strncpy(curntn->passwd,crypt(tempc,SALT),PASSLTH+1);

		/*get your name*/
		valid=FALSE;
		while(valid==FALSE) {
			valid=TRUE;
			mvaddstr(2,0,"Enter the name of your country's leader (Ex. The Ed, Gandalf, Conan)");
			clrtoeol();
			mvprintw(3,0,"    [maximum %d characters]: ",LEADERLTH);
			refresh();
			get_nname(tempc);
			if((strlen(tempc)>LEADERLTH)||(strlen(tempc)<2)) {
				newerror("Invalid Name Length");
				valid=FALSE;
			}
			else strcpy(curntn->leader,tempc);
		}

		mvprintw(2,0,"Leader Name: %s", curntn->leader);
		clrtoeol();
		mvaddstr(3,0,"Enter your Race [ (D)warf (E)lf (H)uman (O)rc ]:");
		clrtoeol();
		refresh();
		valid=FALSE;
		while(valid==FALSE) {
			valid=TRUE;
			switch(getch()) {
			case 'D':
			case 'd':
				/*MINER POWER INATE TO DWARVES*/
				newmsg("Dwarves have MINING skills");
				mvaddstr(3,0,"National Race: Dwarf");
				clrtoeol();
				curntn->powers=MINER;
				x=MINER;
				CHGMGK;
				points -= getclass(DWARF);
				curntn->race=DWARF;
				spent[CH_TREASURY]=NLDGOLD;
				spent[CH_RAWGOODS]=NLDRAW;
				spent[CH_PEOPLE]= NLDCIVIL;
				spent[CH_SOLDIERS]= NLDMILIT;
				spent[CH_REPRO]= NLDREPRO;
				spent[CH_MOVEMENT]= NLDMMOVE;
				spent[CH_ATTACK]= NLDAPLUS;
				spent[CH_DEFEND]= NLDDPLUS;
				spent[CH_LOCATE]= NLRANDOM;
				points-=nstartcst();
				break;
			case 'E':
			case 'e':
				newmsg("Elves are magically cloaked (VOID power)");
				mvaddstr(3,0,"National Race: Elf");
				clrtoeol();
				curntn->powers=THE_VOID;
				x=THE_VOID;
				CHGMGK;
				points -= getclass(ELF);
				curntn->race=ELF;
				spent[CH_TREASURY]=NLEGOLD;
				spent[CH_RAWGOODS]=NLERAW;
				spent[CH_PEOPLE]= NLECIVIL;
				spent[CH_SOLDIERS]= NLEMILIT;
				spent[CH_REPRO]= NLEREPRO;
				spent[CH_MOVEMENT]= NLEMMOVE;
				spent[CH_ATTACK]= NLEAPLUS;
				spent[CH_DEFEND]= NLEDPLUS;
				spent[CH_LOCATE]= NLFAIR;
				points-=nstartcst();
				break;
			case 'O':
			case 'o':
				/*MINOR MONSTER POWER INATE TO ORCS*/
				newmsg("Your leader is a Monster!");
				mvaddstr(3,0,"National Race: Orc");
				clrtoeol();
				curntn->powers=MI_MONST;
				x=MI_MONST;
				CHGMGK;
				points -= getclass(ORC);
				curntn->race=ORC;
				spent[CH_TREASURY]=NLOGOLD;
				spent[CH_RAWGOODS]=NLORAW;
				spent[CH_PEOPLE]= NLOCIVIL;
				spent[CH_SOLDIERS]= NLOMILIT;
				spent[CH_REPRO]= NLOREPRO;
				spent[CH_MOVEMENT]= NLOMMOVE;
				spent[CH_ATTACK]= NLOAPLUS;
				spent[CH_DEFEND]= NLODPLUS;
				spent[CH_LOCATE]= NLRANDOM;
				points-=nstartcst();
				break;
			case 'H':
			case 'h':
				curntn->race=HUMAN;
				newmsg("Humans have the combat skill of a WARRIOR");
				mvaddstr(3,0,"National Race: Human");
				clrtoeol();
				curntn->powers = WARRIOR;
				x=WARRIOR;
				CHGMGK;
				points -= getclass(HUMAN);
				spent[CH_TREASURY]=NLHGOLD;
				spent[CH_RAWGOODS]=NLHRAW;
				spent[CH_PEOPLE]= NLHCIVIL;
				spent[CH_SOLDIERS]= NLHMILIT;
				spent[CH_REPRO]= NLHREPRO;
				spent[CH_MOVEMENT]= NLHMMOVE;
				spent[CH_ATTACK]= NLHAPLUS;
				spent[CH_DEFEND]= NLHDPLUS;
				spent[CH_LOCATE]= NLRANDOM;
				points-=nstartcst();
				break;
			default:
				valid=FALSE;
			}
		}
		mvprintw(4,0,"Nation Class: %s",Class[curntn->class]);
		clrtoeol();

		valid=FALSE;
		if( curntn->race == ORC ) {	/* orcs are always evil */
			valid=TRUE;
			curntn->active=PC_EVIL;
		} else {
			mvaddstr(5,0,"Please Enter Alignment [ (G)ood, (N)eutral, (E)vil ]");
			refresh();
		}
		while (valid==FALSE) {
			valid=TRUE;
			switch(getch()) {
			case 'G':
			case 'g':
				curntn->active=PC_GOOD;
				break;
			case 'N':
			case 'n':
				curntn->active=PC_NEUTRAL;
				break;
			case 'E':
			case 'e':
				curntn->active=PC_EVIL;
				break;
			default:
				valid=FALSE;
				break;
			}
		}
		mvprintw(2,COLS/2,"Alignment: %s", alignment[curntn->active]);
		clrtoeol();


		/* get new nation mark */
		curntn->mark = ' ';
		while(TRUE) {
			temp = 30;
			mvaddstr(6,0,"This can be any of the following:");
			for (tempc[0]='!';tempc[0]<='~';tempc[0]++) {
				if( markok( tempc[0], FALSE ) ) {
					temp += 2;
					if (temp>COLS-20) {
						printw("\n    ");
						temp = 8;
					}
					printw(" %c",tempc[0]);
				}
			}
			mvaddstr(5,0,"Enter National Mark (for maps): ");
			clrtoeol();
			refresh();
			tempc[0] = getch();
			if( markok( tempc[0], TRUE ) ){
				curntn->mark=(*tempc);
				break;
			}
		}

		mvprintw(3,COLS/2,"National Mark [%c]",curntn->mark);
		clrtoeol();
		move(5,0);
		clrtoeol();
		move(6,0);
		clrtoeol();
		move(7,0);
		clrtoeol();
		refresh();

		ypos = 6;
		mvprintw(ypos,0,"  %-13s       %s", "ITEM", "CURRENTLY HAVE" );
		mvprintw(ypos++,COLS/2+5,"%4s    %s", "COST", "AMOUNT" );
		for(i=0; i<CH_NUMBER; i++) {
			mvprintw(ypos,0,"%-15s", Mlabels[i]);
			showitem(ypos,i);
			if (i==CH_LOCATE) {
				mvprintw(ypos,COLS/2+5,"%3d     %s", Mcost[i],
					    "Better Location");
			} else {
				if (curntn->race==ORC) {			
					switch(i) {
					case CH_MOVEMENT:
						mvaddstr(ypos++,COLS/2+5,"  -     --------");
						continue;
					case CH_REPRO:
						x = 2*Munits[i]*Mvalues[i];
						break;
					case CH_ATTACK:
					case CH_DEFEND:
						x = Munits[i]*Mvalues[i]/2;
						break;
					default:
						x = Munits[i]*Mvalues[i];
						break;
					}
					mvprintw(ypos,COLS/2+5,"%3d for",Mcost[i]);
					printw(" %ld %s", x, Mitems[i]);
				} else {
					mvprintw(ypos,COLS/2+5,"%3d for",Mcost[i]);
					printw(" %ld %s", Munits[i]*Mvalues[i], Mitems[i]);
				}
			}
			ypos++;
		}

		/* show everything before menu */
		direct = ADDITION;
		choice = CH_PEOPLE;
		xpos = COLS/2;
		ypos = 7;
		valid = FALSE;
		clr = 1;
		standout();
		mvaddstr(LINES-4,2,"DONE=ESC  EXEC=SPACE  INFO=\"?\"  ADD=\"<+h\"  SUBtract=\">+l\"  UP=\"k\"  DOWN=\"j\"");
		standend();

		while(valid==FALSE) {
			if (clr==1) {
				standout();
				mvprintw(4,COLS/2,"Points Left: %d", points);
				standend();
				clrtoeol();
				clr++;
			} else if (clr==2) {
				newmsg("");
				clr = 0;
			}
			standout();
			mvaddstr(ypos+choice,xpos,Mprompt[direct]);
			standend();
			refresh();
			switch(getch()) {
			case '':
				/* redraw */
				wrefresh(stdscr);
				break;
			case '?':
				/* help on topic */
				newerror(Mhelp[choice]);
				break;
			case '\033':
				/* exit option */
				if (points > 0) {
					newmsg("Use remaining points for population? [ny]");
					if (getch()!='y') {
						newerror("All points must be spent prior to exiting");
						break;
					}
					temp = points * Munits[CH_PEOPLE] / Mcost[CH_PEOPLE];
					x = temp * Mvalues[CH_PEOPLE];
					spent[CH_PEOPLE] += temp;
					showitem(ypos+CH_PEOPLE,CH_PEOPLE);
					points = 0;
					sprintf(tempc,"Buying %ld more civilians", x);
					newerror(tempc);
				}
				newmsg("Is the modification complete? (y or n)");
				while (((temp=getch())!='y')&&(temp!='n')) ;
				if (temp == 'y') {
					valid = TRUE;
				}
				clr = 1;
				break;
			case '-':
			case '>':
			case 'l':
			case 'L':
				/* subtraction */
				direct = SUBTRACTION;
				break;
			case '+':
			case '<':
			case 'h':
			case 'H':
				/* addition */
				direct = ADDITION;
				break;
			case '\b':
			case '\177':
				/* decrease choice -- with wrap */
				mvaddstr(ypos+choice,xpos,"    ");
				if (choice==CH_PEOPLE) {
					choice = CH_RAWGOODS;
				} else {
					choice--;
					if (choice==CH_MOVEMENT && curntn->race==ORC) {
						choice--;
					}
				}
				break;
			case 'k':
			case 'K':
				/* move choice up one */
				if (choice > CH_PEOPLE) {
					mvaddstr(ypos+choice,xpos,"    ");
					choice--;
					if (choice==CH_MOVEMENT && curntn->race==ORC) {
						choice--;
					}
				}
				break;
			case '\r':
			case '\n':
				/* increase choice -- with wrap */
				mvaddstr(ypos+choice,xpos,"    ");
				if (choice==CH_RAWGOODS) {
					choice = CH_PEOPLE;
				} else {
					choice++;
					if (choice==CH_MOVEMENT && curntn->race==ORC) {
						choice++;
					}
				}
				break;
			case 'j':
			case 'J':
				/* move choice down one */
				if (choice < CH_RAWGOODS) {
					mvaddstr(ypos+choice,xpos,"    ");
					choice++;
					if (choice==CH_MOVEMENT && curntn->race==ORC) {
						choice++;
					}
				}
				break;
			case ' ':
			case '.':
				/* make the selection */
				if (curntn->race == ORC) {
					switch(choice) {
					case CH_REPRO:
						temp = 2*Munits[choice];
						break;
					case CH_ATTACK:
					case CH_DEFEND:
						temp = Munits[choice]/2;
						break;
					default:
						temp = Munits[choice];
						break;
					}
				} else temp = Munits[choice];
				if (direct == ADDITION) {
					if (Mcost[choice] > points) {
						sprintf(tempc, "You do not have %d points to spend",
							Mcost[choice]);
						newerror(tempc);
					} else if ((choice == CH_REPRO)&&(curntn->race==ORC)
						&&(spent[choice] + temp > 12)) {
						newerror("You may not purchase any more of that item");
					} else if ((spent[choice] + temp > Maxvalues[choice])
						&&((curntn->race!=ORC)||(choice!=CH_REPRO))) {
						newerror("You may not purchase any more of that item");
					} else {
						spent[choice] += temp;
						newmsg("You now have ");
						dispitem(choice,spent[choice]*Mvalues[choice]);
						showitem(ypos+choice,choice);
						points -= Mcost[choice];
						clr = 1;
					}
				} else if (direct == SUBTRACTION) {
					if (spent[choice] - temp < Minvalues[choice]) {
						newerror("You may not sell back any more of that item");
					} else {
						spent[choice] -= temp;
						newmsg("You now have ");
						dispitem(choice,spent[choice]*Mvalues[choice]);
						showitem(ypos+choice,choice);
						points += Mcost[choice];
						clr = 1;
					}
				}
				break;
			default:
				break;
			}
		}

		/* check for save */
		newmsg("Save this nation? [ny]");
		if(getch()!='y'){
			curntn->active=INACTIVE;
			curntn->powers=0;
			newerror("Ok, Nation Deleted");
			fclose(fexe);
		} else {
			convert();
			place(-1,-1);
			newerror("Ok, Your Nation has been Added to the World");
			att_setup(country);	/* setup values ntn attributes */
#ifdef CHECKUSER
			curntn->uid = realuser;
#endif
			fclose(fexe);
			pccount++;
			sprintf(tempc,"NOTICE: Nation %s added to world on turn %d\n",curntn->name,TURN);
			mailtopc(tempc);
			/* cannot clear until after placement and initializing */
			curntn->powers=0;
		}
#ifndef CHECKUSER
		country=0;
		for(i=1;i<NTOTAL;i++) if (ntn[i].active==INACTIVE) {
			country = i;
			curntn = &ntn[country];
			break;
		}
		if ((country!=0)&&(pccount+1>=NTOTAL-REVSPACE)) {
			newmsg("Do you wish to Add another Nation? [ny]");
			if (getch()!='y') more = FALSE;
			else more = TRUE;
		} else {
			more = FALSE;
			newerror("No More Available Nations");
		}
#else
		more = FALSE;
#endif
	}
	newreset();
	att_base();	/* calculate base nation attributes */
	writedata();
}

/*****************************************************************/
/* PLACE(): put nation on the map.  Fill out army structures too */
/*****************************************************************/
void
place(xloc,yloc)
int	xloc,yloc;	/* if not -1,-1 should place in this spot */
{
	int	placed=0,armysize=100;
	short	armynum=0;
	long	people;
	char tempo[LINELTH+1];
	int	x,y,i,j,temp,t;
	int	n=0, leadtype;
	long	soldsleft;	/* soldiers left to place */

	if( xloc != -1 && yloc != -1 && is_habitable(xloc,yloc)) {
		placed=1;
		x = xloc;
		y = yloc;
	}

	switch(curntn->location) {
	case OOPS:
		while((placed == 0)&&(n++<2000)){
			if(ispc(curntn->active)){
				x = (rand()%(MAPX-8))+4;
				y = (rand()%(MAPY-8))+4;
			} else {
				x = (rand()%(MAPX-2))+1;
				y = (rand()%(MAPY-2))+1;
			}
			if(is_habitable(x,y)) placed=1;

			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[i][j].owner!=0) placed=0;
			temp=0;
			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[i][j].altitude==WATER) temp++;
			if(temp>=7) placed=0;
		}
		if(placed) teraform( x,y,1,25 );
		break;
	case RANDOM:
		while ((placed == 0)&&(n++<2000)){
			if(ispc(curntn->active)){
				if(MAPX>12){
					x = rand()%(MAPX-12)+6;
					y = rand()%(MAPY-12)+6;
				} else {
					x = rand()%(MAPX-8)+4;
					y = rand()%(MAPY-8)+4;
				}
				if(is_habitable(x,y)) placed=1;
				/*important that no countries near*/
				for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++)
					if((isntn(ntn[sct[i][j].owner].active))
					&&(sct[i][j].owner!=0)) placed=0;
			} else {
				x = (rand()%(MAPX-6))+3;
				y = (rand()%(MAPY-6))+3;
				if(is_habitable(x,y)) placed=1;
				/*important that no countries near*/
				for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++)
					if((isntn(ntn[sct[i][j].owner].active))
					&&(sct[i][j].owner!=0)) placed=0;
			}
			temp=0;
			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[i][j].altitude==WATER) temp++;
			if(temp>=7) placed=0;
			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[i][j].owner!=0) placed=0;
		}
		if(placed) teraform( x,y,1,40 );
		break;
	case FAIR:
		while ((placed == 0)&&(n++<2000)) {
			if(ispc(curntn->active)){
				if(MAPX>24) {
					x = rand()%(MAPX-24)+12;
				} else {
					x = rand()%(MAPX-14)+7;
				}
				if(MAPY>24) {
					y = rand()%(MAPY-24)+12;
				} else {
					y = rand()%(MAPY-14)+7;
				}
			} else {
				x = rand()%(MAPX-10)+5;
				y = rand()%(MAPY-10)+5;
			}

			if(!is_habitable(x,y)) continue;
			if(tofood( &sct[x][y],country)<DESFOOD) continue;

			placed=1;
			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[i][j].owner!=0) placed=0;

			if(pwater>50) {
				temp=0;
				for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[i][j].altitude==WATER) temp++;
				if(temp>=7) placed=0;

				/*important that no countries near*/
				for(i=x-3;i<=x+3;i++) for(j=y-3;j<=y+3;j++){
				if((isntn(ntn[sct[i][j].owner].active))
					&&(sct[i][j].owner!=0)) placed=0;
				}
			} else {
				temp=0;
				for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
					if(sct[i][j].altitude==WATER) temp++;
				if(temp>=5) placed=0;

				/*important that no countries near*/
				for(i=x-3;i<=x+3;i++) for(j=y-3;j<=y+3;j++){
				if((isntn(ntn[sct[i][j].owner].active))
				&&(sct[i][j].owner!=0)) placed=0;
				}
			}
		}

		if(placed) teraform( x,y,1,65 );
		break;
	case GREAT:
		placed = 0;
		while ((placed == 0) && (n++<2000)){
			if(ispc(curntn->active)){
				if (MAPX>40){
					x = rand()%(MAPX-40)+20;
				}else{
					x = rand()%(MAPX-18)+9;
				}
				if (MAPY>40){
					y = rand()%(MAPY-40)+20;
				}else{
					y = rand()%(MAPY-18)+9;
				}

				if(is_habitable(x,y)) placed=1;
				/*important that no countries near*/
				for(i=x-4;i<=x+4;i++) for(j=y-4;j<=y+4;j++){
				if((isntn(ntn[sct[i][j].owner].active))
				&&( sct[i][j].owner!=0)) placed=0;
				}
			} else {
				if(MAPX>24){
					x = rand()%(MAPX-24)+12;
				}else {
					x = rand()%(MAPX-12)+6;
				}
				if(MAPY>24){
					y = rand()%(MAPY-24)+12;
				}else {
					y = rand()%(MAPY-12)+6;
				}
				if(is_habitable(x,y)) placed=1;
				/*important that no countries near*/
				for(i=x-4;i<=x+4;i++) for(j=y-4;j<=y+4;j++){
				if((isntn(ntn[sct[i][j].owner].active))
					&&(sct[i][j].owner!=0)) placed=0;
				}
			}

			for(i=x-1;i<=x+1;i++) for(j=y-1;j<=y+1;j++)
				if(sct[i][j].owner!=0) placed=0;

			temp=0;
			/*if any water within 2 sectors placed = 0*/
			for(i=x-2;i<=x+2;i++) for(j=y-2;j<=y+2;j++)
				if(tofood( &sct[x][y],country)<=0)
					temp++;

			if( pwater>50 ) {
				if(temp>=18) placed=0;
			} else {
				if(temp>=15) placed=0;
			}
		}
		if(placed) teraform( x,y,1,100 );
	}

	/*done with one try*/
	if(placed==1) {
		curntn->capx = x;
		curntn->capy = y;
		sct[x][y].designation=DCAPITOL;
		sct[x][y].tradegood=rand()%(END_KNOWLEDGE-END_SPOILRATE)+END_SPOILRATE+1;
		sct[x][y].jewels=0;
		sct[x][y].metal=0;
		sct[x][y].owner=country;
		sct[x][y].people=curntn->tciv;
		sct[x][y].fortress=5;

		/* put all military into armies of armysize */
		armysize = (TAKESECTOR*12)/10;
		if(armysize<100) armysize=100;
		/* cant have more than 50% leaders */
		if( MAXARM < numleaders * 2 ) numleaders = MAXARM / 2;
		armynum=0;
		soldsleft = curntn->tmil;
		P_ASOLD = curntn->tmil/MILINCAP;
		soldsleft-=P_ASOLD;
		P_ATYPE=defaultunit(country);
		P_ASTAT=GARRISON;
		P_AMOVE=0;
		P_AXLOC=curntn->capx;
		P_AYLOC=curntn->capy;
		armynum++;

		armysize = max( armysize, soldsleft  / (MAXARM-numleaders-1));

		/* give you your leaders */
		leadtype = getleader(curntn->class);
		P_ATYPE = leadtype-1;	/* This is the national leader */
		P_ASOLD = *(unitminsth+((leadtype-1)%UTYPE));
		P_AXLOC = curntn->capx;
		P_AYLOC = curntn->capy;
		P_ASTAT = DEFEND;
		P_AMOVE = 2*curntn->maxmove;
		armynum++;
		numleaders--;
		while ((armynum < MAXARM)&&(numleaders>0)) {
			P_ATYPE=leadtype;
			P_ASOLD= *(unitminsth+(leadtype%UTYPE));
			P_AXLOC=curntn->capx;
			P_AYLOC=curntn->capy;
			P_ASTAT=DEFEND;
			P_AMOVE=2*curntn->maxmove;
			armynum++;
			numleaders--;
		}

		/* give you the rest of your armies */
		while((armynum < MAXARM)&&(soldsleft >0)) {
			P_ATYPE=defaultunit(country);
			if(soldsleft >= armysize){
				P_ASOLD=armysize;
				soldsleft -=armysize;
			} else {
				P_ASOLD=soldsleft ;
				soldsleft=0;
			}
			P_AXLOC=curntn->capx;
			P_AYLOC=curntn->capy;
			P_ASTAT=DEFEND;
			P_AMOVE=curntn->maxmove;
			armynum++;
		}

		if(soldsleft >0) {
			curntn->arm[0].sold += soldsleft;
			curntn->arm[0].unittyp = A_INFANTRY;
		}

		/* give you some terain to start with: pc nations get more*/
		if (isnotpc(curntn->active)) t=1;
		else if (curntn->location==OOPS) t=0;
		else if (curntn->location==RANDOM) t=0;
		else if (curntn->location==FAIR) t=1;
		else if (curntn->location==GREAT) t=2;
		else {
			if (ispc(curntn->active))
			newerror("Error in finding placement");
			t=0;
		}
		if( t==1 )
			people = sct[x][y].people / 12;
		else if( t==2 )
			people = sct[x][y].people / 30;

		curntn->tsctrs=1;
		for(i=x-t;i<=x+t;i++) for(j=y-t;j<=y+t;j++)
			if((tofood( &sct[i][j],country)>=DESFOOD)
			&&(sct[i][j].owner==0)
			&&(is_habitable(i,j)==TRUE)
			&&(sct[i][j].people==0)) {
				curntn->tsctrs++;
				sct[i][j].owner=country;
				sct[i][j].designation=DFARM;
				sct[i][j].people=people;
				sct[x][y].people-=people;
			}
		/* make sure status is set properly */
		for(i=0;i<NTOTAL;i++) {
			if (ntn[i].active < NPC_PEASANT) {
				ntn[i].dstatus[country]=UNMET;
				ntn[country].dstatus[i]=UNMET;
			} else {
				ntn[i].dstatus[country]=WAR;
				ntn[country].dstatus[i]=WAR;
			}
		}
	}
	else {
		if(curntn->location==OOPS) {
			newerror("MAJOR ERROR IN PLACEMENT");
		} else if(curntn->location==RANDOM) {
			newerror("Random Place Failed, trying to place again");
			curntn->location=OOPS;
			place(-1,-1);
		} else if(curntn->location==FAIR) {
			sprintf(tempo,"Fair Place Failed, trying again - Adding %ld people to nation",Munits[CH_PEOPLE]*Mvalues[CH_PEOPLE]/Mcost[CH_PEOPLE]);
			newerror(tempo);
			/*give back one point -> NLPOP people*/
			curntn->tciv += Munits[CH_PEOPLE] * Mvalues[CH_PEOPLE]
				/ Mcost[CH_PEOPLE];
			curntn->location=RANDOM;
			place(-1,-1);
		} else if(curntn->location==GREAT) {
			sprintf(tempo,"Great Place Failed, trying again - Adding %ld people to nation",Munits[CH_PEOPLE]*Mvalues[CH_PEOPLE]/Mcost[CH_PEOPLE]);
			newerror(tempo);
			/*give back one point -> NLPOP people*/
			curntn->tciv+= Munits[CH_PEOPLE] * Mvalues[CH_PEOPLE]
				/ Mcost[CH_PEOPLE];
			curntn->location=FAIR;
			place(-1,-1);
		}
	}
}

/*get class routine*/
/* return the number of points needed */
int
getclass(race)
	int race;
{
	short chk=FALSE;
	short tmp;
	short ypos=4;
	int i,j;
	
	mvaddstr(ypos,0,"The List of Possible Nation Classes:");
	ypos+=2;
	mvprintw(ypos++,0,"     %-8s %4s   %15s %8s %4s", "class", "who",
		"", "magic", "cost");
	mvprintw(ypos++,0,"     %-8s %4s   %15s %8s %4s", "--------", "----",
		"", "-------", "----");
	for(i=1;i<NUMCLASS;i++) {
		if (in_str(race,Classwho[i])==TRUE) {
			mvprintw(ypos++,0," %2d) %-8s %4s %15s", i, Class[i],
				Classwho[i], "...............");
			tmp = strlen(CPowlist[i]);
			for(j=0; j < 10-tmp ; j++) {
				addch('.');
			}
			printw(" %s", CPowlist[i]);
			if (i == C_WARLORD && race == HUMAN)
				printw(" %4d", Classcost[i]*2/3);
			else printw(" %4d", Classcost[i]);
		}
	}
	ypos++;
	while(chk==FALSE){
		mvaddstr(ypos,0,"Enter the number of your choice: ");
		clrtoeol();
		refresh();
		tmp = get_number();
		if (tmp < 1 || tmp > NUMCLASS) {
			newerror("Invalid Choice");
		} else if (in_str(race,Classwho[tmp])==TRUE) {
			chk = TRUE;
		} else {
			newerror("That Class is Invalid for your Race");
		}
	}
	curntn->class=tmp;
	for(tmp=ypos; tmp>4; tmp--) {
		move(tmp,0);
		clrtoeol();
	}
	
	return( doclass( curntn->class, TRUE ) );
}

int
doclass( tmp, isupd )
short	tmp;
int	isupd;	/* true if update, false if interactive */
{
	int cost;
	long x;

	/* determine number of leaders you want */
	if((tmp == C_TRADER) || (tmp <= C_WIZARD)) {
		numleaders = spent[CH_LEADERS] = 5;
	} else {
		numleaders = spent[CH_LEADERS] = 7;
	}

	/* assign the powers */
	x=Classpow[tmp];

	/* check for special case */
	if (tmp==C_WARLORD && curntn->race==HUMAN) {
		x ^= WARRIOR;
		cost = Classcost[tmp]*2/3;
	} else {
		cost = Classcost[tmp];
	}
	curntn->powers |= x;

	if ( isupd ) CHGMGK;
	return(cost);
}

int
nstartcst()	/* to be used for new method */
{
	float points=0.0;
	char temp[LINELTH];
	int i;

	/* calculate cost for all so far */
	for (i=0; i<CH_NUMBER; i++) {
		points += Mcost[i] * (float) spent[i] / Munits[i];
	}

	/* extra points for starting late */
	points -= (float) (TURN-1) / LATESTART;
	if( (float)(TURN-1)/LATESTART > 0.0 ) {
		sprintf(temp,"%.1f points added for starting late",
			   (float) (TURN-1) / LATESTART);
		newerror(temp);
	}
	points += 1.0;	/* round up */	
	return((int)points);
}

int
startcost()	/* cant be used for npc nations yet!!! see below */
{
	float	points;	/* points */

	points = ((float)curntn->tciv)/ONLPOP;
	points += ((float)curntn->tgold)/ONLGOLD;
	points += ((float)curntn->tmil)/ONLSOLD;
	if(curntn->race==ORC) {
		points += ((float)curntn->repro)*ONLREPCOST/(ONLREPRO_ORC);
		points += ((float)curntn->aplus*2)/ONLATTACK;
		points += ((float)curntn->dplus*2)/ONLDEFENCE;
	} else {
		points += ((float)curntn->aplus)/ONLATTACK;
		points += ((float)curntn->dplus)/ONLDEFENCE;
		points += ((float)curntn->repro)*ONLREPCOST/ONLREPRO;
	}
	points += ((float)curntn->maxmove)/ONLMOVE;
	if(curntn->location==FAIR)
		points += ONLLOCCOST;
	else if(curntn->location==GREAT)
		points += 2*ONLLOCCOST;
	/* points+=ONLDBLCOST*curntn->tfood/ONLHFOOD; */
	points -= (TURN-1) / LATESTART;	/* extra points if you start late */
	if( TURN > 1 )
	printf("point cost for nation %d is %.2f (bonus for latestart is %f)\n",country,points,(float) (TURN-1)/LATESTART);

	points += 1.0;	/* round up */
	return((int) points);
}
