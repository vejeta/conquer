/* conquer : Copyright (c) 1988 by Ed Barlow.	*/

/*--------I DO NOT BELIEVE IT IS NECESSARY TO ALTER THIS FILE----------------*/
#define	FAIL	1		/* fail return to shell			*/
#define	SUCCESS	0		/* successful return to shell		*/
#define TIME_DEAD	3600	/* number of seconds for file aging	*/
#define	BIG	500000000L	/* BIGGER THAN ANYTHING SHOULD BE	*/
				/* this is used to protect against overflow */
#define	SCRARM	((LINES-14)/2)	/* number of armies to fit on screen	*/

/* sometimes curses.h defines TRUE	*/
#ifndef TRUE
#define	TRUE		1
#define	FALSE		0
#endif

/* definitions for mail sending */
#define	DONEMAIL	(-3)
#define	NEWSMAIL	(-2)
#define	ABORTMAIL	(-1)

/* definitions for screen redrawing */
#define	DONE	0
#define	PART	1
#define	FULL	2

#define	SCREEN_X_SIZE	(( COLS - 21) / 2)	/* divide by two as only 1/2 
						   sectors will be shown */
#define	SCREEN_Y_SIZE	( LINES - 5 )
#define	HAS_SEEN(x,y)	hasseen[(x)+((y)*((COLS-10)/2))]
#define	PASSLTH		7	/* the number of characters in the passwd*/
#define	NAMELTH		9	/* the number of characters in the name*/
#define	LEADERLTH	9	/* the number of characters in the leader*/
#define	FILELTH		80	/*length for filename holders*/
#define	LINELTH		80	/*length for input string lines*/
#define	BIGLTH		256	/*length for large storage strings*/
#define	NUMCLASS	11	/*number of nation classes */

/* environment variable strings to check */
#define	ENVIRON_OPTS	"CONQ_OPTS"

/*simple contour map definitions*/
#define	WATER		(*(ele+0))
#define	PEAK		(*(ele+1))
#define	MOUNTAIN	(*(ele+2))
#define	HILL		(*(ele+3))
#define	CLEAR		(*(ele+4))

/*racial types*/
#define	GOD		'-'
#define	ORC		'O'
#define	ELF		'E'
#define	DWARF		'D'
#define	LIZARD		'L'
#define	HUMAN		'H'
#define	PIRATE		'P'
#define	SAVAGE		'S'
#define	NOMAD		'N'
#define	TUNKNOWN	'?'

/*designations*/
#define	DTOWN		(*(des+0))
#define	DCITY		(*(des+1))
#define	DMINE		(*(des+2))
#define	DFARM		(*(des+3))
#define	DDEVASTATED	(*(des+4))
#define	DGOLDMINE	(*(des+5))
#define	DFORT		(*(des+6))
#define	DRUIN		(*(des+7))
#define	DSTOCKADE	(*(des+8))
#define	DCAPITOL	(*(des+9))
#define	DSPECIAL	(*(des+10))
#define	DLUMBERYD	(*(des+11))
#define	DBLKSMITH	(*(des+12))
#define	DROAD		(*(des+13))
#define	DMILL		(*(des+14))
#define	DGRANARY	(*(des+15))
#define	DCHURCH		(*(des+16))
#define	DUNIVERSITY	(*(des+17))
#define	DNODESIG	(*(des+18))
#define	DBASECAMP	(*(des+19))

/* nation placement variables	*/
#define	GREAT		'G'
#define	FAIR		'F'
#define	RANDOM		'R'
#define	OOPS		'X'

/*vegetation types -- these are legal in designations too*/
#define	VOLCANO		(*(veg+0))
#define	DESERT		(*(veg+1))
#define	TUNDRA		(*(veg+2))
#define	BARREN		(*(veg+3))
#define	LT_VEG		(*(veg+4))
#define	GOOD		(*(veg+5))
#define	WOOD		(*(veg+6))
#define	FOREST		(*(veg+7))
#define	JUNGLE		(*(veg+8))
#define	SWAMP		(*(veg+9))
#define	ICE		(*(veg+10))
#define	NONE		(*(veg+11))

/*Diplomacy Variables*/
#define	JIHAD		7
#define	WAR		6
#define	HOSTILE		5
#define	NEUTRAL		4
#define	FRIENDLY	3
#define	ALLIED		2
#define	TREATY		1
#define	UNMET		0

#define	BREAKJIHAD	200000L /* $ cost to break confederacy or jihad	*/

/*army status*/
#define	MARCH		1	/*March	*/
#define	SCOUT		2	/*Scouting--will not engage enemy if possible*/
#define	GARRISON	3	/*Garrison--for a town or Capitol	*/
#define	TRADED		4	/*Indicates an army that has been traded*/
#define	MILITIA		5	/*Unit is a militia unit */
#define	FLIGHT		6	/*Indicates army is flying*/
#define	DEFEND		7	/*Defend	*/
#define	MAGDEF		8	/*Magically enhanced defend mode*/
#define	ATTACK		9	/*Attack anybody (Hostile+) within 2 sectors*/
#define	MAGATT		10	/*Magically enhanced attack mode*/
#define	GENERAL		11	/* group leader !!!	*/
#define	SORTIE		12	/*Quick attack from a city */
#define	SIEGE		13	/*Set siege on a city */
#define	SIEGED		14	/*Unit under siege */
#define	ONBOARD		15	/*On board a fleet */
#define	RULE		16	/*Nation leader status for capitol */
#define	NUMSTATUS	17	/* number of possible stats	*/
				/* Army groups are implemented in the army
				status vbl.  if >= NUMSTATUS, you belong to
				army group x-NUMSTATUS - army groups always
				of status attack and may not be magicked */

