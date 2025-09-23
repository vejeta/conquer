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

