// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * header.h - Cross-platform game configuration (properly modernized)
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

#ifndef CONQUER_HEADER_H
#define CONQUER_HEADER_H

/* ================================================================== */
/* STANDARD LIBRARY INCLUDES */
/* ================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

/* Modern C standard support with fallbacks */
#ifdef __STDC_VERSION__
    #if __STDC_VERSION__ >= 199901L
        #include <stdint.h>
        #include <stdbool.h>
        #define HAVE_C99 1
    #endif
#endif

#ifndef HAVE_C99
    #ifndef __cplusplus
        #ifndef bool
            typedef int bool;
            #define true 1
            #define false 0
        #endif
    #endif
#endif

/* ================================================================== */
/* CROSS-PLATFORM COMPATIBILITY */
/* ================================================================== */

/* Platform detection (modernized) */
#if defined(__linux__) || defined(__linux) || defined(linux)
    #define PLATFORM_LINUX 1
    #define PLATFORM_UNIX 1
#elif defined(__APPLE__) && defined(__MACH__)
    #define PLATFORM_MACOS 1
    #define PLATFORM_UNIX 1
#elif defined(__FreeBSD__)
    #define PLATFORM_FREEBSD 1
    #define PLATFORM_UNIX 1
#elif defined(__OpenBSD__)
    #define PLATFORM_OPENBSD 1
    #define PLATFORM_UNIX 1
#elif defined(__NetBSD__)
    #define PLATFORM_NETBSD 1
    #define PLATFORM_UNIX 1
#elif defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS 1
#elif defined(__CYGWIN__)
    #define PLATFORM_CYGWIN 1
    #define PLATFORM_UNIX 1
#endif

#if defined(__unix__) || defined(__unix) || defined(unix) || defined(PLATFORM_UNIX)
    #define PLATFORM_UNIX 1
#endif

/* System includes */
#ifdef PLATFORM_UNIX
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #ifdef PLATFORM_LINUX
        #include <sys/file.h>
    #endif
#elif defined(PLATFORM_WINDOWS)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include <io.h>
    #include <direct.h>
    #define unlink _unlink
    #define mkdir(path, mode) _mkdir(path)
    #define access _access
#endif

/* ================================================================== */
/* LEGACY PLATFORM DEFINES (preserved for compatibility) */
/* ================================================================== */

#ifdef PLATFORM_UNIX
    #define BSD 1    /* Most modern Unix systems are BSD-like */
#endif

/* Uncomment these as needed for specific old systems */
/* #define SYSV */     /* uncomment for UNIX SYSV machines */
/* #define HPUX */     /* uncomment for HP-UNIX */
/* #define XENIX */    /* this plus SYSV for XENIX machines */

/* ================================================================== */
/* ADMINISTRATOR CONFIGURATION */
/* ================================================================== */

#define OWNER "God"                /* administrator's name */
#ifndef LOGIN
    #define LOGIN "defaultuser"    /* admin login - overridden by Makefile */
#endif

/* ================================================================== */
/* SYSTEM CAPABILITIES (modernized detection) */
/* ================================================================== */

/* Mail system support */
#ifdef PLATFORM_UNIX
    #define SYSMAIL 1              /* system mail support */
    #ifdef PLATFORM_LINUX
        #define SPOOLDIR "/var/mail"
    #elif defined(PLATFORM_MACOS)
        #define SPOOLDIR "/var/mail"
    #elif defined(PLATFORM_FREEBSD) || defined(PLATFORM_OPENBSD) || defined(PLATFORM_NETBSD)
        #define SPOOLDIR "/var/mail"
    #else
        #define SPOOLDIR "/usr/spool/mail"  /* fallback */
    #endif
#elif defined(PLATFORM_WINDOWS)
    #define SPOOLDIR "C:\\temp"
    /* No SYSMAIL on Windows */
#else
    #define SPOOLDIR "/usr/spool/mail"
#endif

/* File locking support */
#ifdef PLATFORM_UNIX
    #define FILELOCK 1             /* BSD flock() support */
    #ifdef PLATFORM_LINUX
        #define LOCKF 1            /* lockf() for NFS */
    #endif
#endif

/* System utilities */
#ifdef PLATFORM_UNIX
    #define TIMELOG 1              /* date command available */
#elif defined(PLATFORM_WINDOWS)
    #define TIMELOG 1              /* Windows also has date */
#endif

/* ================================================================== */
/* CORE GAME LIMITS (preserved exactly from original) */
/* ================================================================== */