/*seasonal definitions	*/
#define	TURN		world.turn	/* game turn	*/
#define	SEASON(x)	((x)%4)
#define	PSEASON(x)	seasonstr[((x)%4)]
#define	YEAR(x)		((int)((x+3)/4))
#define	WINTER		0
#define	SPRING		1
#define	SUMMER		2
#define	FALL		3

/* definitions for compass directions */
#define	CENTERED	0
#define	NORTH		1
#define	NORTHEAST	2
#define	EAST		3
#define	SOUTHEAST	4
#define	SOUTH		5
#define	SOUTHWEST	6
#define	WEST		7
#define	NORTHWEST	8

struct	s_world
{
	short	mapx,mapy;	/* size of world		*/
	short	nations;	/* number of actual nations	*/
	short	othrntns;	/* god, lizard...		*/
	short	turn;		/* game turn			*/
	long	m_mil;		/* number of mercs available	*/
	short	m_aplus;	/* mercenary attack bonus	*/
	short	m_dplus;	/* mercenary defense bonus	*/
	long	w_jewels;	/* jewels in world		*/
	long	w_gold;		/* gold talons in world		*/
	long	w_food;		/* food in world		*/
	long	w_metal;	/* metal in world		*/
	long	w_civ;		/* world population		*/
	long	w_mil;		/* world military		*/
	long	w_sctrs;	/* owned sectors in world	*/
	long	score;		/* world score total		*/
};

#define MAPX		world.mapx
#define MAPY		world.mapy
#define	MERCMEN		world.m_mil
#define	MERCATT		world.m_aplus
#define	MERCDEF		world.m_dplus
#define	WORLDJEWELS	world.w_jewels
#define	WORLDGOLD	world.w_gold
#define	WORLDMETAL	world.w_metal
#define	WORLDFOOD	world.w_food
#define	WORLDSCORE	world.score
#define	WORLDCIV	world.w_civ
#define	WORLDSCT	world.w_sctrs
#define	WORLDMIL	world.w_mil
#define	WORLDNTN	world.nations

struct s_sector
{
	unsigned char	designation;	/* designation of sector	*/
	unsigned char	altitude;	/* sector altitude		*/
	unsigned char	vegetation;	/* sector vegetation		*/
	unsigned char	owner;		/* nation id of owner		*/
	long	people;			/* civilians in sector		*/
	short	i_people;		/* initial civilians in sector	*/
	unsigned char	jewels;		/* jewel production ability	*/
	unsigned char	fortress;	/* fortification level; 0 to 12	*/
	unsigned char	metal;		/* metal produced by sector	*/
	unsigned char	tradegood;	/* exotic trade goods in sector	*/
	/* unsigned char	region;		/* index of region	*/
};
#define	SOWN	sct[xcurs+xoffset][ycurs+yoffset].owner
#define	XREAL	(xcurs+xoffset)
#define	YREAL	(ycurs+yoffset)

struct navy
{
	unsigned short warships;
	unsigned short merchant;
	unsigned short galleys;
	unsigned char xloc;
	unsigned char yloc;
	unsigned char smove;		/* movement ability of ship */
	unsigned char crew;		/* crew on ship */
	unsigned char people;		/* people carried */
	unsigned char commodity;	/* future commodities */
	unsigned char armynum;		/* army carried */
};


/* NATION STRATEGY: a nation's strategy is a part of the ntn[].active
 * variable which tells if it is a PC or NPC, its alignment, and its
 * constraints alignments can be good, neutral, or evil. 
 * NPC Nations can be Expansionsist (with 0,2,4,6 sectors to expand into)
 * or Isolationist.	*/
#define	INACTIVE	0
#define	PC_GOOD		1		/* PC NATIONS	*/
#define	PC_NEUTRAL	2
#define	PC_EVIL		3
#define	GOOD_0FREE	4		/* NPC NATIONS	*/
#define	GOOD_2FREE	5
#define	GOOD_4FREE	6
#define	GOOD_6FREE	7
#define	NEUTRAL_0FREE	8
#define	NEUTRAL_2FREE	9
#define	NEUTRAL_4FREE	10
#define	NEUTRAL_6FREE	11
#define	EVIL_0FREE	12
#define	EVIL_2FREE	13
#define	EVIL_4FREE	14
#define	EVIL_6FREE	15
#define	ISOLATIONIST	16		/* ISOLATIONIST NATIONS	*/
#define	NPC_PEASANT	17		/* PEASANT REVOLT TYPE NATIONS	*/
#define NPC_PIRATE	18
#define NPC_LIZARD	19
#define NPC_NOMAD	20
#define NPC_SAVAGE	21

#define	ispc(x)		(((x)==PC_GOOD)||((x)==PC_EVIL)||((x)==PC_NEUTRAL))
#define	npctype(x)	(ispc(x) ? (x) : (ismonst(x)?0:(x)/4))
#define	isgood(x)	(npctype(x) == 1)
#define	isneutral(x)	(npctype(x) == 2)
#define	isevil(x)	(npctype(x) == 3)
#define	isnpc(x)	(((x)>=GOOD_0FREE)&&((x)<=ISOLATIONIST))
#define	isnotpc(x)	(((x)>=GOOD_0FREE)&&((x)!=INACTIVE))
#define	ismonst(x)	((x)>=NPC_PEASANT)
#define	ispeasant(x)	((x)==NPC_PEASANT)
#define	isntn(x)	(((x)!=INACTIVE)&&((x)<=ISOLATIONIST))
#define	isntnorp(x)	(((x)!=INACTIVE)&&((x)<=NPC_PEASANT))
#define	isactive(x)	((x)!=INACTIVE)

/* definitions for mail checker */
#define	NO_MAIL		0
#define	NEW_MAIL	1

/* definitions for naval types	*/
#define	N_LIGHT		0
#define	N_MEDIUM	1
#define	N_HEAVY		2
#define	N_BITSIZE	5
#define	N_MASK		((unsigned short) 0x001f)

