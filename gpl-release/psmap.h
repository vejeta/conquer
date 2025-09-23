// SPDX-License-Identifier: GPL-3.0-or-later
/*
/*
 * psmap.c - Header file for conqps. A program to convert conquer-maps to postscript
 *
 * This file is part of Conquer.
 * Originally Copyright (C) 1989 by Martin Forssen (MaF)
 * Copyright (C) 2025 Juan Manuel Méndez Rey (Vejeta) - Licensed under GPL v3 with permission from original author
 *
 * Original author: Martin Forssen <d8forma@dtek.chalmers.se> (historical)
 * Permission granted by: Martin Forssen <maf@recordedfuture.com>
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

#ifdef OTHER
#define DEFAULTPAGE 0
#endif

#ifdef A4
#define DEFAULTPAGE 1
#endif

#ifdef LETTER
#define DEFAULTPAGE 2
#endif

/* Printer dependant entries */
#define PAGEWIDTH_A4       540
#define PAGEHEIGHT_A4      820
#define XOFFSET_A4         30
#define YOFFSET_A4         10

#define PAGEWIDTH_LETTER       575
#define PAGEHEIGHT_LETTER      760
#define XOFFSET_LETTER         15
#define YOFFSET_LETTER         10

#define PAGEWIDTH_OTHER       450
#define PAGEHEIGHT_OTHER      700
#define XOFFSET_OTHER         40
#define YOFFSET_OTHER         30

/* Cosmetics */
#define XMARGINS        30
#define YMARGINS        50

/* Version specific defines */
#define VERSION         "1.0"
#define USAGE           "Usage: %s [cghlnvu] [p pagesize] [f font] [o x,y] [s size]\n\t[W n] [L n] [X n] [Y n] [t title] [infile [outfile]]\n"
#define MATCHSTRING     "Conquer Version"

/* Mathematical definitions */
#define TRUE            1
#define FALSE           0

/* Types of maps possible to be printed */
#define SIMPLE          0
#define ALTITUDES       1
#define DESIGNATIONS    2
#define NATIONS         3
#define VEGETATIONS     4
#define FORCED          5
