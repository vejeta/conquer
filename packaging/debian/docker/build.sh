#!/usr/bin/env sh

#!/bin/bash
# Main build script inside Docker container

set -e

WORK_DIR="/work"
BUILD_DIR="/home/builder/build"
OUTPUT_DIR="/work/packages/debian"

echo "=== Starting Debian package build ==="

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Clone the repository
echo "Cloning repository..."
git clone https://github.com/vejeta/conquer conquer-4.12
cd conquer-4.12

# Copy Debian packaging files
echo "Setting up Debian packaging..."
cp -r "$WORK_DIR/packaging/debian/control" debian/

# Fix Makefile paths (same as melange)
echo "=== Fixing Makefile paths ==="
echo "Original CDEFS:"
grep -n "CDEFS" Makefile | head -3

cp Makefile Makefile.backup

sed -i 's|CDEFS = -DDEFAULTDIR=\\"$(DEFAULT)\\" -DEXEDIR=\\"$(EXEDIR)\\"|CDEFS = -DDEFAULTDIR=\\"/usr/lib/conquer\\" -DEXEDIR=\\"/usr/bin\\"|g' Makefile
sed -i 's|^#CDEFS = -DDEFAULTDIR=\\"$(DEFAULT)\\" -DEXEDIR=\\"$(EXEDIR)\\"|CDEFS = -DDEFAULTDIR=\\"/usr/lib/conquer\\" -DEXEDIR=\\"/usr/bin\\"|g' Makefile

echo "Fixed CDEFS:"
grep -n "CDEFS.*usr/lib" Makefile

# Build the package
echo "=== Building Debian package ==="
export LEGACY_CFLAGS="-std=gnu99 -D_GNU_SOURCE -Wno-implicit-function-declaration -Wno-incompatible-pointer-types -Wno-implicit-int -Wno-return-type -Wno-old-style-definition -Wno-unused-variable -Wno-unused-function -Wno-format"
export DEB_CFLAGS_APPEND="$LEGACY_CFLAGS"

debuild -us -uc -b

# Create output directory and copy packages
mkdir -p "$OUTPUT_DIR"
cp ../*.deb "$OUTPUT_DIR/"
cp ../*.changes "$OUTPUT_DIR/"

echo "=== Package build complete ==="
ls -la "$OUTPUT_DIR/"