#define NTOTAL 35       /* max # of nations (player + npc + monster) */
#define MAXPTS 65       /* points for players to buy stuff with at start */
#define MAXARM 50       /* maximum number of armies per nation */
#define MAXNAVY 10      /* maximum number of fleets per nation */
#define PDEPLETE 30     /* % of armies/sectors depleted without Capitol */
#define PFINDSCOUT 50   /* percentage chance for capturing scouts */

/* ================================================================== */
/* GAME FEATURES (preserved exactly from original) */
/* ================================================================== */

#define RUNSTOP         /* stop update if players are in game */
#define TRADE           /* allow commerce between nations */
#define TRADEPCT 75     /* percent of sectors with exotic trade goods */
#define METALPCT 33     /* percent of tradegoods that are metals */
#define JEWELPCT 33     /* percent of tradegoods that are luxury items */
#define HIDELOC         /* news doesn't report sectors */
#define OGOD            /* enhanced god powers */
#define REMAKE          /* may make world even if datafile exists */
#define NOSCORE         /* only show full scores to god while in game */
#define CHECKUSER       /* only allow owner of nation to play it */
#define REVSPACE 5      /* allow for this many revolts in nation list */
#define LASTADD 5       /* last turn players may join without password */
#define USERLOG         /* log users who play a nation */
#define MASK 037        /* data file protection mask (umask) */
#define DERVDESG        /* allow DERVISH to redesignate in DESERT/ICE */

/* ================================================================== */
/* MONSTER AND NPC CONFIGURATION (preserved exactly) */
/* ================================================================== */

#define MONSTER 45      /* sectors of land per pirate/savage/nomad nation */
#define MORE_MONST      /* destroyed monsters are replaced */
#define NPC 45          /* sectors of land per non-player character nation */
#define CHEAT           /* NPCs cheat to keep competitive */
/* #define NPC_COUNT_ARMIES */ /* NPCs can always count armies */
/* #define NPC_SEE_SECTORS */  /* NPCs can always see sectors */
#define NPC_SEE_CITIES      /* NPCs can always see cities */

/* ================================================================== */
/* ENVIRONMENTAL EFFECTS (preserved exactly) */
/* ================================================================== */

#define STORMS          /* storms strike fleets */
#define VULCANIZE       /* volcano eruptions */
#define PVULCAN 20      /* % chance of eruption each round */
#define ORCTAKE 100000L /* jewel cost for orc takeover */
#define MOVECOST 20L    /* cost per move/screen */
#define TAKEPOINTS 10   /* spell points for orc takeover */
#define PMOUNT 40       /* % of land that is mountains */
#define PSTORM 3        /* % chance storm strikes fleet */
#define CMOVE           /* computer moves for inactive players */

/* ================================================================== */
/* INTERFACE FEATURES (preserved exactly) */
/* ================================================================== */

#define BEEP            /* terminal beep support */
#define HILIGHT         /* inverse video support */

/* ================================================================== */
/* RANDOM EVENTS (preserved exactly) */
/* ================================================================== */

#define RANEVENT 15     /* enable random events */
#define PWEATHER 0      /* percent for weather disasters */
#define PREVOLT 25      /* %/turn that a revolt actually occurs */
#define SPEW            /* NPCs send random messages */

/* ================================================================== */
/* GAME MECHANICS PARAMETERS (preserved exactly) */
/* ================================================================== */

/* Vision and movement ranges */
#define LANDSEE 2       /* how far you can see from your land */
#define NAVYSEE 1       /* how far navies can see */
#define ARMYSEE 2       /* how far armies can see */
#define PRTZONE 3       /* how far pirates roam from basecamp */
#define MEETNTN 2       /* how close nations must be to adjust status */

/* Taxation rates (in gold talons per unit) */
#define TAXFOOD 5L      /* per food unit */
#define TAXMETAL 8L     /* per metal unit */
#define TAXGOLD 8L      /* per gold unit */
#define TAXOTHR 3L      /* per food point equivalent */
#define TAXCITY 100L    /* per person in city */
#define TAXTOWN 80L     /* per person in town */

/* Economic parameters */
#define SHIPMAINT 4000L         /* ship maintenance cost */
#define TOMANYPEOPLE 4000L      /* overpopulation threshold */
#define ABSMAXPEOPLE 50000L     /* absolute max people in any sector */
#define MILLSIZE 500L           /* min people to work a mill */
#define TOMUCHMINED 50000L      /* units mined for 100% depletion chance */
#define DESFOOD 4               /* min food value to redesignate sector */
#define MAXNEWS 5               /* number of news files stored */
#define LONGTRIP 100            /* navy trip length for 100% attrition */

