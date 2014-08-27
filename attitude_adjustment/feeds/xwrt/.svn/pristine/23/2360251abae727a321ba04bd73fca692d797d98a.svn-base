#!/bin/sh
#
# this script embeds the global revision number in document(s) given 
# on command line
# document should have __SVN_REVISION__ where it wants the revision 
# number to be placed. Wildcards okay.
#
if [ $# -lt 1 ]; then
	echo " Invalid usage. Must supply one or more files."
	exit 1
fi
revision_number=$(svn info | grep Revision | cut -c11-)
echo " Revision number is $revision_number"
until [ -z "$1" ]  
do
	for curfile in $(grep -H "__SVN_REVISION__" -r "$1" | cut -d':' -f1 | sed '/\.svn\//d'); do
		echo " Processing $curfile ..."
		sed -i -e "s/__SVN_REVISION__/$revision_number/" "$curfile"
	done
	shift
done

