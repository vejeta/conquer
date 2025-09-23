# Conquer - Classic Unix Strategy Game

**Conquer** is the classic multi-player strategy game originally created by Edward M. Barlow and Adam Bryant in the late 1980s. This repository preserves both the historical distribution and provides a modern GPL-licensed version for continued development.

## 🗺️ Special Recognition: Richard Caley's Map Utility

This repository includes a significant historical contribution: **Richard Caley's map drawing utility** from 1989. Richard Caley, a researcher at the University of Edinburgh's Centre for Speech Technology Research and contributor to the Festival Speech Synthesis System, created a bitmap visualization tool for Conquer game worlds.

His work represents an early example of game data visualization, created with the generous spirit typical of the late 1980s Unix computing community. Richard wrote: *"You may copy, distribute, modify or do what you will with this code"* - embodying the collaborative ethos that shaped early computer gaming.

**📖 Learn more about Richard Caley's contributions**: See [`richard-caley-utilities/`](richard-caley-utilities/) for detailed biographical information, technical context, and links to his archived research and personal materials.

*Richard Caley passed away in 2006. This code is preserved in his memory and as a testament to the early Unix gaming community's collaborative spirit.*

## Relicensing Achievement

This repository represents a successful **15-year relicensing effort** (2006-2025) that transformed Conquer from a restrictively licensed game into a modern GPL v3 project. All original copyright holders provided explicit written permission:

### Summary of Permissions

| Copyright Holder | Role | Permission Date | Status |
|-----------------|------|-----------------|--------|
| **Ed Barlow** | Original creator | March 12, 2016 | ✅ Granted |
| **Adam Bryant** | Co-developer | Feb 23, 2011 | ✅ Granted |
| **Martin Forssen** | PostScript utilities | Sept 15, 2025 | ✅ Granted |

> *"Just wanted to confirm that I had no issues with publication of version 4 of Conquer under the GPL."* - Adam Bryant, 2011

> *"Yes i delegated it all to adam aeons ago. Im easy on it all.... copyleft didnt exist when i wrote it and it was all for fun so..."* - Ed Barlow, 2016

> *"Oh, that was a long time ago. But yes, that was me. And I have no problem with relicensing it to GPL."* - Martin Forssen, 2025

### Legal Validation

This relicensing effort has been:
- ✅ Discussed on [Debian Legal mailing lists](http://lists.debian.org/debian-legal/2006/10/msg00063.html)
- ✅ Tracked as [GNU Savannah task #5945](http://savannah.gnu.org/task/?5945)
- ✅ Documented at [vejeta.com](http://vejeta.com/historia-del-conquer/)
- ✅ Preserved with complete email headers for authentication

📄 **Complete legal documentation**: See [`gpl-release/RELICENSING-PERMISSIONS.md`](gpl-release/RELICENSING-PERMISSIONS.md) for full email permissions with headers.

## Repository Structure

This repository uses a dual-licensing approach to balance historical preservation with modern development:

### 🚀 For Modern Development
- **[`gpl-release/`](gpl-release/)** - Clean GPL v3 licensed distribution
  - Free to use, modify, and distribute commercially
  - Relicensed with explicit permission from all original authors
  - Recommended for new projects and contributions

### 📚 For Historical Research  
- **[`original/`](original/)** - Complete original distribution preserved
  - Original restrictive licensing (personal use only)
  - Includes Richard Caley's map utility in `original/utilities/`
  - Historical accuracy for researchers and gaming archaeologists

### 🎯 Documentation & Attribution
- **[`richard-caley-utilities/`](richard-caley-utilities/)** - Detailed documentation of Richard Caley's contributions
- **[`LICENSES/`](LICENSES/)** - All license texts for REUSE compliance
- **[`LICENSE-NOTICE.md`](LICENSE-NOTICE.md)** - Comprehensive licensing framework explanation

## Quick Start

### Playing the Game
Use the modern GPL version for the best experience:

```bash
cd gpl-release/
# See gpl-release/README.md for complete build and setup instructions
```

### Historical Research
Explore the original distribution exactly as it was shared in the late 1980s:

```bash
cd original/
# See original README and build instructions for historical reference
```

## About Conquer

Conquer is a turn-based strategy game where players control nations in a fantasy world, managing resources, armies, diplomacy, and territorial expansion. Key features include:

- Multi-player strategy gameplay with up to 50+ nations
- Resource management (food, gold, metal, jewels)  
- Military units, naval fleets, and siege warfare
- Magic system with spells and artifacts
- Diplomatic relations and trade between nations
- NPC nations with AI behavior
- Random events and dynamic world systems
- Customizable world generation

## Contributors & History

- **Edward M. Barlow** (1987-1988) - Original creator and core engine
- **Adam Bryant** (1987-1988) - Co-author, maintainer, and enhancements  
- **Richard Caley** (1989) - Map drawing utility for world visualization
- **Martin Forssen** (1989) - PostScript utilities and enhancements
- **Juan Manuel Méndez Rey** (2006-2025) - Historical preservation and GPL relicensing coordination

## Licensing

This repository contains software under multiple licenses:

| Component | Location | License | Commercial Use |
|-----------|----------|---------|----------------|
| **Modern Game** | `gpl-release/` | GPL v3+ | ✅ Allowed |
| **Original Distribution** | `original/` | Restrictive | ❌ Personal use only |
| **Richard Caley Utility** | `original/utilities/` | Custom permissive | ❌ No charging allowed |

**📋 See [`LICENSE-NOTICE.md`](LICENSE-NOTICE.md)** for complete licensing details and legal framework.

## Contributing

Contributions are welcome for the GPL-licensed components in [`gpl-release/`](gpl-release/):

- Bug reports and fixes
- Feature enhancements  
- Documentation improvements
- Modern platform support
- Translation efforts

Please maintain the spirit of the original game while modernizing for current systems.

## Historical Significance

This preservation effort maintains one of the early examples of:
- Unix-based multiplayer gaming (1987-1988)
- Community-driven game development via USENET
- Early game world visualization tools (Richard Caley's work)
- Collaborative open-source game development principles

The generous code-sharing philosophy of contributors like Richard Caley helped establish the collaborative culture that continues in modern open-source gaming.

---

**🎮 Ready to conquer?** Start with [`gpl-release/`](gpl-release/) for the modern experience, or explore [`original/`](original/) for historical gaming archaeology.

**📖 Learn about the people behind the code:** Visit [`richard-caley-utilities/`](richard-caley-utilities/) to discover the fascinating individuals who created this classic game.