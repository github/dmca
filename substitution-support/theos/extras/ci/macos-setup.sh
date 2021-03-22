#!/bin/bash
# exit if anything fails
set -e

# source our common script
source "$(dirname "$0")"/common.sh

# ensure this is only used in appveyor
if [[ -z $CI || -z $TRAVIS ]]; then
	echo "This script is for use in a Travis CI environment." >&2
	exit 1
fi

# check our arguments are there
if [[ -z $THEOS ]]; then
	echo "The Theos env var is missing." >&2
	exit 1
fi

# update homebrew
status "Updating Homebrew"
brew update

# install dependencies
status "Installing dependencies"
brew install ldid xz perl
hash -r
PERL_MM_USE_DEFAULT=1 cpan IO::Compress::Lzma

# ensure we’re in $THEOS
cd $THEOS

# grab the sdk
status "Installing SDK"
cd sdks
curl -fsSL https://github.com/theos/sdks/tarball/master | tar -xz --strip-components=1

# we should be ready now
cd -
echo
echo Done
