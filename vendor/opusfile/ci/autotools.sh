# Continuous integration build script for opusfile.
# This script is run by automated frameworks to verify commits
# see https://mf4.xiph.org/jenkins/job/opusfile-autotools/

# This is intended to be run from the top-level source directory.

set -x

# WARNING: clobbers outside the current tree!
rm  -f ../opus
ln -s /srv/jenkins/jobs/opus/workspace ../opus
rm  -f ../ogg
ln -s /srv/jenkins/jobs/libogg/workspace ../ogg

# HACK: libtool can't link a dynamic library to a static
# library, and the 'unix' makefile build can't link to
# a libopus.la. As a work around, hack our own pkg-config
# file for the uninstalled opus library we want to build
# against.
cat <<EOF > opus-uninstalled.pc
# Opus codec uninstalled pkg-config file
# hacked up for the opusfile autotools build.

libdir=\${pcfiledir}/../opus
includedir=\${libdir}/include

Name: opus uninstalled for opusfile
Description: Opus IETF audio codec (not installed)
Version: 1.0.1
Requires:
Conflicts:
Libs: \${libdir}/libopus.la -lm
Cflags: -I\${includedir}
EOF

cat <<EOF > ogg-uninstalled.pc
# ogg uninstalled pkg-config file
# hacked up for the opusfile autotools build

libdir=\${pcfiledir}/../ogg/src
includedir=\${pcfiledir}/../ogg/include

Name: ogg uninstalled for opusfile
Description: ogg is a library for manipulating ogg bitstreams (not installed)
Version: 1.3.0
Requires:
Conflicts:
Libs: \${libdir}/libogg.la
Cflags: -I\${includedir}
EOF

PKG_CONFIG_PATH=$PWD

# compile
./autogen.sh
./configure PKG_CONFIG_PATH=${PKG_CONFIG_PATH}
make clean
make

# verify distribution target
make distcheck PKG_CONFIG_PATH=${PKG_CONFIG_PATH}

# build the documentation
# currently fails on jenkins (debian stretch)
# make -C doc/latex
