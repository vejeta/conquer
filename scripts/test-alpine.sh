#!/usr/bin/env sh

# Test the Alpine package built from gpl-release source
# Packages should be in packages/x86_64/ after running build-melange.sh

# Check if package exists
if [ ! -f packages/x86_64/conquer-4.12-r0.apk ]; then
    echo "Error: Package not found at packages/x86_64/conquer-4.12-r0.apk"
    echo "Please run scripts/build-melange.sh first"
    echo ""
    echo "Available packages:"
    ls -la packages/x86_64/ 2>/dev/null || echo "packages/x86_64/ directory doesn't exist"
    exit 1
fi

echo "Found package: packages/x86_64/conquer-4.12-r0.apk"

# Start Alpine container with the APK available
docker run --rm -it -v "$PWD":/work alpine:latest sh -c "
  # Install dependencies and the game
  apk add --no-cache ncurses-terminfo-base ncurses
  apk add --allow-untrusted /work/packages/x86_64/conquer-4.12-r0.apk
  
  # Set up environment
  export TERM=xterm
  export HOME=/tmp/conquer-test
  mkdir -p \$HOME
  
  # Show what's available
  echo '=== Game installed! Available commands: ==='
  echo 'conquer -h    # Game help'
  echo 'conqrun -h    # Admin help'
  echo 'conquer       # Start the game'
  echo 'conqrun -m    # Create a world (admin)'
  echo ''
  echo 'Package info:'
  apk info conquer
  echo ''
  echo 'Installed files:'
  apk info -L conquer | head -10
  echo ''
  
  # Drop into interactive shell
  /bin/sh
"