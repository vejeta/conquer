/*
 * sunconqrast.h - Sun workstation graphics definitions
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

#define driver_version "1"
#define conqrast_name "Sunconqrast"

#include <pixrect/pixrect_hs.h>

typedef struct pixrect * bitmap;
typedef char * screen_type;
typedef struct pixfont * font;

#define initialise_bitmaps() /* EMPTY */
#define finish_bitmaps() /* EMPTY */

#define is_screenname(mapname) (!strncmp(mapname,"/dev/",5))
#define get_default_screen_name() "/dev/fb"
#define screen_named(name) (name)
#define get_screen_bitmap(screen) (pr_open(screen))
#define create_bitmap(width,height) (mem_create(width,height,1))
#define destroy_bitmap(bitmap) pr_close(bitmap)
#define font_named(name) pf_open(name)
#define bad_font(font) ((font)==NULL)
#define font_height(font) ((font->pf_defaultsize).y)
#define font_width(font) ((font->pf_defaultsize).x)
#define font_baseline(font) (0-(font->pf_char)['A'].pc_home.y+1)

#define BLACK PIX_SET
#define WHITE PIX_CLR
#define DRAW_BLACK (PIX_DST|PIX_SRC)
#define DRAW_WHITE (PIX_DST&PIX_NOT(PIX_SRC))

#define set_bit(bitmap,x,y,val) pr_put(bitmap,x,y,val)
#define draw_line(bitmap,x1,y1,x2,y2,colour) pr_vector(bitmap,x1,y1,x2,y2,colour,0)
