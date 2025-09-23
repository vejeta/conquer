/* conquer : Copyright (c) 1988, 1989 by Ed Barlow.	
   MODIFICATION OF THIS FILE IMPLIES THAT THE MODIFIER WILL ACCEPT
	A LIMITED USE COPYRIGHT AS FOLLOWS:


	1) This software is copyrighted and protected by law. The
		sole owner of this software, which hereafter is known as
		"conquer" is Edward M. Barlow, who hereby grants you a
		personal, non-exclusive right to use this software.
		All rights on this software are reserved.
	2) conquer may not be redistributed in any form. Any requests for
		new software shall, for now, be the perogative of the author.
	3) loss or damage caused by this software shall not be
		the responsibility of the author.
	4) Ed Barlow or Adam Bryant shall be notified of enhancements to this 
		software via electronic mail and, if there is no response, via 
		US mail to:

			Ed Barlow
			115 E 34ths St.
			NY, NY 10016

		My home phone is 212-679-1439. Use it sparingly and call
		before 11PM if it is important.  Note that I no longer have
		access to the arpanet and cant really support conquer like
		I used to.  If somebody knows of a cheap (free) way of getting
		a login somewhere where there is network access - HELP ME!!!!
		Since i am "volunteering" my time for this project, I am not 
		about to spend millions of dollars to buy network access...  
		I can get a modem for my PC if somebody will help me get
		something to call into. -ED-
	5) no attempt shall be made to make any money from this game or to
		use any portion of this code to make any money without the
		authors permission.
	6) no attempt shall be made to port this software to any form of 
		personal computer without the permission of Ed Barlow.
	7) you agree to use your best efforts to see that any user
		of conquer complies with the terms and conditions stated above.
	8) The above copyright agreement will not be tampered with in any form.


   Special thanks to Brian Beuning, Adam Bryant, and any others who helped
	me by making enhancements and bug reports	
*/

/* --- MODIFICATION IS REQUIRED OF THE FOLLOWING DEFINE STATEMENTS ---	*/
#define OWNER	"Adam Bryant"	/* administrators name			*/
#define LOGIN	"adb"		/* administrators login id. IMPORTANT!	*/
                		/* only this UID may update.		*/
/* #define SYSV		/* uncomment this line on a UNIX SYSV machine	*/
#define BSD		/* uncomment this line on a BSD machine		*/
/* #define HPUX		/* uncomment for HP-UNIX			*/
/*#define XENIX		/* this plus SYSV for XENIX machines, untested  */
#define SYSMAIL		/* if your system supports mail			*/
			/* conquer will notify you about system mail	*/

/* ------------------MODIFICATION IS OPTIONAL ON THE FOLLOWING----------*/
#define SPOOLDIR "/usr/spool/mail"	/* location of mail spool	*/
#define FILELOCK	/* if your system supports BSD flock()		*/
			/* other file locking is not well implemented	*/
/*#define LOCKF		/* in addition to FILELOCK if you have lockf    */
			/* this is needed on NFS remote file systems    */
#define TIMELOG		/* if your system has the date command		*/

#define NTOTAL 25	/* max # of nations ( player + npc + monster )	*/
#define MAXPTS	65	/* points for players to buy stuff with at start*/
#define MAXARM	50	/* maximum number of armies per nation		*/
#define MAXNAVY	10	/* maximum number of fleets per nation		*/
#define PDEPLETE 30	/* % of armies/sectors depleted without Capitol	*/
#define PFINDSCOUT 50	/* percentage chance for capturing scouts	*/

#define RUNSTOP		/* defined to stop update if players are in	*/
			/* game -- not recommended with automatic update*/
#define TRADE		/* defined to allow commerce between nations	*/
#define	TRADEPCT 75	/* percent of sectors with exotic trade goods	*/
#define METALPCT 33	/* percent of tradegoods that are metals	*/
#define JEWELPCT 33	/* percent of tradegoods that are luxury items	*/
#define HIDELOC		/* defined if news is not to report sectors	*/
#define OGOD		/* defined if you wish to enhance god powers.   */
#define REMAKE		/* may make a world even if datafile exists.	*/
			/* this allows demi-gods the ability to remake  */
			/* their world.                                 */
#define NOSCORE		/* only show full scores to god while in game	*/
/* #define CHECKUSER */	/* only allow owner of nation to play it        */
#define REVSPACE 5	/* allow for this many revolts in nation list   */
#define LASTADD 5	/* last turn players may w/out password         */
#define USERLOG		/* log users who play a nation                  */
#define MASK 037	/* data file protection mask (umask)            */
#define DERVDESG	/* allow DERVISH to redesignate in DESERT/ICE	*/
#define MONSTER	45	/* defined if pirates/savages/nomads/lzard exist.
			   represents # of sectors of land that need to be
			   in world per pirate/savage/nomad nation	*/
#define MORE_MONST	/* defined if destroyed monsters are replaced	*/
#define NPC	45	/* defined if NPC nations should exist. The numeric
			   represents # of sectors of land that need to be
			   in world per non-player character nation	*/
#define CHEAT		/* npcs will cheat to keep up - this is a very weak
			   form of cheating.  I use good npc algorithms 
			   (i think... comments)			*/
/*#define NPC_COUNT_ARMIES   /* defined if NPC nations can always count armies
			   This makes them to cheat by seeing even VOID and
			   HIDDEN armies when counting enemy units.	*/
/*#define NPC_SEE_SECTORS    /* defined if NPC nations can always see sectors
			   This allows them to cheat by being allowed to see
			   all sector attributes of even VOID sectors.	*/
