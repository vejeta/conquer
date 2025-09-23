#	conquer: Copyright (c) 1989 by Edward M Barlow
#
#	BY CHANGING THIS FILE, YOU AGREE TO ABIDE BY THE LIMITATIONS STATED IN
#	THE LIMITED USE CONTRACT CONTAINED IN THE FILE "header.h"
#
#	Make sure to set your desired configuration by
#	editing the file "header.h".
#
#       This makefile has been modified to allow compilation using
#       a parallelized make program.  It has been used successfully
#       on an Encore Multimax parallel computer with both 4 and
#       6 cpus.
#
#       It should pose no problems for non parallel makes.
#
#       Please report any problems to adb@bucsf.bu.edu
#				   or adb@bu-cs.bu.edu
#
#	Conquer news mailing list: conquer-news-request@bu-cs.bu.edu.
#
MAKE	= /usr/bin/make
CC	= /usr/bin/gcc
RM      = /bin/rm -f

#	LN must be "ln -s" if source and default directories span disks
LN	= /bin/ln
CP	= /bin/cp
NULL	= 2>/dev/null

#	Flags to lint
LTFLG   = -h -lcurses

#	Options for the postscript map printing program.
#	The file 'conqps.ps' will be installed in the EXEDIR
#	directory which is determined below.
#
#	To avoid building this program, remove $(PSPROG) from
#	both the 'all:' list and the 'install:' list
PSPROG  = conqps
PSSRC   = psmap.c
PSHEAD  = psmap.h
PSDATA  = psmap.ps
#	Default Pagesize Setting:
#		A4 for European page size.
#		LETTER for American page size.
#		OTHER for Local page size.  [edit conqps.h]
PSPAGE  = LETTER

#	Options for shar program, SHARLIM is limit of each shar
#	file created in kilobytes and SHARNAM is the prefix for
#	SHARFILE name.
#	[This is for a public domain shar from USENET, I can send
#	copies if you wish - adb@bu-cs.bu.edu]
SHAR	= /usr/bin/shar
SHARLIM	= 50
SHARNAM	= shar.
SHARFLG = -D -c -l$(SHARLIM) -o$(SHARNAM)

#	This should be installed by whomever you want to own the game.
#	I recommend "games" or "root".

#	uncomment the next line if you dont have getopt in your library
#	(eg you are on a pc, or a non unix system).  getopt.c is a
#	public domain software, and has not been tested by the authors
#	of conquer.  [not distributed with conquer V4]
#GETOPT	= getopt.o

#
#	libraries for BSD systems:
LIBRARIES = -lcurses -ltermcap -lcrypt
#
#	libraries for SYSV systems:
#LIBRARIES = -lcurses
#
#	libraries for Xenix systems:
#LIBRARIES = -ltermlib -ltcap -lcrypt

#	CURRENT is this directory.  The directory where the source
#	and Makefile are located
CURRENT = /home/conquer/src

#	DEFAULT is the directory where default nations & help files will be 
#	stored.	 It is also the default directory = where players will play 
#	if they do not use the -d option.
DEFAULT = /home/conquer/lib

#	This directory is where the executables will be stored
EXEDIR = /home/conquer/bin

#	Definitions used for compiling conquer
CDEFS  = -DDEFAULTDIR=\"$(DEFAULT)\" -DEXEDIR=\"$(EXEDIR)\"

#	Options flag used for normal compilation
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
#
#	Set this to some unique identifier for each game you wish
#	to create via 'make new_game'.  It will make a subdirectory
#	$(GAMEID) to the DEFAULT data directory where it will store
#	data for the new game.  [Leave it blank to create the default
#	game]
GAMEID = 

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

# List of temporary C files
DAFILS = cexecuteA.c ioA.c miscA.c navyA.c magicA.c dataA.c tradeA.c
DGFILS = cexecuteG.c ioG.c miscG.c navyG.c magicG.c dataG.c tradeG.c

#txt[0-4] are input help files.  help[0-4] are output. HELPSCR is sed script.
HELP=txt
HELPOUT=help
HELPSCR=sed

HEADERS=header.h data.h newlogin.h patchlevel.h
SUPT1=nations Makefile $(HELP)[0-5] README run man.pag rules
SUPT2=execute messages news commerce CONQPS.INFO
ALLFILS=$(SUPT1) $(HEADERS) $(AFILS) commands.c forms.c main.c move.c \
reports.c display.c extcmds.c newhelp.c sort.c getopt.c \
$(PSSRC) $(PSHEAD) $(PSDATA)

