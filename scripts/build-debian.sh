#!/usr/bin/env bash

# Debian package build for ConquerV5 from gpl-release source
# Updated to work with source code in gpl-release/ folder without copying back to root

set -e

# Get the directory containing this script
SCRIPT_DIR="$(dirname "$0")"
# Get the repo root (parent of scripts directory)
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "Script directory: $SCRIPT_DIR"
echo "Repository root: $REPO_ROOT"

# Build the Docker image if it doesn't exist
if ! docker images | grep -q conquer-debian-builder; then
    echo "Building Debian packaging Docker image..."
    docker build -t conquer-debian-builder "$REPO_ROOT/packaging/debian/docker/"
fi

# Build the Debian package
echo "Building Debian package from gpl-release source..."
docker run --privileged --rm \
    -v "$REPO_ROOT":/work \
    -w /work \
    conquer-debian-builder \
    bash -c "
        # Create a temporary build directory to avoid polluting the repo root
        mkdir -p /tmp/build-conquer
        cd /tmp/build-conquer

        # Copy source from gpl-release to the temporary build directory
        echo 'Copying source from gpl-release to temporary build directory...'
        cp -r /work/gpl-release/* . 2>/dev/null || true

        # Copy debian packaging files to the build directory
        cp -r /work/packaging/debian ./

        # Run the build from the temporary directory
        echo 'Building Debian package...'
        /work/packaging/debian/docker/build.sh

        # Copy the built packages back to the mounted work directory
        mkdir -p /work/packages/debian
        if [ -d packages/debian ]; then
            cp packages/debian/* /work/packages/debian/ 2>/dev/null || true
        fi

        # Also check for packages in parent directory (common debian build location)
        if [ -f ../*.deb ]; then
            cp ../*.deb /work/packages/debian/ 2>/dev/null || true
        fi

        echo 'Package files copied to /work/packages/debian/'
    "

echo "Debian package built successfully!"
echo "Package location: packages/debian/"

# Verify packages were created
if [ -d "$REPO_ROOT/packages/debian" ] && [ "$(ls -A "$REPO_ROOT/packages/debian" 2>/dev/null)" ]; then
    echo "✅ Found Debian packages:"
    ls -la "$REPO_ROOT/packages/debian/"
else
    echo "⚠️  No packages found in packages/debian/"
    echo "Checking for packages in other locations..."
    find "$REPO_ROOT" -name "*.deb" -type f 2>/dev/null | head -5
fi
