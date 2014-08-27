#!/bin/sh
themes_lst="/etc/themes.lst"
tmpthemeslst=$(mktemp "/tmp/.webif-XXXXXX")
opkg list | awk '/webif-theme/ { gsub("webif-theme-",""); if ($5) print "option|"$1"|"$5}' | sort | uniq >> "$tmpthemeslst"
if [ "'cat $tmpthemeslst'" != "" ]; then
	if [ "'cat $tmpthemeslst'" != "'cat $themes_lst'" ]; then
		rm -f "$themes_lst"
		chmod 0644 "$tmpthemeslst"
		mv -f "$tmpthemeslst" "$themes_lst"
	fi
fi
rm -f $tmpthemeslst
