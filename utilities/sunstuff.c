// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * sunstuff.c - Sun workstation specific graphics code
 * 
 * This file is part of Conquer Utilities.
 * Originally Copyright (C) 1989 by Richard Caley
 * Copyright (C) 2025 Juan Manuel Méndez Rey (Vejeta) - Licensed under GPL v3 with permission from original license
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

#include <stdio.h>
#include "sunconqrast.h"

struct				/* maps from size of map to the font */
				/* to use */
    {				/* if size > mag use font */
    int mag;
    char *font,*bold;
    } fonts[] =
        {
	9,  "/usr/lib/fonts/fixedwidthfonts/screen.r.7", NULL,
	11, "/usr/lib/fonts/fixedwidthfonts/cour.r.10",
	    "/usr/lib/fonts/fixedwidthfonts/cour.b.10",
	12, "/usr/lib/fonts/fixedwidthfonts/cour.r.12",
	    "/usr/lib/fonts/fixedwidthfonts/cour.b.12",
	14, "/usr/lib/fonts/fixedwidthfonts/cour.r.14",
	    "/usr/lib/fonts/fixedwidthfonts/cour.b.14",
	16, "/usr/lib/fonts/fixedwidthfonts/cour.r.16",
	    "/usr/lib/fonts/fixedwidthfonts/cour.b.16",
	18, "/usr/lib/fonts/fixedwidthfonts/cour.r.18",
	    "/usr/lib/fonts/fixedwidthfonts/cour.b.18",
	22, "/usr/lib/fonts/fixedwidthfonts/cour.r.24",
	    "/usr/lib/fonts/fixedwidthfonts/cour.b.24",
	999999, NULL
	};

static int encoding_type = RT_STANDARD ;

int
argument_parse(argv)

char *argv[];

{
if ( !strcmp(*argv,"-compact"))
    {
    encoding_type = RT_BYTE_ENCODED ;
    return 1;
    }
else
    return 0;
}

/*ARGSUSED*/
int
write_bitmap(bm,file,width,height)

bitmap bm;
char *file;
int width,height;

{
    FILE *f;

    if ((f=fopen(file,"w"))==NULL)
	    ioerror("Can't open output file %s",file);

    return pr_dump( bm, f, RMT_NONE, encoding_type,0);
    }



void
display_bitmap(bm,x,y,w,h)

bitmap bm;
int x,y,w,h;

{
    struct pixrect *screen=pr_open("/dev/fb");

    pr_rop(screen,x,y,w,h,PIX_SRC,bm,0,0);

    }

bitmap_text(pr,x,y,colour,pf,str)

bitmap pr;
int x,y;
int colour;
font pf;
char *str;

{
    struct pr_prpos pos;
  
    pos.pos.x=x;
    pos.pos.y=y;
    pos.pr=pr;

    pf_text(pos,colour,pf,str);
    }

