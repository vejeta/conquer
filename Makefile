# Modernized Makefile for Conquer Game
# Based on the original 1988-1989 Makefile by Edward M. Barlow and Adam Bryant
# Copyright (C) 2025 Juan Manuel Méndez Rey (vejeta) - Licensed under GPL v3

# =============================================================================
# PROJECT CONFIGURATION
# =============================================================================

PACKAGE_NAME = conquer
VERSION = 4
PATCHLEVEL = 11a

# Main targets
TARGET_GAME = conquer
TARGET_ADMIN = conqrun
TARGET_SORT = conqsort
TARGET_PS = conqps

# =============================================================================
# PLATFORM AND TOOL DETECTION
# =============================================================================

UNAME_S := $(shell uname -s 2>/dev/null || echo Unknown)
UNAME_M := $(shell uname -m 2>/dev/null || echo Unknown)

# Modern tool detection (use PATH instead of hardcoded paths)
CC := $(shell command -v gcc 2>/dev/null || command -v clang 2>/dev/null || command -v cc 2>/dev/null || echo gcc)
MAKE := $(shell command -v make 2>/dev/null || echo make)
RM = rm -f
LN = ln -sf
CP = cp
MKDIR_P = mkdir -p
STRIP = strip
NULL = 2>/dev/null

# =============================================================================
# BUILD CONFIGURATION
# =============================================================================

BUILD_TYPE ?= release
PREFIX ?= $(HOME)/conquer
CURRENT = $(PWD)
DEFAULT = $(PREFIX)/lib
EXEDIR = $(PREFIX)/bin

# =============================================================================
# COMPILER AND LIBRARY DETECTION
# =============================================================================

# Base compiler definitions
CDEFS = -DDEFAULTDIR=\"$(DEFAULT)\" -DEXEDIR=\"$(EXEDIR)\"
CDEFS += -DVERSION=\"$(VERSION)\" -DPATCHLEVEL=\"$(PATCHLEVEL)\"
CDEFS += -DLOGIN=\"$(shell whoami)\"

# Platform-specific library detection
ifeq ($(UNAME_S),Linux)
    # Try pkg-config first, fallback to manual detection
    NCURSES_LIBS := $(shell pkg-config --libs ncurses 2>/dev/null || echo "-lncurses")
    NCURSES_CFLAGS := $(shell pkg-config --cflags ncurses 2>/dev/null)
    LIBRARIES = $(NCURSES_LIBS) -lcrypt
    LTFLG = -h $(NCURSES_LIBS)
else ifeq ($(UNAME_S),Darwin)
    # macOS
    NCURSES_LIBS := $(shell pkg-config --libs ncurses 2>/dev/null || echo "-lncurses")
    NCURSES_CFLAGS := $(shell pkg-config --cflags ncurses 2>/dev/null)
    LIBRARIES = $(NCURSES_LIBS)
    LTFLG = -h $(NCURSES_LIBS)
else
    # BSD and other Unix-like systems
    LIBRARIES = -lcurses -ltermcap
    LTFLG = -h -lcurses
endif

# Build type specific flags
ifeq ($(BUILD_TYPE),debug)
    OPTFLG = -DDEBUG -g3 -O0 -fno-omit-frame-pointer -Wall -Wextra -Wpedantic
    DO_STRIP = false
else
    OPTFLG = -g -fno-strict-aliasing -fwrapv -Wall -Wextra -O2
    DO_STRIP = true
endif

# Add ncurses flags if detected
ifneq ($(NCURSES_CFLAGS),)
    OPTFLG += $(NCURSES_CFLAGS)
endif

# =============================================================================
# SOURCE FILES AND OBJECTS
# =============================================================================

# PostScript program configuration
PSPROG = $(TARGET_PS)
PSSRC = psmap.c
PSHEAD = psmap.h
PSDATA = psmap.ps
PSPAGE = LETTER
PSOPTS = -DPSFILE=\"$(EXEDIR)/$(PSDATA)\" -D$(PSPAGE)

# Administrative executable sources (includes combat.c)
AFILS = combat.c cexecute.c io.c admin.c makeworl.c navy.c spew.c \
        newlogin.c update.c magic.c npc.c misc.c randeven.c data.c trade.c check.c

# Game executable sources (does NOT include combat.c)
GFILS = commands.c cexecute.c forms.c io.c main.c move.c navy.c \
        magic.c misc.c reports.c data.c display.c extcmds.c trade.c check.c

# Object files - using the traditional naming scheme that works
AOBJS = combat.o cexecuteA.o ioA.o admin.o makeworl.o navyA.o spew.o \
        newlogin.o update.o magicA.o npc.o miscA.o randeven.o dataA.o \
        tradeA.o check.o

