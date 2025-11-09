# Conquer - GPL v3 Release

This directory contains the **modern GPL v3 licensed version** of Conquer, relicensed with explicit permission from all original copyright holders (Ed Barlow, Adam Bryant, and Martin Forssen).

**Why use this version?** This GPL v3 release is freely available for commercial and non-commercial use, modification, and distribution. The original version in `../original/` has restrictive licensing (personal use only).

---

## Quick Start

### Prerequisites

**Build dependencies:**
- `libncurses-dev` (or `ncurses-dev` on Alpine) - ncurses development headers
- `build-essential` - gcc, make, libc-dev
- `pkg-config`
- `sed`, `coreutils`

**Runtime dependencies:**
- `libncurses6` (or `ncurses` on Alpine)

**Install on Debian/Ubuntu:**
```bash
sudo apt-get update
sudo apt-get install -y libncurses-dev build-essential pkg-config
```

**Install on Alpine:**
```bash
apk add ncurses-dev build-base pkgconfig make sed coreutils
```

---

## Building from Source

### Basic Compilation

```bash
# Clone the repository
git clone https://github.com/vejeta/conquer.git
cd conquer/gpl-release

# Build the game (IMPORTANT: use -std=gnu99 flag)
make all OPTFLG="-std=gnu99 -D_GNU_SOURCE"
```

**Important:** Modern GCC (15.x+) requires the `-std=gnu99 -D_GNU_SOURCE` flags due to legacy function declarations in the codebase. Without these flags, compilation will fail with function declaration errors.

### What Gets Built

The build process creates the following executables:

| Executable | Size | Description |
|------------|------|-------------|
| `conquer` | ~304KB | Main game executable (player client) |
| `conqrun` | ~345KB | Server/administration executable |
| `conqsort` | ~15KB | Sorting utility for game data |
| `conqps` | ~19KB | PostScript map generator |

Additionally, help files (`help0` through `help5`) are generated for the in-game help system.

### Verify the Build

```bash
# Check that executables were created
ls -lh conquer conqrun conqsort conqps

# Test that executables run
./conquer -h || echo "Help test complete"
./conqrun -h || echo "Help test complete"
```

---

## Installation

### Default Installation (No sudo required)

By default, the game installs to `$HOME/conquer`:

```bash
make install
```

This creates:
- `$HOME/conquer/bin/` - Game executables
- `$HOME/conquer/lib/` - Game data files

### Custom Installation Path

```bash
# Install to a custom location (no sudo required if you own the directory)
make install PREFIX=/path/to/your/directory

# Install to system directory (requires sudo)
sudo make install PREFIX=/usr
```

### Create New Game World

Sets up a complete new game world with executables and data files:

```bash
make new_game
```

This will:
- Install all executables to `$HOME/conquer/bin/`
- Create game data directory at `$HOME/conquer/lib/`
- Generate initial world data
- Set proper permissions

---

## Building Distribution Packages

For distribution maintainers and advanced users, you can build binary packages using the provided scripts.

**Note:** These scripts use Docker containers to ensure reproducible builds.

### Build Debian Package (.deb)

From the repository root:

```bash
# Build the package
./scripts/build-debian.sh

# Package will be created in: packages/debian/
# Filename format: conquer_4.12-1_ARCH.deb
# Where ARCH is: amd64, arm64, armhf, etc.
```

**Test the package (optional):**
```bash
./scripts/test-debian.sh
```

**Install the package:**
```bash
sudo dpkg -i packages/debian/conquer_4.12-1_amd64.deb
sudo apt-get install -f  # Fix any dependencies
```

### Build Alpine Package (.apk)

From the repository root:

```bash
# Build the package
./scripts/build-melange.sh

# Package will be created in: packages/x86_64/conquer-4.12-r0.apk
```

**Test the package (optional):**
```bash
./scripts/test-alpine.sh
```

**Install the package:**
```bash
sudo apk add --allow-untrusted packages/x86_64/conquer-4.12-r0.apk
```

**Requirements:** Both scripts require Docker to be installed and running.

---

## Download Pre-built Packages