/* definitions for naval calculations */
#define	N_WSPD		20	/* speed of warships */
#define	N_GSPD		18	/* speed of galleys */
#define	N_MSPD		15	/* speed of merchants */
#define	N_NOSPD		0	/* no ships no speed */
#define	N_SIZESPD	3	/* bonus speed for lighter ships */

#define	UTYPE		75
#define	TWOUTYPE	150	/* two times value of UTYPE above	*/

#define	A_MILITIA	0
#define	A_GOBLIN	1
#define	A_ORC		2
#define	A_INFANTRY	3
#define	A_SAILOR	4
#define	A_MARINES	5
#define	A_ARCHER	6
#define	A_URUK		7
#define	A_NINJA		8
#define	A_PHALANX	9
#define	A_OLOG		10
#define	A_LEGION	11
#define	A_DRAGOON	12
#define	A_MERCENARY	13
#define	A_TROLL		14
#define	A_ELITE		15
#define	A_LT_CAV	16
#define	A_CAVALRY	17
#define	A_CATAPULT	18
#define	A_SIEGE		19
#define	A_ROC		20
#define	A_KNIGHT	21
#define	A_GRIFFON	22
#define	A_ELEPHANT	23
#define	A_ZOMBIE	24
#define A_SPY		25
#define A_SCOUT		26
#define	NOUNITTYPES	26	/*number of unit types*/

#define	MINLEADER	(27+UTYPE)	/* min value of a leader -1*/
#define	L_KING		(27+UTYPE)
#define	L_BARON		(28+UTYPE)
#define	L_EMPEROR	(29+UTYPE)
#define	L_PRINCE	(30+UTYPE)
#define	L_WIZARD	(31+UTYPE)
#define	L_MAGI		(32+UTYPE)
#define	L_APOSTLE	(33+UTYPE)
#define	L_BISHOP	(34+UTYPE)
#define	L_ADMIRAL	(35+UTYPE)
#define	L_CAPTAIN	(36+UTYPE)
#define	L_WARLORD	(37+UTYPE)
#define	L_LORD		(38+UTYPE)
#define	L_DEMON		(39+UTYPE)
#define	L_DEVIL		(40+UTYPE)
#define	L_DRAGON	(41+UTYPE)
#define	L_WYRM		(42+UTYPE)
#define	L_SHADOW	(43+UTYPE)
#define	L_NAZGUL	(44+UTYPE)

#define	MINMONSTER	(45+TWOUTYPE)	/* min value of a monster	*/
#define	SPIRIT		(45+TWOUTYPE)
#define	ASSASSIN	(46+TWOUTYPE)
#define	DJINNI		(47+TWOUTYPE)
#define	GARGOYLE	(48+TWOUTYPE)
#define	WRAITH		(49+TWOUTYPE)
#define	HERO		(50+TWOUTYPE)
#define	CENTAUR		(51+TWOUTYPE)
#define	GIANT		(52+TWOUTYPE)
#define	SUPERHERO	(53+TWOUTYPE)
#define	MUMMY		(54+TWOUTYPE)
#define	ELEMENTAL	(55+TWOUTYPE)
#define	MINOTAUR	(56+TWOUTYPE)
#define	DEMON		(57+TWOUTYPE)
#define	BALROG		(58+TWOUTYPE)
#define	DRAGON		(59+TWOUTYPE)
#define	MAXMONSTER	(59+TWOUTYPE)

struct army
{
	unsigned char unittyp;
	unsigned char xloc;
	unsigned char yloc;
	unsigned char smove;
	long sold;
	unsigned char stat;
};

struct	s_nation		/* player nation stats	*/
{
	char	name[NAMELTH+1];	/* name			*/
	char	passwd[PASSLTH+1];	/* password		*/
	char	leader[LEADERLTH+1];	/* leader title		*/
	char	race;		/* national race (integer--see header.h)*/
	char	location;	/* location variable (gfr)	*/
	char	mark;		/* unique mark for nation	*/
#ifdef CHECKUSER
	short int uid;		/* for uid checking (CHECKUSER) */
#endif /* CHECKUSER */
	unsigned char	capx;	/* Capitol x coordinate		*/
	unsigned char	capy;	/* Capitol y coordinate		*/
	unsigned char	active;	/* nation type and strategy	*/
	unsigned char	maxmove;/* maximum movement of soldiers */
	char	repro;		/* reproduction rate of nation	*/
	long	score;		/* score			*/
	long	tgold;		/* gold in treasury		*/
	long	jewels;		/* raw amount of jewels in treasury	*/
	long	tmil;		/* total military		*/
	long	tciv;		/* total civilians		*/
	long	metals;		/* total real metal in nation	*/
	long	tfood;		/* total food in nation		*/
	long	powers;
	short	class;		/* national class		*/
	short	aplus;		/* attack plus of all soldiers	*/
	short	dplus;		/* attack plus of all soldiers	*/
	short	spellpts;	/* spell points			*/
	short	tsctrs;		/* total number sectors		*/
	short	tships;		/* number warships		*/
	short	inflation;	/* inflation rate		*/
	unsigned char charity;	/* charity budget (% of Taxes)	*/
	struct	army arm[MAXARM];
	struct	navy nvy[MAXNAVY];
	char	dstatus[NTOTAL];	/* diplomatic status	*/
	unsigned char	tax_rate;	/* taxrate populace	*/
	unsigned char	prestige;	/* nations prestige	*/
	unsigned char	popularity;	/* governments popularity	*/
	unsigned char	power;		/* nation power		*/
	unsigned char	communications;	/* leader communication	*/
	unsigned char	wealth;		/* per capita income	*/
	unsigned char	eatrate;	/* food eaten / 10 people*/
	unsigned char	spoilrate;	/* food spoilage rate	*/
	unsigned char	knowledge;	/* general knowledge	*/
	unsigned char	farm_ability;	/* farming ability	*/
	unsigned char	mine_ability;	/* mine ability		*/
	unsigned char	poverty;	/* % poor people	*/
	unsigned char	terror;		/* peoples terror of you*/
	unsigned char	reputation;	/* reputation of nation	*/
};
#define	P_NTNCOM	((((float) curntn->communications)/ 50.0 ))
#define	P_EATRATE	((((float) curntn->eatrate) / 25.0 ))
#define	P_REPRORATE	( ((float) (100.0 + 25.0*curntn->repro )/100.0))

