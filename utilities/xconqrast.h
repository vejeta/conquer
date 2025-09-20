// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * xconqrast.h - X Windows graphics definitions
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

screen_type screen_named();

bitmap get_screen_bitmap();
bitmap create_bitmap();

#define destroy_bitmap(bm)	/* I don't know how to */
#define font_named(name) (name)	/* ditto */
#define bad_font(font) ((font)==NULL) /* ??? */
#define font_height(font) (0)	/* ??? */
#define font_width(font) (0)	/* ??? */
#define font_baseline(font) (0) /* ??? */

#define BLACK (1)
#define WHITE (0)		/* guessing */
#define DRAW_BLACK (1)		/* ?? */
#define DRAW_WHITE (2)

void set_bit();

#define draw_line(bitmap,x1,y1,x2,y2,colour) /* again no idea! */


