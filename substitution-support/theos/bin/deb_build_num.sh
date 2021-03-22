#!/bin/bash
GETONLY=0
if [[ $1 == "-g" ]]; then
	GETONLY=1
	shift
fi

if [[ $# -lt 2 ]]; then
	echo "Syntax: $0 [-g] packagename versionname" >&2
	exit 1
fi

if [[ ! -d $TOP_DIR/.debmake ]]; then
	mkdir $TOP_DIR/.debmake
fi

PACKAGE=$1
VERSION=$2
INFOFILE=$TOP_DIR/.debmake/$PACKAGE-$VERSION
if [[ ! -e $INFOFILE ]]; then
	echo -n 1 > $INFOFILE
	echo -n 1
	exit 0
else
	CURNUM=$(cat $INFOFILE)
	if [[ $GETONLY -eq 0 ]]; then
		let CURNUM++
		echo -n $CURNUM > $INFOFILE
	fi
	echo $CURNUM
fi
