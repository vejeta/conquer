#!/bin/bash
# Equivalent to your build-melange.sh but for Debian packages

set -e

# Change to repo root directory
cd "$(dirname "$0")/.."

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
    /work/packaging/debian/docker/build.sh

echo "Debian package built successfully!"
echo "Package location: packages/debian/"
