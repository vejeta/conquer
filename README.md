*********************************************************
*	README FILE FOR THE INSTALLER OF CONQUER	*
*********************************************************

CONQUER - Classic Multi-Player Strategy Game

What you have here is a GPL v3 licensed version of the classic CONQUER game,
originally created by Edward M. Barlow and Adam Bryant. This version has been
relicensed to GPL v3 in 2025 by Juan Manuel Méndez Rey (Vejeta) with explicit
permission from the original authors.

LICENSING INFORMATION:
======================
- Originally Copyright (C) 1988-1989 by Edward M. Barlow and Adam Bryant
- Copyright (C) 2025 Juan Manuel Méndez Rey (Vejeta)
- Licensed under GPL v3 with permission from original authors

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version. See the COPYING file for the complete GPL v3 license text.


### Relicensing Documentation

This game has been successfully relicensed from its original restrictive license to GPL v3 through a **15-year effort** (2006-2025) with explicit written permission from all copyright holders:

📄 **[Full Legal Documentation](./RELICENSING-PERMISSIONS.md)** - Complete email permissions with headers  
📄 **[Authors & Attribution](./AUTHORS.md)** - All contributors and their roles  
📄 **[Project History](./HISTORY.md)** - Timeline from 1987 USENET release to present  
📄 **[License Text](./COPYING)** - Full GPL v3 license  

### Summary of Permissions

| Copyright Holder | Role | Permission Date | Status |
|-----------------|------|-----------------|--------|
| **Ed Barlow** | Original creator | March 12, 2016 | ✅ Granted |
| **Adam Bryant** | Co-developer | Feb 23, 2011 | ✅ Granted |
| **Martin Forssen** | PostScript utilities | Sept 15, 2025 | ✅ Granted |

> *"Just wanted to confirm that I had no issues with publication of version 4 of Conquer under the GPL."* - Adam Bryant, 2011

> *"Yes i delegated it all to adam aeons ago. Im easy on it all.... copyleft didnt exist when i wrote it and it was all for fun so..."* - Ed Barlow, 2016

> *""Oh, that was a long time ago. But yes, that was me. And I have no problem with relicensing it to GPL."* - Martin Forssen, 2025

### Legal Validation

This relicensing effort has been:
- ✅ Discussed on [Debian Legal mailing lists](http://lists.debian.org/debian-legal/2006/10/msg00063.html)
- ✅ Tracked as [GNU Savannah task #5945](http://savannah.gnu.org/task/?5945)
- ✅ Documented at [vejeta.com](http://vejeta.com/historia-del-conquer/)
- ✅ Preserved with complete email headers for authentication

### What This Means For You

- ✅ **Free to use, modify, and distribute** under GPL v3 terms
- ✅ **No legal concerns** - all permissions properly obtained
- ✅ **Contribute with confidence** - clear legal foundation
- ✅ **Fork freely** - create your own versions under GPL v3

For questions about the relicensing process: vejeta@gmail.com

---


HISTORICAL NOTE:
================
This is based on the original release 4 version of CONQUER. The original
authors made no guarantees to the sanity or style of this code, but believed
that it should work as documented. It included numerous bugfixes from previous
releases and various enhancements.

The original development team included Edward M. Barlow and Adam Bryant, who
set up mailing lists and provided community support in the late 1980s. Their
innovative work created one of the early multi-player strategy games that
influenced many later games in the genre.

CURRENT STATUS:
===============
This version maintains the original gameplay and functionality while being
made available under modern open-source licensing terms. Bug reports, feature
requests, and contributions are welcome through the project's repository.

INCLUDED IN THIS DISTRIBUTION:
===============================
	1) A Brief Description of Conquer
	2) Installation (unpacking) Instructions
	3) Configuration Instructions
	4) Compilation Instructions
	5) Administration instructions
	6) GPL v3 License Information

-----------------------------------------------------------
I   A Brief Description of Conquer
-----------------------------------------------------------
A complete description of Conquer v4 is contained in "man.pag" and can be
generated in "conquer.docs". The game is a multi-player strategy simulation
where players control nations, managing resources, armies, diplomacy, and
territorial expansion.

Key features include:
- Multi-player strategy gameplay
- Resource management (food, gold, metal, jewels)
- Military units and naval fleets
- Magic system and special powers
- Diplomatic relations between nations
- NPC nations with AI behavior
- Random events and world dynamics
- Customizable world generation

The documentation files txt[0-5] contain help information that is customized
from data in the header files and converted to help[0-5] files during
compilation. "make docs" will create documentation from current data.

