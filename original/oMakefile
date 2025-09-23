#	conquer: Copyright (c) 1989 by Edward M Barlow
#
#	BY CHANGING THIS FILE, YOU AGREE TO ABIDE BY THE LIMITATIONS STATED IN
#	THE LIMITED USE CONTRACT CONTAINED IN THE FILE "header.h"
#
#       This makefile has been modified to allow compilation using
#       a parallelized make program.  It has been used successfully
#       on an Encore Multimax parallel computer with both 4 and
#       6 cpus.
#
#       It should pose no problems for non parallel makes.
#
#       Please report any problems to adb@bucsf.bu.edu
#
MAKE	= /bin/make
CC	= /bin/cc
RM      = /bin/rm -f

#	LN must be "ln -s" if source, data, and default span disks
LN	= ln
CP	= cp
NULL	= 2>/dev/null

#	Flags to lint
LTFLG   = -h -lcurses

#	Options for shar program, SHARLIM is limit of each shar
#	file created in kilobytes and SHARNAM is the prefix for
#	SHARFILE name.
#	[This is for a public domain shar from USENET, I can send
#	copies if you wish - adb@bu-cs.bu.edu]
SHAR	= xshar
SHARLIM	= 50
SHARNAM	= shar.
SHARFLG = -D -c -l$(SHARLIM) -o$(SHARNAM)

#	This should be installed by whomever you want to own the game.
#	I recommend "games" or "root".

#	uncomment the next line if you dont have getopt in your library
#	(eg you are on a pc, or a non unix system).  getopt.c is a
#	public domain software, and has not been tested by the authors
#	of conquer.
#GETOPT	= getopt.o

#if the final link does not compile change to the line below
LIBRARIES = -lcurses -ltermcap
#LIBRARIES = -lcurses

#	DEFAULT is the directory where default nations & help files will be 
#	stored.	 It is also the default directory = where players will play 
#	if they do not use the -d option.
DEFAULT = /usr4/acm/stud/adb/games/conqlib

#	This directory is where the executables will be stored
EXEDIR = /usr4/acm/stud/adb/games

#	Definitions used for compiling conquer
CDEFS  = -DDEFAULTDIR=\"$(DEFAULT)\" -DEXEDIR=\"$(EXEDIR)\"

#	Options flag used for non-debugging purposes
OPTFLG  = -O

#	Options flag used for debugging purposes
#	[make sure to comment out 'strip' commands in install section]
#OPTFLG  = -DDEBUG -g

#	this is the name of the user executable
#       the user executable contains commands for the games players
GAME  = conquer
#	this is the name of the administrative executable
#       the administrative executable contains commands for the game super user
ADMIN = conqrun
#	this is the name of the sorting program which conquer uses
SORT  = conqsort

#       GAME IDENTIFICATION
#	this is the game identifier.  See the DATA variable below
GAMEID = 1
#	This directory is where individual Conquer game data will be stored.
#	As multiple simultaneous games are supported, each game must have its 
#	own directory.
DATA  = $(DEFAULT)/$(GAMEID)

#	Suffixes for conquer files.
.SUFFIXES:	A.o G.o

# AFILS are files needed for game updating...
AFILS = combat.c cexecute.c io.c admin.c makeworl.c navy.c spew.c \
newlogin.c update.c magic.c npc.c misc.c randeven.c data.c trade.c check.c
AOBJS = combat.o cexecuteA.o ioA.o admin.o makeworl.o navyA.o spew.o \
newlogin.o update.o magicA.o npc.o miscA.o randeven.o dataA.o \
tradeA.o $(GETOPT) check.o

# GFILS are files needed to run a normal interactive game
GFILS = commands.c cexecute.c forms.c io.c main.c move.c navy.c \
magic.c misc.c reports.c data.c display.c extcmds.c trade.c check.c
GOBJS = commands.o cexecuteG.o forms.o ioG.o main.o move.o navyG.o \
magicG.o miscG.o reports.o dataG.o display.o extcmds.o tradeG.o \
$(GETOPT) check.o

#txt[0-4] are input help files.  help[0-4] are output. HELPSCR is sed script.
HELP=txt
HELPOUT=help
HELPSCR=sed

