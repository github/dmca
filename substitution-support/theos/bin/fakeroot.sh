#!/bin/bash
required=0
persistence=/tmp/dhbxxx

while getopts ":p:rc" flag; do
	case "$flag" in
		:)	echo "$0: Option -$OPTARG requires an argument." 1>&2
			exit 1
			;;
		\?)	echo "$0: Option -$OPTARG unrecognized." 1>&2
			exit 1
			;;
		p)	persistence="$OPTARG" ;;
		r)	required=1 ;;
		c)	delpersistence=1 ;;
	esac
done
shift $((OPTIND-1))
cmd=$*

mkdir -p $(dirname $persistence)
touch $persistence

if [[ $delpersistence -eq 1 ]]; then
	rm -f $persistence
	exit 0
fi

if [[ "$USER" == "root" ]]; then
	fakeroot=""
elif type fauxsu &> /dev/null; then
	fakeroot="fauxsu -p $persistence -- "
elif type fakeroot-ng &> /dev/null; then
	fakeroot="fakeroot-ng -p $persistence -- "
elif type fakeroot &> /dev/null; then
	fakeroot="fakeroot -i $persistence -s $persistence -- "
else
	if [[ $required -eq 1 ]]; then
		fakeroot=""
	else
		fakeroot=": "
	fi
fi

#echo $fakeroot $cmd
$fakeroot $cmd