-----------------------------------------------------------
II  Installation Instructions
-----------------------------------------------------------
SYSTEM REQUIREMENTS:
- Unix-like operating system (Linux, BSD, macOS)
- C compiler (gcc recommended)
- make utility
- curses library (ncurses)
- Standard Unix utilities

COMPILATION:
1. Extract the source code to a directory
2. Review and modify configuration files (see Configuration section)
3. Compile: `make`
4. Install and set up new game: `make new_game`

If curses linking fails, you may need to add "-ltermcap" or "-lncurses"
to the library flags in the Makefile.

-----------------------------------------------------------
III Configuration
-----------------------------------------------------------
THE FOLLOWING FILES SHOULD BE MODIFIED TO REFLECT YOUR ENVIRONMENT
AND THE TYPE OF GAME YOU WISH TO PLAY:

REQUIRED MODIFICATIONS:
- header.h: Game configuration constants and settings
- Makefile: Build configuration and paths

OPTIONAL MODIFICATIONS:
- rules: Grammar rules for NPC message generation
- nations: NPC nation configurations for world creation

The options specified in these files will be reflected in the documentation
and help files when the program is compiled.

IMPORTANT: Edit the following defines in header.h:
- OWNER: Administrator name
- LOGIN: Administrator login ID
- Directory paths for game data and executables
- System-specific settings (BSD vs SYSV)

The "rules" file contains grammar rules for random messages generated by
NPCs. You may customize this with local names or creative content following
the format: %CLASS declares a class, with %MAIN being the top level.

-----------------------------------------------------------
IV  Compilation Instructions
-----------------------------------------------------------
After configuring header.h and Makefile:

Basic compilation:
	make			# Compile the game
	make clean		# Clean up object files
	make clobber		# Remove all generated files

Game setup:
	make new_game		# Build and install a complete new game
	make install		# Install executables only
	make docs		# Generate documentation

TROUBLESHOOTING:
- If linking fails with curses errors, add "-ltermcap" to LIBRARIES in Makefile
- If make gives "command not found" errors, try: setenv SHELL /bin/sh
- Ensure all directory paths in header.h exist and are writable

-----------------------------------------------------------
V   Administration Instructions
-----------------------------------------------------------
COMMAND LINE ADMINISTRATION:

conqrun options:
	-m          Create/make a new world
	-a          Add new player to existing world
	-x          Execute update (process turn)
	-d DIR      Use specified game directory
	-r SCENARIO Read map from scenario files during world creation

GAME ADMINISTRATION LEVELS:
1. God: Primary administrator (defined by LOGIN in header.h)
2. Demi-God: World-specific administrator (can be changed during game)

During world creation, you'll be prompted to designate a demi-god for the
world, or you can serve as demi-god yourself.

SETUP PROCEDURE:
1. Create world: `conqrun -m`
   - Sets up the game world and NPC nations per the nations file
   - NPCs use the same password as god initially

2. Add players: `conqrun -a`
   - Interactive player addition
   - Won't work if world is full from scenario loading

3. Alternative - Scenario-based: `conqrun -r SCENARIO`
   - Uses SCENARIO.ele, SCENARIO.veg, and SCENARIO.ntn files
   - Pre-configured world setup

GOD FUNCTIONS:
Access god functions by logging in with: `conquer -n god`
- Modify terrain and sectors
- Manage nations (create/destroy)
- Adjust game balance
- Administrative oversight

GAME BALANCE:
The world generation is not perfectly balanced. Some players may start in
difficult positions. As god, you can:
- Redesignate terrain to improve starting positions
- Destroy and recreate nations in better locations
- Modify surrounding terrain for fairness
- Use the change nation command to adjust player situations

REGULAR OPERATION:
- Players connect: `conquer`
- Process turns: `conqrun -x` (typically automated via cron)
- Monitor via god login for administrative needs

AUTOMATED UPDATES:
Use the included "run" script as a template for automated turn processing.
Modify it for your preferred update schedule and system configuration.

For detailed gameplay help, use the '?' command within the game.

-----------------------------------------------------------
VI  Contributing and Support
-----------------------------------------------------------
This open-source version welcomes contributions:
- Bug reports and fixes
- Feature enhancements
- Documentation improvements
- Platform compatibility updates
- Translation efforts

Please maintain the spirit of the original game while modernizing the codebase
for current systems and development practices.

The original creators, Edward M. Barlow and Adam Bryant, laid the foundation
for this classic strategy game. This GPL v3 version ensures it remains
available for future generations of strategy game enthusiasts.

-----------------------------------------------------------
For more information, see the man page (man.pag) and in-game help system.
