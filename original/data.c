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
#include "header.h"
#include "data.h"

/* ele is display characters */
char *ele=       "~#^%-0";

/* Movement costs for varying types of terain by race - '/'=error */
char *HElecost=  "//521/";
char *OElecost=  "//222/";
char *EElecost=  "//631/";
char *DElecost=  "//311/";
char *FElecost=  "16211/";	/* flight costs */

/* BELOW ARE MOVEMENT COSTS FOR VEG */
char *veg=       "vdtblgwfjsi~0";
char *HVegcost=  "63210001332//";	/* Human */
char *OVegcost=  "43100022527//";	/* Orcs  */
char *EVegcost=  "86221000027//";	/* Elves */
char *DVegcost=  "47100013577//";	/* Dwarves */
char *FVegcost=  "410000001000/";	/* Flight */
char *vegfood=   "0004697400000";	/* food value given veg */

char *des=       "tcmfx$!&sC?lb+*g=u-P0";
#ifdef CONQUER
char *elename[]= {  "WATER","PEAK", "MOUNTAIN", "HILL", "FLAT","ERROR"};
char *vegname[]= { "VOLCANO", "DESERT", "TUNDRA", "BARREN", "LT VEG",
"GOOD", "WOOD", "FOREST", "JUNGLE", "SWAMP", "ICE", "NONE"};
char *desname[]= {"TOWN", "CITY", "MINE", "FARM", "DEVASTATED", "GOLDMINE",
"FORT","RUIN","STOCKADE","CAPITOL","SPECIAL","LUMBERYD","BLKSMITH","ROAD",
"MILL","GRANARY","CHURCH","UNIVERSITY","NODESIG","BASE CAMP","ERROR"};
char *races[]= { "GOD","ORC","ELF","DWARF","LIZARD",
"HUMAN","PIRATE","SAVAGE","NOMAD","UNKNOWN"};
char *diploname[]= { "UNMET", "TREATY", "ALLIED", "FRIENDLY",
"NEUTRAL", "HOSTILE", "WAR", "JIHAD"};
char *soldname[]= { "?","MARCH","SCOUT","GARRISON","TRADED","MILITIA",
	"FLYING","DEFEND","MAG_DEF","ATTACK","MAG_ATT","GENERAL","SORTIE",
	"SIEGE","BESIEGED","ON_BOARD","RULE"};
#endif /* CONQUER */

char *directions[]= { "here", "north", "northeast", "east", "southeast",
	"south", "southwest", "west", "northwest"};
char *Class[]= { "monster", "king", "emperor", "wizard", "priest", "pirate",
"trader", "warlord", "demon", "dragon", "shadow"};
char *alignment[]= { "Other","Good","Neutral","Evil","Other" };

char *unittype[]= {
"Militia", "Goblins", "Orcs", "Infantry", "Sailors", "Marines", "Archers",
"Uruk-Hai", "Ninjas", "Phalanx", "Olog-Hai", "Legionaries", "Dragoons", "Mercenaries",
"Trolls", "Elite", "Lt_Cavalry", "Hv_Cavalry", "Catapults", "Siege", "Rocs",
"Knights", "Gryfins","Elephants","Zombies","Spy","Scout",
"King","Baron","Emperor","Prince","Wizard","Mage",
"Pope","Bishop","Admiral","Captain","Warlord","Lord",
"Demon","Devil","Dragyn","Wyrm","Shadow","Nazgul",
"Spirit", "Assasin", "Efreet", "Gargoyl", "Wraith",
"Hero", "Centaur", "Giant", "Suphero", "Mummy",
"Elmentl", "Mintaur", "Daemon", "Balrog", "Dragon"
};