Instead of building from source, you can download pre-built binary packages from the [GitHub Releases page](https://github.com/vejeta/conquer/releases).

### Available Downloads

Visit: https://github.com/vejeta/conquer/releases

Each release includes:
- **Debian packages**: `conquer_4.12-1_amd64.deb` (and other architectures)
- **Alpine packages**: `conquer-4.12-r0.apk`
- **Source tarball**: `conquer-v4.12-1.tar.gz`
- **Checksums**: `checksums.txt` (for verification)

### Debian/Ubuntu Installation

```bash
# Visit the releases page and download the .deb for your architecture
# Example for amd64:
wget https://github.com/vejeta/conquer/releases/download/v4.12-1/conquer_4.12-1_amd64.deb

# Install
sudo dpkg -i conquer_4.12-1_amd64.deb
sudo apt-get install -f
```

### Alpine Linux Installation

```bash
# Download the .apk package
wget https://github.com/vejeta/conquer/releases/download/v4.12-1/conquer-4.12-r0.apk

# Install
sudo apk add --allow-untrusted conquer-4.12-r0.apk
```

### Verify Download Integrity

All releases include a `checksums.txt` file:

```bash
# Download checksums
wget https://github.com/vejeta/conquer/releases/download/v4.12-1/checksums.txt

# Verify the downloaded file
sha256sum -c checksums.txt
```

---

## Playing the Game

After installation, start the game:

```bash
# If installed with 'make install' (default $HOME/conquer):
$HOME/conquer/bin/conquer

# Or add to your PATH:
export PATH="$HOME/conquer/bin:$PATH"
conquer

# If installed from .deb or .apk package:
conquer
```

For server administration:

```bash
conqrun -h    # Show help
conqrun -m    # Create a new world (admin)
```

---

## Makefile Targets

| Target | Description |
|--------|-------------|
| `all` | Build all executables (default) |
| `clean` | Remove object files |
| `clobber` | Remove all generated files |
| `install` | Install to PREFIX (default: $HOME/conquer) |
| `new_game` | Build and setup new game world |
| `check` | Run basic tests |
| `config` | Show build configuration |
| `debug` | Build with debug flags |
| `help` | Show all available targets |

**Examples:**

```bash
# Build with debug symbols
make debug

# Install to /opt/conquer (requires sudo)
sudo make install PREFIX=/opt/conquer

# Install to your home directory (no sudo)
make install PREFIX=$HOME/games/conquer

# Show current build configuration
make config

# Clean and rebuild
make clobber && make
```

---

## Troubleshooting

### Compilation Errors

**Problem:** Function declaration conflicts (e.g., `conflicting types for 'access'`)

**Solution:** Use the `-std=gnu99 -D_GNU_SOURCE` flags:
```bash
make clean
make all OPTFLG="-std=gnu99 -D_GNU_SOURCE"
```

### Missing ncurses

**Problem:** `ncurses.h: No such file or directory`

**Solution:** Install development headers:
```bash
# Debian/Ubuntu
sudo apt-get install libncurses-dev

# Alpine
apk add ncurses-dev
```

### Permission Errors During Installation

**Problem:** Permission denied when running `make install`

**Solution:** Check your PREFIX:
- Default `$HOME/conquer` - no sudo needed
- System directories like `/usr` or `/opt` - use sudo
```bash
# For user installation (no sudo)
make install

# For system installation (requires sudo)
sudo make install PREFIX=/usr
```

---

## Additional Documentation

- **Root README**: [`../README.md`](../README.md) - Project overview and relicensing history
- **License Details**: [`../LICENSE-NOTICE.md`](../LICENSE-NOTICE.md) - Complete licensing framework
- **Relicensing Proof**: [`RELICENSING-PERMISSIONS.md`](RELICENSING-PERMISSIONS.md) - Email permissions from original authors
- **Original Version**: [`../original/`](../original/) - Historical distribution (restrictive license)

---

## Contributing

This is the recommended version for contributions. See the main [README](../README.md) for contribution guidelines.

**Quick contribution workflow:**
```bash
git clone https://github.com/vejeta/conquer.git
cd conquer/gpl-release
# Make your changes
make all OPTFLG="-std=gnu99 -D_GNU_SOURCE"
make check
# Submit pull request
```

---

## License

This version is licensed under **GPL v3 or later**. See [`COPYING`](COPYING) for the full license text.

All code in this directory has been relicensed with explicit written permission from the original copyright holders:
- Edward M. Barlow (original creator)
- Adam Bryant (co-developer)
- Martin Forssen (PostScript utilities)

---

For questions or issues, please visit the [GitHub repository](https://github.com/vejeta/conquer).
