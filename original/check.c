/* Conquer: Copyright (c) 1988 by Edward M Barlow	*/
/* check.c contains checking and debugging functions	*/

#include	<ctype.h>
#include	<stdio.h>
#ifndef FILELOCK
#include	<sys/types.h>
#include	<sys/stat.h>
#endif
#include	"header.h"
#include	"data.h"

void
verify_ntn( __file__, __line__ )
char	__file__[];
int	__line__;
{
	register struct s_nation	*nptr;
	register int	i;
	register int	country;
	int j, k;
	struct army	*a;

	for( country = 0; country < NTOTAL; country++ ) {
		nptr = &ntn[country];

		if(country !=0 && nptr->active == 0) continue;
		if( nptr->metals < 0.0 ) {
			fprintf( stderr, "file %s: line %d: nation[%d] metal = %ld\n",
				__file__, __line__, country, nptr->metals );
			nptr->metals = 0.0;
		}

		if( nptr->jewels < 0 ) {
			fprintf( stderr, "file %s: line %d: nation[%d] jewels = %ld\n",
				__file__, __line__, country, nptr->jewels );
			nptr->jewels = 0.0;
		}

		for( i = 0; i < MAXARM; i++ ) {
			a = &nptr->arm[i];
			if( a->sold < 0 ) {
				fprintf( stderr, "file %s: line %d: nation[%d] army[%d] sold = %d\n",
					__file__, __line__, country, i, a->sold );
				a->sold = 0;
			}
			if( a->sold==0 ) continue;
			if( a->xloc >= MAPX ) {
				fprintf( stderr, "file %s: line %d: nation[%d] army[%d] xlocation = %d\n",
					__file__, __line__, country, i, a->xloc );
				a->xloc = 0;
			}
			if( a->yloc >= MAPY ) {
				fprintf( stderr, "file %s: line %d: nation[%d] army[%d] ylocation = %d\n",
					__file__, __line__, country, i, a->yloc );
				a->yloc = 0;
			}
			if( a->stat == ONBOARD) {
				if (a->smove != 0) {
					fprintf( stderr, "file %s: line %d: nation[%d] army[%d] onboard move = %d\n",
						__file__, __line__, country, i, a->smove );
					a->smove = 0;
				}
				k = 0;
				for (j = 0; j < MAXNAVY; j++) {
					if (nptr->nvy[j].warships == 0
					  && nptr->nvy[j].merchant == 0
					  && nptr->nvy[j].galleys == 0)
						continue;
					if (nptr->nvy[j].armynum == i) {
						k = 1;
					}
				}
				if (k == 0) {
					fprintf(stderr, "files %s: line %d: nation[%d] army[%d] onboard nothing\n",
						__file__,__line__,country,i);
					a->stat = DEFEND;
				}
			}
			if( a->stat != ONBOARD && sct[a->xloc][a->yloc].altitude==WATER ) {
				fprintf( stderr, "file %s: line %d: nation[%d] army[%d] loc=%d,%d (water) men=%d\n",
				__file__,__line__,country,i,a->xloc,a->yloc,a->sold);
				a->sold = 0;
			}
		} /* for */

		for( i = 0; i < MAXNAVY; i++ ) {
			if (nptr->nvy[i].warships == 0
			  && nptr->nvy[i].merchant == 0
			  && nptr->nvy[i].galleys == 0)
				continue;
			if (nptr->nvy[i].armynum != MAXARM) {
				a = &(nptr->arm[nptr->nvy[i].armynum]);
				if (a->sold == 0 || a->stat != ONBOARD) {
					fprintf(stderr, "file %s: line %d: nation[%d] navy[%d] carrying invalid troop\n",
					       __file__,__line__,country,i);
					nptr->nvy[i].armynum = MAXARM;
				}
			}
		} /* for */

		for( i = 0; i < NTOTAL; i++ ) {
			if( ntn[i].active >= NPC_PEASANT ) {
				if ( nptr->dstatus[i] != WAR ) {
					nptr->dstatus[i] = WAR;
				}
				if ( ntn[i].dstatus[country] != WAR ) {
					ntn[i].dstatus[country] = WAR;
				}
			} if( nptr->dstatus[i] > JIHAD ) {
				fprintf( stderr, "file %s: line %d: nation[%d] diplomatic status with %d = %d\n",
					__file__, __line__, country, i, nptr->dstatus[i] );
				nptr->dstatus[i] = WAR;
			}
		} /* for */
	} /* for */
} /* verify_ntn() */

