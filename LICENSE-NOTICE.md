# License Notice

This document explains the legal framework for the multi-component Conquer repository and how different licenses interact.

## Repository Structure

This repository follows the proven dual-licensing structure established in [conquerv5](https://github.com/vejeta/conquerv5):

- **`gpl-release/`** - Clean GPL v3 distribution for modern development
- **`original/`** - Complete historical preservation of original distribution  
- **`richard-caley-utilities/`** - Detailed documentation of Richard Caley's contributions
- **Root level** - Licensing documentation and prominent attribution

## Component Details

### Core Game (GPL v3)
- **Location**: `gpl-release/` folder
- **License**: GNU General Public License v3.0+
- **Authors**: Edward M. Barlow and Adam Bryant (original), Juan Manuel Méndez Rey (GPL coordination)
- **Usage**: Free to use, modify, and distribute under GPL terms

**Legal Basis**:
- Explicit written permission obtained from both original authors
- Relicensing process completed in 2025
- All copyright holders have agreed to GPL terms

**Rights Granted**:
- ✅ Use for any purpose (including commercial)
- ✅ Modify and create derivative works
- ✅ Distribute original and modified versions
- ✅ Charge fees for distribution
- ✅ Private use and modification

**Obligations**:
- 📋 Provide source code with distribution
- 📋 Preserve copyright notices
- 📋 Include GPL license text
- 📋 Document modifications
- 📋 License derivatives under GPL

### Richard Caley Map Utility (Custom License)
- **Location**: `original/utilities/` folder (original code), `richard-caley-utilities/` folder (documentation)
- **License**: Custom permissive license (see `richard-caley-utilities/LICENSE`)
- **Author**: Richard Caley (deceased 2006)
- **Usage**: Free to copy, distribute, and modify with restrictions (no charging, must include source)

**Legal Basis**:
- Original author's published license terms
- Permissive license allows copying and modification
- Used under original terms as published

**Rights Granted**:
- ✅ Copy and distribute
- ✅ Modify ("do what you will")
- ✅ Use for any non-commercial purpose
- ✅ Create derivative works

**Obligations**:
- 📋 Preserve license notice in all copies
- 📋 Distribute source code with program
- ❌ Cannot charge money for the software

**Restrictions**:
- ❌ No commercial distribution (charging fees)
- ❌ Cannot distribute without source code

## License Interaction

### Compatibility Analysis

The two licenses are **NOT directly compatible** for code merging due to conflicting terms:

- **GPL v3**: Permits charging fees (essential freedom)
- **Caley License**: Prohibits charging fees (explicit restriction)

### Safe Usage Patterns

✅ **ALLOWED**:
- Distribute both components in the same repository
- Use both components in the same project
- Document both components together
- Link to both from the same website
- Include both in the same documentation

✅ **ALLOWED with care**:
- Create wrapper scripts that use both (keep components separate)
- Build systems that compile both (as separate executables)
- Configuration that coordinates both (without code merging)

❌ **NOT ALLOWED**:
- Merge Caley code directly into GPL components
- Include Caley code in GPL-licensed files
- Create combined works under single license
- Sublicense Caley code under GPL terms

### Distribution Guidelines

When distributing this software:

1. **Preserve both license texts** in their respective locations (`gpl-release/LICENSE` and `richard-caley-utilities/LICENSE`)
2. **Document the dual licensing** clearly (this document serves that purpose)
3. **Separate the components** in distribution packages:
   - `gpl-release/` contains clean GPL code
   - `original/` preserves complete historical distribution
   - `richard-caley-utilities/` provides documentation context
4. **Inform users** about different terms for each component

## Legal Precedents

This approach follows established patterns:

- **Debian**: Multi-license packages with clear component separation
- **GNU/Linux**: Combines GPL and non-GPL components (kernel modules, firmware)
- **Web browsers**: Bundle components under different licenses
- **Academic software**: Preserves historical code with original licenses

## Contribution Guidelines

### For Core Game (GPL Components)

- Submit contributions under GPL v3+ terms
- Ensure new code is compatible with GPL
- Follow GPL requirements for derivative works
- Can freely modify and enhance

### For Richard Caley Components

- **Cannot accept contributions** to original code (author deceased)
- Can document bugs or historical behavior
- Can create separate derivative works under different licenses
- Original code preserved as historical artifact

## Future Considerations

### Modern Ports

Developers wishing to create modern versions of Richard Caley's map utility can:

1. **Study the original** for reference (educational use)
2. **Implement from scratch** under new license
3. **Create inspired-by versions** under GPL or other licenses
4. **Document historical techniques** for educational purposes

### License Evolution

- **Core Game**: Can be upgraded to future GPL versions
- **Caley Code**: Frozen at original 1989 terms (cannot be changed)

## Quick Reference

| Component | Location | License | Can Charge? | Source Required? | Modifications OK? |
|-----------|----------|---------|-------------|------------------|-------------------|
| Core Game | `gpl-release/` | GPL v3  | ✅ Yes      | ✅ Yes           | ✅ Yes            |
| Map Utility | `original/utilities/` | Custom | ❌ No       | ✅ Yes           | ✅ Yes*           |

*Modifications to Richard Caley's code are permitted by his license but preserved historically in `original/`.

## FAQ

**Q: Can I charge for the complete package?**  
A: No, because it includes Richard Caley's component which prohibits charging.

**Q: Can I charge for just the GPL component?**  
A: Yes, if distributed separately from the Caley component.

**Q: Can I modify Richard Caley's code?**  
A: Yes, his license explicitly permits modification.

**Q: Can I contribute improvements to Richard Caley's code?**  
A: No, we preserve it as a historical artifact in its original form.

**Q: Are the licenses compatible for linking?**  
A: No, they cannot be directly linked into a single executable under unified terms.

**Q: Can I create a GUI that uses both?**  
A: Yes, as long as you keep the components separate and respect both license terms.

## Contact

For legal questions about this licensing framework:
- Create an issue in the GitHub repository
- Consult with legal counsel for commercial use cases
- Reference this document in license discussions

---

*This document provides guidance but is not legal advice. Consult qualified legal counsel for specific situations.*