#define	P_ATYPE	curntn->arm[armynum].unittyp
#define	P_AXLOC	curntn->arm[armynum].xloc
#define	P_AYLOC	curntn->arm[armynum].yloc
#define	P_AMOVE	curntn->arm[armynum].smove
#define	P_ASOLD	curntn->arm[armynum].sold
#define	P_ASTAT	curntn->arm[armynum].stat
#define	P_NWSHP	curntn->nvy[nvynum].warships
#define	P_NMSHP	curntn->nvy[nvynum].merchant
#define P_NGSHP	curntn->nvy[nvynum].galleys
#define	P_NCREW	curntn->nvy[nvynum].crew
#define	P_NARMY	curntn->nvy[nvynum].armynum
#define	P_NPEOP	curntn->nvy[nvynum].people
#define	P_NXLOC	curntn->nvy[nvynum].xloc
#define	P_NYLOC	curntn->nvy[nvynum].yloc
#define	P_NMOVE	curntn->nvy[nvynum].smove

/* naval routine macros */
#define	SHIPS(x,y)	(short)( ((x)&(N_MASK<<((y)*N_BITSIZE))) >> ((y)*N_BITSIZE) )
#define	P_NWAR(x)	SHIPS(P_NWSHP,x)
#define	P_NMER(x)	SHIPS(P_NMSHP,x)
#define	P_NGAL(x)	SHIPS(P_NGSHP,x)
#define	NADD_WAR(x)	addwships(nvynum,shipsize,(x));
#define	NADD_MER(x)	addmships(nvynum,shipsize,(x));
#define	NADD_GAL(x)	addgships(nvynum,shipsize,(x));
#define	NSUB_WAR(x)	subwships(nvynum,shipsize,(x));
#define	NSUB_MER(x)	submships(nvynum,shipsize,(x));
#define	NSUB_GAL(x)	subgships(nvynum,shipsize,(x));

#define	ATYPE	ntn[country].arm[armynum].unittyp
#define	AXLOC	ntn[country].arm[armynum].xloc
#define	AYLOC	ntn[country].arm[armynum].yloc
#define	AMOVE	ntn[country].arm[armynum].smove
#define	ASOLD	ntn[country].arm[armynum].sold
#define	ASTAT	ntn[country].arm[armynum].stat
#define	NWSHP	ntn[country].nvy[nvynum].warships
#define	NMSHP	ntn[country].nvy[nvynum].merchant
#define	NGSHP	ntn[country].nvy[nvynum].galleys
#define	NCREW	ntn[country].nvy[nvynum].crew
#define	NARMY	ntn[country].nvy[nvynum].armynum
#define	NPEOP	ntn[country].nvy[nvynum].people
#define	NXLOC	ntn[country].nvy[nvynum].xloc
#define	NYLOC	ntn[country].nvy[nvynum].yloc
#define	NMOVE	ntn[country].nvy[nvynum].smove

/* these defines are for types of magic	powers	*/
#define	M_MIL		1	/* military	*/
#define	M_CIV		2	/* civilian	*/
#define	M_MGK		3	/* magical	*/
#define M_TECH		4	/* technological	*/
#define	M_ALL		5

#define	AORN		0
#define	ARMY		1
#define	NAVY		2

#define	XASTAT		1
#define	XAMEN		2
#define	XBRIBE		3
#define	XALOC		4
#define	XNLOC		5
#define	XNAMER		6
#define	XNACREW		7
#define	XECNAME		8
#define	XECPAS		9
#define	EDSPL		10
#define	XSADES		11
#define	XSACIV		12
#define	XSIFORT		13
#define	XNAGOLD		14
#define	XAMOV		15
#define	XNMOV		16
#define	XSAOWN		17
#define	EDADJ		18
#define	XNARGOLD	19
#define	XNAMETAL	20
#define	INCAPLUS	22
#define	INCDPLUS	23
#define	CHG_MGK		24
#define	DESTRY		25
#define	MSETA		26
#define	MSETB		27
#define NTAX		28
#define XNAWAR		29
#define XNAGAL		30
#define XNAHOLD		31
#define NPOP		32
#define XSACIV3	33