all:	$(ADMIN) $(GAME) $(SORT) $(PSPROG) helpfile
	@echo YAY! make new_game to set up permissions, zero appropriate
	@echo initial files, move $(GAME) and $(ADMIN) to 
	@echo $(EXEDIR), and set up the world.
	@echo If a game is in progress, make install will just move $(GAME) 
	@echo and $(ADMIN) to $(EXEDIR).
	@echo

$(ADMIN):	$(AOBJS)
	@echo phew...
	-$(RM) $(DAFILS) $(NULL)
	@echo if the next command does not work you might also need -ltermcap
	@echo === compiling administrative functions
	$(CC) $(OPTFLG) -o $(ADMIN) $(AOBJS) $(LIBRARIES)
#	comment out the next line during debugging	
	strip $(ADMIN)

$(GAME):	$(GOBJS)
	@echo phew... 
	-$(RM) $(DGFILS) $(NULL)
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
	-$(RM) *.o $(HELPOUT)[0-5] $(PSPROG) $(SORT) insthelp helpfile $(NULL)
	-$(RM) newhelp in$(GAME) in$(SORT) in$(ADMIN) in$(PSPROG) $(NULL)
	-$(RM) $(HELPSCR).[12] lint[ag] conquer.doc $(GAME) $(ADMIN) $(NULL)

clean:
	$(RM) *.o lint[ag] conquer.doc $(NULL)

in$(GAME):	$(GAME)
	-$(RM) $(EXEDIR)/$(GAME)
	mv $(GAME) $(EXEDIR)
	chmod 4751 $(EXEDIR)/$(GAME)
	touch $(GAME)
	touch in$(GAME)

in$(ADMIN):	$(ADMIN)
	-$(RM) $(EXEDIR)/$(ADMIN)
	mv $(ADMIN) $(EXEDIR)
	chmod 4751 $(EXEDIR)/$(ADMIN)
	touch $(ADMIN)
	touch in$(ADMIN)

in$(SORT):	$(SORT)
	-$(RM) $(EXEDIR)/$(SORT)
	mv $(SORT) $(EXEDIR)
	chmod 751 $(EXEDIR)/$(SORT)
	touch $(SORT)
	touch in$(SORT)

in$(PSPROG):	$(PSPROG)
	-$(RM) $(EXEDIR)/$(PSPROG)
	mv $(PSPROG) $(EXEDIR)
	$(CP) $(PSDATA) $(EXEDIR)
	chmod 751 $(EXEDIR)/$(PSPROG)
	chmod 644 $(EXEDIR)/$(PSDATA)
	touch $(PSPROG)
	touch in$(PSPROG)

install:	in$(GAME) in$(ADMIN) in$(SORT) in$(PSPROG) insthelp instman
	@echo ""
	@echo "Installation complete"

new_game:	all insthelp
	@echo Installing new game
	-mkdir $(EXEDIR) $(NULL)
	-mkdir $(DEFAULT)  $(NULL)
	-mkdir $(DEFAULT)/$(GAMEID) $(NULL)
	chmod 755 $(EXEDIR)
	chmod 750 $(DEFAULT)/$(GAMEID) $(DEFAULT)
	$(CP) $(GAME) $(ADMIN) $(SORT) $(PSPROG) $(PSDATA) $(EXEDIR)
	chmod 4755 $(EXEDIR)/$(GAME) $(EXEDIR)/$(ADMIN)
	chmod 0755 $(EXEDIR)/$(SORT) $(EXEDIR)/$(PSPROG)
	chmod 0644 $(EXEDIR)/$(PSDATA)
	chmod 0600 nations
	chmod 0700 run
	$(CP) nations rules $(DEFAULT)/$(GAMEID)
	$(CP) nations rules $(DEFAULT)
	@echo now making the world
	$(EXEDIR)/$(ADMIN) -d "$(GAMEID)" -m
	$(EXEDIR)/$(ADMIN) -d "$(GAMEID)" -a