GOBJS = commands.o cexecuteG.o forms.o ioG.o main.o move.o navyG.o \
        magicG.o miscG.o reports.o dataG.o display.o extcmds.o tradeG.o \
        check.o

# Temporary files created during build
DAFILS = cexecuteA.c ioA.c miscA.c navyA.c magicA.c dataA.c tradeA.c
DGFILS = cexecuteG.c ioG.c miscG.c navyG.c magicG.c dataG.c tradeG.c

# Help files
HELP = txt
HELPOUT = help
HELPSCR = sed

# Headers and support files
HEADERS = header.h data.h newlogin.h patchlevel.h
SUPT1 = nations Makefile $(HELP)[0-5] README run man.pag rules
SUPT2 = execute messages news commerce CONQPS.INFO
ALLFILS = $(SUPT1) $(HEADERS) $(AFILS) commands.c forms.c main.c move.c \
          reports.c display.c extcmds.c newhelp.c sort.c $(PSSRC) $(PSHEAD) $(PSDATA)

# =============================================================================
# BUILD TARGETS
# =============================================================================

.PHONY: all clean clobber install new_game debug release help config check lint

# Default target
all: $(TARGET_ADMIN) $(TARGET_GAME) $(TARGET_SORT) $(PSPROG) helpfile
	@echo "✓ Build complete for $(UNAME_S) ($(UNAME_M))"
	@echo "Run 'make new_game' to set up a new game world"
	@echo "Run 'make install' to install existing game"

# Administrative executable
$(TARGET_ADMIN): $(AOBJS)
	@echo "=== Linking administrative executable..."
	-$(RM) $(DAFILS) $(NULL)
	$(CC) $(OPTFLG) $(CDEFS) -o $@ $(AOBJS) $(LIBRARIES)
ifeq ($(DO_STRIP),true)
	$(STRIP) $@
endif

# Game executable
$(TARGET_GAME): $(GOBJS)
	@echo "=== Linking game executable..."
	-$(RM) $(DGFILS) $(NULL)
	$(CC) $(OPTFLG) -o $@ $(GOBJS) $(LIBRARIES)
ifeq ($(DO_STRIP),true)
	$(STRIP) $@
endif

# Sort utility
$(TARGET_SORT): sort.c
	$(CC) $(OPTFLG) -o $@ sort.c
ifeq ($(DO_STRIP),true)
	$(STRIP) $@
endif

# PostScript map program
$(PSPROG): $(PSSRC) $(PSDATA) $(PSHEAD)
	$(CC) $(OPTFLG) $(PSOPTS) $(PSSRC) -o $@
ifeq ($(DO_STRIP),true)
	$(STRIP) $@
endif

# =============================================================================
# COMPILATION RULES
# =============================================================================

# Clear default suffixes and define our own
.SUFFIXES:
.SUFFIXES: A.o G.o .c .h .o

# Generic compilation rule for files that don't need special treatment
.c.o:
	$(CC) $(OPTFLG) $(CDEFS) -DADMIN -DCONQUER -c $<

# Admin-specific compilation (creates temporary symlinks)
.cA.o:
	@trap "" 0 1 2 3 4; \
	$(LN) $*.c $*A.c; \
	$(CC) $(OPTFLG) $(CDEFS) -DADMIN -c $*A.c -o $@; \
	$(RM) $*A.c

# Game-specific compilation (creates temporary symlinks)
.cG.o:
	@trap "" 0 1 2 3 4; \
	$(LN) $*.c $*G.c; \
	$(CC) $(OPTFLG) $(CDEFS) -DCONQUER -c $*G.c -o $@; \
	$(RM) $*G.c

# =============================================================================
# HELP FILE GENERATION
# =============================================================================

helpfile: $(HELPOUT)0 $(HELPOUT)1 $(HELPOUT)2 $(HELPOUT)3 $(HELPOUT)4 $(HELPOUT)5
	@echo "✓ Help files built"
	@touch helpfile

$(HELPOUT)0: $(HELP)0 $(HELPSCR).1 $(HELPSCR).2
	@echo "Building $(HELPOUT)0..."
	@cat $(HELP)0 | sed -f $(HELPSCR).1 | sed -f $(HELPSCR).2 > $@

$(HELPOUT)1: $(HELP)1 $(HELPSCR).1 $(HELPSCR).2
	@echo "Building $(HELPOUT)1..."
	@cat $(HELP)1 | sed -f $(HELPSCR).1 | sed -f $(HELPSCR).2 > $@

$(HELPOUT)2: $(HELP)2 $(HELPSCR).1 $(HELPSCR).2
	@echo "Building $(HELPOUT)2..."
	@cat $(HELP)2 | sed -f $(HELPSCR).1 | sed -f $(HELPSCR).2 > $@

