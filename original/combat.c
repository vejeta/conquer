/*conquer : Copyright (c) 1988 by Ed Barlow.
 *  I spent a long time writing this code & I hope that you respect this.
 *  I give permission to alter the code, but not to copy or redistribute
 *  it without my explicit permission.  If you alter the code,
 *  please document changes and send me a copy, so all can have it.
 *  This code, to the best of my knowledge works well,  but it is my first
 *  'C' program and should be treated as such.  I disclaim any
 *  responsibility for the codes actions (use at your own risk).  I guess
 *  I am saying "Happy gaming", and am trying not to get sued in the process.
 *                                                Ed
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "header.h"
#include "data.h"

#define	MGKNUM	32		/* number of units possible in battle */
#define	ATKR	2
#define	DFND	1
#define	NTRL	0
#define	WIMP	3

extern	FILE	*fpmsg;
extern	FILE	*fnews;
extern	short	country;

short	retreatside;	/* ATKR, DFND, or none (0) */
short	retreatx;	/* retreat x square */
short	retreaty;	/* retreat y square */

int	unit[MGKNUM];		/*armynum*/
int	owner[MGKNUM];		/*owner*/
int	side[MGKNUM];		/*see definitions->1=units 2=unit*/
long	troops[MGKNUM];		/*starting troops in army */
int	xspot,yspot;		/*location of battles*/
int	anation;		/*nation attacking in this fight*/
int	dnation;		/*one nation defending in this fight*/
int	count=0;                /*number of armies or navies in sector*/

/* indicators of naval or army combat */
#define COMBAT_X	0
#define COMBAT_A	1
#define COMBAT_N	2
#define FOUGHT_A	4
#define FOUGHT_N	8

/************************************************************************/
/*	COMBAT()	run all combat on the map			*/
/*  	for each sector, determine if armies in with attack mode	*/
/************************************************************************/
void
combat()
{
	register int i,j;
	char	**fought; 		/* SET: if already fought in sctr */
	int	temp,ctry;
	int	initialized=FALSE;	/* TRUE if arrays initialized */
	short	armynum,nvynum;
	int	valid;
	struct  s_nation *nptr;
	struct  army	 *aptr;

	fought = (char **) m2alloc(MAPX,MAPY,sizeof(char));

	printf("Run Combat Routines\n");
	fprintf(fnews,"4\tBATTLE SUMMARY STATISTICS\n");
	/*for each nation, if in attack mode run a check*/

	/* no sectors have been fought in yet */
	for(i=0;i<MAPX;i++) for(j=0;j<MAPY;j++) fought[i][j]=COMBAT_X;

	for(ctry=NTOTAL-1;ctry>0;ctry--) if(isactive(ntn[ctry].active)) {

		nptr = &ntn[ctry];

		/*army combat*/
		for(j=0;j<MAXARM;j++) {
			aptr = &nptr->arm[j];
			if((aptr->sold>0)
			&&(aptr->stat>=ATTACK)
			&&(aptr->stat<=SORTIE||aptr->stat>=NUMSTATUS)
			&&(!(fought[aptr->xloc][aptr->yloc]&COMBAT_A))){

			/* someone can initiate combat in xspot,yspot */
			xspot=aptr->xloc;
			yspot=aptr->yloc;
			fought[xspot][yspot]|=COMBAT_A;

			/*initialize matrix*/
			if( !initialized ) {
				for(temp=0;temp<MGKNUM;temp++){
					unit[temp]=owner[temp]=(-1);
					side[temp]=NTRL;
					troops[temp]=0;
				}
				initialized=TRUE;
			}

			/*check all armies in sector and add to matrix*/
			count=0;
			valid=FALSE;
			/*is valid,set matrix*/
			for(country=0;country<NTOTAL;country++)
			if(isactive(ntn[country].active))
			for(armynum=0;armynum<MAXARM;armynum++)
			if((ASOLD>0)
			&&(ASTAT!=SCOUT)
			&&(AXLOC==xspot)
			&&(AYLOC==yspot)
			&&(count<MGKNUM)) {
				if((country!=ctry)
				&&(nptr->dstatus[country]>HOSTILE)) {
					valid=TRUE;
					if( sct[xspot][yspot].owner==ctry ) {
						dnation=ctry;
						anation=country;
					} else if(( rand()%2==0 )
					||( sct[xspot][yspot].owner==country )){
						anation=ctry;
						dnation=country;
					} else {
						dnation=ctry;
						anation=country;
					}
				}
				unit[count]=armynum;
				owner[count]=country;
				count++;
			}

			if(valid==TRUE) {
				fight();
				initialized=FALSE;
			}
		}
		}

		/*navy combat*/
		for(j=0;j<MAXNAVY;j++)
		if((nptr->nvy[j].warships!=0)
		&&(!(fought[nptr->nvy[j].xloc][nptr->nvy[j].yloc]&COMBAT_N))) {

			xspot=nptr->nvy[j].xloc;
			yspot=nptr->nvy[j].yloc;
			fought[xspot][yspot]|=COMBAT_N;

			/*initialize matrix*/
			if( !initialized ){
				for(temp=0;temp<MGKNUM;temp++){
					unit[temp]= owner[temp]=(-1);
					side[temp]=NTRL;
					troops[temp]=0;
				}
				initialized=TRUE;
			}

			/*check all fleets in 2 sector range and add to matrix*/
			count=0;
			valid=FALSE;
			/*is valid,set matrix*/
			for(country=0;country<NTOTAL;country++)
			if(isactive(ntn[country].active))
			for(nvynum=0;nvynum<MAXNAVY;nvynum++)
			if((NWSHP+NMSHP+NGSHP!=0)
			&&(((NXLOC==xspot) && (NYLOC==yspot)) ||
			   (sct[NXLOC][NYLOC].altitude==WATER
			    &&(abs(NXLOC-xspot)<=2)
			    &&(abs(NYLOC-yspot)<=2)))
			&&(count<MGKNUM)) {
				fought[NXLOC][NYLOC]|=COMBAT_N;
				if((country!=ctry)
				&&(nptr->dstatus[country]>HOSTILE)){
					valid=TRUE;
					anation=ctry;
					dnation=country;
				}
				unit[count]=nvynum;
				owner[count]=country;
				count++;
			}
			if(valid==TRUE) {
				navalcbt();
				initialized=FALSE;
			}
		}
	}
	free(fought);
	printf("\nall army and navy attacks completed\n");
}