#define	BRIBENATION fprintf(fm,"L_NGOLD\t%d\t%d\t%ld\t0\t%d\t%s\n",XBRIBE,country,bribecost,nation,"null");
#define	DESTROY fprintf(fexe,"DESTROY\t%d\t%d\t%hd\t0\t0\t%s\n",DESTRY,save,country,"null")
#define	CHGMGK fprintf(fexe,"L_MAGIC\t%d\t%hd\t%ld\t%ld\t0\t%s\n",CHG_MGK,country,ntn[country].powers,x,"null")
#define	I_APLUS	fprintf(fexe,"INC_APLUS\t%d\t%hd\t0\t0\t0\t%s\n",INCAPLUS,country,"null")
#define	I_DPLUS	fprintf(fexe,"INC_DPLUS\t%d\t%hd\t0\t0\t0\t%s\n",INCDPLUS,country,"null")
#define	AADJMERC fprintf(fexe,"A_MERC\t%d\t%d\t%ld\t0\t0\t%s\n",MSETA,country,men,"null")
#define	AADJDISB fprintf(fexe,"A_MERC\t%d\t%d\t%ld\t%d\t%d\t%s\n",MSETB,country,bemerc,attset,defset,"null")
#define	AADJSTAT fprintf(fexe,"A_STAT\t%d\t%hd\t%d\t%d\t0\t%s\n",XASTAT,country,armynum,ntn[country].arm[armynum].stat,"null")
#define	AADJMEN	fprintf(fexe,"L_ADJMEN\t%d\t%hd\t%hd\t%ld\t%d\t%s\n",XAMEN,country,armynum,P_ASOLD,P_ATYPE,"null")
#define	NADJCRW	fprintf(fexe,"N_ASHP\t%d\t%hd\t%hd\t%d\t%d\t%s\n",XNACREW,country,nvynum,NCREW,ntn[country].nvy[nvynum].armynum,"null")
#define	NADJMER	fprintf(fexe,"N_ASHP\t%d\t%hd\t%d\t%hd\t%d\t%s\n",XNAMER,country,nvynum,ntn[country].nvy[nvynum].merchant,0,"null")
#define	NADJWAR	fprintf(fexe,"N_ASHP\t%d\t%hd\t%d\t%hd\t%d\t%s\n",XNAWAR,country,nvynum,ntn[country].nvy[nvynum].warships,0,"null")
#define	NADJGAL	fprintf(fexe,"N_ASHP\t%d\t%hd\t%d\t%hd\t%d\t%s\n",XNAGAL,country,nvynum,ntn[country].nvy[nvynum].galleys,0,"null")
#define	NADJHLD	fprintf(fexe,"N_ASHP\t%d\t%hd\t%d\t%hd\t%d\t%s\n",XNAHOLD,country,nvynum,ntn[country].nvy[nvynum].armynum,ntn[country].nvy[nvynum].people,"null")
#define	AADJLOC	fprintf(fexe,"A_LOC\t%d\t%hd\t%d\t%d\t%d\t%s\n",XALOC,country,armynum,ntn[country].arm[armynum].xloc,ntn[country].arm[armynum].yloc,"null")
#define	NADJLOC	fprintf(fexe,"N_LOC\t%d\t%hd\t%d\t%d\t%d\t%s\n",XNLOC,country,nvynum,ntn[country].nvy[nvynum].xloc,ntn[country].nvy[nvynum].yloc,"null")
#define	AADJMOV	fprintf(fexe,"A_MOV\t%d\t%hd\t%d\t%d\t0\t%s\n",XAMOV,country,armynum,ntn[country].arm[armynum].smove,"null")
#define	NADJMOV	fprintf(fexe,"N_MOV\t%d\t%hd\t%d\t%d\t0\t%s\n",XNMOV,country,nvynum,ntn[country].nvy[nvynum].smove,"null")
#define	ECHGNAME fprintf(fexe,"E_CNAME\t%d\t%hd\t0\t0\t0\t%s\n",XECNAME,country,ntn[country].name)
#define	ECHGPAS	fprintf(fexe,"E_CPAS\t%d\t%hd\t0\t0\t0\t%s\n",XECPAS,country,curntn->passwd)
#define	SADJDES	fprintf(fexe,"S_ADES\t%d\t%hd\t0\t%d\t%d\t%c\n",XSADES,country,xcurs+xoffset,ycurs+yoffset,sct[xcurs+xoffset][ycurs+yoffset].designation)
#define	SADJDES2	fprintf(fexe,"S_ADES\t%d\t%hd\t0\t%d\t%d\t%c\n",XSADES,country,x,y,sct[x][y].designation)
#define	SADJCIV2 fprintf(fexe,"S_ACIV\t%d\t%hd\t%ld\t%d\t%d\t%s\n",XSACIV,country,sct[i][j].people,i,j,"null")
#define	SADJCIV	fprintf(fexe,"S_ACIV\t%d\t%hd\t%ld\t%d\t%d\t%s\n",XSACIV,country,sct[xcurs+xoffset][ycurs+yoffset].people,xcurs+xoffset,ycurs+yoffset,"null")
#define	SADJCIV3 fprintf(fexe,"S_ACIV3\t%d\t%hd\t%ld\t%d\t%d\t%s\n",XSACIV3,country,people_to_add,i,j,"null")
#define	INCFORT fprintf(fexe,"SIFORT\t%d\t%hd\t0\t%d\t%d\t%s\n",XSIFORT,country,xcurs+xoffset,ycurs+yoffset,"null")
#define	SADJOWN	fprintf(fexe,"S_AOWN\t%d\t%hd\t0\t%d\t%d\t%s\n",XSAOWN,country,xcurs+xoffset,ycurs+yoffset,"null")
#define	EADJDIP(a,b)	fprintf(fexe,"E_ADJ\t%d\t%hd\t%d\t%d\t0\t%s\n",EDADJ,a,b,ntn[a].dstatus[b],"null")
#define	EDECSPL	fprintf(fexe,"E_SPL\t%d\t%hd\t%d\t%d\t0\t%s\n",EDSPL,country,s_cost,0,"null")
#define	NADJNTN  fprintf(fexe,"N_TAX \t%d \t%hd \t%d \t%d \t%d \tnull\n",NTAX,country,(int) curntn->tax_rate,(int) curntn->active, (int) curntn->charity)
#define	NADJNTN2  fprintf(fexe,"N_POP \t%d \t%hd \t%d \t%d \t%d \tnull\n",NPOP,country,(int) curntn->popularity,(int) curntn->terror, (int) curntn->reputation)

#define	S_MIL		0		/* position in powers array	*/
#define	WARRIOR		0x00000001L
#define	CAPTAIN		0x00000002L
#define	WARLORD		0x00000004L
#define	ARCHER		0x00000008L
#define	CAVALRY		0x00000010L
#define	SAPPER		0x00000020L
#define	ARMOR		0x00000040L
#define	AVIAN		0x00000080L
#define	MI_MONST	0x00000100L
#define	AV_MONST	0x00000200L
#define	MA_MONST	0x00000400L
#define	E_MIL		11
		/*CIVILIAN POWERS	*/
