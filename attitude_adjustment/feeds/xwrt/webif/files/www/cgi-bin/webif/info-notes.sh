#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
###################################################################
# Notes page
#
# Description:
#       Makes possible to leave simple text notes that persist
#       through updates
#
# Author(s) [in order of work date]:
#       IVBela <ivbela@gmail.com>
#
# Major revisions (ISO 8601):
#       2009-05-25 - Initial release
#
# NVRAM variables referenced:
#       none
#
# Configuration files referenced:
#       none
#
# Required components:
#       /usr/lib/webif/common.awk
#       /usr/lib/webif/notes.awk

header "Info" "Notes" "@TR<<Notes>>"

edit_pathname="/etc/config/notes"

! empty "$FORM_save" && {
	SAVED=1
	echo "$FORM_filecontent" > "$edit_pathname"
}

empty "$FORM_cancel" || FORM_edit=""

cat "$edit_pathname" 2>/dev/null | awk \
	-v url="$SCRIPT_NAME" \
	-v path="/etc/config" \
	-v file="notes" \
	-f /usr/lib/webif/common.awk \
	-f /usr/lib/webif/notes.awk

footer ?>

<!--
##WEBIF:name:Info:002:Notes
-->
