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

/*include files*/
#include <stdio.h>
#include "header.h"
#include "data.h"
#include "patchlevel.h"

void
main()
{
	FILE *fp, *fp2, *fopen();
	if((fp=fopen("sed.1","w"))==NULL) {
		printf("cannot open sed.1 file\n");
		exit(FAIL);
	}
	/* needed as sed was reaching its limit on number of commands */
	if((fp2=fopen("sed.2","w"))==NULL) {	
		printf("cannot open sed.2 file\n");
		exit(FAIL);
	}

	/* create sed script */
	fprintf(fp,"s/XOWNER/%s/g\n",OWNER);
	fprintf(fp,"s/XLOGIN/%s/g\n",LOGIN);
	fprintf(fp,"s/XVERSION/%s.%s/g\n",VERSION,PATCHLEVEL);
	fprintf(fp,"s/XPVULCAN/%d/g\n",PVULCAN);
	fprintf(fp,"s/XPMOUNT/%d/g\n",PMOUNT);
	fprintf(fp,"s/XDEPLETE/%d/g\n",PDEPLETE);
	fprintf(fp,"s/XPSTORM/%d/g\n",PSTORM);
	fprintf(fp,"s/XNTOTAL/%d/g\n",NTOTAL);
	fprintf(fp,"s/XMAXPTS/%d/g\n",MAXPTS);
	fprintf(fp,"s/XMAXARM/%d/g\n",MAXARM);
	fprintf(fp,"s/XTRADEPCT/%d/g\n",TRADEPCT);
	fprintf(fp,"s/XMAXNAVY/%d/g\n",MAXNAVY);
	fprintf(fp,"s/XBREAKJIHAD/%ld/g\n",BREAKJIHAD);
	fprintf(fp,"s/XCONQENV/%s/g\n",ENVIRON_OPTS);
	fprintf(fp,"s/XNCITYCOST/%d/g\n",N_CITYCOST);
	fprintf(fp,"s/XREBUILDCOST/%d/g\n",REBUILDCOST);

	/* check all the defined options */
#ifdef OGOD
	fprintf(fp,"s/XOGOD/(True )/g\n");
#else
	fprintf(fp,"s/XOGOD/(False)/g\n");
#endif	
#ifdef DERVDESG
	fprintf(fp,"s/XDERVDESG/Dervish can/g\n");
#else
	fprintf(fp,"s/XDERVDESG/Dervish cannot/g\n");
#endif
#ifdef SYSMAIL
	fprintf(fp,"s/XSYSMAIL/(True )/g\n");
#else
	fprintf(fp,"s/XSYSMAIL/(False)/g\n");
#endif
#ifdef SYSV
	fprintf(fp,"s/XSYSV/(True ) /g\n");
#endif
#ifdef BSD
	fprintf(fp,"s/XSYSV/(False:BSD)/g\n");
#endif
#ifdef MONSTER
	fprintf(fp,"s/XMONSTER/(True )/g\n");
#else
	fprintf(fp,"s/XMONSTER/(False)/g\n");
#endif
#ifdef NPC
	fprintf(fp,"s/XNPC/(True )/g\n");
#ifdef CMOVE
	fprintf(fp,"s/XCMOVE/(True )/g\n");
#else
	fprintf(fp,"s/XCMOVE/(False)/g\n");
#endif
#else
	fprintf(fp,"s/XCMOVE/(False)/g\n");
	fprintf(fp,"s/XNPC/(False)/g\n");
#endif
#ifdef CHEAT
	fprintf(fp,"s/XCHEAT/(True )/g\n");
#else
	fprintf(fp,"s/XCHEAT/(False)/g\n");
#endif
#ifdef RANEVENT
	fprintf(fp,"s/XRANEVENT/(True )/g\n");
#else
	fprintf(fp,"s/XRANEVENT/(False)/g\n");
#endif
#ifdef VULCANIZE
	fprintf(fp,"s/XVULCANIZE/(True )/g\n");
#else
	fprintf(fp,"s/XVULCANIZE/(False)/g\n");
#endif
#ifdef STORMS
	fprintf(fp,"s/XSTORMS/(True )/g\n");
#else
	fprintf(fp,"s/XSTORMS/(False)/g\n");
#endif
#ifdef ORCTAKE
	fprintf(fp,"s/XORCTAKE/(True )/g\n");
#else
	fprintf(fp,"s/XORCTAKE/(False)/g\n");
#endif
#ifdef HIDELOC
	fprintf(fp,"s/XHIDELOC/(True )/g\n");
#else
	fprintf(fp,"s/XHIDELOC/(False)/g\n");
#endif
#ifdef TRADE
	fprintf(fp,"s/XTRADE/(True )/g\n");
#else
	fprintf(fp,"s/XTRADE/(False)/g\n");
#endif
	/* map sectors */
	fprintf(fp,"s/ZMOUNTAIN/MOUNTAIN (%c)/g\n",MOUNTAIN);
	fprintf(fp,"s/ZHILL/HILL (%c)/g\n",HILL);
	fprintf(fp,"s/ZCLEAR/CLEAR (%c)/g\n",CLEAR);
	fprintf(fp,"s/ZWATER/WATER (%c)/g\n",WATER);
	fprintf(fp,"s/ZPEAK/PEAK (%c)/g\n",PEAK);
	fprintf(fp,"s/ZVOLCANO/VOLCANO (%c)/g\n",VOLCANO);
	fprintf(fp,"s/ZDESERT/DESERT (%c)/g\n",DESERT);
	fprintf(fp,"s/ZJUNGLE/JUNGLE (%c)/g\n",JUNGLE);
	fprintf(fp,"s/ZTUNDRA/TUNDRA (%c)/g\n",TUNDRA);
	fprintf(fp,"s/ZICE/ICE (%c)/g\n",ICE);
	fprintf(fp,"s/ZSWAMP/SWAMP (%c)/g\n",SWAMP);
	fprintf(fp,"s/ZGOOD/GOOD (%c)/g\n",GOOD);
	fprintf(fp,"s/ZFOREST/FOREST (%c)/g\n",FOREST);
	fprintf(fp,"s/ZLIGHT/LIGHT VEG (%c)/g\n",LT_VEG);
	fprintf(fp,"s/ZWOOD/WOOD (%c)/g\n",WOOD);
	fprintf(fp,"s/ZBARREN/BARREN (%c)/g\n",BARREN);

	/* designations */
	fprintf(fp,"s/ZMINE/MINE      (%c)/g\n",DMINE);
	fprintf(fp,"s/ZFARM/FARM      (%c)/g\n",DFARM);
	fprintf(fp,"s/ZGOLD/GOLD MINE (%c)/g\n",DGOLDMINE);
	fprintf(fp,"s/ZFORT/FORT      (%c)/g\n",DFORT);
	fprintf(fp,"s/ZDEVASTATED/DEVASTATED(%c)/g\n",DDEVASTATED);
	fprintf(fp,"s/ZTOWN/TOWN      (%c)/g\n",DTOWN);
	fprintf(fp,"s/ZCITY/CITY      (%c)/g\n",DCITY);
	fprintf(fp,"s/ZCAPITOL/CAPITOL   (%c)/g\n",DCAPITOL);
	fprintf(fp,"s/ZSPECIAL/SPECIAL   (%c)/g\n",DSPECIAL);
	if (DRUIN=='&')
	fprintf(fp,"s/ZRUIN/RUIN      (\\%c)/g\n",DRUIN);
	else
	fprintf(fp,"s/ZRUIN/RUIN      (%c)/g\n",DRUIN);
	fprintf(fp,"s/ZUNIVERSITY/UNIVERSITY(%c)/g\n",DUNIVERSITY);
	fprintf(fp,"s/ZBLACKSMITH/BLACKSMITH(%c)/g\n",DBLKSMITH);
	fprintf(fp,"s/ZLUMBERYARD/LUMBERYARD(%c)/g\n",DLUMBERYD);
	fprintf(fp,"s/ZROAD/ROAD      (%c)/g\n",DROAD);
	fprintf(fp,"s/ZMILL/MILL      (%c)/g\n",DMILL);
	fprintf(fp,"s/ZGRANARY/GRANARY   (%c)/g\n",DGRANARY);
	fprintf(fp,"s/ZCHURCH/CHURCH    (%c)/g\n",DCHURCH);
	fprintf(fp,"s/ZSTOCKADE/STOCKADE  (%c)/g\n",DSTOCKADE);
	fprintf(fp,"s/ZSTOCKCOST/%d/g\n",STOCKCOST);
	
	/* other defines */
#ifdef RANEVENT
	fprintf(fp,"s/XNORANDEVENTS/%d/g\n",RANEVENT);
#else
	fprintf(fp,"s/XNORANDEVENTS/0/g\n");
#endif
	fprintf(fp,"s/XLANDSEE/%d/g\n",LANDSEE);
	fprintf(fp,"s/XNAVYSEE/%d/g\n",NAVYSEE);
	fprintf(fp,"s/XARMYSEE/%d/g\n",ARMYSEE);
	fprintf(fp,"s/XTAXFOOD/%ld/g\n",TAXFOOD);
	fprintf(fp,"s/XTAXMETAL/%ld/g\n",TAXMETAL);
	fprintf(fp,"s/XTAXGOLD/%ld/g\n",TAXGOLD);
	fprintf(fp,"s/XTAXCITY/%ld/g\n",TAXCITY);
	fprintf(fp,"s/XTAXTOWN/%ld/g\n",TAXTOWN);
	fprintf(fp,"s/XSHIPMAINT/%ld/g\n",SHIPMAINT);
	fprintf(fp,"s/XSHIPCP/%ld/g\n",SHIPHOLD);
	fprintf(fp,"s/XDESCOST/%ld/g\n",DESCOST);
	fprintf(fp,"s/XFORTCOST/%ld/g\n",FORTCOST);
	fprintf(fp,"s/XWARSHPCOST/%ld/g\n",WARSHPCOST);
	fprintf(fp,"s/XMERSHPCOST/%ld/g\n",MERSHPCOST);
	fprintf(fp,"s/XGALSHPCOST/%ld/g\n",GALSHPCOST);
	fprintf(fp,"s/XSHIPCREW/%ld/g\n",SHIPCREW);
	fprintf(fp,"s/XSHIPHOLD/%d/g\n",SHIPHOLD);
	fprintf(fp,"s/XWARSPD/%d/g\n",N_WSPD);
	fprintf(fp,"s/XGALSPD/%d/g\n",N_GSPD);
	fprintf(fp,"s/XMERSPD/%d/g\n",N_MSPD);
	fprintf(fp,"s/XSIZESPD/%d/g\n",N_SIZESPD);
	fprintf(fp,"s/XNMASK/%d/g\n",(int)N_MASK);
	fprintf(fp,"s/XCITYLIMIT/%d/g\n",CITYLIMIT);
	fprintf(fp,"s/XMILRATIO/%d/g\n",MILRATIO);
	fprintf(fp,"s/XMILINCAP/%d/g\n",MILINCAP);
	fprintf(fp,"s/XBRIBE/%ld/g\n",BRIBE);
	fprintf(fp,"s/XDESFOOD/%d/g\n",DESFOOD);
	fprintf(fp,"s/XTOMUCHMINED/%d/g\n",TOMUCHMINED);
	fprintf(fp,"s/XTOMANYPEOPLE/%d/g\n",TOMANYPEOPLE);
	fprintf(fp,"s/XABSMAXPEOPLE/%d/g\n",ABSMAXPEOPLE);
	fprintf(fp,"s/XFINDPERCENT/%d/g\n",FINDPERCENT);
#ifdef ORCTAKE
	fprintf(fp,"s/XTAKEPRICE/%d/g\n",ORCTAKE);
#endif /* ORCTAKE */

	fprintf(fp,"s/XTGATTR/%d/g\n",TGATTR);
	fprintf(fp,"s/XOTHRATTR/%d/g\n",OTHRATTR);
	fprintf(fp2,"s/XGOLDATTR/%d/g\n",GOLDATTR);
	fprintf(fp2,"s/XFARMATTR/%d/g\n",FARMATTR);
	fprintf(fp2,"s/XMINEATTR/%d/g\n",MINEATTR);
	fprintf(fp2,"s/XTOWNATTR/%d/g\n",TOWNATTR);
	fprintf(fp2,"s/XCITYATTR/%d/g\n",CITYATTR);
	fprintf(fp2,"s/XCAPATTR/%d/g\n",CITYATTR);

	fprintf(fp2,"s/XDMNTNATTR/%3d/g\n",DMNTNATTR);
	fprintf(fp2,"s/XDHILLATTR/%3d/g\n",DHILLATTR);
	fprintf(fp2,"s/XDCLERATTR/%3d/g\n",DCLERATTR);
	fprintf(fp2,"s/XDCITYATTR/%3d/g\n",DCITYATTR);
	fprintf(fp2,"s/XDTOWNATTR/%3d/g\n",DTOWNATTR);
	fprintf(fp2,"s/XDGOLDATTR/%3d/g\n",DGOLDATTR);
	fprintf(fp2,"s/XDMINEATTR/%3d/g\n",DMINEATTR);
	fprintf(fp2,"s/XDFOREATTR/%3d/g\n",DFOREATTR);
	fprintf(fp2,"s/XDWOODATTR/%3d/g\n",DWOODATTR);

	fprintf(fp2,"s/XEMNTNATTR/%3d/g\n",EMNTNATTR);
	fprintf(fp2,"s/XEHILLATTR/%3d/g\n",EHILLATTR);
	fprintf(fp2,"s/XECLERATTR/%3d/g\n",ECLERATTR);
	fprintf(fp2,"s/XECITYATTR/%3d/g\n",ECITYATTR);
	fprintf(fp2,"s/XETOWNATTR/%3d/g\n",ETOWNATTR);
	fprintf(fp2,"s/XEGOLDATTR/%3d/g\n",EGOLDATTR);
	fprintf(fp2,"s/XEMINEATTR/%3d/g\n",EMINEATTR);
	fprintf(fp2,"s/XEFOREATTR/%3d/g\n",EFOREATTR);
	fprintf(fp2,"s/XEWOODATTR/%3d/g\n",EWOODATTR);

	fprintf(fp2,"s/XOMNTNATTR/%3d/g\n",OMNTNATTR);
	fprintf(fp2,"s/XOHILLATTR/%3d/g\n",OHILLATTR);
	fprintf(fp2,"s/XOCLERATTR/%3d/g\n",OCLERATTR);
	fprintf(fp2,"s/XOCITYATTR/%3d/g\n",OCITYATTR);
	fprintf(fp2,"s/XOTOWNATTR/%3d/g\n",OTOWNATTR);
	fprintf(fp2,"s/XOGOLDATTR/%3d/g\n",OGOLDATTR);
	fprintf(fp2,"s/XOMINEATTR/%3d/g\n",OMINEATTR);
	fprintf(fp2,"s/XOFOREATTR/%3d/g\n",OFOREATTR);
	fprintf(fp2,"s/XOWOODATTR/%3d/g\n",OWOODATTR);

	fprintf(fp2,"s/XHMNTNATTR/%3d/g\n",HMNTNATTR);
	fprintf(fp2,"s/XHHILLATTR/%3d/g\n",HHILLATTR);
	fprintf(fp2,"s/XHCLERATTR/%3d/g\n",HCLERATTR);
	fprintf(fp2,"s/XHCITYATTR/%3d/g\n",HCITYATTR);
	fprintf(fp2,"s/XHTOWNATTR/%3d/g\n",HTOWNATTR);
	fprintf(fp2,"s/XHGOLDATTR/%3d/g\n",HGOLDATTR);
	fprintf(fp2,"s/XHMINEATTR/%3d/g\n",HMINEATTR);
	fprintf(fp2,"s/XHFOREATTR/%3d/g\n",HFOREATTR);
	fprintf(fp2,"s/XHWOODATTR/%3d/g\n",HWOODATTR);

	/* magical power costs for races */
#ifdef ELFMAGIC
	fprintf(fp2,"s/XEMMAG/%3ldK/g\n",ELFMAGIC/1000L);
#else
	fprintf(fp2,"s/XEMMAG/%3ldK/g\n",BASEMAGIC/1000L);
#endif
#ifdef DWFMAGIC
	fprintf(fp2,"s/XDMMAG/%3ldK/g\n",DWFMAGIC/1000L);
#else
	fprintf(fp2,"s/XDMMAG/%3ldK/g\n",BASEMAGIC/1000L);
#endif
#ifdef HUMMAGIC
	fprintf(fp2,"s/XHMMAG/%3ldK/g\n",HUMMAGIC/1000L);
#else
	fprintf(fp2,"s/XHMMAG/%3ldK/g\n",BASEMAGIC/1000L);
#endif
#ifdef ORCMAGIC
	fprintf(fp2,"s/XOMMAG/%3ldK/g\n",ORCMAGIC/1000L);
#else
	fprintf(fp2,"s/XOMMAG/%3ldK/g\n",BASEMAGIC/1000L);
#endif

	/* civilian power costs for races */
#ifdef ELFCIVIL
	fprintf(fp2,"s/XECMAG/%3ldK/g\n",ELFCIVIL/1000L);
#else
	fprintf(fp2,"s/XECMAG/%3ldK/g\n",BASEMAGIC/1000L);
#endif
#ifdef DWFCIVIL
	fprintf(fp2,"s/XDCMAG/%3ldK/g\n",DWFCIVIL/1000L);
#else
	fprintf(fp2,"s/XDCMAG/%3ldK/g\n",BASEMAGIC/1000L);
#endif
#ifdef HUMCIVIL
	fprintf(fp2,"s/XHCMAG/%3ldK/g\n",HUMCIVIL/1000L);
#else
	fprintf(fp2,"s/XHCMAG/%3ldK/g\n",BASEMAGIC/1000L);
#endif
#ifdef ORCCIVIL
	fprintf(fp2,"s/XOCMAG/%3ldK/g\n",ORCCIVIL/1000L);
#else
	fprintf(fp2,"s/XOCMAG/%3ldK/g\n",BASEMAGIC/1000L);
#endif

	/* military power costs for races */
#ifdef ELFMILIT
	fprintf(fp2,"s/XEWMAG/%3ldK/g\n",ELFMILIT/1000L);
#else
	fprintf(fp2,"s/XEWMAG/%3ldK/g\n",BASEMAGIC/1000L);
#endif
#ifdef DWFMILIT
	fprintf(fp2,"s/XDWMAG/%3ldK/g\n",DWFMILIT/1000L);
#else
	fprintf(fp2,"s/XDWMAG/%3ldK/g\n",BASEMAGIC/1000L);
#endif
#ifdef HUMMILIT
	fprintf(fp2,"s/XHWMAG/%3ldK/g\n",HUMMILIT/1000L);
#else
	fprintf(fp2,"s/XHWMAG/%3ldK/g\n",BASEMAGIC/1000L);
#endif
#ifdef ORCMILIT
	fprintf(fp2,"s/XOWMAG/%3ldK/g\n",ORCMILIT/1000L);
#else
	fprintf(fp2,"s/XOWMAG/%3ldK/g\n",BASEMAGIC/1000L);
#endif
	fclose(fp);
	fclose(fp2);
	exit(SUCCESS);
}