#define	S_CIV		11
#define	SLAVER		0x00000800L
#define	DERVISH		0x00001000L
#define	HIDDEN		0x00002000L
#define	ARCHITECT	0x00004000L
#define	RELIGION	0x00008000L
#define	MINER		0x00010000L
#define	BREEDER		0x00020000L
#define	URBAN		0x00040000L
#define	STEEL		0x00080000L
#define	NINJA		0x00100000L
#define	SAILOR		0x00200000L
#define	DEMOCRACY	0x00400000L
#define	ROADS		0x00800000L
#define	E_CIV		13
			/* MAGICAL SKILLS	*/
#define	S_MGK		24
#define	THE_VOID	0x01000000L
#define	KNOWALL		0x02000000L
#define	DESTROYER	0x04000000L
#define	VAMPIRE		0x08000000L
#define	SUMMON		0x10000000L
#define	WYZARD		0x20000000L
#define	SORCERER	0x40000000L
#define	E_MGK		7
#define	MAXPOWER	31 /* number of powers	*/

/* magic macro: returns TRUE if the nation has that power*/
#define	magic(NATION,POWER)	((ntn[NATION].powers&(POWER))!=0)
#define	ONMAP(x,y)	((x)>=0 && (y)>=0 && (x)<MAPX && (y)<MAPY)

#ifndef HILIGHT
#define	standout()
#endif /* HILIGHT */

#ifdef BEEP
#define	beep()		putc('\007',stderr)
#else
#define	beep()
#endif

#ifdef SYSV
extern	long		lrand48();
#define	rand()		lrand48()
#define	srand(x)	srand48(x)
#endif

#ifdef	BSD
#define	rand()		random()
#define	srand(x)	srandom(x)
#endif

#ifndef DEBUG
#define	check()	;
#else
#define	check()	checkout(__FILE__,__LINE__)
#endif /* DEBUG */

#define	GOLDTHRESH	10L	/* min ratio of gold:jewels */

#define	SALT "aa"		/* seed for crypt() encryption	*/

/* extern all subroutine calls	*/
extern long	getmagic(), getmagic(), getmgkcost(), score_one();
extern long	get_number(), solds_in_sector(),defaultunit();

extern int	move_file(), land_2reachp(), land_reachp(), canbeseen();
extern int	water_reachp(), markok(), is_habitable(), parse();
extern int	units_in_sector(), num_powers(), tofood(), mailopen();
extern int	get_god(), flightcost(), todigit(), getclass(), startcost();
extern int	water_2reachp(),tg_ok(), readmap(), avian();
extern int	cbonus(), armymove(),takeover(),getnewname();
extern int	getleader(),execute(),peasant_revolt(),other_revolt();
extern int	aretheyon(),armygoto(),navygoto(),getselunit();
extern int	unitvalid(),access(),orctake(),fort_val();
extern int	addgships(),addmships(),addwships(),fltships();
extern int	fltghold(),fltwhold(),fltmhold(),flthold(),compass();
extern int	get_country(),check_lock(),doclass(),get_pass();
extern unsigned short	fltspeed();
extern void	do_pirate(), do_nomad(), do_savage(), do_lizard();
extern void	getjewel(),getmetal(),loadfleet(),removemgk(),exenewmgk();
extern struct	s_sector *rand_sector();
extern void	subgships(),submships(),subwships(),getspace(),sackem();
extern void	sleep(), whatcansee(), reset_god(), get_nname(), camp_info();
extern void	main(), makebottom(), makeside(), check_mail(), centermap();
extern void	checkout(),copyscreen(),bye(),credits(),init_hasseen();
extern void	combinearmies(),change_status(),reducearmy(),splitarmy();
extern void	errormsg(), clear_bottom(), addgroup(),ext_cmd();
extern void	randomevent(), wdisaster(), weather(), deplete();
extern void	verify_ntn(), verify_sct(), verifydata(), prep();
extern void	errorbar(), newbye(), newreset(), newmsg(), newerror();
extern void	newinit(), jump_to();
extern void	destroy(), updmove(), spreadsheet(), mailclose();
extern void	updexecs(), updcapture(), updsectors();
extern void	updmil(), updcomodities(), updleader();
extern void	nationrun(), n_atpeace(), n_trespass(), n_people();
extern void	n_toofar(), n_unowned(), pceattr(), checkout();
extern void	fdxyretreat(), retreat(), rawmaterials(), createworld();
extern void	att_setup(), att_base(), att_bonus();
extern void	adjarm(),armyrpt(),atkattr(),blowup();
extern void	budget(),change(),cheat(),coffmap(),combat(),construct();
extern void	defattr(),diploscrn(),domagic(),draft(),erupt();
extern void	fight();
extern void	fill_edge(),flee(),fleetrpt(),hangup(),help();
extern void	highlight(),makemap(),makeside();
extern void	makeworld(),monster(),moveciv();
extern void	mymove(),navalcbt(),newdip(),newdisplay(),newlogin();
extern void	newspaper(),npcredes(),offmap(),place(),populate();
extern void	printele(),printnat(),printscore(),printveg();
extern void	pr_ntns(),pr_desg(),produce();
extern void	readdata(),redesignate(),redomil(),reduce(),rmessage(),score();
extern void	see(),showscore(),update();
extern void	wmessage(),writedata(),getdstatus(),exit();
extern void	wizardry();
extern	char	*crypt(),**m2alloc();
#ifdef TRADE
void trade(),uptrade(),checktrade();
#endif /* TRADE */

#define	HI_OWN		0	/* hilight modes	*/
#define	HI_ARMY		1
#define	HI_NONE		2
#define	HI_YARM		3
#define	HI_MOVE		4
#define HI_GOOD		5