#ifdef CONQUER
/*short unit types...for printing in upper right corner*/
char *shunittype[]= {
"mlta", "Gob", "Orc", "Inf", "Sail", "XMar", "Arch",
"Uruk", "Ninj", "Phax", "olog", "Legn", "Drag", "Merc",
"Trol", "Elt", "lCav", "hCav", "cat", "sge", "Roc",
"Kni", "grif","ele","zom","Spy","Scout",
"King","Bar","Emp","Prin","Wizd","Magi",
"Apos","Bish","Admi","Capt","Warl","Lord",
"Demn","Devl","Drag","Wyrm","Shad","Nazg",
"spir", "Assn", "efr", "Garg", "Wra",
"Hero", "Cent", "gt", "Shro", "Mumm",
"Elem", "mino", "daem", "Bal", "Drgn"
};
#endif /* CONQUER */
int unitminsth[]=		/* min strength of a unit */
{
1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1,
100,50,100,50,250,50,
100,50,100,50,250,125,
250,50,500,100,250,125,
50, 50, 50, 75, 75,
75, 50, 150, 150, 150,
175, 150, 500, 500, 1000
};

int unitattack[]=
{ -40, -15,   0,   0,   0,  5,   0,
5,  20,  10,  15,  20,  10,  0,
25,  20,  20,  30, -20, -20,  20,
40,  40, 50, -15, -30, -30,
30,20,30,20,30,20,
30,20,30,20,30,30,
50,20,50,40,50,40,
0, 20, 10, 10, 10,
0, 10, 0, 15, 15,
5, 20, 50, 40, 50
};
int unitdefend[]=
{
-25, -15,   0,   0,   0,  0,  10,
5,   0,  10,  15,  20,  10,  0,
15,  20,  20,  30,  20,  20,  30,
40,  50, 50, -15, -30, -30,
30,20,30,20,30,20,
30,20,30,20,30,30,
50,20,50,40,50,40,
0, 20, 10, 10, 10,
0, 10, 0, 15, 15,
5, 20, 50, 40, 50
};
/*ten times the move rate of a unit*/
int unitmove[]=
{
 0, 10, 10, 10, 0, 0, 10,
10, 10, 10, 10, 10, 20, 10,
10, 13, 20, 20, 5, 5, 10,
20, 15, 5, 10, 10, 20,
20,20,20,20,20,20,
20,20,20,20,20,20,
20,20,20,20,20,20,
10, 10, 15, 10, 10,
10, 15, 10, 10, 10,
15, 10, 10, 15, 20
};
/*metal cost of a unit*/
int u_enmetal[]=
{
  0,  80,  80, 100, 100, 100, 100,
150, 150, 150, 150, 150, 100,   0,
200, 200, 100, 300,1000,1000, 300,
600, 400, 600, 100, 0, 0,
0,0,0,0,0,0,
0,0,0,0,0,0,
0,0,0,0,0,0,
0, 0, 0, 0, 0,
0, 0, 0, 0, 0,
0, 0, 0, 0, 0
};
/*enlist cost for a unit*/
int u_encost[]=
{
  50,  70, 85, 100, 100, 100, 100,	/* gold cost for enlistment */
 125, 125, 150, 180, 180, 300, 225,
 225, 225, 300, 450, 600, 600, 600,
 600, 800, 600, 100, 10000, 100,
0,0,0,0,0,0,
0,0,0,0,0,0,
0,0,0,0,0,0,
2, 2, 2, 2, 2,			/* spell points for enlistment */
2, 2, 5, 5, 5,
5, 5,10,10,15
};
/*maintenance cost for a unit*/
int unitmaint[]= {
 20,  20,  50,  50,  50,  50,  50,
 50,  50,  50,  75,  75, 200, 100,
100, 100, 175, 225, 250, 250, 250,
250, 250, 250, 0, 2000, 100,
0,0,0,0,0,0,		/* GOLD COST PER EQUIVALENT MAN FOR MONSTERS */
0,0,0,0,0,0,
0,0,0,0,0,0,
1000, 400, 400, 450, 450, 	/* JEWELS / TURN COST FOR ENTIRE UNIT */
200, 200, 2100, 450, 1000,	/* MULTIPLY BY 5 FOR GOLD COST FOR UNIT */
1900, 2100, 6000, 6000, 10000
};

char	*exefile =  "exec";
char	*datafile = "data";
char	*msgfile =  "mesg";
char *timefile = "timelog";
char	*sortname = "conqsort";