/* macro for owner, accounts for runaway indicator */
#define UOWNER(x) ((owner[(x)]<(-1))?(-owner[(x)]-1):(owner[(x)]))
/************************************************************************/
/*	FIGHT()	-	fight an individual battle given the three	*/
/*	matricies global to this module					*/
/************************************************************************/
void
fight()
{
	int	roll,strength,fortdam=FALSE;
	int	odds;			/* odds (asold/dsold) times 100 */
	int	done;
	int	i,j,k;
	long	asold=0,dsold=0;	/*a's and d's total soldiers*/
	float astr=0,dstr=0;		/*a's and d's relative strength*/
	long	Aloss,Dloss;    	/*a's and d's total losses*/
	int	PAloss,PDloss;		/*percent a and d loss*/
	long	loss;
	int	abonus=0,dbonus=0;	/* bonus aggregate */
	long	vampire=0;		/* # non vamps deaded */
	short	nvamps=0;		/* number of vampire armies */

	/* determine who is attacker & who is on defenders side?*/
	for(j=0;j<count;j++) if(owner[j]!=(-1)){
		if(owner[j]==anation) side[j]=ATKR;
		else if(owner[j]==dnation) side[j]=DFND;
		else if(ntn[anation].dstatus[owner[j]]==JIHAD) side[j]=DFND;
		else if(ntn[owner[j]].dstatus[anation]==JIHAD) side[j]=DFND;
		else if(ntn[anation].dstatus[owner[j]]==WAR)   side[j]=DFND;
		else if(ntn[owner[j]].dstatus[anation]==WAR)   side[j]=DFND;
		else if((ntn[owner[j]].dstatus[anation]==TREATY)&&(ntn[owner[j]].dstatus[dnation]>HOSTILE)) side[j]=ATKR;
		else if((ntn[owner[j]].dstatus[anation]==ALLIED)&&(ntn[owner[j]].dstatus[dnation]>HOSTILE)) side[j]=ATKR;
	}

	/*calculate number of troops and assign statuses */
	asold=0;
	dsold=0;
	for(i=0;i<count;i++) if(owner[i]>(-1)) {
		/* record troops for all units in sector */
		troops[i]=ntn[owner[i]].arm[unit[i]].sold;

		if(((ntn[owner[i]].arm[unit[i]].unittyp == A_MERCENARY)
		||(ntn[owner[i]].arm[unit[i]].unittyp == A_ORC)
		||(ntn[owner[i]].arm[unit[i]].unittyp == A_GOBLIN))
		&&( ntn[owner[i]].arm[unit[i]].stat < NUMSTATUS )
		&&( rand()%100<15 )) {
			if( ispc(ntn[owner[i]].active)) {
				if (mailopen( owner[i] )!=(-1)) {
				fprintf(fm,"Message to %s from Conquer\n\n",ntn[owner[i]].name);
				fprintf(fm,"  Your %s Army %d Refuses to Fight\n",
				  unittype[ntn[owner[i]].arm[unit[i]].unittyp],
				  unit[i]);
				mailclose(owner[i]);
				}
			}
			retreatside = side[i];
			fdxyretreat();
			if((retreatx==xspot)&&(retreaty==yspot)){
				/* move to capitol & kill 30% */
				ntn[owner[i]].arm[unit[i]].xloc=ntn[owner[i]].capx;
				ntn[owner[i]].arm[unit[i]].yloc=ntn[owner[i]].capy;
				ntn[owner[i]].arm[unit[i]].sold*=7;
				ntn[owner[i]].arm[unit[i]].sold/=10;
			} else {
				/* retreat normally and kill 20% */
				ntn[owner[i]].arm[unit[i]].sold*=8;
				ntn[owner[i]].arm[unit[i]].sold/=10;
				retreat( i );
			}
			owner[i]=(-1-owner[i]);
			continue;
		}
		else if(side[i]==ATKR){
			if ((ntn[owner[i]].arm[unit[i]].stat >= ATTACK)
			 &&(ntn[owner[i]].arm[unit[i]].stat <= SORTIE
			 ||ntn[owner[i]].arm[unit[i]].stat >= NUMSTATUS)) {
				asold += ntn[owner[i]].arm[unit[i]].sold;
			} else {
				side[i]=NTRL;
			}
			/* sortie 20% bonus in odds */
			if(ntn[owner[i]].arm[unit[i]].stat==SORTIE)
				asold += ntn[owner[i]].arm[unit[i]].sold/5;
		} else if(side[i]==DFND){
			if(ntn[owner[i]].arm[unit[i]].stat!=RULE) {
				dsold += ntn[owner[i]].arm[unit[i]].sold;
			}
		}
		if((magic(owner[i],VAMPIRE)==TRUE) &&
		(ntn[owner[i]].arm[unit[i]].unittyp==A_ZOMBIE)) nvamps++;
	}

	if(asold<=0) {
		printf("\nCombat aborted due to lack of attackers.\n");
		return;
	}
	if( asold > dsold*100)		odds=10000;
	else if( dsold > asold*100 )	odds=1;
	else	odds = (asold*100)/dsold;

	/* mercenaries/orcs/goblins might run away */
	for(i=0;i<count;i++) if(owner[i]>(-1)) {
		if(((( odds > 200 )&&(side[i]==DFND))
		||(( odds < 100 )&&(side[i]==ATKR)))
		&&((ntn[owner[i]].arm[unit[i]].unittyp == A_MERCENARY)
		  ||(ntn[owner[i]].arm[unit[i]].unittyp == A_ORC)
		  ||(ntn[owner[i]].arm[unit[i]].unittyp == A_GOBLIN))
		&&(ntn[owner[i]].arm[unit[i]].stat < NUMSTATUS)
		&&( rand()%100<30 )) {
			if( ispc(ntn[owner[i]].active)) {
				if(mailopen( owner[i] )!=(-1)) {
				fprintf(fm,"Message to %s from Conquer\n\n",ntn[owner[i]].name);
				fprintf(fm,"  Your %s Army %d Runs Away\n",
				  unittype[ntn[owner[i]].arm[unit[i]].unittyp],
				  unit[i]);
				mailclose(owner[i]);
				}
			}
			retreatside = side[i];
			if( side[i] == ATKR ) asold-= troops[i];
			if( side[i] == DFND ) dsold-= troops[i];
			fdxyretreat();
			if((retreatx==xspot)&&(retreaty==yspot)){
				/* move to capitol & kill 75% */
				ntn[owner[i]].arm[unit[i]].xloc=ntn[owner[i]].capx;
				ntn[owner[i]].arm[unit[i]].yloc=ntn[owner[i]].capy;
				ntn[owner[i]].arm[unit[i]].sold/=4;
			} else {
				/* retreat normally and kill 50% */
				ntn[owner[i]].arm[unit[i]].sold/=2;
				retreat( i );
			}
			owner[i]=(-1-owner[i]);
			continue;
		}
	}
	retreatside=0;
	if( asold<=0 ) {
		printf("Exit from battle due to lack of attackers\n");
		return;
	}

	/* CALCULATE AVERAGE COMBAT BONUS */
	abonus=0;
	dbonus=0;
	for(i=0;i<count;i++) if(owner[i]>(-1)) {
		if(side[i]==ATKR)
			abonus += cbonus(i)*troops[i];
		else if(side[i]==DFND && ntn[owner[i]].arm[unit[i]].stat!=RULE)
			dbonus += cbonus(i)*troops[i];
	}

	/*archer bonus if not in fort vs knights/cavalry*/
	j=0;
	k=0;
	for(i=0;i<count;i++) if(owner[i]>(-1))
	if(ISCITY(sct[xspot][yspot].designation)){
		if((ntn[owner[i]].arm[unit[i]].unittyp == A_CAVALRY)
		||(ntn[owner[i]].arm[unit[i]].unittyp == A_KNIGHT))
		if(side[i]==ATKR) j+=troops[i];
		else if(side[i]==DFND) k+=troops[i];
	}

	for(i=0;i<count;i++) if(owner[i]>(-1)) {
		if(j>0) abonus += (15 * j * troops[i]) / asold;
		if(k>0 && dsold>0) dbonus += (15 * k * troops[i]) / dsold;
	}

	abonus/=asold;
	if (dsold>0) dbonus/=dsold;

	/*CALCULATED BONUSES TO WHOLE COMBAT*/
	for(i=0;i<count;i++) if(owner[i]>(-1)) {
		if(fort_val(&sct[xspot][yspot]) != 0){
			/*Catapults add +1%/20 men defending castle (max +10%)*/
			if((ntn[owner[i]].arm[unit[i]].unittyp == A_CATAPULT)
			&&(side[i]==DFND))
				dbonus += max((troops[i]/20),10);
			/*Catapults add +1%/40 men attacking castle (max +10%)*/
			else if((ntn[owner[i]].arm[unit[i]].unittyp == A_CATAPULT)
			&&(side[i]==ATKR)) {
				strength = max((troops[i]/40),10);
				abonus += strength;
				/* possible damage 20% chance */
				if(rand()%100<2*strength) {
					fortdam=TRUE;
					sct[xspot][yspot].fortress--;
					if(sct[xspot][yspot].fortress == 0)
					sct[xspot][yspot].designation = DRUIN;
				}
			}
			/*Siege_engines add +1%/20 men when attacking fortress*/
			else if((ntn[owner[i]].arm[unit[i]].unittyp == A_SIEGE)
			&&(side[i]==ATKR)) {
				strength = max((troops[i]/20),30);
				abonus += strength;
				/* possible damage 15% chance */
				if(rand()%100<strength/2) {
					fortdam=TRUE;
					sct[xspot][yspot].fortress--;
					if(sct[xspot][yspot].fortress == 0)
						sct[xspot][yspot].designation = DRUIN;
				}
			}
		} else {
			/*Catapults add +1%/40 men normal combat (max +10%)*/
			if(ntn[owner[i]].arm[unit[i]].unittyp == A_CATAPULT)
				abonus+=max((troops[i]/40),10);
		}
	}

	/*RUN COMBAT */
	/*FIRST GIVE RANDOM ROLL FROM 0 to 100 */
	/*WITH A PROBABILITY BELL CURVE */
	/* high roll favors attacker [ 5 d21 - 5 ] */
	roll = 0;
	for(i=0;i<5;i++) {
		roll += rand()%21+1;
	}
	roll -= 5;

	/*find relative strength of troops*/
	astr = asold * (100 + abonus);
	dstr = dsold * (100 + dbonus);

	/*Recalculate odds based on quality of troops*/
	if( astr > dstr*100)		odds=10000;
	else if( dstr > astr*100 )	odds=1;
	else	odds = (astr*100)/dstr;

	/* calculate loss for an even battle */
	PDloss = MAXLOSS * roll / 100;
	PAloss = MAXLOSS * (100 - roll) / 100;

	/* adjust for odds */
	if( odds == 1 ) {
		PDloss=0;
		PAloss=200;
	} else if( odds == 10000 ) {
		PAloss=0;
		PDloss=200;
	} else if(odds > 100) {
		PDloss += (odds / 12 - 8);	/* 8.33% for higher odds */
		PAloss -= (odds / 16 - 6);	/* 6.25% for lower odds */
		if(PAloss<(100-roll)/20)
			PAloss=(100-roll)/20;	/* can't get too small */
	} else {
		PAloss += ( 800 / odds - 8);	/* 8% for higher odds */
		PDloss -= ( 600 / odds - 6);	/* 6% for lower odds */
		if(PDloss<roll/20)
			PDloss = roll/20;	/* can't get too small */
	}

	if( fort_val(&sct[xspot][yspot]) > 0 ){
		PDloss *= 120;
		PAloss *= 120;
		PDloss /= 100;
		PAloss /= 100;
	}

	retreatside = 0;

	if((PDloss > 2* PAloss)
	&&(odds>150)
	&&(((PDloss>=50)&&(rand()%4==0))
	  ||(rand()%8)))	retreatside=DFND;

	if((PAloss > 2* PDloss)
	&&(odds<150)
	&&(((PAloss>=50)&&(rand()%2==0))
	  ||(rand()%6)))	retreatside=ATKR;

	if(retreatside!=0) {
		fdxyretreat();
		/* no legal retreat route */
		if((retreatside!=0) && (retreatx== xspot)
		&& (retreaty== yspot)){
			if(retreatside==ATKR) PAloss+=15;
			else if(retreatside==DFND) PDloss+=15;
#ifdef DEBUG
			printf("side %d (%d %d) can't retreat...+15%% loss\n",retreatside,retreatx,retreaty);
#endif /* DEBUG */
			retreatside = 0;
		}
#ifdef DEBUG
		else printf("retreat side %d to %d %d\n",retreatside,retreatx,retreaty);
#endif /* DEBUG */
	}

	if(PAloss>100) PAloss = 100;
	if(PDloss>100) PDloss = 100;

	Aloss = Dloss = 0;
	for(i=0;i<count;i++) if(owner[i]>(-1)){
		if(side[i]==ATKR){
			if( ntn[owner[i]].arm[unit[i]].unittyp >= MINLEADER) {
				if((rand()%100) < PAloss){ /* kill it */
					for(j=0;j<MAXARM;j++)
					if(ntn[owner[i]].arm[j].stat==unit[i]+NUMSTATUS)
					ntn[owner[i]].arm[j].stat=ATTACK;
					Aloss += troops[i];
					ntn[owner[i]].arm[unit[i]].sold=0;
				}
			} else {
				loss=(troops[i]*PAloss)/100;
				/*archers/catapults on sortie take 1/4 damage*/
				if((ntn[owner[i]].arm[unit[i]].stat==SORTIE)
				&&(fort_val(&sct[xspot][yspot]) > 0)
				&&(sct[xspot][yspot].owner==country)
				&&((ntn[owner[i]].arm[unit[i]].unittyp==A_ARCHER)
				||(ntn[owner[i]].arm[unit[i]].unittyp==A_CATAPULT)))
					loss /= 4;
				/*army can't have less than 25 men in it*/
				if(troops[i]-loss<25)
					loss=troops[i];
if( loss>troops[i] ) {
printf("I AM VERY CONFUSED - PLEASE HELP... combat.c\n");
}
				Aloss+=loss;
				ntn[owner[i]].arm[unit[i]].sold-=loss;
				if((ntn[owner[i]].arm[unit[i]].unittyp==A_MILITIA)&&(retreatside==ATKR)) {
					sct[ntn[owner[i]].arm[unit[i]].xloc][ntn[owner[i]].arm[unit[i]].yloc].people += ntn[owner[i]].arm[unit[i]].sold;
					ntn[owner[i]].arm[unit[i]].sold=0;
				}
			}
		} else if(side[i]==DFND){
			if( ntn[owner[i]].arm[unit[i]].unittyp >= MINLEADER) {
				if((ntn[owner[i]].arm[unit[i]].stat!=RULE
				||PDloss>=80)&&((rand()%100) < PDloss)){ /* kill it */
					for(j=0;j<MAXARM;j++)
					if(ntn[owner[i]].arm[j].stat==unit[i]+NUMSTATUS)
					ntn[owner[i]].arm[j].stat=ATTACK;
					Dloss +=troops[i];
					ntn[owner[i]].arm[unit[i]].sold=0;
				}
			} else {
				loss=(troops[i]*PDloss)/100;
				/*destroy army if < 25 men*/
				if(troops[i]-loss<25)
					loss=troops[i];
				Dloss+=loss;
				ntn[owner[i]].arm[unit[i]].sold-=loss;
				if((ntn[owner[i]].arm[unit[i]].unittyp==A_MILITIA)&&(retreatside==DFND)) {
					sct[ntn[owner[i]].arm[unit[i]].xloc][ntn[owner[i]].arm[unit[i]].yloc].people += ntn[owner[i]].arm[unit[i]].sold;
					ntn[owner[i]].arm[unit[i]].sold=0;
				}
			}
		}
		/* non-vampire troops are sucked in by vampires */
		if((nvamps>0)&&(magic(owner[i],VAMPIRE)==FALSE)
		&&(ntn[owner[i]].arm[unit[i]].unittyp!=A_ZOMBIE)
		&&(ntn[owner[i]].arm[unit[i]].unittyp<MINLEADER))
			vampire+= loss / 3;
	}
	/* use k variable to hold length */
#ifdef HIDELOC
	if( isntn( ntn[sct[xspot][yspot].owner].active )) {
	fprintf(fnews,"4.\tBattle occurs in %s", ntn[sct[xspot][yspot].owner].name);
	k = 27+strlen(ntn[sct[xspot][yspot].owner].name);
	} else {
	fprintf(fnews,"4.\tBattle on unowned land");
	k = 30;
	}
#else
	fprintf(fnews,"4.\tBattle in %d,%d",xspot,yspot);
	k = 25;
#endif
	for(j=0;j<count;j++) if(UOWNER(j)>(-1)){
		done=FALSE;
		for(i=0;i<j;i++) if(UOWNER(j)==UOWNER(i)) done=TRUE;
		if(done==FALSE) {
			loss=NTRL;
			for(i=j;(loss==NTRL||loss==WIMP) && i<count;i++)
				if(UOWNER(i)==UOWNER(j)) {
					if(owner[i]<(-1)) loss=WIMP;
					else loss=side[i];
				}
			if(loss!=NTRL) {
				k += 11 + strlen(ntn[UOWNER(j)].name);
				if(loss==WIMP) k++;
				if(k>79) {
					k = 30;
					fprintf(fnews,",\n4.\t  ");
				} else fprintf(fnews,", ");
				if(loss==ATKR)
					fprintf(fnews,"attacker %s",ntn[UOWNER(j)].name);
				else if(loss==DFND)
					fprintf(fnews,"defender %s",ntn[UOWNER(j)].name);
				else if(loss==WIMP)
					fprintf(fnews,"retreater %s",ntn[UOWNER(j)].name);
			}
		}
	}
	fprintf(fnews,"\n");
	if(nvamps>0){
		for(i=0;i<count;i++) if(owner[i]>(-1)){
			if((magic(owner[i],VAMPIRE)==TRUE)
			&&(ntn[owner[i]].arm[unit[i]].unittyp==A_ZOMBIE)
			&&(ntn[owner[i]].arm[unit[i]].sold > 0))
				ntn[owner[i]].arm[unit[i]].sold+=vampire/nvamps;
		}
	}

	/*who is in the battle; but don't send to scared armies */
	for(j=0;j<count;j++) if(owner[j]>(-1)){
		done=FALSE;

		/*first time your nation appears done=FALSE*/
		for(i=0;i<j;i++) if(owner[j]==owner[i]) done=TRUE;

		if((done==FALSE)&&(ispc(ntn[owner[j]].active))) {

			loss=NTRL;
			for(i=j;loss==NTRL && i<count;i++)
				loss=side[i];
			if (mailopen( owner[j] )==(-1)) continue;

			fprintf(fm,"BATTLE SUMMARY for sector %d, %d\n",xspot,yspot);
			fprintf(fm,"Battle occured during %s of Year %d\n",PSEASON(TURN),YEAR(TURN));

			if(loss==ATKR)
				fprintf(fm,"You are on the Attacking Side\n");
			else if(loss==DFND)
				fprintf(fm,"You are on the Defending Side\n");
			else	fprintf(fm,"You are Neutral\n");

			/*detail all participants in battle*/
			for(k=0;k<count;k++) if(owner[k]!=(-1)){
				fprintf(fm," %s ",ntn[UOWNER(k)].name);
				if(owner[k]<(-1))
					fprintf(fm,"chickens out: ");
				else if(side[k]==DFND
				&& ntn[owner[k]].arm[unit[k]].stat!=RULE)
					fprintf(fm,"defending: ");
				else if(side[k]==ATKR)
					fprintf(fm,"attacking: ");
				else if(side[k]==NTRL
				|| (side[k]==DFND
				    && ntn[owner[k]].arm[unit[k]].stat==RULE))
					fprintf(fm,"neutral: ");
				else
					fprintf(fm,"in limbo: ");
			
				fprintf(fm,"army %d (%s, men %d, bonus=%d, loss=%d)",
					unit[k],
					unittype[ntn[UOWNER(k)].arm[unit[k]].unittyp%UTYPE],
					troops[k],
					cbonus(k),
					troops[k]-ntn[UOWNER(k)].arm[unit[k]].sold);
				if((ntn[UOWNER(k)].arm[unit[k]].unittyp >= MINLEADER)
				&&( ntn[UOWNER(k)].arm[unit[k]].sold == 0))
					fprintf(fm," (killed)\n");
				else if((ntn[UOWNER(k)].arm[unit[k]].unittyp == A_MILITIA)
				&&( ntn[UOWNER(k)].arm[unit[k]].sold == 0))
				fprintf(fm," (disbanded)\n");
				else fputc('\n',fm);
			}

			fprintf(fm,"attacking soldiers=%ld -> percent loss %d%%\n",asold,PAloss);
			fprintf(fm,"defending soldiers=%ld -> percent loss %d%%\n",dsold,PDloss);
			fprintf(fm,"ODDS=%d => adjusted to %d to 100; Die Roll is %d\n",odds*(100+dbonus)/(100+abonus),odds,roll);
			fprintf(fm,"RESULT: Attackers lose %ld men, Defenders lose %ld men\n",Aloss, Dloss);
			if(fortdam==TRUE) fprintf(fm,"Fortifications damaged during the attack\n");
			if(retreatside==ATKR){
			if(Aloss<asold)
			fprintf(fm,"Additionally, All attackers retreat to %d %d\n",retreatx,retreaty);
			} else if(retreatside==DFND){
			if(Dloss<dsold)
			fprintf(fm,"Additionally, All defenders retreat to %d %d\n",retreatx,retreaty);
			}
			mailclose(owner[j]);
		}
	}
	retreat( -1 );
}

