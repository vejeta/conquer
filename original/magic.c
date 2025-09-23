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

/*create a new login for a new player*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <ctype.h>
#include "header.h"
#include "data.h"

extern short country;
extern short redraw;
extern FILE *fexe;
extern FILE *fnews;

/*give player one new magic power in current magic (powers)*/
/*do nothing if that player has that power or it is not permitted*/
/*getmagic() returns the value of the power gained, and stores it in power*/

long
getmagic(type)
int type;
{
	long newpower;
	int start,end;
	switch(type){
		case M_MGK:
			start=S_MGK;
			end=E_MGK;
			break;
		case M_CIV:
			start=S_CIV;
			end=E_CIV;
			break;
		case M_MIL:
			start=S_MIL;
			end=E_MIL;
			break;
		default:
			printf("fatal error in num_powers");
			abrt();
	}
	newpower=powers[start+(rand()%end)];
	if(newpower==0) {
		printf("ILLEGAL POWER");
		abrt();
	}

	if((newpower==WARRIOR)||(newpower==CAPTAIN)||(newpower==WARLORD)){
		if(magic(country,WARRIOR)!=TRUE){
			curntn->powers|=WARRIOR;
			return(WARRIOR);
		}
		else if(magic(country,CAPTAIN)!=TRUE){
			curntn->powers|=CAPTAIN;
			return(CAPTAIN);
		}
		else if(magic(country,WARLORD)!=TRUE){
			curntn->powers|=WARLORD;
			return(WARLORD);
		}
		else return(0L);
	}else if((newpower==MI_MONST) ||(newpower==AV_MONST) ||(newpower==MA_MONST)){
		if(curntn->race!=ORC) return(0L);
		if(magic(country,MI_MONST)!=TRUE){
			curntn->powers|=MI_MONST;
			return(MI_MONST);
		}
		else if(magic(country,AV_MONST)!=TRUE){
			curntn->powers|=AV_MONST;
			return(AV_MONST);
		}
		else if(magic(country,MA_MONST)==TRUE){
			curntn->powers|=MA_MONST;
			return(MA_MONST);
		}
		else return(0L);
	}else if(newpower==CAVALRY){
		if(curntn->race==ORC) return(0L);
		if(isnotpc(curntn->active)) return(0L);	/*npc nation*/
		if(magic(country,newpower)==TRUE) return(0L);
		curntn->powers|=newpower;
		return(newpower);
	}else if(newpower==URBAN){
		if(magic(country,BREEDER)==TRUE) return(0L);
		if(magic(country,newpower)==TRUE) return(0L);
		curntn->powers|=newpower;
		return(newpower);
	}else if(newpower==RELIGION){
		if(curntn->race==ORC) return(0L);
		if(magic(country,newpower)==TRUE) return(0L);
		curntn->powers|=newpower;
		return(newpower);
	}else if(newpower==KNOWALL){
#ifdef OGOD
#ifdef HIDELOC
	     /* only god should have KNOWALL if sectors hidden */
	     if(country!=0) return(0L);
#endif
#endif
	     if(magic(country,KNOWALL)==TRUE) return(0L);
	     curntn->powers|=KNOWALL;
	     return(KNOWALL);
	}else if((newpower==SLAVER)
	||(newpower==DERVISH)
	||(newpower==HIDDEN)
	||(newpower==ARCHITECT)
	||(newpower==THE_VOID)
	||(newpower==ARCHER)){
		if((magic(country,newpower)==TRUE)
		||((newpower==DERVISH)&&(magic(country,DESTROYER)==TRUE)))
			 return(0L);
		curntn->powers|=newpower;
		return(newpower);
	}else if(newpower==DESTROYER){
		if((curntn->race!=ELF)
		&&(magic(country,DESTROYER)!=TRUE)
		&&(magic(country,DERVISH)!=TRUE)){
			curntn->powers|=DESTROYER;
			return(DESTROYER);
		}
		return(0L);
	}else if(newpower==VAMPIRE){
		if((curntn->race!=ELF)&&(magic(country,VAMPIRE)!=TRUE)){
			curntn->powers|=VAMPIRE;
			return(VAMPIRE);
		}
		return(0L);
	}else if(newpower==MINER){
		if((curntn->race!=ELF)&&(curntn->race!=DWARF)&&(magic(country,MINER)!=TRUE)){
			curntn->powers|=MINER;
			return(MINER);
		}
		return(0L);
	}else if(newpower==STEEL){
		if(magic(country,STEEL)==TRUE) return(0L);
		if(magic(country,MINER)!=TRUE) return(0L);
		curntn->powers|=STEEL;
		return(STEEL);
	}else if(newpower==BREEDER){
		if(magic(country,URBAN)==TRUE) return(0L);
		if(magic(country,BREEDER)==TRUE) return(0L);
		if(curntn->race!=ORC) return(0L);
		curntn->powers|=BREEDER;
		return(BREEDER);
	}
	else if(isnotpc(curntn->active)) {
		return(0L);	/* remaining powers only for pc's */
	} else if((newpower==NINJA)
	||(newpower==SLAVER)
	||(newpower==SAILOR)
	||(newpower==DEMOCRACY)
	||(newpower==ROADS)
	||(newpower==SAPPER)
	||(newpower==ARMOR)
	||(newpower==AVIAN)){
		if(magic(country,newpower)==TRUE) return(0L);
		curntn->powers|=newpower;
		return(newpower);
	}
	else if((newpower==SUMMON)||(newpower==WYZARD)||(newpower==SORCERER)){
		/* dwarves may not cast spells */
		if(curntn->race==DWARF) return(0L);
		if(magic(country,SUMMON)!=TRUE) {
			curntn->powers|=SUMMON;
			return(SUMMON);
		} else if(magic(country,WYZARD)!=TRUE) {
			curntn->powers|=WYZARD;
			return(WYZARD);
		} else if(magic(country,SORCERER)!=TRUE) {
			curntn->powers|=SORCERER;
			return(SORCERER);
		} else return(0L);
	} else return(0L);
}
#ifdef CONQUER
/*form to interactively get a magic power*/
void
domagic()
{
	int county, countx, done=FALSE, loop=0, i,type;
	long price,x;
#ifdef OGOD
	void god_magk();
#endif /* OGOD */

	short isgod=0;
	if(country==0) {
		isgod=TRUE;
		if (get_god()) return;
	}

	clear();
	while(done==FALSE){
		done=TRUE;
		move(0,0);
		clrtobot();
		county=3;
		countx=0;
		redraw=FULL;
		standout();
		mvprintw(0,(COLS/2)-15,"MAGIC POWERS FOR %s",curntn->name);
		mvprintw(county++,30,"1) %d military powers: %ld jewels",
			num_powers(country,M_MIL) ,getmgkcost(M_MIL,country));
		mvprintw(county++,30,"2) %d civilian powers: %ld jewels",
			num_powers(country,M_CIV) ,getmgkcost(M_CIV,country));
		mvprintw(county++,30,"3) %d magic powers:    %ld jewels",
			num_powers(country,M_MGK),getmgkcost(M_MGK,country));

		price =  getmgkcost(M_MIL,country);
		if(price > getmgkcost(M_CIV,country))
			price = getmgkcost(M_CIV,country);
		if(price > getmgkcost(M_MGK,country))
			price = getmgkcost(M_MGK,country);

		standend();
		county=3;
		/*print the powers that you have*/
		i=0;
		while( powers[i] != 0 ){
			if(magic(country,powers[i])==TRUE)
			mvprintw(county++,countx,"you have power %s",*(pwrname+i));
			i++;
			if (county > 18) {
				county=7;
				countx=40;
			}
		}

		if(county<=7) county=8;
		else if(countx == 40) county = 20;
		else county++;
		standout();
		mvprintw(county++,0,"YOU HAVE %ld JEWELS IN YOUR TREASURY",curntn->jewels);
#ifdef OGOD
		if (isgod==TRUE) {
			mvaddstr(county++,0,"DO YOU WISH TO CHANGE THIS NATION'S POWERS? [yn]");
			standend();
			refresh();
			if (getch()=='y') god_magk();
		} else
#endif /* OGOD */
		if(price <= curntn->jewels){

		mvaddstr(county++,0,"DO YOU WISH TO BUY A RANDOM NEW POWER? [yn]");
		standend();
		refresh();
		if(getch()=='y'){
			done=FALSE;
			mvaddstr(county++,0,"ENTER SELECTION (1,2,3):");
			refresh();
			type = getch() - '0';
			if(type==M_MIL || type==M_CIV || type==M_MGK){
			price=getmgkcost(type,country);
			if(curntn->jewels>=price) {
				loop = 0;
				while(loop++ < 500) if((x=getmagic(type))!=0){
					curntn->jewels -= price;
					CHGMGK;
					exenewmgk(x);
					refresh();
					if (isgod==TRUE) reset_god();
					break;
				}
				if (loop >= 500)
					errormsg("You have too many powers!");
			} else errormsg("CAN'T AFFORD A NEW POWER");
			} else errormsg("BAD SELECTION");
		}
		} else {
			mvaddstr(LINES-1,0,"CAN'T AFFORD A NEW POWER");
			clrtoeol();
			mvaddstr(LINES-1,60,"PRESS ANY KEY");
			refresh();
			standend();
			getch();
		}
#ifdef ORCTAKE
		if((curntn->race==ORC)&&(curntn->jewels>=ORCTAKE)&&(curntn->spellpts>=TAKEPOINTS))
			done |= orctake(&county);
#endif /* ORCTAKE */
	}
	if(isgod==TRUE) reset_god();
}
#endif /* CONQUER */
#ifdef ORCTAKE
/*do magic for both npcs and pcs in update*/
/*if target is 0 then it is update and target will be picked randomly*/
int
takeover(percent,target)
int percent,target;
{
	int loop=1,y,save,isupdate=0;
	save=country;
	if(target==country) return(0);
	if(target==0) isupdate=1;
	country=target;
	if(rand()%100<percent){
		loop=0;
		y=0;
		if (target==0) while(loop==FALSE){
			y++;
			country=rand()%NTOTAL;
			if((ntn[country].race==ntn[save].race)
			&&(isnpc(ntn[country].active))
			&&(curntn->dstatus[country]<HOSTILE)
			&&(curntn->dstatus[country]!=UNMET)
			&&(country!=save))
				loop=TRUE;
			else if(y>=500) {
				country=save;
				return(0);
			}
		}
		sct[ntn[country].capx][ntn[country].capy].owner=save;
		if(isupdate==1){
		printf("nation %s magically taken over by %s\n",ntn[country].name,ntn[save].name);
		fprintf(fnews,"1.\tnation %s magically taken over by %s\n",ntn[country].name,ntn[save].name);
		}
		else {
			DESTROY;
			if ((fnews=fopen("/dev/null","w"))==NULL){
				printf("error opening null file\n");
				exit(FAIL);
			}
		}
		destroy(country);
		sct[ntn[country].capx][ntn[country].capy].designation=DCITY;
		if(isupdate!=1) fclose(fnews);
		y=country;
		country=save;
		return(y);
	}
	country=save;
	return(0);
}
#endif /* ORCTAKE */

