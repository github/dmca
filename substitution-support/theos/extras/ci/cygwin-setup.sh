#!/bin/bash
# exit if anything fails
set -e

# source our common script
source "$(dirname "$0")"/common.sh

# ensure this is only used in appveyor
if [[ -z $CI || -z $APPVEYOR ]]; then
	echo "This script is for use in an AppVeyor CI environment." >&2
	exit 1
fi

# check our arguments are there
if [[ -z $THEOS ]]; then
	echo "The Theos env var is missing." >&2
	exit 1
fi

# install dependencies
status "Installing dependencies"
PERL_MM_USE_DEFAULT=1 cpan IO::Compress::Lzma

# ensure we’re in $THEOS
cd $THEOS

# grab submodules
status "Cloning submodules"
git submodule update --init --recursive

# grab the toolchain
status "Installing toolchain"
mkdir -p toolchain/windows/iphone
cd toolchain/windows/iphone
curl -fsSL https://github.com/coolstar/iOSToolchain4Win/tarball/x86_64 | tar -xz --strip-components=1

# grab the sdk
status "Installing SDK"
cd -
cd sdks
curl -fsSL https://github.com/theos/sdks/tarball/master | tar -xz --strip-components=1

# we should be ready now
cd -
echo
echo Done