/************************************************************************/
/*	CBONUS() - return combat bonuses for unit i			*/
/************************************************************************/
int
cbonus(num)
{
	short	armynum;
	int	armbonus;

	armbonus=0;
	armynum=unit[num];
	country=UOWNER(num);

	/*Racial combat bonus due to terrain (the faster you move the better)*/
	armbonus+=5*(9-movecost[xspot][yspot]);	/* this line always has */
				/* the same result... must fix -- ADB */

	if(((magic(country,DESTROYER)==1)
	||(magic(country,DERVISH)==1))
	&&((sct[xspot][yspot].vegetation==ICE)
	||(sct[xspot][yspot].vegetation==DESERT)))
		armbonus+=30;

	if(ASTAT>=NUMSTATUS) armbonus+=20;	/* army group */
	if(side[num]==DFND){

		if(sct[xspot][yspot].altitude==MOUNTAIN) armbonus+=20;
		else if(sct[xspot][yspot].altitude==HILL) armbonus+=10;

		if(sct[xspot][yspot].vegetation==JUNGLE) armbonus+=20;
		else if(sct[xspot][yspot].vegetation==FOREST) armbonus+=15;
		else if(sct[xspot][yspot].vegetation==WOOD) armbonus+=10;

		if(ATYPE==A_MERCENARY) armbonus += MERCDEF;
		else armbonus += ntn[UOWNER(num)].dplus;

		if(ASTAT==MAGDEF) armbonus+=30;
		else if(ASTAT==SORTIE) armbonus-=30;
		else if(ASTAT==SIEGED) armbonus-=20;

		if((sct[xspot][yspot].owner==country)
		&&(ASTAT==GARRISON||ASTAT==MILITIA||ASTAT==SIEGED)){
			if(ATYPE == A_ZOMBIE) /* don't utilize walls well */
				armbonus += fort_val(&sct[xspot][yspot])/2;
			else armbonus += fort_val(&sct[xspot][yspot]);
		}
	}
	else if(side[num]==ATKR) {
		if( (fort_val(&sct[xspot][yspot]) > 0)
		&&( magic(country,SAPPER)==TRUE)) armbonus += 10;

		if(ATYPE == A_MERCENARY) armbonus += MERCATT;
		else armbonus += ntn[UOWNER(num)].aplus;
		if(ASTAT==MAGATT) armbonus += 30;
		if(ASTAT==SORTIE && (fort_val(&sct[xspot][yspot]) > 0)
		&& sct[xspot][yspot].owner==country) {
			armbonus += 10;
			if((ATYPE==A_DRAGOON)||(ATYPE==A_LEGION)
			||(ATYPE==A_PHALANX)) {
				/* bonus for organization or riding cavalry */
				armbonus += 5;
			} else if ((ATYPE==A_LT_CAV)||(ATYPE==A_CAVALRY)) {
				/* bonus for mounted sortie */
				armbonus += 10;
			} else if (avian(ATYPE)||ATYPE==A_ELEPHANT||ATYPE==A_KNIGHT) {
				/* bonus for mounted or flying sortie */
				armbonus += 15;
			}
			if ((ATYPE>=MINMONSTER)||(ATYPE<=MAXMONSTER)) {
				/* bonus for monsters (scare factor) */
				armbonus += 5;
			}
		}
	}

	/*army status is important*/
	if(ASTAT==MARCH) armbonus-=40;

	/*if a fortress*/
	if(fort_val(&sct[xspot][yspot]) > 0){
		/*Cavalry and Knights get -20%*/
		if((ATYPE == A_CAVALRY) ||(ATYPE == A_KNIGHT)) armbonus -= 20;
		/*Archers gain pluses*/
		else if((ATYPE == A_ARCHER)&&(sct[xspot][yspot].owner==country))
			armbonus += 15;
		else if(ATYPE == A_ARCHER) armbonus += 5;
	}

	if(side[num]==ATKR) armbonus+= *(unitattack+(ATYPE%UTYPE));
	else armbonus+= *(unitdefend+(ATYPE%UTYPE));

	/*Phalanx and Legionaires need certain numbers of troops*/
	if((ATYPE==A_PHALANX)||(ATYPE==A_LEGION)) {
 		if(ASOLD>1000){	armbonus+=20;
 		} else if(ASOLD>500) armbonus+=10;
	}

	return(armbonus);
}

