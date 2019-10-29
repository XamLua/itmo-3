#!/bin/bash

shopt -s expand_aliases
alias id='/usr/xpg4/bin/id'
tres=""
res=""
user="$USER"

if ! getent passwd "$user" > /dev/null ; then
    echo "No such user"
    exit 0
fi 

for file in $(ls $1)
do    
if [ -f "$file" ]; then 

    ouid=$(ls -n "$file" | nawk '{print $3}')
    ogid=$(ls -n "$file" | nawk '{print $4}')

    acl=$(ls -V "$file" | tail +2 | egrep '.*:.-.p..........:......:(allow|deny)$' | tr -d " \t") 
	
    acl=$(echo "$acl" | sed 's/\(.*\):\(.*:..............:......:.*\)/\1;\2/p' | nawk -F ":" '!_[$1]++')
    acl=$(echo "$acl" | sed 's/\(.*\);\(.*:..............:......:.*\)/\1:\2/p' | egrep "w$" | uniq)

    exec_acl=$(echo "$acl" | nawk -F ":" '{print $1,$2}')

    if [ -z "$exec_acl" ]; then
	exit 0
    fi	

    IFS=$'\n';
    for temp in $exec_acl
	do
	    fp=$(echo $temp | cut -d " " -f1)
	    sp=$(echo $temp | cut -d " " -f2)
	    case $fp in
		"owner@" )
			if [ "$(id -u $user)"=="$ouid" ]; then
			    tres="$file"
			fi    
		;;
		"group@" )
			members="$(getent passwd | nawk -v "gid=$ogid" -F ':' '$4==gid { printf "%s\\n",$1 }')"
			members+="$(getent group $ogid | cut -d ":" -f4 | nawk -v RS=',' '{ printf "%s\\n",$1 }')"
			if echo -e $members | grep "$user" > /dev/null ; then
			    tres="$file"
			fi    

		;;
		"everyone@" )
			tres="$file"
		;;
		"user" )
			if [ "$sp"=="$user" ]; then
			   tres="$file"
			fi    
		;;    
		"group" )
			sgid="$(getent group $sp | cut -d ":" -f3)"
			members="$(getent passwd | nawk -v "gid=$sgid" -F ':' '$4==gid { print $1 }')" 
			members+="$(getent group $sgid | cut -d ":" -f4 | nawk -v RS=',' '{print $1 }')"
			if echo -e $members | grep "$user" > /dev/null ; then
			    tres="$file"
			fi
		;;	
	    esac	
	done
    if [ ! -z "$tres" ]; then
	res+="$tres\n"
    fi

fi
done
echo -e $res | uniq | sed '/^$/d'
exit 0
