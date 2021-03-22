#!/bin/bash
echo Signing $1
file $1
file $1 | grep 'Mach-O 64-bit executable' &> /dev/null
if [ $? == 0 ]; then
	echo "Resigning executable: $1"
	ldid -e $1 > ent.xml
	ldid -K$HOME/signcert.p12 -Sent.xml $1
	rm -f ent.xml
	exit 0
fi
file $1 | grep 'Mach-O 64-bit dynamically linked shared library' &> /dev/null
if [ $? == 0 ]; then
	echo "Resigning dynamically linked shared library"
	ldid -K$HOME/signcert.p12 -S $1
	exit 0
fi
exit 0