$(HELPOUT)3: $(HELP)3 $(HELPSCR).1 $(HELPSCR).2
	@echo "Building $(HELPOUT)3..."
	@cat $(HELP)3 | sed -f $(HELPSCR).1 | sed -f $(HELPSCR).2 > $@

$(HELPOUT)4: $(HELP)4 $(HELPSCR).1 $(HELPSCR).2
	@echo "Building $(HELPOUT)4..."
	@cat $(HELP)4 | sed -f $(HELPSCR).1 | sed -f $(HELPSCR).2 > $@

$(HELPOUT)5: $(HELP)5 $(HELPSCR).1 $(HELPSCR).2
	@echo "Building $(HELPOUT)5..."
	@cat $(HELP)5 | sed -f $(HELPSCR).1 | sed -f $(HELPSCR).2 > $@

$(HELPSCR).1 $(HELPSCR).2: newhelp
	@./newhelp

newhelp: dataG.o newhelp.o
	$(CC) $(OPTFLG) dataG.o newhelp.o -o $@
ifeq ($(DO_STRIP),true)
	$(STRIP) $@
endif

# =============================================================================
# INSTALLATION TARGETS
# =============================================================================

# Individual installation targets
in$(TARGET_GAME): $(TARGET_GAME)
	@echo "Installing game executable..."
	@$(MKDIR_P) $(EXEDIR) $(NULL)
	-$(RM) $(EXEDIR)/$(TARGET_GAME)
	@mv $(TARGET_GAME) $(EXEDIR)
	@chmod 4751 $(EXEDIR)/$(TARGET_GAME)
	@touch in$(TARGET_GAME)

in$(TARGET_ADMIN): $(TARGET_ADMIN)
	@echo "Installing admin executable..."
	@$(MKDIR_P) $(EXEDIR) $(NULL)
	-$(RM) $(EXEDIR)/$(TARGET_ADMIN)
	@mv $(TARGET_ADMIN) $(EXEDIR)
	@chmod 4751 $(EXEDIR)/$(TARGET_ADMIN)
	@touch in$(TARGET_ADMIN)

in$(TARGET_SORT): $(TARGET_SORT)
	@echo "Installing sort utility..."
	@$(MKDIR_P) $(EXEDIR) $(NULL)
	-$(RM) $(EXEDIR)/$(TARGET_SORT)
	@mv $(TARGET_SORT) $(EXEDIR)
	@chmod 751 $(EXEDIR)/$(TARGET_SORT)
	@touch in$(TARGET_SORT)

in$(PSPROG): $(PSPROG)
	@echo "Installing PostScript utility..."
	@$(MKDIR_P) $(EXEDIR) $(NULL)
	-$(RM) $(EXEDIR)/$(PSPROG)
	@mv $(PSPROG) $(EXEDIR)
	@$(CP) $(PSDATA) $(EXEDIR)
	@chmod 751 $(EXEDIR)/$(PSPROG)
	@chmod 644 $(EXEDIR)/$(PSDATA)
	@touch in$(PSPROG)

insthelp: helpfile
	@echo "Installing help files..."
	@$(MKDIR_P) $(DEFAULT) $(NULL)
	@$(CP) $(HELPOUT)0 $(DEFAULT)/$(HELPOUT)0
	@$(CP) $(HELPOUT)1 $(DEFAULT)/$(HELPOUT)1
	@$(CP) $(HELPOUT)2 $(DEFAULT)/$(HELPOUT)2
	@$(CP) $(HELPOUT)3 $(DEFAULT)/$(HELPOUT)3
	@$(CP) $(HELPOUT)4 $(DEFAULT)/$(HELPOUT)4
	@$(CP) $(HELPOUT)5 $(DEFAULT)/$(HELPOUT)5
	@touch insthelp

instman:
	@echo "Installing man pages..."
	@$(MKDIR_P) $(EXEDIR) $(NULL)
	@$(CP) man.pag $(EXEDIR)

# Complete installation
install: in$(TARGET_GAME) in$(TARGET_ADMIN) in$(TARGET_SORT) in$(PSPROG) insthelp instman
	@echo ""
	@echo "✓ Installation complete to $(PREFIX)"

# Set up a new game world
new_game: all insthelp
	@echo "Setting up new game world..."
	@$(MKDIR_P) $(EXEDIR) $(DEFAULT) $(NULL)
	@chmod 755 $(EXEDIR)
	@chmod 750 $(DEFAULT)
	@$(CP) $(TARGET_GAME) $(TARGET_ADMIN) $(TARGET_SORT) $(PSPROG) $(PSDATA) $(EXEDIR)
	@chmod 4755 $(EXEDIR)/$(TARGET_GAME) $(EXEDIR)/$(TARGET_ADMIN)
	@chmod 0755 $(EXEDIR)/$(TARGET_SORT) $(EXEDIR)/$(PSPROG)
	@chmod 0644 $(EXEDIR)/$(PSDATA)
	@chmod 0600 nations
	@chmod 0700 run
	@$(CP) nations rules $(DEFAULT)
	@echo "Creating world..."
	@$(EXEDIR)/$(TARGET_ADMIN) -m
	@$(EXEDIR)/$(TARGET_ADMIN) -a
	@echo "✓ New game world created!"

