#!/bin/bash
pushd "apt/"
for i in *.deb; do
./resign.sh $i
done
popd
