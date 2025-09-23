/*conquer : Copyright (c) 1988 by Ed Barlow. */

/*selection defines */
#define CH_PEOPLE	0
#define CH_TREASURY	1
#define CH_LOCATE	2
#define CH_SOLDIERS	3
#define CH_ATTACK	4
#define CH_DEFEND	5
#define CH_REPRO	6
#define CH_MOVEMENT	7
#define CH_MAGIC	8
#define CH_LEADERS	9
#define CH_RAWGOODS	10
/* how many selections there are */
#define CH_NUMBER	11

/* location defines */
#define NLRANDOM	0
#define NLFAIR		1
#define NLGOOD		2

/* direction defines */
#define ADDITION	0
#define SUBTRACTION	1

/* number of units for dwarf nations */
#define NLDGOLD		1
#define NLDRAW		1
#define NLDCIVIL	6
#define NLDMILIT	2
#define NLDREPRO	4
#define NLDMMOVE	6
#define NLDAPLUS	20
#define NLDDPLUS	20

/* number of units for elf nations */
#define NLEGOLD		1
#define NLERAW		1
#define NLECIVIL	7
#define NLEMILIT	1
#define NLEREPRO	5
#define NLEMMOVE	8
#define NLEAPLUS	10
#define NLEDPLUS	40

/* number of units for orc nations */
#define NLOGOLD		0
#define NLORAW		1
#define NLOCIVIL	8
#define NLOMILIT	2
#define NLOREPRO	8
#define NLOMMOVE	6
#define NLOAPLUS	0
#define NLODPLUS	0

/* number of units for human nations */
#define NLHGOLD		0
#define NLHRAW		1
#define NLHCIVIL	6
#define NLHMILIT	2
#define NLHREPRO	5
#define NLHMMOVE	10
#define NLHAPLUS	20
#define NLHDPLUS	10

/* the following has been placed in the header for clarity in configuration */
/* since newlogin.c is the only file to include this file, there should     */
/* be no problems; even though it is probably not a good idea.              */

/* the minimum/maximum levels for each selection */
int Minvalues[]= { 6, 0, 0, 1, 0, 0, 4, 6, 0, 4, 1 };
int Maxvalues[]= { 50, 10, 2, 20, 80, 80, 10, 30, 5, MAXARM/2, 10 };

/* the number of points spent on each selection */
int Mcost[]= { 1, 1, 1, 1, 1, 1, 3, 1, 3, 1, 1 };

/* the number of units per cost */
int Munits[]= { 1, 1, 1, 1, 8, 8, 1, 2, 1, 3, 1 };

/* The list of corresponding values of each unit */
long Mvalues[]= { 1000L, 100000L, 1L, 1000L, 1L, 1L, 1L,
	1L, 1L, 1L, 30000L };

/* values for jewels and metal (food is last in array above) */
#define NLJEWELS	15000L
#define NLMETAL		15000L

/* the old definitions */
#define	ONLPOP		1000L
#define	ONLGOLD		100000L
#define	ONLLOCCOST	1
#define	ONLSOLD		900L
#define	ONLATTACK	10
#define	ONLDEFENCE	10
#define	ONLREPRO_ORC	2	/* repro points per ONLREPCOST for orcs */
#define	ONLREPRO	1	/* repro points per ONLREPCOST for others */
#define	ONLREPCOST	3
#define	ONLMOVE		2
#define	ONLDBLCOST	1
#define	ONLMAGIC	3
/* get ONLEADER leaders for ONLEADPT points */
#define	ONLEADER	3
#define	ONLEADPT	1
