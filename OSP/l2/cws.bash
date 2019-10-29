#!/bin/bash

pth=$1

if [ -z "$pth" ]; then
	echo "cws: missing directory path operand"
	exit 1
fi

if [ -d "$pth" ]; then
	cd $pth
	for temp in $(ls)
	do
		if [ -d "$temp" ] && [ "$(ls -A $temp)" ]  && ! ls -l $temp | grep -q "^d" ; then
			echo $temp
		fi
	done
	exit 0
else
	echo "'$pth': no such directory"
	exit 1
fi