insthelp:	helpfile
	@echo Installing helpfiles
	-$(RM) $(DEFAULT)/$(HELPOUT)0
	-$(LN) $(CURRENT)/$(HELPOUT)0 $(DEFAULT)/$(HELPOUT)0
	-$(RM) $(DEFAULT)/$(HELPOUT)1
	-$(LN) $(CURRENT)/$(HELPOUT)1 $(DEFAULT)/$(HELPOUT)1
	-$(RM) $(DEFAULT)/$(HELPOUT)2
	-$(LN) $(CURRENT)/$(HELPOUT)2 $(DEFAULT)/$(HELPOUT)2
	-$(RM) $(DEFAULT)/$(HELPOUT)3
	-$(LN) $(CURRENT)/$(HELPOUT)3 $(DEFAULT)/$(HELPOUT)3
	-$(RM) $(DEFAULT)/$(HELPOUT)4
	-$(LN) $(CURRENT)/$(HELPOUT)4 $(DEFAULT)/$(HELPOUT)4
	-$(RM) $(DEFAULT)/$(HELPOUT)5
	-$(LN) $(CURRENT)/$(HELPOUT)5 $(DEFAULT)/$(HELPOUT)5
	touch insthelp

instman:
	@echo Installing man pages
	$(CP) man.pag $(EXEDIR)

helpfile:	$(HELPOUT)0 $(HELPOUT)1 $(HELPOUT)2 $(HELPOUT)3 $(HELPOUT)4 $(HELPOUT)5
	@echo Helpfiles built
	touch helpfile

$(HELPOUT)0:	$(HELP)0 $(HELPSCR).1 $(HELPSCR).2
	@echo building $(HELPOUT)0
	cat $(HELP)0 | sed -f $(HELPSCR).1 | sed -f $(HELPSCR).2 > $(HELPOUT)0

$(HELPOUT)1:	$(HELP)1 $(HELPSCR).1 $(HELPSCR).2
	@echo building $(HELPOUT)1
	cat $(HELP)1 | sed -f $(HELPSCR).1 | sed -f $(HELPSCR).2 > $(HELPOUT)1

$(HELPOUT)2:	$(HELP)2 $(HELPSCR).1 $(HELPSCR).2
	@echo building $(HELPOUT)2
	cat $(HELP)2 | sed -f $(HELPSCR).1 | sed -f $(HELPSCR).2 > $(HELPOUT)2

$(HELPOUT)3:	$(HELP)3 $(HELPSCR).1 $(HELPSCR).2
	@echo building $(HELPOUT)3
	cat $(HELP)3 | sed -f $(HELPSCR).1 | sed -f $(HELPSCR).2 > $(HELPOUT)3

$(HELPOUT)4:	$(HELP)4 $(HELPSCR).1 $(HELPSCR).2
	@echo building $(HELPOUT)4
	cat $(HELP)4 | sed -f $(HELPSCR).1 | sed -f $(HELPSCR).2 > $(HELPOUT)4

$(HELPOUT)5:	$(HELP)5 $(HELPSCR).1 $(HELPSCR).2
	@echo building $(HELPOUT)5
	cat $(HELP)5 | sed -f $(HELPSCR).1 | sed -f $(HELPSCR).2 > $(HELPOUT)5

$(HELPSCR).1:	newhelp
	newhelp

$(HELPSCR).2:	newhelp
	newhelp

newhelp:	dataG.o	newhelp.o
	$(CC) $(OPTFLG) dataG.o newhelp.o -o newhelp
	strip newhelp
#
#	postscript map program
PSOPTS  = -DPSFILE=\"$(EXEDIR)/$(PSDATA)\" -D$(PSPAGE)
#
$(PSPROG):	$(PSSRC) $(PSDATA) $(PSHEAD)
	$(CC) $(OPTFLG) $(PSOPTS) $(PSSRC) -o $(PSPROG)
#	comment out the next line if debugging
	strip $(PSPROG)

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
	$(SHAR) $(SHARFLG) $(ALLFILS) MANIFEST

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

ioG.o:	data.h header.h patchlevel.h io.c

ioA.o:	data.h header.h patchlevel.h io.c

newlogin.o:	data.h header.h newlogin.h patchlevel.h newlogin.c

main.o:	data.h header.h patchlevel.h main.c

newhelp.o:	data.h header.h patchlevel.h newhelp.c

#	Clear suffixes
.SUFFIXES:	

#	Suffixes for conquer files.
.SUFFIXES:	A.o G.o .c .h .o
