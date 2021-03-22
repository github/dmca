#!/bin/bash
echo Signing $1
dpkg-deb -R $1 Package
pushd "Package"
find . -type f -perm +0111 \( -exec ../../sign.sh {} \; \)
popd
dpkg-deb -b Package
mv Package.deb $1
rm -rf Package