HEADERS=header.h data.h newlogin.h patchlevel.h
SUPT1=nations Makefile $(HELP)[0-5] README run man.page rules
SUPT2=execute messages news commerce
ALLFILS=$(SUPT1) $(HEADERS) $(AFILS) commands.c forms.c main.c move.c \
reports.c display.c extcmds.c newhelp.c sort.c getopt.c

all:	$(ADMIN) $(GAME) $(SORT) helpfile
	@echo YAY! make new_game to set up permissions, zero appropriate
	@echo initial files, move $(GAME) and $(ADMIN) to 
	@echo $(EXEDIR), and set up the world.
	@echo If a game is in progress, make install will just move $(GAME) 
	@echo and $(ADMIN) to $(EXEDIR).
	@echo

$(ADMIN):	$(AOBJS)
	@echo phew...
	@echo if the next command does not work you might also need -ltermcap
	@echo === compiling administrative functions
	$(CC) $(OPTFLG) -o $(ADMIN) $(AOBJS) $(LIBRARIES)
#	comment out the next line during debugging	
	strip $(ADMIN)

$(GAME):	$(GOBJS)
	@echo phew... 
	@echo if the next command does not work you might also need -ltermcap
	@echo === compiling user interface
	$(CC) $(OPTFLG) -o $(GAME) $(GOBJS) $(LIBRARIES)
#	comment out the next line during debugging
	strip $(GAME)

$(SORT):	sort.c
	$(CC) $(OPTFLG) -o $(SORT) sort.c
#	comment out the next line if debugging
	strip $(SORT)

clobber:
	$(RM) *.o $(HELPOUT)[0-5] $(SORT) newhelp in$(GAME) in$(SORT) in$(ADMIN) $(HELPSCR).[12] lint[aghs] conquer.doc $(GAME) $(ADMIN) $(NULL)

clean:
	$(RM) *.o lint[aghs] conquer.doc $(NULL)

in$(GAME):	$(GAME)
	-$(RM) $(EXEDIR)/$(GAME)
	$(CP) $(GAME) $(EXEDIR)
	chmod 4751 $(EXEDIR)/$(GAME)
	touch in$(GAME)

in$(ADMIN):	$(ADMIN)
	-$(RM) $(EXEDIR)/$(ADMIN)
	$(CP) $(ADMIN) $(EXEDIR)
	chmod 4751 $(EXEDIR)/$(ADMIN)
	touch in$(ADMIN)

in$(SORT):	$(SORT)
	-$(RM) $(EXEDIR)/$(SORT)
	$(CP) $(SORT) $(EXEDIR)
	chmod 751 $(EXEDIR)/$(SORT)
	touch in$(SORT)

install:	in$(GAME) in$(ADMIN) in$(SORT) helpfile
	@echo ""
	@echo "Installation complete"

new_game:	all
	@echo Installing new game
	-mkdir $(EXEDIR) $(NULL)
	-mkdir $(DATA) $(NULL)
	-mkdir $(DEFAULT)  $(NULL)
	chmod 755 $(EXEDIR)
	chmod 750 $(DATA) $(DEFAULT)
	$(CP) $(GAME) $(ADMIN) $(SORT) $(EXEDIR)
	chmod 4755 $(EXEDIR)/$(GAME) $(EXEDIR)/$(ADMIN)
	chmod 0755 $(EXEDIR)/$(SORT)
	chmod 0600 nations
	chmod 0700 run
	$(CP) nations rules $(DATA)
	$(CP) nations rules $(DEFAULT)
	@echo now making the world
	$(EXEDIR)/$(ADMIN) -d$(DATA) -m
	$(EXEDIR)/$(ADMIN) -d$(DATA) -a

helpfile:	$(HELPOUT)0 $(HELPOUT)1 $(HELPOUT)2 $(HELPOUT)3 $(HELPOUT)4 $(HELPOUT)5
	@echo Helpfiles built
	touch helpfile

$(HELPOUT)0:	$(HELP)0 $(HELPSCR).1 $(HELPSCR).2
	@echo building $(HELPOUT)0
	cat $(HELP)0 | sed -f $(HELPSCR).1 | sed -f $(HELPSCR).2 > $(HELPOUT)0
	-$(RM) $(DEFAULT)/$(HELPOUT)0
	-$(LN) $(HELPOUT)0 $(DEFAULT)/$(HELPOUT)0

