#!/bin/bash

shopt -s expand_aliases
alias id='/usr/xpg4/bin/id'
res=""

file=$1

if [ -f "$file" ]; then 

    ouid=$(ls -n "$file" | nawk '{print $3}')
    ogid=$(ls -n "$file" | nawk '{print $4}')

    acl=$(ls -V "$file" | tail +2 | egrep '.*x...........:......:(allow|deny)$' | tr -d " \t") 

    acl=$(echo "$acl" | sed 's/\(.*\):\(.*:..............:......:.*\)/\1;\2/p' | nawk -F ":" '!_[$1]++')
    acl=$(echo "$acl" | sed 's/\(.*\);\(.*:..............:......:.*\)/\1:\2/p' | egrep "w$" | uniq)

    exec_acl=$(echo "$acl" | nawk -F ":" '{print $1,$2}')


    if [ -z "$exec_acl" ]; then
	acl=$(ls -v "$file" | tail +2 | egrep '....:..x' | egrep -v "allow$|deny$" | tr -d " \t")
	exec_acl=$(echo "$acl" | nawk -F ":" '{print $2,$3}')
    fi	


    IFS=$'\n'; 
    for temp in $exec_acl
	do
	    fp=$(echo $temp | cut -d " " -f1)
	    sp=$(echo $temp | cut -d " " -f2)
	    case $fp in
		"owner@" )
			res+=$'\n'"$(getent passwd $ouid | cut -d ":" -f1)"$'\n'
		;;
		"group@" )
			res+="$(getent passwd | nawk -v "gid=$ogid" -F ':' '$4==gid { printf "%s\\n",$1 }')"
			res+="$(getent group $ogid | cut -d ":" -f4 | nawk -v RS=',' '{ printf "%s\\n",$1 }')"

		;;
		"everyone@"|"other" )
			res+="$(getent passwd | nawk -F ':' '{printf "%s\\n", $1}')"
		;;
		"user" )
			if [ -z $sp ]; then
			    sp=$(getent passwd $ouid | cut -d ":" -f1)
			fi    
			res+="$sp\n"  
		;;    
		"group" )
			sgid="$(getent group $sp | cut -d ":" -f3)"
			if [ -z $sgid ]; then 
			    sgid="$ogid"
			fi    
			res+="$(getent passwd | nawk -v "gid=$sgid" -F ':' '$4==gid { printf "%s\\n", $1 }')" 
			res+="$(getent group $sgid | cut -d ":" -f4 | nawk -v RS=',' '{printf "%s\\n", $1}')"
		;;	
	    esac	
	done
    echo -e "$res" | sed '/^$/d' | uniq

else
    echo "No such file."
fi
exit 0