#ifdef ADMIN
char	scenario[NAMELTH+1];
char	*npcsfile = "nations";
#endif /* ADMIN */
char	*helpfile = "help";
char	*newsfile = "news";
char	*isonfile = "lock";
#ifdef TRADE
char	*tradefile = "commerce";
#endif /* TRADE */

char	*seasonstr[] = { "Winter", "Spring", "Summer", "Fall" };

char	*pwrname[] = {
"WARRIOR", "CAPTAIN", "WARLORD", "ARCHER", "CAVALRY", "SAPPER",
"ARMOR", "AVIAN", "MI_MONST", "AV_MONST", "MA_MONST",

"SLAVER", "DERVISH", "HIDDEN", "ARCHITECT", "RELIGION", "MINER",
"BREEDER", "URBAN", "STEEL", "NINJA", "SAILOR", "DEMOCRACY", "ROADS",

"THE_VOID", "KNOWALL", "DESTROYER", "VAMPIRE",
"SUMMON", "WYZARD", "SORCERER", "ERROR"
};

long	powers[] = {
	WARRIOR,	/*MILITARY POWERS => START=0*/
	CAPTAIN,	/*1*/
	WARLORD,	/*2*/
	ARCHER,		/*3*/
	CAVALRY,	/*4*/
	SAPPER,		/*5*/
	ARMOR,		/*6*/
	AVIAN,		/*7*/
	MI_MONST,	/*8*/
	AV_MONST,	/*9*/
	MA_MONST,	/*10th power; Mil power #11*/
	SLAVER,		/*CIVILIAN POWERS => START=11*/
	DERVISH,	/*12*/
	HIDDEN,		/*13*/
	ARCHITECT,	/*14*/
	RELIGION,	/*15*/
	MINER,		/*16*/
	BREEDER,	/*17*/
	URBAN,		/*18*/
	STEEL,		/*19*/
	NINJA,		/*20*/
	SAILOR,		/*21*/
	DEMOCRACY,	/*22*/
	ROADS,		/* 13th CIVILIAN POWER (23rd total) */
	THE_VOID,	/* MAGICAL SKILLS => START=24 */
	KNOWALL,	/*25*/
	DESTROYER,	/*26*/
	VAMPIRE,	/*27*/
	SUMMON,		/*28*/
	WYZARD,		/*29*/
	SORCERER,	/*30th Entry => 31 total powers => End=7*/
	0
};

FILE *fnews, *fopen();
struct sprd_sht spread;

FILE	*fm;
int	mailok=DONEMAIL;

#ifdef CONQUER
#ifdef SYSMAIL
char	sysmail[FILELTH];
int	sys_mail_status;
#endif /* SYSMAIL */
char conqmail[FILELTH];
int	conq_mail_status;
#endif /* CONQUER */

/* trade good value - zeroes in this comment line mark increments of ten*/
/*              0         0         0         0         0         0 */
/* a value of x means any designation is possible */
/*              fwbcwmhpgcfshfrwdpbcpstgponpmldpllpiopshmtpbcltismasspdsgridp*/
char *tg_stype="xffffttttffffffffffftxlxllttuuctcccfff?xtccmmmmmmmm$$$$$$$$$$0";
char *tg_value="13335157911433442331131135734567789123937571111111111111111110";

char	*tg_name[] = {
"furs", "wool", "beer", "cloth", "wine",
"mules", "horses", "pigeons", "griffons",
"corn", "fish", "sugar", "honey", "fruit",
"rice", "wheat", "dairy", "peas", "bread", "cereal",
"pottery", "salt", "timber", "granite", "pine", "oak", "nails",
"papyrus","math","library","drama","paper","literature","law","philosophy",
"irrigation", "oxen", "plows",
"stones",
"herbs", "medicine",
"torture", "prison",
"bronze", "copper", "lead", "tin", "iron","steel", "mithral", "adamantine",
"spice","silver","pearls","dye","silk","gold","rubys","ivory","diamonds","platinum",
"none"
};
