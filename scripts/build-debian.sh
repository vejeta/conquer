#!/usr/bin/env bash

# Equivalent to your build-melange.sh but for Debian packages
# Updated to work with source code in gpl-release/ folder

set -e

# Build the Docker image if it doesn't exist
if ! docker images | grep -q conquer-debian-builder; then
    echo "Building Debian packaging Docker image..."
    docker build -t conquer-debian-builder packaging/debian/docker/
fi

# Build the Debian package
echo "Building Debian package..."
docker run --privileged --rm \
    -v "$PWD":/work \
    -w /work \
    conquer-debian-builder \
    bash -c "
        # Copy source from gpl-release to current directory for debian packaging
        echo 'Copying source from gpl-release...'
        cp -r gpl-release/* . 2>/dev/null || true
        
        # Run the original build script
        /work/packaging/debian/docker/build.sh
    "

echo "Debian package built successfully!"
echo "Package location: packages/debian/"