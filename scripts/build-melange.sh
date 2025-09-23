#!/bin/bash
# ConquerV5 Melange package build script - using local copy approach

set -e

# Get the directory containing this script
SCRIPT_DIR="$(dirname "$0")"
# Get the repo root (parent of scripts directory)
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "Script directory: $SCRIPT_DIR"
echo "Repository root: $REPO_ROOT"

# Change to repo root directory
cd "$REPO_ROOT"

# Ensure we have the melange configuration
if [ ! -f "packaging/melange/melange.yaml" ]; then
    echo "Error: melange.yaml not found in packaging/melange/"
    exit 1
fi

# Verify gpl-release directory exists
if [ ! -d "gpl-release" ]; then
    echo "Error: gpl-release directory not found in repo root"
    echo "Contents of repo root:"
    ls -la
    exit 1
fi

# Create packages directory
mkdir -p packages/alpine

echo "=== Building ConquerV5 APK package with Melange ==="

# Install melange if not available
if ! command -v melange &> /dev/null; then
    echo "Installing melange..."
    # Using Docker with melange
    docker run --privileged --rm -v "$PWD":/work -w /work \
        cgr.dev/chainguard/melange build packaging/melange/melange.yaml --arch x86_64

else
    # Use local melange installation
    melange build \
        --arch=x86_64 \
        --out-dir=packages/alpine \
        packaging/melange/melange.yaml
fi

echo "=== APK package built successfully! ==="
echo "Package location: packages/alpine/"
ls -la packages/alpine/

# Verify the package was created
APK_COUNT=$(find packages/alpine/ -name "*.apk" | wc -l)
if [ "$APK_COUNT" -eq 0 ]; then
    echo "ERROR: No APK packages were created!"
    exit 1
fi

echo "✅ Found $APK_COUNT APK package(s)"
find packages/alpine/ -name "*.apk" -exec ls -lh {} \;
