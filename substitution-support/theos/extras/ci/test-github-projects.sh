#!/bin/bash
# exit if anything fails
set -e

# ensure this is only used in CI
if [[ -z $CI ]]; then
	echo "This script is for use in a CI environment." >&2
	exit 1
fi

# check our vars are there
if [[ -z $THEOS ]]; then
	echo "The Theos env var is missing." >&2
	exit 1
fi

for i in hbang/libcephei hbang/TypeStatus thomasfinch/PriorityHub; do
	# create a directory
	TEMP_DIR=$(mktemp -d)
	cd $TEMP_DIR

	# download and extract the repo
	curl -fsSL https://github.com/$i/tarball/master | tar -xz --strip-components=1

	# some of the test repos still use a theos symlink
	ln -s $THEOS theos

	# build!
	make package messages=yes

	# go up to the parent dir and remove this test dir
	cd ..
	rm -rf $(basename $TEMP_DIR)
done