/*execute new magic*/
void
exenewmgk(newpower)
long newpower;
{
	short x,armynum;
#ifdef ADMIN
	short y;
#endif
	if(newpower==WARRIOR) {
		curntn->aplus+=10;
		curntn->dplus+=10;
		return;
	}
	if(newpower==CAPTAIN) {
		curntn->aplus+=10;
		curntn->dplus+=10;
		return;
	}
	if(newpower==WARLORD) {
		curntn->aplus+=10;
		curntn->dplus+=10;
		return;
	}
	if(newpower==RELIGION) {
		if(curntn->race==ORC) {
			printf("ORCS CANT HAVE RELIGION POWER\n");
			abrt();
		} else if(curntn->repro<=8){
			curntn->repro+=2;
		} else if(curntn->repro==9){
			curntn->repro=10;
			curntn->dplus+=5;
		} else if(curntn->repro>=10){
			curntn->dplus+=10;
		}
		return;
	}
	if(newpower==DESTROYER) {
/* this ifdef is so that destroyer only takes place in an update */
#ifdef ADMIN
		for(x=(int)curntn->capx-3;x<=(int)curntn->capx+3;x++) {
			for(y=(int)curntn->capy-3;y<=(int)curntn->capy+3;y++){
				if((ONMAP(x,y))
				&&(sct[x][y].altitude!=WATER)
#ifdef DERVDESG
				&&((rand()%2)==0)
#else
				&&(tofood( &sct[x][y],0)<6)
#endif /* DERVDESG */
				&&((x!=curntn->capx)
					||(y!=curntn->capy))){
					sct[x][y].vegetation=DESERT;
					sct[x][y].designation=DNODESIG;
				}
			}
		}
		fprintf(fnews,"1.\tnation %s gets destroyer power: land turns to desert\n",curntn->name);
#endif /* ADMIN */
		updmove(curntn->race,country);
		return;
	}
	if(newpower==DERVISH) {
		updmove(curntn->race,country);
		return;
	}
	if((newpower==MI_MONST)
	||(newpower==AV_MONST)
	||(newpower==MA_MONST)
	||(newpower==KNOWALL)
	||(newpower==HIDDEN)
	||(newpower==THE_VOID)
	||(newpower==ARCHITECT))
		return;
	if(newpower==MINER) {
		curntn->mine_ability+=25;
		return;
	}
	if(newpower==VAMPIRE) {
		curntn->aplus-=35;
		curntn->dplus-=35;
		for(armynum=0;armynum<MAXARM;armynum++){
			if((P_ATYPE == A_INFANTRY)||(P_ATYPE == A_MILITIA))
				P_ATYPE=A_ZOMBIE;
		}
		return;
	}
	if(newpower==URBAN) {
		if(curntn->race==ORC) {
			x=curntn->repro;
			if(curntn->repro>=14){
				curntn->maxmove+=3;
			}
			else if(curntn->repro>11){
				curntn->maxmove+= x-11;
				curntn->repro=14;
			}
			else curntn->repro+=3;
		}
		else if(curntn->repro<=9){
			curntn->repro+=3;
		}
		else {
			curntn->maxmove+=2*(curntn->repro-9);
			curntn->repro=12;
		}
		return;
	}
	if(newpower==BREEDER) {
		x=curntn->repro;
		if(curntn->repro>=14){
			curntn->maxmove+=3;
		}
		else if(curntn->repro>11){
			curntn->maxmove+= x-11;
			curntn->repro=14;
		}
		else curntn->repro+=3;
		curntn->dplus-=10;
		curntn->aplus-=10;
		return;
	}
	if(newpower==DEMOCRACY){
		curntn->maxmove+=1;
		curntn->repro+=1;
		curntn->dplus+=10;
		curntn->aplus+=10;
		return;
	}
	if(newpower==ROADS){
		curntn->maxmove+=4;
		return;
	}
	if(newpower==ARMOR){
		curntn->maxmove-=3;
		if( curntn->maxmove<4) curntn->maxmove=4;
		curntn->dplus+=20;
	}
	if((newpower==NINJA)
	||(newpower==STEEL)
	||(newpower==ARCHER)
	||(newpower==CAVALRY)
	||(newpower==SAILOR)
	||(newpower==SUMMON)
	||(newpower==WYZARD)
	||(newpower==SORCERER)
	||(newpower==SAPPER)
	||(newpower==AVIAN)){	/* these powers are only for pc's */
		return;
	}
}
#ifdef CONQUER
/* returns 0 if summon occurred, 1 else */
void
dosummon()
{
	int x,count,i,armynum;
	long e_cost;
	int newtype,s_cost;
	char line[LINELTH+1],ch;

	x=0;
	count=LINES-4;
	clear_bottom(0);
	mvaddstr(count,x,"options:");
	x+=9;
	for(i=MINMONSTER;i<=MAXMONSTER;i++){
		if(unitvalid(i)==TRUE) {
			mvprintw(count,x+2,"%s",*(shunittype+(i%UTYPE)));
			mvprintw(count,x,"(%c)",*(shunittype+(i%UTYPE))[0]);
			x+=7;
			if(x>COLS-20){
				x=0;
				count++;
			}
		}
	}
	count++;
	mvaddstr(count++,0,"what type of unit do you want to raise:");
	refresh();
	ch=getch();
	for(newtype=MINMONSTER;newtype<=MAXMONSTER;newtype++){
		if( *(shunittype+(newtype%UTYPE))[0] == ch ) break;
	}

	if (newtype==MAXMONSTER+1) {
		/* quick exit */
		return;
	}
	if(unitvalid(newtype)==FALSE) {
		beep();
		clear_bottom(0);
		errormsg("you are unable to summon that monster");
		return;
	}

	s_cost= *(u_encost+(newtype%UTYPE));
	if(s_cost > curntn->spellpts) {
		sprintf(line,"you dont have %d spell points",s_cost);
		clear_bottom(0);
		errormsg(line);
		return;
	}

	e_cost= (long) *(u_encost+(newtype%UTYPE)) * *(unitminsth+(newtype%UTYPE));
	if(e_cost >  curntn->tgold) {
		sprintf(line,"you dont have %ld gold talons in your treasury",e_cost);
		clear_bottom(0);
		errormsg(line);
		return;
	}

	armynum=0;
	while(armynum<MAXARM) {
		if(P_ASOLD<=0) {
			P_ASOLD= *(unitminsth+(newtype%UTYPE));
			P_ATYPE=newtype;
			P_ASTAT=DEFEND; /* set new armies to DEFEND */
			P_AXLOC=curntn->capx;
			P_AYLOC=curntn->capy;
			P_AMOVE=0;
			AADJLOC;
			AADJSTAT;
			AADJMEN;
			AADJMOV;
			armynum=MAXARM;
		} else if(armynum==MAXARM-1) {
			clear_bottom(0);
			errormsg("NO FREE ARMIES");
			return;
		} else armynum++;
  	}
  	curntn->tgold -= e_cost;
  	curntn->spellpts -= s_cost;
	EDECSPL;
}
#ifdef ORCTAKE
/* orc takeover routine... returns 0 if run, 1 if not */
int
orctake(count)
int *count;
{
	int chance=0,done=TRUE,i,s_cost;
	if((*count)>20) {
		(*count)=2;
		clear();
	}
	if(magic(country,MA_MONST)==TRUE) {
	mvprintw((*count)++,0,"You have a 10 percent chance for %ld Jewels take over other orcs",ORCTAKE);
	chance=10;
	} else if(magic(country,AV_MONST)==TRUE) {
	mvprintw((*count)++,0,"You have a 6 percent chance for %ld Jewels take over other orcs",ORCTAKE);
	chance=6;
	} else if(magic(country,MI_MONST)==TRUE){
	mvprintw((*count)++,0,"You have a 3 percent chance for %ld Jewels to take over other orcs",ORCTAKE);
	chance=3;
	}
	if(chance==0) return(TRUE);

	mvaddstr((*count)++,0,"  Target can't be Unmet, Hostile, War, or Jihad");
	mvaddstr((*count)++,0,"DO YOU WISH TO TAKE OVER AN ORC NPC NATION: [ny]");
	refresh();
	if(getch()=='y'){
		done=FALSE;
		mvaddstr((*count)++,0,"  What orc nation:");
		refresh();
		i=get_country();
		if(i<=0 || i>=NTOTAL || !isntn(ntn[i].active) )
			mvaddstr((*count)++,0,"  Invalid Nation");
		else if((curntn->dstatus[i]<HOSTILE)
		&&(curntn->dstatus[i]!=UNMET)
		&&(ntn[i].race==ORC)){
			curntn->jewels-=ORCTAKE;
			s_cost=TAKEPOINTS;
			curntn->spellpts-=s_cost;
			EDECSPL;
			if(( takeover(chance,i)) !=0 )
				mvprintw((*count)++,0,"  Successful: %d",i);
			else {
				mvaddstr((*count)++,0,"  Failed: Nation becomes more hostile");
				curntn->dstatus[i]++;
				EADJDIP(country,i);
			}
		} else {
			if (ntn[i].race==ORC) 
				mvaddstr((*count)++,0,"  That Nation is Outside Your Influence");
			else mvaddstr((*count)++,0,"  Wrong Race");
		}
		refresh();
		sleep(2);
	}
	return(done);
}
#endif /* ORCTAKE */
#endif /* CONQUER */
#ifdef CONQUER