/* Combat and military */
#define TAKESECTOR min(500,max(75,(ntn[country].tciv/350)))  /* soldiers needed to take sector */
#define MAXLOSS 60              /* maximum % of men lost in 1:1 battle */
#define FINDPERCENT 1           /* percent chance to find gold/metal */
#define DESCOST 2000L           /* cost to redesignate + metal cost for cities */
#define FORTCOST 1000L          /* cost to build a fort point */
#define STOCKCOST 3000L         /* cost to build a stockade */
#define REBUILDCOST 3000L       /* cost to remove a ruin */

/* Naval configuration */
#define WARSHPCOST 20000L       /* cost to build one light warship */
#define MERSHPCOST 25000L       /* cost to build one light merchant */
#define GALSHPCOST 25000L       /* cost to build one light galley */
#define N_CITYCOST 4            /* movement lost in (un)loading in cities */
#define SHIPCREW 100            /* full strength crew on a ship */
#define SHIPHOLD 100L           /* storage space of a ship unit */

/* NPC behavior parameters */
#define CITYLIMIT 8L            /* % of NPC pop in sector before => city */
#define CITYPERCENT 20L         /* % of NPC pop able to be in cities */
#define MILRATIO 8L             /* ratio civ:mil for NPCs */
#define MILINCAP 8L             /* ratio (mil in cap):mil for NPCs */
#define MILINCITY 10L           /* militia = people/MILINCITY in city/cap */
#define NPCTOOFAR 15            /* NPCs shouldn't go this far from capitol */
#define BRIBE 50000L            /* gold/1000 men to bribe */
#define METALORE 7L             /* metal/soldier needed for +1% weapons */

/* Defense values */
#define DEF_BASE 10             /* base defense value, 2x in city/caps */
#define FORTSTR 5               /* percent per fortress point in forts */
#define TOWNSTR 5               /* percent per fortress point in towns */
#define CITYSTR 8               /* percent per fortress point in cities */
#define LATESTART 2             /* new player gets 1 point/LATESTART turns */

/* Starting mercenary values */
#define ST_MMEN (NTOTAL*500)    /* starting mercenary pool */
#define ST_MATT 40              /* mercenary attack bonus */
#define ST_MDEF 40              /* mercenary defense bonus */

/* ================================================================== */
/* VERSION INFORMATION */
/* ================================================================== */

#define VERSION "Version 4"     /* version number of the game */

/* ================================================================== */
/* MODERN UTILITY MACROS (safe additions) */
/* ================================================================== */

/* Safe string operations */
#define SAFE_STRNCPY(dest, src, size) do { \
    strncpy((dest), (src), (size) - 1); \
    (dest)[(size) - 1] = '\0'; \
} while(0)

/* Utility macros */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/* Cross-platform file operations */
#ifdef PLATFORM_WINDOWS
    #define FILE_EXISTS(path) (_access((path), 0) == 0)
#else
    #define FILE_EXISTS(path) (access((path), F_OK) == 0)
#endif

/* Terminal control */
#ifdef PLATFORM_UNIX
    #define CLEAR_SCREEN() printf("\033[2J\033[H")
#elif defined(PLATFORM_WINDOWS)
    #define CLEAR_SCREEN() system("cls")
#else
    #define CLEAR_SCREEN()
#endif

/* Debug support */
#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) \
        fprintf(stderr, "[DEBUG] %s:%d: " fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...)
#endif

/* Random number generation */
#define RAND() rand()
#define SRAND(x) srand(x)

/* ================================================================== */
/* PATH CONFIGURATION (modernized defaults) */
/* ================================================================== */

#ifndef DEFAULTDIR
    #ifdef PLATFORM_WINDOWS
        #define DEFAULTDIR "C:\\Program Files\\Conquer\\share"
    #else
        #define DEFAULTDIR "/usr/local/share/conquer"
    #endif
#endif

#ifndef EXEDIR
    #ifdef PLATFORM_WINDOWS
        #define EXEDIR "C:\\Program Files\\Conquer\\bin"
    #else
        #define EXEDIR "/usr/local/bin"
    #endif
#endif

/* ================================================================== */
/* COMPATIBILITY NOTES */
/* ================================================================== */

/*
 * This modernized header.h preserves all original game constants and
 * functionality while adding cross-platform support. Key differences
 * from the over-modernized version:
 * 
 * 1. All original game balance parameters preserved exactly
 * 2. All original feature flags kept (#define TRADE, etc.)
 * 3. Platform detection added without breaking existing code
 * 4. Modern C standard support with fallbacks
 * 5. Cross-platform utility functions added safely
 * 6. No map constants (MAPX/MAPY) - these are defined in data.h
 * 7. Compatible with existing data files and save games
 */

#endif /* CONQUER_HEADER_H */