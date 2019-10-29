#!/bin/bash

group=$1

if [ -z "$group" ]; then
	echo "pbg: missing group operand"
	exit 1
fi

res_set=$(ps -G $group -o comm | tail +2 | sort)

prev=''
count=1

for next in $res_set
    do
	if [[ $prev = $next ]]; then
	    ((count++))
	elif [ ! -z $prev ]; then
	    echo "$prev - $count"
	    count=1
	fi   
	prev=$next
    done	    

exit 0
	