/**********************************************************************/
/* unitvalid() returns TRUE if nation has powers needed to draft unit */
/**********************************************************************/
int
unitvalid(type)
int type;
{
	int valid=FALSE;
	switch(type){
		case A_INFANTRY: /* not everyone gets infantry now */
				if(defaultunit(country)==A_INFANTRY) valid=TRUE;
				break;
		case GARGOYLE:
		case A_GOBLIN:
		case A_ORC:	if(magic(country,MI_MONST)==TRUE) valid=TRUE;
				break;
		case A_MARINES: if(magic(country,SAILOR)==TRUE) valid=TRUE;
				break;
		case A_ARCHER:	if(magic(country,ARCHER)==TRUE) valid=TRUE;
				break;
		case A_URUK:	if(magic(country,AV_MONST)==TRUE) valid=TRUE;
				break;
		case A_NINJA:	if(magic(country,NINJA)==TRUE) valid=TRUE;
				break;
		case A_PHALANX:	if(magic(country,CAPTAIN)==TRUE) valid=TRUE;
				break;
		case A_OLOG:	if((magic(country,BREEDER)==TRUE)
				&&(magic(country,AV_MONST)==TRUE)) valid=TRUE;
				break;
		case A_ELEPHANT:if(magic(country,DERVISH)==TRUE)  valid=TRUE;
				break;
		case SUPERHERO:
		case A_LEGION:	if(magic(country,WARLORD)==TRUE) valid=TRUE;
				break;
		case A_TROLL:	if(magic(country,MA_MONST)==TRUE) valid=TRUE;
				break;
		case A_ELITE:	if(magic(country,ARMOR)==TRUE) valid=TRUE;
				break;
		case CENTAUR:
		case A_LT_CAV:
		case A_CAVALRY:	if(magic(country,CAVALRY)==TRUE) valid=TRUE;
				break;
		case A_KNIGHT:	if((magic(country,ARMOR)==TRUE)
				&&(magic(country,CAVALRY)==TRUE)) valid=TRUE;
				break;
		case A_ROC:
		case A_GRIFFON: if(magic(country,AVIAN)==TRUE) valid=TRUE;
				break;
		case ASSASSIN:	if(magic(country,NINJA)==TRUE) valid=TRUE;
				break;
		case DJINNI:	if(magic(country,DERVISH)==TRUE) valid=TRUE;
				break;
		case HERO:	if(magic(country,WARRIOR)==TRUE) valid=TRUE;
				break;
		case ELEMENTAL:	if(magic(country,SORCERER)==TRUE) valid=TRUE;
				break;
		case A_ZOMBIE:
		case WRAITH:
		case MUMMY:	if(magic(country,VAMPIRE)==TRUE) valid=TRUE;
				break;
		case MINOTAUR:
		case DEMON:	if(magic(country,DESTROYER)==TRUE) valid=TRUE;
				break;
		case BALROG:	if((magic(country,WYZARD)==TRUE)
				&&(magic(country,VAMPIRE)==TRUE)) valid=TRUE;
				break;
		case DRAGON:	if((magic(country,MA_MONST)==TRUE)
				&&(magic(country,WYZARD)==TRUE)) valid=TRUE;
				break;
		case A_SPY:
		case A_SCOUT:	break;	/* handled elsewhere */
		default:	valid=TRUE;	/* for all unrestricted types */
	}
	return(valid);
}
#endif /* CONQUER */