void
fdxyretreat()	/* finds retreat location */
{
	int	x,y,nation=(-1);
	int	xsctr= xspot;
	int	ysctr= yspot;

	retreatx=xsctr;
	retreaty=ysctr;

	if((sct[xsctr][ysctr].designation==DTOWN)
	||(sct[xsctr][ysctr].designation==DCAPITOL)
	||(sct[xsctr][ysctr].designation==DCITY)){
		retreatside=0;
		return;
	}

	if(retreatside == ATKR) nation=anation;
	else nation=dnation;

	for(x= xsctr-1; x<=xsctr+1; x++)
	for(y= ysctr-1; y<=ysctr+1; y++) if(ONMAP(x,y)){
		if(tofood( &sct[x][y],
			sct[x][y].owner == country ? country : 0)==0) continue;
		if(((sct[x][y].owner == nation)
		   ||(ntn[sct[x][y].owner].dstatus[nation] < NEUTRAL))
		||(solds_in_sector( x, y, sct[x][y].owner) == 0)){
			retreatx=x;
			retreaty=y;
#ifdef DEBUG
			printf("armies in %d %d retreat to %d %d\n",xsctr,ysctr,x,y);
#endif /* DEBUG */
			return;
		}
	}
}

void
retreat(unitnum)
int	unitnum;	/* if -1 then normal, else retreat only unit ismerc */
{
	int cnum;

	if(retreatside == 0) return;

	for(cnum=0;cnum<count;cnum++) if(owner[cnum]>(-1)){
		if( unitnum != (-1) ) cnum=unitnum;
		if(side[cnum] == retreatside){
			if ((ntn[owner[cnum]].arm[unit[cnum]].unittyp==A_MARINES)||
			    (ntn[owner[cnum]].arm[unit[cnum]].unittyp==A_SAILOR)){
				ntn[owner[cnum]].arm[unit[cnum]].sold *= 85;
				ntn[owner[cnum]].arm[unit[cnum]].sold /= 100;
			} else {
				ntn[owner[cnum]].arm[unit[cnum]].xloc = retreatx;
				ntn[owner[cnum]].arm[unit[cnum]].yloc = retreaty;
			}
		}
		if( unitnum != (-1) ) return;
	}
}


