/*
 * header.h - Game configuration constants and settings
 * 
 * This file is part of Conquer.
 * Originally Copyright (C) 1988-1989 by Edward M. Barlow and Adam Bryant
 * Copyright (C) 2025 Juan Manuel Méndez Rey (Vejeta) - Licensed under GPL v3 with permission from original authors
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