# =============================================================================
# CLEANING TARGETS
# =============================================================================

clean:
	@echo "Cleaning object files..."
	@$(RM) *.o lint[ag] conquer.doc $(NULL)
	@$(RM) $(DAFILS) $(DGFILS) $(NULL)

clobber: clean
	@echo "Removing all generated files..."
	@$(RM) $(HELPOUT)[0-5] $(PSPROG) $(TARGET_SORT) insthelp helpfile $(NULL)
	@$(RM) newhelp in$(TARGET_GAME) in$(TARGET_SORT) in$(TARGET_ADMIN) in$(PSPROG) $(NULL)
	@$(RM) $(HELPSCR).[12] $(TARGET_GAME) $(TARGET_ADMIN) $(NULL)

# =============================================================================
# DEVELOPMENT AND TESTING TARGETS
# =============================================================================

debug: BUILD_TYPE=debug
debug: all

release: BUILD_TYPE=release
release: all

check: all
	@echo "Running basic tests..."
	@if [ -x ./$(TARGET_GAME) ]; then echo "✓ Game executable OK"; else echo "✗ Game missing"; exit 1; fi
	@if [ -x ./$(TARGET_ADMIN) ]; then echo "✓ Admin executable OK"; else echo "✗ Admin missing"; exit 1; fi
	@echo "✓ Build verification complete"

lint:
	@echo "Running lint analysis..."
	@if command -v lint >/dev/null 2>&1; then \
		lint $(LTFLG) $(CDEFS) -DCONQUER $(GFILS) > lintg 2>/dev/null || true; \
		lint $(LTFLG) $(CDEFS) -DADMIN $(AFILS) > linta 2>/dev/null || true; \
		echo "✓ Lint analysis complete (see lintg and linta)"; \
	else \
		echo "⚠ lint not available"; \
	fi

config:
	@echo "Build Configuration:"
	@echo "  Platform: $(UNAME_S) ($(UNAME_M))"
	@echo "  Compiler: $(CC)"
	@echo "  Build Type: $(BUILD_TYPE)"
	@echo "  Install Prefix: $(PREFIX)"
	@echo "  Libraries: $(LIBRARIES)"
	@echo "  C Flags: $(OPTFLG) $(CDEFS)"

help:
	@echo "Conquer Game Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build all executables (default)"
	@echo "  clean        - Remove object files"
	@echo "  clobber      - Remove all generated files"
	@echo "  install      - Install existing game"
	@echo "  new_game     - Build and setup new game world"
	@echo "  check        - Run basic tests"
	@echo "  lint         - Run lint analysis"
	@echo "  config       - Show build configuration"
	@echo "  debug        - Build with debug flags"
	@echo "  release      - Build optimized version"
	@echo ""
	@echo "Variables:"
	@echo "  BUILD_TYPE=debug|release  (default: release)"
	@echo "  PREFIX=/path/to/install   (default: $(HOME)/conquer)"
	@echo "  CC=compiler               (auto-detected: $(CC))"
	@echo ""
	@echo "Examples:"
	@echo "  make BUILD_TYPE=debug"
	@echo "  make new_game PREFIX=/opt/conquer"

# =============================================================================
# DOCUMENTATION TARGETS
# =============================================================================

docs: conquer.doc

conquer.doc: $(HELPOUT)0 $(HELPOUT)1 $(HELPOUT)2 $(HELPOUT)3 $(HELPOUT)4 $(HELPOUT)5
	@echo "Creating documentation..."
	@cat $(HELPOUT)? | sed -e "s/^DONE//g" | sed -e "s/^END//g" > $@

# Archive targets (legacy)
cpio:
	-$(RM) core
	find . -name '*[CrpsEech]' -print | cpio -ocBv > cpiosv

shar:
	echo " lines   words chars   FILENAME" > MANIFEST
	wc $(ALLFILS) >> MANIFEST
	shar -D -c -l50 -oshar. $(ALLFILS) MANIFEST

# =============================================================================
# DEPENDENCY RULES
# =============================================================================

# Explicit dependencies for key files
$(GOBJS): data.h header.h
$(AOBJS): data.h header.h

ioG.o: data.h header.h patchlevel.h io.c
ioA.o: data.h header.h patchlevel.h io.c
newlogin.o: data.h header.h newlogin.h patchlevel.h newlogin.c
main.o: data.h header.h patchlevel.h main.c
newhelp.o: data.h header.h patchlevel.h newhelp.c