/*SUBROUTINE TO RUN NAVAL COMBAT ON ALL SHIPS */
/* quick define for easier reading */
#define QWAR 1
#define QGAL 2
#define QMER 3
/* just like fight, this takes array of owner,side,unit and calculates */
/* a random battle based on the strengths of the combatants.           */
void
navalcbt()
{
	int acrew=0,dcrew=0;	/*a's and d's crew and soldier strength*/
	int ahold=0,dhold=0;	/*a's and d's warship strength*/
	int awsunk=0,dwsunk=0;	/*a's and d's warship losses for the round*/
	int agsunk=0,dgsunk=0;	/*a's and d's galley losses for the round*/
	int amsunk=0,dmsunk=0;	/*a's and d's merchent losses for the round*/
	int awcapt=0,dwcapt=0;	/*a's and d's warship captures for the round*/
	int agcapt=0,dgcapt=0;	/*a's and d's galley captures for the round*/
	int amcapt=0,dmcapt=0;	/*a's and d's merchant captures for the round*/
	int akcrew=0,dkcrew=0;	/*a's and d's crew losses for the round*/
	char wnum[MGKNUM],gnum[MGKNUM],mnum[MGKNUM];
	register int done,i,j,k;
	int roll,odds,savecntry=country;
	int PAloss, PDloss, Ploss, which, shipsize;
	int thold, ghold, nvynum, armynum;
	int dcptpct, acptpct, cptpct;
	struct s_nation *saventn=curntn;
	void show_ships(),capture();

	printf("In Naval Combat....\n");

	/* determine who is attacker & who is on defenders side?*/
	for(j=0;j<count;j++) if(owner[j]!=(-1)){
		if(owner[j]==anation) side[j]=ATKR;
		else if(ntn[anation].dstatus[owner[j]]==JIHAD) side[j]=DFND;
		else if(ntn[owner[j]].dstatus[anation]==JIHAD) side[j]=DFND;
		else if(ntn[anation].dstatus[owner[j]]==WAR)   side[j]=DFND;
		else if(ntn[owner[j]].dstatus[anation]==WAR)   side[j]=DFND;
		else if((ntn[owner[j]].dstatus[anation]==TREATY)
		&&(ntn[owner[j]].dstatus[dnation]>HOSTILE)) side[j]=ATKR;
		else if((ntn[owner[j]].dstatus[anation]==ALLIED)
		&&(ntn[owner[j]].dstatus[dnation]>HOSTILE)) side[j]=ATKR;
	}

	/* Loop through all competitors to determine
	 *  relative combat strengths:
	 *                              men/unit strength
	 * crew on ship:
	 *     warship crew                     1
	 *     galley crew                      2
	 *     merchant crew                    4
	 * soldiers onboard:
         *     SAILOR/ARCHER                   3/4
         *     MARINE                          1/3
	 *     others                          4/3
	 */
	for(j=0;j<count;j++) if(owner[j]!=(-1)){
		curntn= &ntn[owner[j]];
		country= owner[j];
		wnum[j]=SHIPS(ntn[country].nvy[unit[j]].warships,N_LIGHT)+
			SHIPS(ntn[country].nvy[unit[j]].warships,N_MEDIUM)+
			SHIPS(ntn[country].nvy[unit[j]].warships,N_HEAVY);
		mnum[j]=SHIPS(ntn[country].nvy[unit[j]].merchant,N_LIGHT)+
			SHIPS(ntn[country].nvy[unit[j]].merchant,N_MEDIUM)+
			SHIPS(ntn[country].nvy[unit[j]].merchant,N_HEAVY);
		gnum[j]=SHIPS(ntn[country].nvy[unit[j]].galleys,N_LIGHT)+
			SHIPS(ntn[country].nvy[unit[j]].galleys,N_MEDIUM)+
			SHIPS(ntn[country].nvy[unit[j]].galleys,N_HEAVY);
		if(side[j]==DFND) {
			if((k=fltwhold(unit[j]))>0) {
				dhold += k;
				if (magic(country,SAILOR)==TRUE) {
					dcrew += 5*k*curntn->nvy[unit[j]].crew/4;
				} else dcrew += k*curntn->nvy[unit[j]].crew;
			}
			if((k=fltmhold(unit[j]))>0) {
				if (magic(country,SAILOR)==TRUE) {
					dcrew += 5*k*curntn->nvy[unit[j]].crew/16;
				} else dcrew += k*curntn->nvy[unit[j]].crew/4;
			}
			if((k=fltghold(unit[j]))>0) {
				if (magic(country,SAILOR)==TRUE) {
					dcrew += 5*k*curntn->nvy[unit[j]].crew/8;
				} else dcrew += k*curntn->nvy[unit[j]].crew/2;
				if(curntn->nvy[unit[j]].armynum!=MAXARM) {
					k = curntn->nvy[unit[j]].armynum;
					switch (curntn->arm[k].unittyp) {
					case A_ARCHER:
					case A_SAILOR:
						dcrew += 3*curntn->arm[k].sold/2;
						break;
					case A_MARINES:
						dcrew += 3*curntn->arm[k].sold;
						break;
					default:
						dcrew += 3*curntn->arm[k].sold/4;
						break;
					}
				}
			}
		} else if(side[j]==ATKR) {
			if((k=fltwhold(unit[j]))>0) {
				ahold += k;
				if (magic(country,SAILOR)==TRUE) {
					acrew += 5*k*curntn->nvy[unit[j]].crew/4;
				} else acrew += k*curntn->nvy[unit[j]].crew;
			}
			if((k=fltmhold(unit[j]))>0) {
				if (magic(country,SAILOR)==TRUE) {
					acrew += 5*k*curntn->nvy[unit[j]].crew/16;
				} else acrew += k*curntn->nvy[unit[j]].crew/4;
			}
			if((k=fltghold(unit[j]))>0) {
				if (magic(country,SAILOR)==TRUE) {
					acrew += 5*k*curntn->nvy[unit[j]].crew/8;
				} else acrew += k*curntn->nvy[unit[j]].crew/2;
				if(curntn->nvy[unit[j]].armynum!=MAXARM) {
					k = curntn->nvy[unit[j]].armynum;
					switch (curntn->arm[k].unittyp) {
					case A_ARCHER:
					case A_SAILOR:
						acrew += 3*curntn->arm[k].sold/2;
						break;
					case A_MARINES:
						acrew += 3*curntn->arm[k].sold;
						break;
					default:
						acrew += 3*curntn->arm[k].sold/4;
						break;
					}
				}
			}
		}
	}

	/*find battle odds*/
	if( acrew > dcrew*100 ) odds=10000;
	else if ( dcrew > acrew*100 ) odds=1;
	else odds = (acrew*100)/dcrew;

	/* calculate capture percentages */
	/*
	 *  This formula produces:
         *        0% capture for   1:100  odds
	 *        2% capture for   1:10   odds
         *       15% capture for   1:1    odds
	 *       60% capture for  10:1    odds
	 *      100% capture for >60:1    odds
	 *      with linear progression between each.
	 */
	if (odds>6000) {
		dcptpct=0;
		acptpct=100;
	} else if (odds>1000) {
		dcptpct= (6000-odds)/2500;
		acptpct= (odds-1000)/125+60;
	} else if (odds>100) {
		dcptpct= (1000-odds)/69+2;
		acptpct= (odds-100)/20+15;
	} else if (odds>10) {
		dcptpct= (100-odds)/2+15;
		acptpct= (odds-10)/6.9+2;
	} else if (odds>6) {
		dcptpct= (10-odds)*14+60;
		acptpct= (odds-6)/2;
	} else {
		dcptpct= 100;
		acptpct= 0;
	}
#ifdef DEBUG
	printf("capture percentage: attack = %d  defend = %d\n",acptpct,dcptpct);
#endif /* DEBUG */

	/*figure combat on a one-to-one basis */
	/* use a bell curve roll */
	roll = 0;
	for(i=0;i<5;i++) {
		roll += rand()%21+1;
	}
	roll -= 5;
	PDloss = MAXLOSS * roll / 100;
	PAloss = MAXLOSS * (100 - roll) / 100;
#ifdef DEBUG
	printf("Pdloss = %d  PAloss = %d\n",PDloss,PAloss);
#endif /* DEBUG */

	/* adjust based on the odds */
	if( odds == 1 ) {
		PDloss = 0;
		PAloss = 100;
	} else if ( odds == 10000 ) {
		PAloss = 0;
		PDloss = 100;
	} else if ( odds > 100 ) {
		PDloss += (odds / 10 - 10);		/* 10% increase */
		PAloss -= (odds / 25 - 4);		/* 4% decrease */
		if(PAloss<(100-roll)/5)
			PAloss=(100-roll)/5;
	} else {
		PAloss += ( 1000 / odds - 10);	/* 10% increase */
		PDloss -= ( 400 / odds - 4);		/* 4% decrease  */
		if(PDloss<roll/5)
			PDloss = roll/5;
	}
	if (PAloss>100) PAloss=100;
	if (PDloss>100) PDloss=100;

	/* calculate actual losses */
	for(j=0;j<count;j++) if(owner[j]!=(-1)){
		curntn= &ntn[owner[j]];
		country= owner[j];

		/* determine side */
		which=side[j];
		/* capturing is by foe so use other percent */
		if(which==ATKR) {
			cptpct=dcptpct;
			Ploss= PAloss;
		} else if(which==DFND) {
			cptpct=acptpct;
			Ploss= PDloss;
		} else continue;

		/* find all weighting info */
		nvynum = unit[j];
		thold = flthold(nvynum);
		ghold = fltghold(nvynum);

		/* go through all ships in the navy */
		for(shipsize=N_LIGHT;shipsize<=N_HEAVY;shipsize++) {
			/* check warships */
			for(i=0;i<P_NWAR(shipsize);i++)
			if (rand()%100<Ploss) {
				if(rand()%100<cptpct/2) {
					/* capture a ship */
					if(which==ATKR){
						if (dhold) {
							awcapt++;
							ahold-=(shipsize+1);
							capture(QWAR,DFND,shipsize,rand()%dhold+1);
							NSUB_WAR(1);
						}
					} else {
						if (ahold) {
							dwcapt++;
							dhold-=(shipsize+1);
							capture(QWAR,ATKR,shipsize,rand()%ahold+1);
							NSUB_WAR(1);
						}
					}
					if(which==ATKR) akcrew += P_NCREW;
					else dkcrew += P_NCREW;
				} else if (rand()%2==0 || Ploss>90) {
					/* destroy a ship */
					NSUB_WAR(1);
					k = P_NCREW*(shipsize+1);
					thold -= (shipsize+1);
					if(which==ATKR) {
						awsunk++;
						ahold-=(shipsize+1);
						akcrew+=k;
					} else {
						dwsunk++;
						dhold-=(shipsize+1);
						dkcrew+=k;
					}
				} else {
					/* damage a ship */
					k = (shipsize+1)*Ploss*P_NCREW/100;
					if(which==ATKR) akcrew += k;
					else dkcrew += k;
					P_NCREW -= k/thold;
				}
			}
			/* check galleys */
			for(i=0;i<P_NGAL(shipsize);i++)
			if (rand()%100<Ploss) {
				if(rand()%100<cptpct) {
					/* capture a ship */
					if(which==ATKR){
						if (dhold) {
							agcapt++;
							ghold-=(shipsize+1);
							thold-=(shipsize+1);
							capture(QGAL,DFND,shipsize,rand()%dhold+1);
							NSUB_GAL(1);
						}
					} else if(which==DFND) {
						if (ahold) {
							dgcapt++;
							ghold-=(shipsize+1);
							thold-=(shipsize+1);
							capture(QGAL,ATKR,shipsize,rand()%ahold+1);
							NSUB_GAL(1);
						}
					}
					if(which==ATKR) akcrew += P_NCREW;
					else dkcrew += P_NCREW;
					if (P_NARMY!=MAXARM) {
						armynum = P_NARMY;
						k = P_ASOLD*(shipsize+1)/(ghold+shipsize+1);
						if(P_ATYPE<MINLEADER) {
						if(which==ATKR) akcrew += k;
						else dkcrew += k;
						P_ASOLD -= k;
						} else if(rand()%100<k*100/P_ASOLD){
						if(which==ATKR) akcrew += P_ASOLD;
						else dkcrew += P_ASOLD;
						P_ASOLD = 0;
						P_NARMY = MAXARM;
						}
					}
				} else if (rand()%3==0||Ploss>90) {
					/* ship destroyed */
					k = (shipsize+1)*SHIPCREW;
					if(which==ATKR) akcrew += k;
					else dkcrew += k;
					NSUB_GAL(1);
					ghold-=(shipsize+1);
					thold-=(shipsize+1);
					/* kill all soldiers onboard */
					if (P_NARMY!=MAXARM) {
						armynum = P_NARMY;
						k = P_ASOLD*(shipsize+1)/(ghold+shipsize+1);
						if(P_ATYPE<MINLEADER) {
						if(which==ATKR) akcrew += k;
						else dkcrew += k;
						P_ASOLD -= k;
						} else if(rand()%100<k*100/P_ASOLD){
						if(which==ATKR) akcrew += P_ASOLD;
						else dkcrew += P_ASOLD;
						P_ASOLD = 0;
						P_NARMY = MAXARM;
						}
					}
					if(which==ATKR) agsunk++;
					else dgsunk++;
				} else {
					/* damage a ship */
					k = (shipsize+1)*Ploss*P_NCREW/100;
					if(which==ATKR) akcrew += k;
					else dkcrew += k;
					P_NCREW -= k/thold;
					/* damage any soldiers onboard */
					if (P_NARMY!=MAXARM) {
						armynum = P_NARMY;
						k = P_ASOLD*(shipsize+1)*Ploss/(ghold*100);
						if(P_ATYPE<MINLEADER) {
						P_ASOLD -= k;
						if(which==ATKR) akcrew += k;
						else dkcrew += k;
						} else if(rand()%100<k*100/P_ASOLD){
						if(which==ATKR) akcrew += P_ASOLD;
						else dkcrew += P_ASOLD;
						P_ASOLD = 0;
						P_NARMY = MAXARM;
						}
					}
				}
			}
			/* check merchants */
			for(i=0;i<P_NMER(shipsize);i++)
			if (rand()%100<Ploss) {
				if(rand()%100<cptpct) {
					/* capture a ship */
					if(which==ATKR){
						if (dhold) {
							amcapt++;
							capture(QMER,DFND,shipsize,rand()%dhold+1);
							NSUB_MER(1);
						}
					} else if(which==DFND) {
						if (ahold) {
							dmcapt++;
							capture(QMER,ATKR,shipsize,rand()%ahold+1);
							NSUB_MER(1);
						}
					}
					if(which==ATKR) akcrew += P_NCREW;
					else dkcrew += P_NCREW;
				} else if (rand()%3==0 || Ploss>90) {
					/* ship destroyed */
					k = (shipsize+1)*P_NCREW;
					if(which==ATKR) akcrew += k;
					else dkcrew += k;
					P_NCREW -= k/thold;
					k = P_NCREW*thold;
					NSUB_MER(1);
					if((thold-=(shipsize+1))!=0) P_NCREW = k / thold;
					else P_NCREW = 0;
					/* kill all people onboard */
					k = (shipsize+1)*P_NPEOP;
					if(which==ATKR) akcrew += k;
					else dkcrew += k;
					P_NPEOP -= k/(thold+shipsize+1);
					k = P_NPEOP*(thold+shipsize+1);
					if (thold>0) P_NPEOP = k / thold;
					else P_NPEOP=0;
					if(which==ATKR) amsunk++;
					else dmsunk++;
				} else {
					/* damage a ship */
					k = (shipsize+1)*Ploss*P_NCREW/100;
					if(which==ATKR) akcrew += k;
					else dkcrew += k;
					P_NCREW -= k/thold;
					/* damage any people onboard */
					k = (shipsize+1)*Ploss*P_NPEOP/100;
					if(which==ATKR) akcrew += k;
					else dkcrew += k;
					P_NPEOP -= k/thold;
				}
			}
		}
	}
#ifdef HIDELOC
	fprintf(fnews,"4.\tNaval Battle occurs");
#else
	fprintf(fnews,"4.\t%d,%d: Naval Battle",xspot,yspot);
#endif
	for(j=0;j<count;j++) if(owner[j]!=(-1)){
		k=0;
		for(i=0;i<j;i++) if(owner[j]==owner[i]) k=1;
		if(k==0) {
		if(side[j]==ATKR)
			fprintf(fnews,", attacker %s",ntn[owner[j]].name);
		else if(side[j]==DFND)
			fprintf(fnews,", defender %s",ntn[owner[j]].name);
		else fprintf(fnews,", neutral %s",ntn[owner[j]].name);
		}
	}
	fprintf(fnews,"\n");

	/*mail results; who is in the battle*/
	for(j=0;j<count;j++) if(owner[j]!=(-1)){
		done=FALSE;

		/*first time your nation appears done=FALSE*/
		for(i=0;i<j;i++) if(owner[j]==owner[i]) done=TRUE;

		if((done==FALSE)&&(ispc(ntn[owner[j]].active))) {
			if (mailopen( owner[j] )==(-1)) continue;

			fprintf(fm,"NAVAL BATTLE in sector %d %d\n",xspot,yspot);
			fprintf(fm,"Battle occured during %s of Year %d\n",
				PSEASON(TURN),YEAR(TURN));

			if(side[j]==ATKR)
				fprintf(fm,"You are on the Attacking Side\n");
			else	if(side[j]==DFND)
				fprintf(fm,"You are on the Defending Side\n");
			else	fprintf(fm,"You are on the Neutral Side\n");

			/*detail all participants in battle*/
			for(k=0;k<count;k++) if(owner[k]!=(-1)){
				if(side[k]==DFND)
				fprintf(fm," %s is defender with navy ",ntn[owner[k]].name);
				else if(side[k]==ATKR)
				fprintf(fm," %s is attacker with navy ",ntn[owner[k]].name);
				else
				fprintf(fm," %s is neutral with navy ",ntn[owner[k]].name);
				fprintf(fm,"%d (%d warships %d galleys %d merchants)",
					unit[k],wnum[k],gnum[k],mnum[k]);
				putc('\n',fm);
 
			}

			fprintf(fm,"attacker strength (%d men) -> percent loss %d%%\n",acrew,PAloss);
			fprintf(fm,"defender strength (%d men) -> percent loss %d%%\n",dcrew,PDloss);
			fprintf(fm,"Odds are %d to 100; RANDOM ROLL is %d\n",odds,roll);
			fprintf(fm,"RESULT:  Attackers lose %d men  Defenders lose %d men\n",akcrew,dkcrew);
			/* display any other results */
			show_ships("Attacking","sunk",awsunk,agsunk,amsunk);
			show_ships("Defending","sunk",dwsunk,dgsunk,dmsunk);
			show_ships("Attacking","captured",awcapt,agcapt,amcapt);
			show_ships("Defending","captured",dwcapt,dgcapt,dmcapt);
			mailclose(owner[j]);
		}
	}
	curntn= saventn;
	country= savecntry;
	printf("Out Naval Combat....\n");
}

