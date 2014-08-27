#!/bin/sh

cachedir=/tmp/.webcache

uci_load network

subcategories() {
	[ -s "$cachedir/graphs" ] || {          # create a cache if it does not exists already
		[ -d "$cachedir" ] || mkdir "$cachedir" 2>/dev/null 1>&2
		(echo "Graphs:10:CPU:graphs-cpu.sh"
		 echo "Graphs:20:Bandwidth:graphs-bandwidth.sh"
		 echo "Graphs:30:Vnstat:graphs-vnstat.sh"
		 grep -v -e "mon." -e "ifb[0-9]" /proc/net/dev |sed -n -e "/:/"'{s/:.*//;s/^ *\(.*\)/Graphs:50:Traffic\>\> \1@TR\<\<:graphs-if.sh?if=\1/;p}' 2>/dev/null
		) | sort -n >$cachedir/graphs
	}
	awk -F: -v "category=Graphs" -v "selected=$2" -f /usr/lib/webif/common.awk -f /usr/lib/webif/subcategories.awk $cachedir/graphs
}