$(HELPOUT)1:	$(HELP)1 $(HELPSCR).1 $(HELPSCR).2
	@echo building $(HELPOUT)1
	cat $(HELP)1 | sed -f $(HELPSCR).1 | sed -f $(HELPSCR).2 > $(HELPOUT)1
	-$(RM) $(DEFAULT)/$(HELPOUT)1
	-$(LN) $(HELPOUT)1 $(DEFAULT)/$(HELPOUT)1

$(HELPOUT)2:	$(HELP)2 $(HELPSCR).1 $(HELPSCR).2
	@echo building $(HELPOUT)2
	cat $(HELP)2 | sed -f $(HELPSCR).1 | sed -f $(HELPSCR).2 > $(HELPOUT)2
	-$(RM) $(DEFAULT)/$(HELPOUT)2
	-$(LN) $(HELPOUT)2 $(DEFAULT)/$(HELPOUT)2

$(HELPOUT)3:	$(HELP)3 $(HELPSCR).1 $(HELPSCR).2
	@echo building $(HELPOUT)3
	cat $(HELP)3 | sed -f $(HELPSCR).1 | sed -f $(HELPSCR).2 > $(HELPOUT)3
	-$(RM) $(DEFAULT)/$(HELPOUT)3
	-$(LN) $(HELPOUT)3 $(DEFAULT)/$(HELPOUT)3

$(HELPOUT)4:	$(HELP)4 $(HELPSCR).1 $(HELPSCR).2
	@echo building $(HELPOUT)4
	cat $(HELP)4 | sed -f $(HELPSCR).1 | sed -f $(HELPSCR).2 > $(HELPOUT)4
	-$(RM) $(DEFAULT)/$(HELPOUT)4
	-$(LN) $(HELPOUT)4 $(DEFAULT)/$(HELPOUT)4

$(HELPOUT)5:	$(HELP)5 $(HELPSCR).1 $(HELPSCR).2
	@echo building $(HELPOUT)5
	cat $(HELP)5 | sed -f $(HELPSCR).1 | sed -f $(HELPSCR).2 > $(HELPOUT)5
	-$(RM) $(DEFAULT)/$(HELPOUT)5
	-$(LN) $(HELPOUT)5 $(DEFAULT)/$(HELPOUT)5

$(HELPSCR).1:	newhelp
	newhelp

$(HELPSCR).2:	newhelp
	newhelp

newhelp:	dataG.o	newhelp.o
	$(CC) dataG.o newhelp.o -o newhelp

lint:
	lint $(LTFLG) $(CDEFS) -DCONQUER $(GFILS) > lintg
	lint $(LTFLG) $(CDEFS) -DADMIN $(AFILS) > linta

docs:	conquer.doc

conquer.doc:	$(HELPOUT)0 $(HELPOUT)1 $(HELPOUT)2 $(HELPOUT)3 $(HELPOUT)4 $(HELPOUT)5
	cat $(HELPOUT)? |sed -e "s/^DONE//g"|sed -e "s/^END//g" >conquer.doc

cpio:
	-$(RM) core
	find . -name '*[CrpsEech]' -print | cpio -ocBv > cpiosv

shar:	
	echo " lines   words chars   FILENAME" > MANIFEST
	wc $(ALLFILS) >> MANIFEST
	$(SHAR) $(SHARFLG) $(ALLFILS)

.cA.o:	$<
	( trap "" 0 1 2 3 4 ; $(LN) $*.c $*A.c ;\
	$(CC) $(OPTFLG) $(CDEFS) -DADMIN -c $*A.c ;\
	$(RM) $*A.c )

.cG.o:	$<
	( trap "" 0 1 2 3 4 ; $(LN) $*.c $*G.c ;\
	$(CC) $(OPTFLG) $(CDEFS) -DCONQUER -c $*G.c ;\
	$(RM) $*G.c )

.c.o:	$<
#	compiles using both defines since they
#	are needed for the data.h definitions for
#	each file... but should not be needed for
#	the actual C source file being compiled
	$(CC) $(OPTFLG) $(CDEFS) -DADMIN -DCONQUER -c $*.c

$(GOBJS):	data.h header.h

$(AOBJS):	data.h header.h

