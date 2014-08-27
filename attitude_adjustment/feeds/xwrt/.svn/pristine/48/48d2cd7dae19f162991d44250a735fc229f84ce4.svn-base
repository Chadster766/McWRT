#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
###################################################################
# status-modules.sh
#
# Description:
#  Display loaded kernel modules.
#
# Author(s) [in order of work date]:
#  Lubos Stanek <lubek@users.berlios.de>
#
# Major revisions:
#

header_inject_head=$(cat <<EOF
<style type="text/css">
<!--
#modtable table {
	width: 98%;
	margin-left: auto;
	margin-right: auto;
	text-align: left;
	font-size: 0.9em;
	border-style: none;
	border-spacing: 0;
}
#modtable td, th {
	padding-left: 0.2em;
	padding-right: 0.2em;
}
#modtable .number {
	text-align: right;
}
-->
</style>

EOF

)

kernelrelease=$(cat /proc/sys/kernel/osrelease 2>/dev/null | sed 's/^\([[:digit:]]*\)\.\([[:digit:]]*\)\..*/\1\2/')

header "Status" "Modules" "@TR<<status_modules_Kernel_Modules#Kernel Modules>>"
?>

<div class="settings">
<h3><strong>@TR<<status_modules_Loaded_Modules#Loaded Modules>></strong></h3>
<div id="modtable">
<table>
<tbody>
<tr>
	<th>@TR<<status_modules_th_Module#Module>></th>
	<th class="number">@TR<<status_modules_th_Size#Size>></th>
	<th class="number">@TR<<status_modules_th_Count#Count>></th>
<?
[ "$kernelrelease" != "24" ] && {
	cat <<EOF
	<th>@TR<<status_modules_th_State#State>></th>
	<th>@TR<<status_modules_th_Address#Address>></th>
EOF
}
?>
	<th>@TR<<status_modules_th_Used_by#Used by>></th>
</tr>

<?
cat /proc/modules 2>/dev/null | sort | awk -v kernelrelease="$kernelrelease" '
BEGIN{
	counter = 0
	odd=1
	tr_ind = ""
	td_ind = "\t"
}
function oddline() {
	if (odd == 1) {
		print tr_ind "<tr>"
		odd--
	} else {
		print tr_ind "<tr class=\"odd\">"
		odd++
	}
}
{
	if (length($0) != 0) {
		oddline()
		print td_ind "<td>" $1 "</td>"
		print td_ind "<td class=\"number\">" $2 "</td>"
		counter += $2
		print td_ind "<td class=\"number\">" $3 "</td>"
		if (kernelrelease == 24) {
			usedby = $4
			for (i=5; i<=NF; i++) usedby = usedby " " $i
			gsub(/\(deleted\)/, "@TR<<status_modules_flags_deleted|(deleted)>>", usedby)
			gsub(/\(autoclean\)/, "@TR<<status_modules_flags_autoclean|(autoclean)>>", usedby)
			gsub(/\(unused\)/, "@TR<<status_modules_flags_unused|(unused)>>", usedby)
			gsub(/\(initializing\)/, "@TR<<status_modules_flags_initializing|(initializing)>>", usedby)
			gsub(/\(uninitialized\)/, "@TR<<status_modules_flags_uninitialized|(uninitialized)>>", usedby)
		} else {
			if ($4 == "-") {
				usedby = "&nbsp;"
				mode = $5
				memory = $6
			} else {
				usedby = $4
				sub(/,$/, "", usedby)
				gsub(/,/, ", ", usedby)
				mode = $5
				memory = $6
			}
			gsub(/Live/, "@TR<<status_modules_flags_Live|Live>>", mode)
			gsub(/Loading/, "@TR<<status_modules_flags_Loading|Loading>>", mode)
			gsub(/Unloading/, "@TR<<status_modules_flags_Unloading|Unloading>>", mode)
			print td_ind "<td>" mode "</td>"
			print td_ind "<td>" memory "</td>"
		}
		print td_ind "<td>" usedby "</td>"
		print tr_ind "</tr>"
	}
}
END{
	if (counter == 0) {
		print "<tr>"
		print "<td colspan=\""(kernelrelease == 24 ? 4 : 6)"\">@TR<<status_modules_No_modules_loaded#There are currently no kernel modules loaded>>.</td>"
		print "</tr>"
	} else {
		oddline()
		print "<td>@TR<<status_modules_size_Total#Total>></td>"
		print "<td class=\"number\">" counter "</td>"
		print "<td colspan=\""(kernelrelease == 24 ? 2 : 4)"\">&nbsp;</td>"
		print "</tr>"
	}
}
'
display_form <<EOF
string|</tbody>
end_form
EOF

footer ?>
<!--
##WEBIF:name:Status:105:Modules
-->
