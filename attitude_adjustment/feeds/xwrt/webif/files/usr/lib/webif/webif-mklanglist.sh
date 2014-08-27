#!/bin/sh
LANGLIST_NEEDS_UPDATE=0
languages_root="/etc/languages.root"
languages_lst="/etc/languages.lst"
if [ ! -f "$languages_lst" ]; then
	cp -pf "$languages_root" "$languages_lst" 2>/dev/null
fi
lists_path=$(cat /etc/opkg.conf 2>/dev/null | grep ^lists_dir | cut -d' ' -f3)
if [ -z "$lists_path" ]; then
	lists_path="/usr/lib/opkg/lists"
else
	lists_path="${lists_path%%/}"
fi
# rebuild the language list only after opkg update
for listfile in $(ls "${lists_path}/*" 2>/dev/null); do
	[ "$listfile" -nt "$languages_lst" ] && LANGLIST_NEEDS_UPDATE=1
done
if [ "$LANGLIST_NEEDS_UPDATE" -eq "1" ]; then
	tmplanglst=$(mktemp "/tmp/.webif-XXXXXX")
	version_current=$(opkg list_installed webif | grep "^webif -" | cut -d' ' -f3 | sed 's/\./\./')
	for lngpkg in $(opkg list webif-lang-* | sed "/webif-lang-[^[:space:]]* - ${version_current} -/!d; s/webif-lang-\([^[:space:]]*\)[[:space:]]-[[:space:]]${version_current}[[:space:]]-[[:space:]]\([^[:space:]]*\).*/option\|\1\|\2/"); do
		if [ -n "$lngpkg" ]; then
	                lngshort=$(echo "$lngpkg" | cut -d'|' -f1,2)
        	        newlang=$(grep "^$lngshort" "$languages_root")
	                if [ -z "$newlang" ]; then
	                        echo "$lngpkg" >>"$tmplanglst"
	                fi
		fi
	done
	cat "$languages_root" | grep -v "^option|en|" >>"$tmplanglst"
	rm -f "$languages_lst"
	echo "option|en|English" >"$languages_lst"
	cat "$tmplanglst" | sort | uniq | grep -v "^$" >>"$languages_lst"
	rm -f "$tmplanglst"
fi
