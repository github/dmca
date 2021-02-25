#!/bin/bash

TARGET=release_ps2

set -e

LIBRWDIR=$(dirname "$(readlink -f "$0")")
echo "LIBRWDIR is $LIBRWDIR"

cd "$LIBRWDIR"

premake5 gmake

docker rm librw_instance -f >/dev/null 2>&1 || true
docker stop librw_instance -t 0 >/dev/null 2>&1 || true

docker pull librw/librw
docker run -v "$LIBRWDIR:/librw:rw,z" --name librw_instance -d librw/librw sleep infinity
docker exec -u builder librw_instance /bin/bash -c "cd /librw/build && make clean config=$TARGET && make config=$TARGET verbose=1"

docker rm librw_instance -f >/dev/null 2>&1 || true
docker stop librw_instance -t 0 >/dev/null 2>&1 || true