void
verify_sct( __file__, __line__ )
char	__file__[];
int	__line__;
{
	register struct s_sector	*sptr;
	register int		x, y;

	for( x = 0; x < MAPX; x++ ) {
		for( y = 0; y < MAPY; y++ ) {
			sptr = &sct[x][y];

			if(sptr->tradegood>TG_none) {
				fprintf( stderr, "file %s: line %d: sct[%d][%d].tradegood = %d (invalid)\n", __file__, __line__, x, y, sptr->tradegood );
				sptr->tradegood=TG_none;
			}
			if(( sptr->metal != 0 )
			&&(( sptr->tradegood>END_MINE)
			  ||(sptr->tradegood<=END_NORMAL))) {
				fprintf( stderr, "file %s: line %d: sct[%d][%d].metal = %d with no tradegood\n", __file__, __line__, x, y, sptr->metal );
				sptr->metal = 0;
			}

			if(( sptr->jewels != 0 )
			&&((sptr->tradegood>END_WEALTH)
			  ||(sptr->tradegood<=END_MINE))) {
				fprintf( stderr, "file %s: line %d: sct[%d][%d].jewels = %d with no tradegood\n", __file__, __line__, x, y, sptr->jewels );
				sptr->jewels = 0;
			}

			if( sptr->people > ABSMAXPEOPLE )
				sptr->people = ABSMAXPEOPLE;

			if( sptr->people < 0 ) {
				fprintf( stderr, "file %s: line %d: sct[%d][%d].people = %d\n", __file__, __line__, x, y, sptr->people );
				if( sptr->people < -1*ABSMAXPEOPLE )
					sptr->people = ABSMAXPEOPLE;
				else sptr->people = 0;
			}

			if( sptr->owner != 0 && sptr->altitude == WATER ) {
				fprintf( stderr, "file %s: line %d: sct[%d][%d].owner = %s (a water sector)\n",__file__,__line__, x, y, ntn[sptr->owner].name );
				sptr->owner = 0;
			}
			if( sptr->fortress > 12 ){
				fprintf( stderr, "file %s: line %d: sct[%d][%d].fortress = %d \n",__file__,__line__, x, y, sptr->fortress );
				sptr->fortress = 12;
			}
		} /* for */
	} /* for */
} /* verify_sct() */

void
verifydata( __file__, __line__ )
char	__file__[];
int	__line__;
{
	/* check for invalid values */
	verify_ntn( __file__, __line__ );
	verify_sct( __file__, __line__ );
}/* verifydata() */

#ifdef DEBUG
void
checkout(file,line)
int	line;
char	*file;
{
	fprintf(stderr,"file %s line %d\n",file,line);
	verifydata(file,line);
}
#endif /* DEBUG */

#include <fcntl.h>
#ifdef FILELOCK
#ifdef LOCKF
#    include <unistd.h>
#    define do_lock(fd) lockf(fd,F_TLOCK,0)
#else
#    include <sys/types.h>
#    include <sys/file.h>
#    define do_lock(fd) flock(fd,LOCK_EX|LOCK_NB)
#endif
#endif /* FILELOCK */

/*
 * check_lock() -- routine to check if a file is locked.
 *   - returns true if a lock is active false is not.
 *   - setting keeplock to true means leaves lock active
 *      if it is not already active.
 */
int
check_lock(filename,keeplock)
	char *filename;
	int keeplock;
{
	int hold=FALSE;
#ifdef FILELOCK
	int fd;

	if ((fd=open(filename,O_WRONLY|O_CREAT,0600))!=(-1)) {
		if(do_lock(fd)==(-1)) {
			hold=TRUE;
		}
		/* remove lock after checking */
		if(keeplock==FALSE && hold==FALSE) {
			close(fd);
			unlink(filename);
		}
	} else {
		printf("error opening lock file <%s>\n",filename);
		exit(FAIL);
	}
#else
	struct stat fst;

	if( stat( filename, &fst ) == 0 ) {
		long now;
		now = time(0);
		if (now - fst.st_mtime < TIME_DEAD*3) {
			hold=TRUE;
		} else {
			/* remove useless file */
			unlink(filename);
		}
	}
	if (hold==FALSE && keeplock==TRUE) {
		/* create lock file */
		if(open(filename,O_CREAT,0600)==(-1)) {
			printf("error opening lock file <%s>\n",filename);
			exit(FAIL);
		}
	}
#endif /* FILELOCK */
	return(hold);
}