/*remove properties of magic power; must first remove power */
void
removemgk(oldpower)
long oldpower;
{
	short x,y,armynum;
	if((oldpower==WARRIOR)
	||(oldpower==CAPTAIN)
	||(oldpower==WARLORD)) {
		curntn->aplus-=10;
		curntn->dplus-=10;
		return;
	}
	if(oldpower==RELIGION) {
		if(curntn->race==ORC) {
			printf("ORCS CANT HAVE RELIGION POWER\n");
			abrt();
		} else curntn->repro -= 2;
		return;
	}
	if(oldpower==DESTROYER) {
		for(x=(int)curntn->capx-3;x<=(int)curntn->capx+3;x++) {
			for(y=(int)curntn->capy-3;y<=(int)curntn->capy+3;y++){
				if((ONMAP(x,y))
				&&(sct[x][y].altitude!=WATER)
				&&((x!=curntn->capx)
					||(y!=curntn->capy))){
					if (sct[x][y].vegetation==DESERT)
					{
						/* LT_VEG has medium value*/
						sct[x][y].vegetation=LT_VEG;
						sct[x][y].designation=DNODESIG;
					}
				}
			}
		}
		updmove(curntn->race,country);
		return;
	}
	if(oldpower==DERVISH) {
		updmove(curntn->race,country);
		return;
	}
	if(oldpower==VAMPIRE) {
		curntn->aplus+=35;
		curntn->dplus+=35;
		for(armynum=0;armynum<MAXARM;armynum++){
			if(P_ATYPE == A_ZOMBIE) P_ATYPE=defaultunit(country);
		}
		return;
	}
	if(oldpower==URBAN) {
		curntn->repro -= 3;
		return;
	}
	if(oldpower==BREEDER) {
		curntn->repro-=3;
		curntn->dplus+=10;
		curntn->aplus+=10;
		for(armynum=0;armynum<MAXARM;armynum++){
			if(P_ATYPE == A_OLOG) P_ATYPE=A_URUK;
		}
		return;
	}
	if(oldpower==DEMOCRACY){
		curntn->maxmove-=1;
		curntn->repro-=1;
		curntn->dplus-=10;
		curntn->aplus-=10;
		return;
	}
	if(oldpower==ROADS){
		curntn->maxmove-=4;
		return;
	}
	if(oldpower==ARMOR){
		curntn->maxmove+=3;
		curntn->dplus-=20;
		return;
	}
	if(oldpower==MI_MONST) {
		for(armynum=0;armynum<MAXARM;armynum++){
			if(P_ATYPE == A_ORC) P_ATYPE=defaultunit(country);
		}
		return;
	}
	if(oldpower==AV_MONST) {
		for(armynum=0;armynum<MAXARM;armynum++){
			if(P_ATYPE == A_URUK || P_ATYPE == A_OLOG)
				P_ATYPE=defaultunit(country);
		}
		return;
	}
	if(oldpower==ARCHER) {
		for(armynum=0;armynum<MAXARM;armynum++){
			if(P_ATYPE == A_ARCHER) P_ATYPE=defaultunit(country);
		}
		return;
	}
	/* remaining cause no change in statistics */
}