#define	DI_VEGE		1	/* display modes	*/
#define	DI_DESI		2
#define	DI_CONT		3
#define	DI_FOOD		4
#define	DI_NATI		5
#define	DI_RACE		6
#define	DI_MOVE		7
#define	DI_DEFE		8
#define	DI_PEOP		9
#define	DI_GOLD		10
#define	DI_METAL	11
#define	DI_ITEMS	12

extern	struct sprd_sht
{
	long food,gold,jewels,metal; 		/* total @ end of turn	*/
	long revfood,revjewels,revmetal,
	     revcap,revcity,revothr; 		/* revenue in turn	*/
	long ingold,inmetal,infarm,incity,incap,inothr; 
						/* civilians in area	*/
	long civilians;				/* total civilians	*/
	int sectors;
} spread;

#ifndef max
#define	max(x,y)	((x)>(y) ? (x) : (y))
#define	min(x,y)	((x)<(y) ? (x) : (y))
#endif

/* attractiveness of sector to general populace	*/
#define	GOLDATTR	9	/* per jewel value of sector */
#define	FARMATTR	7	/* per point of food producable */
#define	MINEATTR	9	/* per metal value of sector */
#define	TOWNATTR	150
#define	CITYATTR 	300
#define TGATTR		10	/* bonus per point of value for tradegoods */
#define OTHRATTR	50	/* attractiveness of other sector types */


#define	DMNTNATTR	40	/* DWARF ATTRACTIVENESS	*/
#define	DHILLATTR	20
#define	DCLERATTR	0
#define	DCITYATTR	-20
#define	DTOWNATTR	-20
#define	DGOLDATTR	40
#define	DMINEATTR	40
#define	DFOREATTR	-20
#define	DWOODATTR	-10

#define	EMNTNATTR	-40	/* ELF	*/
#define	EHILLATTR	-20
#define	ECLERATTR	0
#define	ECITYATTR	-50
#define	ETOWNATTR	-50
#define	EGOLDATTR	0
#define	EMINEATTR	0
#define	EFOREATTR	40
#define	EWOODATTR	40

#define	OMNTNATTR	30	/* ORC	*/
#define	OHILLATTR	20
#define	OCLERATTR	0
#define	OCITYATTR	50
#define	OTOWNATTR	25
#define	OGOLDATTR	20
#define	OMINEATTR	20
#define	OFOREATTR	-40
#define	OWOODATTR	-20

#define	HMNTNATTR	-10	/* HUMAN	*/
#define	HHILLATTR	0
#define	HCLERATTR	30
#define	HCITYATTR	50
#define	HTOWNATTR	40
#define	HGOLDATTR	10
#define	HMINEATTR	10
#define	HFOREATTR	-20
#define	HWOODATTR	0

/* EXOTIC TRADE GOODS	*/
#define	TG_furs		0
#define	TG_wool		1
#define	TG_beer		2
#define	TG_cloth	3
#define	TG_wine		4
#define	END_POPULARITY	4

#define	TG_mules	5
#define	TG_horses	6
#define	TG_pigeons	7
#define	TG_griffons	8
#define	END_COMMUNICATION	8

#define	TG_corn		9
#define	TG_fish		10
#define	TG_sugar	11
#define	TG_honey	12
#define	TG_fruit	13
#define	TG_rice		14
#define	TG_wheat	15
#define	TG_dairy	16
#define	TG_peas		17
#define	TG_bread	18
#define	TG_cereal	19
#define	END_EATRATE	19

#define	TG_pottery	20
#define	TG_salt		21
#define	TG_timber	22
#define	TG_granite	23
#define	TG_pine		24
#define	TG_oak		25
#define	TG_nails	26
#define	END_SPOILRATE	26	/* also Knowledge start */

#define	TG_papyrus	27
#define	TG_math		28
#define	TG_library	29
#define	TG_drama	30
#define	TG_paper	31
#define	TG_literature	32
#define	TG_law		33
#define	TG_philosophy	34
#define	END_KNOWLEDGE	34

#define	TG_irregation	35
#define	TG_oxen		36
#define	TG_plows	37
#define	END_FARM	37	

#define	TG_stones	38
#define	END_SPELL	38

#define	TG_herbs	39
#define	TG_medecine	40
#define	END_HEALTH	40

#define	TG_torture	41
#define	TG_prison	42
#define	END_TERROR	42	
#define	END_NORMAL	42	/* also end of non mine/jewel goods */

#define	TG_bronze	43
#define	TG_copper	44
#define	TG_lead		45
#define	TG_tin		46
#define	TG_iron		47
#define	TG_steel	48
#define	TG_mithral	49
#define	TG_adamantine	50
#define	END_MINE	50

#define	TG_spice	51
#define	TG_silver	52
#define	TG_pearls	53
#define	TG_dye		54
#define	TG_silk		55
#define	TG_gold		56
#define	TG_rubys	57
#define	TG_ivory	58
#define	TG_diamonds	59
#define	TG_platinum	60
#define	END_WEALTH	60
#define	TG_none		61	/* no trade goods in sector	*/
#define MAXTGVAL	100	/* maximum value for stat */

/* defines for a nations class	*/
#define	C_NPC		0
#define	C_KING		1
#define	C_EMPEROR	2
#define	C_WIZARD	3
#define	C_PRIEST	4
#define	C_PIRATE	5
#define	C_TRADER	6
#define	C_WARLORD	7
#define	C_DEMON		8
#define	C_DRAGON	9
#define	C_SHADOW	10
#define	C_END		10

#define	ISCITY(desig)	((desig==DCITY)||(desig==DCAPITOL)||(desig==DFORT)||(desig==DTOWN))

#define	PWR_NA	10	/* national attributes gained from power	*/
#define	CLA_NA	30	/* national attributes gained from class	*/

