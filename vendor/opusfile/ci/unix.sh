# Continuous integration build script for opusfile.
# This script is run by automated frameworks to verify commits
# see https://mf4.xiph.org/jenkins/job/opusfile-unix/

# This is intended to be run from the top-level source directory.

set -x

# WARNING: clobbers outside the current tree!
rm  -f ../opus
ln -s /srv/jenkins/jobs/opus/workspace ../opus

# compile
make -C unix PKG_CONFIG_PATH=$PWD/../opus clean
make -C unix PKG_CONFIG_PATH=$PWD/../opus

# run any built-in tests
make -C unix PKG_CONFIG_PATH=$PWD/../opus check

# build the documentation
make -C doc
# currently fails on jenkins (debian stretch)
# make -C doc/latex
