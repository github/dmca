#!/bin/bash
cd "$1"
# Use fakeroot.sh to ensure that permissions are preserved, and install.exec to ensure that we are running tar -x on the right system.
fakeroot.sh -r tar -c . | install.exec "tar -x -C \"$2\""