/* MAGIC/CIVILIAN/MILITARY POWER COSTS BY RACE	*/
#define	BASEMAGIC	50000L	/* default for all not mentioned	*/
#define	DWFMAGIC	80000L
#define	HUMMAGIC	100000L
#define	ORCMAGIC	150000L
#define	DWFCIVIL	40000L
#define	ORCCIVIL	75000L
#define	HUMCIVIL	25000L
#define	DWFMILIT	40000L
#define	ORCMILIT	45000L

#define	abrt() { \
fprintf(stderr,"\nSerious Error (File %s, Line %d) - Aborting\n",__FILE__,__LINE__); \
abort(); \
}

#define	DEVASTATE(X,Y) { \
if(is_habitable(X,Y)) { \
if((sct[X][Y].designation == DCAPITOL) \
||(sct[X][Y].designation == DRUIN) \
||(sct[X][Y].designation == DCITY)) { \
	if(sct[X][Y].fortress >= 4) { \
		sct[X][Y].fortress -= 4; \
	} else sct[X][Y].fortress = 0; \
	sct[X][Y].designation = DRUIN; \
} else sct[X][Y].designation = DDEVASTATED; \
} \
}

extern	FILE	*fm;
extern	int	mailok;
extern	char	*seasonstr[];
extern	char	*alignment[];	
extern	struct	s_sector	**sct;
extern	struct	s_nation	ntn[NTOTAL];
extern	struct	s_nation	*curntn;	
extern	struct	s_world		world;

extern	short	**movecost;
extern	char	**occ;		/* sector occupied?, NTOTAL+1 if contested */
extern	char	*ele, *elename[], *veg,	*vegfood, *vegname[];
extern	char	*Class[],*races[],*diploname[];
extern	char	*soldname[], *unittype[], *shunittype[], *directions[];
extern	int	unitminsth[], u_enmetal[], u_encost[], unitmaint[];
extern	char	*des, *desname[], *pwrname[];
extern	long	powers[];
extern	char	*tg_value, *tg_name[], *tg_stype;	/* trade goods	*/

#ifdef CONQUER
extern	long	mercgot;
#endif /* CONQUER */

#ifdef ADMIN
extern	char	*npcsfile;
extern	char	scenario[];
#endif /* ADMIN */
extern	int	unitmove[], unitattack[], unitdefend[];

/*	file name definitions	*/
extern	char *sortname,*exefile,*datafile,*msgfile;
extern	char *helpfile,*newsfile,*isonfile,*timefile;

#ifdef CONQUER
extern	int	conq_mail_status;
extern	char	conqmail[];
#ifdef SYSMAIL
extern	int	sys_mail_status;
extern	char	sysmail[];
#endif /* SYSMAIL */
#endif /* CONQUER */
#ifdef TRADE
extern	char	*tradefile;
#endif /* TRADE */

#define EXT_CMD '\033'		/* ESC to start extended command */

#define MAXX		(MAPX / 8)	/* Number of X areas on map */
#define MAXY		(MAPY / 8)	/* Number of Y areas on map */
#define NUMAREAS	(MAXX * MAXY)	/* total # areas, MAXX*MAXY */
#define NUMSECTS	(MAPX * MAPY)	/* total # areas, MAXX*MAXY */
#define MAXHELP		6

#ifdef HPUX
#define SYSV
#endif /* HPUX */

/* minor market items */
#define GETFOOD		97		/* response needed to get food */
#define GETMETAL	98
#define GETJEWL		99
#define GODFOOD		8000L		/* food recieved for GODPRICE */
#define GODMETAL	2000L		/* " */
#define GODJEWL		3000L		/* " */
#define GODPRICE	25000L

#ifdef XYZ
/* THE FOLLOWING DEFINES ARE NOT IMPLEMENTED YET		*/
/* THEY DEFINE THE ATTRIBUTES OF A (TO BE IMPLEMENTED) REGION	*/

/* REGIONS REVOLT STATUS:	if larger than the value	*/
#define	R_OPEN_REVOLT	220
#define R_GUERILLA_WAR	170
#define R_UNREST	150
#define R_DISSENT	120
#define R_CALM		80
#define R_HAPPY		50
#define R_PROSPEROUS	0

/* REGIONS GOVERNMENTAL STRENGTH	*/
#define R_BARBARIAN	0
#define R_IND_VILAGES	20
#define R_TRIBAL	40
#define R_CITY_STATES	80
#define R_BALKANIZED	110
#define R_CONFEDERACY	140
#define R_NATION	160
#define R_MONARCHY	180
#define R_EMPIRE	200

/* REGION STRATEGY: region strategy by owning nation	*/
#define R_ISOLATION	37
#define R_BUILD_ECON	36
#define R_EXPAND_PCE	35
#define R_TAX		34
#define R_BUILD_MIL	33
#define R_EXPAND_MIL	32
#define R_AT_WAR	31
#define R_SUBDUE	20	/* 20-30: # of turns left in unrest	*/
#define R_CIVIL_WAR	0	/* 0-19: number of turns left in war	*/

#define	NUMREGIONS	250	/* number of regions in the world	*/
#define	STARTYEAR	-10000	/* year that game treats as 0.  In this	
					case - ten thousand years B.C	*/

struct s_region
{
	char	*name;			/* name of region	*/
	long	people;			/* civilians in sector*/
	long	tax;			/* tax production ability*/
	unsigned char owner;		/* nation id of owner	*/
	unsigned char color;		/* color to display region*/
	unsigned char tech_level;	/* technology level	*/
	unsigned char law_level;	/* law level of region	*/
	unsigned char social_level;	/* social level of region*/
	unsigned char govt_sth;	/* owners sth. in region	*/
	unsigned char strategy;	/* owners strategy in region	*/
	unsigned char revolt;	/* status of region	*/
	unsigned char prestige;	/* prestige for owning	*/
};
#endif /* 0 */
