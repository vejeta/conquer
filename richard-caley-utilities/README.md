# Richard Caley Map Drawing Utility

**Original Author**: Richard Caley  
**Written**: July 1989  
**Purpose**: Draws maps of the Conquer world on bitmap systems

## About This Code

This utility was written by Richard Caley in 1989 to create visual maps of Conquer game worlds. It reads Conquer game data and generates bitmap representations of the game world, useful for:

- Visualizing the game world layout
- Creating reference maps for players
- Administrative overview of game state
- Historical documentation of games

## Original Description

From the original source code header:
> "This program draws a map of the conquer world on a bitmap system"

## Technical Notes

- **Era**: Written in 1989 for Unix bitmap systems
- **Language**: C (typical for the period)
- **Dependencies**: Historic Unix bitmap libraries
- **Compatibility**: May require adaptation for modern systems

## License

This code is distributed under Richard Caley's original license terms:

```
You may copy, distribute, modify or do what you will with this code 
so long as this message remains in it and so long as you do not charge 
for it, nor distribute the program without the source.
```

**Key Restrictions**:
- ❌ Cannot charge money for this software
- ✅ Can copy, distribute, and modify freely
- ✅ Must include source code when distributing
- ✅ Must preserve license notice

See `LICENSE` file for complete terms.


### Contact Attempts

In September 2025, we attempted to contact Richard Caley through the University of Edinburgh Alumni Services to request explicit permission for GPL relicensing. Due to data protection policies, the university was unable to provide contact information or forward messages. This preservation effort proceeds under his original permissive license terms, using his code exactly as he intended: freely shared with full attribution and source code availability.

## About Richard Caley

Richard Caley was a researcher at the University of Edinburgh's Centre for Speech Technology Research (CSTR), where he made significant contributions to computational linguistics and speech synthesis technology. He was a key contributor to the Festival Speech Synthesis System, an influential open-source text-to-speech platform that became widely used in research and industry.

Beyond his professional research work, Richard's interests ranged far beyond speech technology. A self-described "long haired, beer drinking, bisexual, computer addicted, under-lifed, gravitationally gifted" individual, Richard embodied the curious, generous spirit of the early Unix computing community.

His technical interests were impressively diverse: ray tracing and graphics programming (using Povray and custom tools), Unix system administration (he ran FreeBSD systems), web development, telecommunications work, and of course, gaming utilities like this map drawer. He was also a devoted Kate Bush fan who contributed to the early web by converting fan documentation to HTML.

Richard's sense of humor shines through in his archived writings. When asked about USENET etiquette, he quipped that poor punctuation was "like bad handwriting" and suggested the offender "probably write[s] in green crayon anyway." He described source code as being "like ketchup but more taisty" and admitted his book-buying habit was so bad that libraries needed "a crowbar to force it out of my hands."

This wit and technical generosity are perfectly captured in his software license: "You may copy, distribute, modify or do what you will with this code" - followed immediately by a self-deprecating comment about "horrid hack[s]" in his implementation. This was someone who shared freely while being refreshingly honest about the pragmatic compromises of 1989 Unix programming.

### Academic and Professional Contributions

Richard's professional work at Edinburgh CSTR included significant contributions to computational linguistics and speech technology:

- **Festival Speech Synthesis System**: Key contributor to this influential open-source text-to-speech platform
- **Research Publications**: Academic work in speech technology and computational linguistics (see [AMiner profile](https://aminer.org/profile/r-caley/65d6f2bdc136ef133132e1d0))
- **Open Source Development**: Applied the same generous sharing philosophy to both academic and hobby programming

His involvement in Festival demonstrates how his collaborative approach extended from game utilities like this map drawer to serious academic software development. The same spirit of open sharing and technical pragmatism that characterizes this 1989 gaming utility also influenced major speech synthesis research tools.

### Historical Context

This utility represents an early example of game data visualization tools, created during the pioneering era of Unix-based multiplayer gaming. Richard's approach to sharing code with such permissive terms was typical of the collaborative culture that defined early Unix computing communities.

## In Memory

Richard Caley passed away in 2005. This code is preserved in his memory and as a testament to the early Unix gaming community's collaborative spirit.

### Historical Materials

For researchers interested in early Unix computing culture and Richard Caley's broader contributions:

- **[Personal Homepage](https://web.archive.org/web/20060621153455/http://richard.caley.org.uk/)** - His main site showing diverse technical interests
- **[Academic Directory](https://web.archive.org/web/20050212154854/http://www.cogsci.ed.ac.uk/~rjc/)** - Research work at Edinburgh CSTR  
- **[Festival Speech Synthesis](https://github.com/festvox/festival/blob/master/ACKNOWLEDGMENTS)** - His contributions to major speech synthesis research
- **[Research Publications](https://aminer.org/profile/r-caley/65d6f2bdc136ef133132e1d0)** - Academic work in computational linguistics
- **[Graphics Gallery](https://web.archive.org/web/20021209075818fw_/http://flowers.ofthenight.org/gallery/index.html)** - Ray tracing art "created with Povray, perl, qbasic, brute force and awkwardness"
- **[The Wit and Wisdom of Richard Caley](https://web.archive.org/web/20060428074215/http://richard.caley.org.uk/caleyisms.html)** - Collection of his humorous USENET posts
- **[Kate Bush Fan Work](https://gaffa.org/cloud/richard_caley.html)** - His contribution to early web fan communities

These archived materials provide invaluable insight into the personalities and culture of the early Unix development community. Richard's combination of technical skill, generous code sharing, and self-deprecating humor represents the collaborative spirit that made early computing communities so innovative.

### Note on Biographical Information

The information about Richard Caley presented here has been compiled from publicly archived web materials and USENET posts. While we have made every effort to represent him accurately and respectfully, we acknowledge that internet archives may not provide the complete picture of his life and work.

We welcome corrections, additional information, or clarification from anyone who knew Richard personally or professionally. If any of the information presented here causes discomfort to family members, colleagues, or friends, we are committed to updating or removing content as appropriate.

Please contact the repository maintainers if you have:
- Corrections to biographical details
- Additional context about his work or contributions  
- Concerns about how his memory is being represented
- Family or personal perspectives we should consider

Our goal is to honor Richard Caley's contribution to early computing while being respectful of his memory and those who knew him.

## Usage Notes

1. **Historical Code**: This is 1989-era C code and may require significant adaptation for modern systems
2. **Game Integration**: Designed to work with Conquer game data files
3. **Bitmap Output**: Originally targeted Unix bitmap display systems
4. **Educational Value**: Excellent reference for understanding early game visualization techniques

## Contributing

Due to the original author's passing, we cannot accept contributions to this specific code. However:

- Bug reports documenting historical behavior are welcome
- Documentation improvements are appreciated
- Modern ports or spiritual successors can be developed separately under different licensing

## Building

Original build instructions (may require adaptation):

```bash
# Note: This is historical - modern systems will likely need modification
make
```

Modern developers should expect to:
- Update include files for current systems
- Adapt bitmap library calls
- Modify file I/O for current Conquer data formats

## Related Files

- Integrates with main Conquer game data (see `../gpl-release/`)
- May reference game world files and player data
- Output intended for period-appropriate bitmap viewers

---

*This software component is preserved for historical and educational purposes, maintaining the original author's contribution to early Unix gaming.*