/* routine to distribute a captured ship */
void
capture(type,to,shipsize,holdcount)
	int type,to,shipsize,holdcount;
{
	int i,nvynum;
	struct s_nation *saventn=curntn;
#ifdef DEBUG
	printf("capture: hdcnt==%d typ==%d spsz==%d to==%d\n",holdcount,
		  type,shipsize,to);
#endif /* DEBUG */
	for (i=0;holdcount && i<count;i++) {
		if (owner[i]!=(-1) && side[i]==to) {
			curntn= &ntn[owner[i]];
			holdcount -= fltwhold(unit[i]);
		}
		if(holdcount<=0) {
			holdcount=0;
			i--;
		}
	}
#ifdef DEBUG
	printf("capture 2: holdcount==%d i==%d\n",holdcount,i);
#endif /* DEBUG */
	if (i==count) {
		curntn = saventn;
		return;
	}
	nvynum = unit[i];
#ifdef DEBUG
	printf("Should have successful capture of %d\n",type);
#endif /* DEBUG */
	switch(type) {
	case QWAR:
		(void) NADD_WAR(1);
		break;
	case QGAL:
		(void) NADD_GAL(1);
		break;
	case QMER:
		(void) NADD_MER(1);
		break;
	default:
		fprintf(stderr,"unknown type in function capture");
		break;
	}
	curntn = saventn;
}

/* routine to display combat results */
void
show_ships(who,what,war,gal,mer)
	char *who,*what;
	int war, gal, mer;
{
	if (war+gal+mer>0) {
		fprintf(fm,"%s ships %s: ",who,what);
		if (war) fprintf(fm,"%d Warships ",war);
		if (gal) fprintf(fm,"%d Galleys ",gal);
		if (mer) fprintf(fm,"%d Merchants",mer);
		putc('\n',fm);
	}
}
