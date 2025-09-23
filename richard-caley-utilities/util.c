#include <stdio.h>
#include <sgtty.h>
#include <stdlib.h>

/*********************************************************************
*                                                                    *
* various utility bits split off to try and get conqrast.c down to a *
* reasonable size                                                    *
*                                                                    *
*********************************************************************/


/*********************************************************************
*                                                                    *
* routine to build the dither matrix used to shade                   *
*                                                                    *
*********************************************************************/

#define MAXDITHER 4		/* don't touch this */

short dit[1<<MAXDITHER][1<<MAXDITHER];

void
build_dither_matrix(depth)

int depth;

{
    register int d,i,j,p;
    depth= 1<<depth;

    dit[0][0]=0; dit[1][0]=2;

    dit[0][1]=3; dit[1][1]=1;

    for(d=2;d<depth;d=d<<1)
	for(i=0;i<d;i++)
	    for(j=0;j<d;j++)
		{
		p= (dit[i][j] *=4);
		dit[d+i][j] = p+2;
		dit[i][d+j] = p+3;
		dit[d+i][d+j] = p+1;
		}

    if (depth != 16 )
	for(i=0;i<depth;i++)
	    for (j=0;j<depth;j++)
		dit[i][j] = (dit[i][j]*256)/d/d;

    }

/*********************************************************************
*                                                                    *
* disconnect from controlling tty                                    *
*                                                                    *
*********************************************************************/

void
disconnect()

{
    int tty;

    tty=open("/dev/tty",0);

    ioctl(tty,TIOCNOTTY,0);

    close(tty);
    }

/*********************************************************************
*                                                                    *
* go into background                                                 *
*                                                                    *
*********************************************************************/

void
background()

{
    switch (fork())
	{
    case 0: break;

    default: exit(0);
	}
    printf("(%d)\n",getpid());
    }

/*********************************************************************
*                                                                    *
* Print a useage message and exit. This needs to be expanded.        *
*                                                                    *
*********************************************************************/

void
help_message(me)

char *me;

{
    printf("%s: Usage --\n",me);
    printf("\n%s\t\t-o <file-or-display>\n",me);
    printf("\t\t-N <nation>\n");
    printf("\t\t-D <game-directory>\n");
    printf("\t\t-c<contour-character> <brightness>\n");
    printf("\t\t-f <which-font> <font>\n");
    printf("\t\t-d -n{a} -b{a} -m <magnification> -a -bg -l -t -v\n");
    exit(1);
    }