#ifdef CONQUER
#ifdef OGOD
/* godmagk() -- routine to allow god to selectively add or remove magic */
void
god_magk()
{
	int county,countx,choice;
	int remove,i,done=FALSE;

	clear();
	while (done==FALSE) {
		county=3;
		countx=0;
		standout();
		mvaddstr(0,0,"Do you wish to (A)dd or (R)emove a power?");
		clrtoeol();
		refresh();
		while (done==FALSE) {
			done=TRUE;
			switch(getch()) {
			case 'A':
			case 'a':
				remove=FALSE;
				break;
			case 'R':
			case 'r':
				remove=TRUE;
				break;
			default:
				done=FALSE;
				break;
			}
		}
		move(0,0);
		clrtobot();
		if (remove)
		mvprintw(0,(COLS/2)-15,"CURRENT POWERS FOR %s",curntn->name);
		else
		mvprintw(0,(COLS/2)-15,"LACKING POWERS FOR %s",curntn->name);
		standend();
		i=0;
		while( powers[i] != 0 ){
			if(magic(country,powers[i])==remove) {
				mvprintw(county,countx,"%2d: %s Power",i+1,*(pwrname+i));
				county++;
			}
			i++;
			if (county > LINES-6) {
				county = 3;
				countx = COLS/2;
			}
		}
		if (countx == COLS/2) {
			county = LINES-4;
		}
		else county++;
		standout();
		if (remove) mvaddstr(county++,0,"Which power to remove?");
		else mvaddstr(county++,0,"Which power to add?");
		standend();
		addch(' ');
		refresh();
		choice=get_number();
		if(choice > 0 && choice <= MAXPOWER) {
			if (magic(country,powers[choice-1])==remove) {
				if (remove)
				mvprintw(county++,0,"Remove magic #%d? (y or [n])",choice);
				else
				mvprintw(county++,0,"Add magic #%d? (y or [n])",choice);
				refresh();
				if (getch()=='y') {
					curntn->powers ^= powers[choice-1];
					if (remove) removemgk(powers[choice-1]);
					else exenewmgk(powers[choice-1]);
				}
			} else {
				if (remove) mvaddstr(county++,0,"The nation doesn't have that power.");
				else mvaddstr(county++,0,"The nation already has that power.");
			}
		}
		mvaddstr(county++,0,"Do you wish to add or remove another power?");
		done=TRUE;
		refresh();
		if (getch()=='y') done=FALSE;
	}
}
#endif /* OGOD */