#define	NPC_SEE_CITIES	     /* defined if NPC nations can always see cities
			   This allows them to cheat by being able to see
			   if a VOID sector is a city/town.  Simulates the
			   players ability to tell cities via movement.	*/
#define STORMS		/* have storms strike fleets			*/
#define VULCANIZE	/* add in volcano eruptions....			*/
#define PVULCAN 20	/* % chance of eruption each round (see above)	*/
#define ORCTAKE 100000L /* comment out if dont want orcs to takeover orc
			   NPCs.  else is takeover price in jewels	*/
#define MOVECOST 20l	/* cost to do a move, get a screen...		*/
#define TAKEPOINTS 10	/* spell points for orc takeover		*/
#define PMOUNT 40	/* % of land that is mountains			*/
#define PSTORM 3	/* % chance that a storm will strike a fleet	*/
			/* unless it is in harbor			*/
#define CMOVE		/* #ifdef NPC; defined for the computer to move
			   for Player nations if they forget to move	*/
#define BEEP		/* defined if you wish terminal to beep		*/
#define HILIGHT		/* defined if terminals support inverse video	*/
#define RANEVENT 15	/* comment out if you dont want random events
			   weather, tax revolts, and volcanoes all are	
			   considered random events. 			*/
#define PWEATHER 0	/* percent for weather disaster - unimplemented	*/
#define	PREVOLT	25	/* %/turn that a revolt acutally occurs		*/
			/* a turn is 1 season and 25% is a large value	*/
#define	SPEW		/* spew random messages from npcs 		*/

/* -BELOW THIS POINT ARE PARAMETERS YOU MIGHT OPTIONALLY WISH TO CHANGE-*/

/* making these numbers large takes more CPU time			*/
#define LANDSEE 2	/* how far you can see from your land		*/
#define NAVYSEE 1	/* how far navies can see			*/
#define ARMYSEE 2	/* how far armies can see			*/
#define PRTZONE 3	/* how far pirates roam from their basecamp	*/
#define MEETNTN 2	/* how close nations must be to adjust status	*/

/* Below taxation rates are in gold talons per unit of product produced	*/
#define TAXFOOD		5L
#define TAXMETAL	8L
#define TAXGOLD		8L
#define TAXOTHR		3L	/* per food point equivalent		*/
/* Town and City/Capitol tax rates based on # of people	*/
#define TAXCITY		100L
#define TAXTOWN		80L

#define SHIPMAINT	4000L	/* ship mainatinance cost		*/
#define TOMANYPEOPLE	4000L	/* too many people in sector - 1/2 repro and
				   1/2 production; not in cities/caps	*/
#define ABSMAXPEOPLE	50000L	/* absolute max people in any sector	*/
#define	MILLSIZE	500L	/* min number of people to work a mill	*/
#define TOMUCHMINED	50000L	/* units mined for 100% chance of metal	*/
				/* depletion actual chance is prorated	*/
#define DESFOOD		4	/* min food val to redesignate sector	*/
#define MAXNEWS		5	/* number of news files stored		*/
#define LONGTRIP	100	/* navy trip lth for 100% attrition	*/

/* min soldiers to take sector - either 75 or based on your civilians	*/
#define TAKESECTOR	min(500,max(75,(ntn[country].tciv/350)))

#define MAXLOSS		60	/* maximum % of men lost in 1:1 battle	*/
#define	FINDPERCENT	1	/* percent to find gold/metal in sector	*/
#define DESCOST		2000L	/* cost to redesignate and the metal cost
				   for cities				*/
#define FORTCOST	1000L	/* cost to build a fort point		*/
#define STOCKCOST	3000L	/* cost to build a stockade		*/
#define REBUILDCOST	3000L	/* cost to remove a ruin		*/
#define WARSHPCOST	20000L	/* cost to build one light warship	*/
#define MERSHPCOST	25000L	/* cost to build one light merchant	*/
#define GALSHPCOST	25000L	/* cost to build one light galley	*/
#define N_CITYCOST	4	/* move lost in (un)loading in cities	*/
#define SHIPCREW	100	/* full strength crew on a ship		*/
#define SHIPHOLD	100L	/* storage space of a ship unit		*/
#define CITYLIMIT	8L	/* % of npc pop in sctr before => city	*/
#define CITYPERCENT	20L	/* % of npc pop able to be in cities	*/
/* note that militia are not considered military below	*/
#define MILRATIO	8L	/* ratio civ:mil for NPCs		*/
#define MILINCAP	8L	/* ratio (mil in cap):mil for NPCs	*/
#define	MILINCITY	10L	/* militia=people/MILINCITY in city/cap */
#define NPCTOOFAR	15	/* npcs should not go this far from cap	*/
#define BRIBE		50000L	/* amount of gold/1000 men to bribe	*/
#define METALORE	7L	/* metal/soldier needed for +1% weapons	*/
/* strength value for fortifications	*/
#define DEF_BASE	10	/* base defense value 2 * in city/caps	*/
#define FORTSTR		5	/* percent per fortress point in forts	*/
#define TOWNSTR		5	/* percent per fortress point in towns	*/
#define CITYSTR		8	/* percent per fortress point in city	*/
#define	LATESTART	2	/* new player gets 1 point/LATESTART turns, 
				   when they start late into the game	*/

/*	starting values for mercenaries	*/
#define ST_MMEN	 (NTOTAL*500)	/* a nation may draft ST_MMEN/NTOTAL	*/
				/* mercenaries per turn. Added to when	*/
				/* armies are disbanded.	*/
#define ST_MATT		40	/* mercenary attack bonus	*/
#define ST_MDEF		40	/* mercenary defense bonus	*/

#define VERSION "Version 4"	/* version number of the game	*/
