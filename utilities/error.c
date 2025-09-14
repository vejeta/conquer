/*
 * error.c - Error handling and reporting utilities
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
#include <errno.h>
#include <stdlib.h>

/* int errno; */
/* char *sys_errlist[]; */

char *myname="Someone";

/*VARARGS1*/
void
ioerror(str,arg1,arg2,arg3)

char *str;
int arg1, arg2, arg3;

{
fprintf(stderr,"%s: ",myname);
fprintf(stderr,str,arg1,arg2,arg3);
fprintf(stderr," - %s\n",sys_errlist[errno]);
exit(1);
}

/*VARARGS1*/
void
error(str,arg1,arg2,arg3)

char *str;
int arg1, arg2, arg3;

{
fprintf(stderr,"%s: ",myname);
fprintf(stderr,str,arg1,arg2,arg3);
fputc('\n',stderr);
exit(1);
}