#define NUMSPELLS 4
char *spellstr[NUMSPELLS]={"(S)ummon","(F)light","(A)ttack Enhancement",
	"(D)efense Enhancement"};
/* quick adjustment to allow magical status change */
int magicstat[NUMSPELLS]={DEFEND, FLIGHT, MAGATT, MAGDEF};
/* number of soldiers per point of spell cost */
int magiccost[NUMSPELLS]={0,100,300,300};
/* routine to perform spells */
void
wizardry()
{
	int i,xspt,yspt,choice,armynum,s_cost;
	char line[LINELTH+1];
	void dosummon();

	clear_bottom(0);
	if(curntn->spellpts>0)
	{
		xspt=0; yspt=LINES-3;
		/* summon only to those with summon */
		if (magic(country,SUMMON)==TRUE) i=0;
		else i=1;
		for (;i<NUMSPELLS;i++) {
			sprintf(line,"  %s",spellstr[i]);
			mvaddstr(yspt,xspt,line);
			xspt += strlen(line);
			if (xspt>COLS-20) {
				xspt=0;
				yspt++;
			}
		}
		mvaddstr(LINES-4,0,"Which spell to cast:");
		refresh();
		choice=NUMSPELLS;
		switch(getch()) {
		case 's':
		case 'S':
			if (magic(country,SUMMON)==TRUE) dosummon();
			else {
				clear_bottom(0);
				errormsg("you do not have SUMMON power");
			}
			break;
		case 'f':
		case 'F':
			choice--;
		case 'a':
		case 'A':
			choice--;
		case 'd':
		case 'D':
			choice--;
			/* change status of currently selected army */
			armynum = getselunit();
			clear_bottom(0);
			if (armynum<0 || armynum>=MAXARM ||
			P_ASTAT==SCOUT || P_ASTAT==TRADED ||
			P_ASTAT==GENERAL || P_ASTAT>NUMSTATUS ||
			P_ASOLD<=0) {
				errormsg("Invalid Unit for Magicking");
			} else if(P_ASTAT==ONBOARD) {
				errormsg("Carried armies must be unloaded");
			} else if(P_ASTAT==magicstat[choice]) {
				errormsg("Unit has already been magicked");
			} else if(P_ASTAT==MARCH) {
				errormsg("That unit is too busy marching");
			} else {
				/*cost of 1 spell point for magiccost men*/
				s_cost = (P_ASOLD-1) / magiccost[choice] + 1;
				if (s_cost > curntn->spellpts) {
					sprintf(line,"You don't have %d spell points",s_cost);
					errormsg(line);
				} else {
					change_status(armynum,magicstat[choice]);
					curntn->spellpts -= s_cost;
					EDECSPL;
				}
			}
			break;
		default:
			break;
		}
	}
	else errormsg("You have no spell points for spell casting");
	makebottom();
}
#endif /* CONQUER */
