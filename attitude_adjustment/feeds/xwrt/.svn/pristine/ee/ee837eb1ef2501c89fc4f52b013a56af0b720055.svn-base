#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
###################################################################
# QoS status page
#
# Description:
#	Shows QoS numbers.
#
# Author(s) [in order of work date]:
#	Original webif developers
#	Jeremy Collake <jeremy.collake@gmail.com>
#
# Major revisions:
#
# NVRAM variables referenced:
#	none
#
# Configuration files referenced:
#	/etc/config/qos
#

header_inject_head=$(cat <<EOF
<style type="text/css">
<!--
.qostable table {
	margin-left: 2em;
	margin-right: 2em;
	border-style: none;
	border-spacing: 0;
	padding: 0.5em;
	padding-bottom: 1em;
}
.qostable th,
.qostable td {
	text-align: right;
	padding-top: 0.30em;
	padding-bottom: 0.30em;
	padding-left: 0.30em;
	padding-right: 0.30em;
}
.qostable th.text,
.qostable td.text {
	text-align: left;
}
.qosraw table {
	width: 90%;
	border-style: none;
	border-spacing: 0;
}
.qosraw th,
.qosraw td {
	text-align: left;
}
-->
</style>

EOF
)

header "Status" "QoS" "@TR<<Quality of Service Statistics>>"

gress_stats() {
	awk \
		-v root_class="$root_class" \
		-v parent_class="$parent_class" \
		-v priority_class="$priority_class" \
		-v express_class="$express_class" \
		-v normal_class="$normal_class" \
		-v bulk_class="$bulk_class" \
	'BEGIN {
		total_packets = 0
		total_bytes = 0
	}
	function bytes2human(num) {
		if (num == "") return "&nbsp;"
		if (num >= 2 ** 30) {
			return sprintf("(%.1f @TR<<GiB>>)", num / (2 ** 30))
		} else if (num >= 2 ** 20) {
			return sprintf("(%.1f @TR<<MiB>>)", num / (2 ** 20))
		} else if (num >= 2 ** 10) {
			return sprintf("(%.1f @TR<<KiB>>)", num / (2 ** 10))
		} else {
			return "&nbsp;"
		}
	}
	/class/ {
		if ($3 != root_class && $3 != parent_class) {
			if ($3 == priority_class) {
				class="@TR<<qos_class_Priority#Priority>>"
			} else if ($3 == express_class) {
				class="@TR<<qos_class_Express#Express>>"
			} else if ($3 == normal_class) {
				class="@TR<<qos_class_Normal#Normal>>"
			} else if ($3 == bulk_class) {
				class="@TR<<qos_class_Bulk#Bulk>>"
			} else {
				class="@TR<<qos_class_Unknown#Unknown>> " $3
			}
			getline
			if (length($0) > 0) {
				print "<tr>"
				print "	<td class=\"text\">" class "</td>"
				print "	<td>" $4 "</td>"
				total_packets += $4
				print "	<td>" $2 "</td>"
				total_bytes += $2
				print "	<td>" bytes2human($2) "</td>"
				print "</tr>"
			}
		}
	}
	END {
		print "<tr>"
			print "	<td class=\"text\">@TR<<qos_line_Total#Total>></td>"
			print "	<td>" total_packets "</td>"
			print "	<td>" total_bytes "</td>"
			print "	<td>" bytes2human(total_bytes) "</td>"
		print "</tr>"
	}'
}

uci_load "qos"
if equal "$CONFIG_wan_enabled" "1"; then

	# todo: don't do these statically..
	root_class="1:"
	parent_class="1:1"
	priority_class="1:10"
	express_class="1:20"
	normal_class="1:30"
	bulk_class="1:40"

	qos_status=$(qos-stat 2>&-)
	if ! empty "$qos_status" && exists "/usr/bin/qos-stat"; then
		ingress_start_line=$(echo "$qos_status" | grep INGRESS -n | cut -d ':' -f 1)
		ingress_start_line=$(( $ingress_start_line - 2 )) 2>/dev/null
		egress_status=$(echo "$qos_status" | sed "$ingress_start_line,\$ d")
		ingress_status=$(echo "$qos_status" | sed "1,$ingress_start_line d")
		ingress_stats_table=$(echo -e "$ingress_status\n" | gress_stats)
		cat <<EOF
<div class="settings">
<h3><strong>@TR<<Incoming Traffic>></strong></h3>
<div id="qostable1" class="qostable">
<table summary="@TR<<Incoming Traffic>>">
<tbody>
<tr>
	<th class="text">@TR<<Class>></th>
	<th>@TR<<Packets>></th>
	<th>@TR<<Bytes>></th>
	<th>&nbsp;</th>
</tr>
$ingress_stats_table
</tbody>
</table>
</div>
EOF
		egress_stats_table=$(echo -e "$egress_status\n" | gress_stats)
		cat <<EOF
<h3><strong>@TR<<Outgoing Traffic>></strong></h3>
<div id="qostable2" class="qostable">
<table summary="@TR<<Outgoing Traffic>>">
<tbody>
<tr>
	<th class="text">@TR<<Class>></th>
	<th>@TR<<Packets>></th>
	<th>@TR<<Bytes>></th>
	<th>&nbsp;</th>
</tr>
$egress_stats_table
</tbody>
</table>
</div>
<div class="settings-content">
<table>
EOF
		display_form <<EOF
field||spacer1
string|<br /><br />
field||show_raw
formtag_begin|raw_stats|$SCRIPT_NAME
submit|show_raw_stats| @TR<<&nbsp;Show raw statistics&nbsp;>>
formtag_end
end_form
EOF

		#########################################
		# raw stats
		! empty "$FORM_show_raw_stats" && {
			echo "<br />"
			echo "<div id=\"qostable3\" class=\"qosraw\">"
			echo "<table>"
			echo "<tbody>"
			echo "<tr>"
			echo "	<th>@TR<<QoS Packets | Raw Stats>></th>"
			echo "</tr>"
			echo "<tr>"
			echo "	<td><br /><div class=\"smalltext\"><pre>"
			qos-stat
			echo "</pre></div></td>"
			echo "</tr>"
			echo "<tr>"
			echo "<td><br /></td>"
			echo "</tr>"
			echo "</tbody>"
			echo "</table>"
			echo "</div>"
		}
	else
		#########################################
		# no QoS Service
		echo "<br />@TR<<no_qos#No QoS Service found running so no parsed QoS statistics can be shown! We recommend to install nbd's QoS scripts.>><br />"	
	fi
else	
	echo "@TR<<qos_scripts_disabled#The qos-scripts package is not active. Visit the <a href=\"./network-qos.sh\">QoS page</a> to install and/or enable it.>>"
fi

footer ?>
<!--
##WEBIF:name:Status:425:QoS
-->
