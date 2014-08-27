#!/usr/bin/webif-page
<%
. /usr/lib/webif/webif.sh
###################################################################
# Static routes display and configuration
#
# Description:
#	Route configuration
#
# Author(s) [in order of work date]:
#	Lubos Stanek <lubek@users.berlios.de>
#	Adam Hill	<adam@voip-x.co.uk>
# Major revisions:
#	Allow static IPV4 routes to be added
#
# Configuration files referenced:
#   network
#


if [ "$FORM_route_remove" != "" ]
then
	uci_remove "network" "$FORM_route_remove"
fi

if [ "$FORM_add_route" != "" ]
then
	validate <<EOF
string|FORM_rtname|@TR<<Name>>|required|$FORM_rtname
ip|FORM_target|@TR<<Destination>>|required|$FORM_target
ip|FORM_gateway|@TR<<Gateway>>|required|$FORM_gateway
netmask|FORM_netmask|@TR<<Netmask>>||$FORM_netmask
int|FORM_metric|@TR<<Target>>|max=254 min=0|$FORM_metric
EOF
	equal "$?" 0 && {
		[ "$FORM_netmask" = "" ] && FORM_netmask="255.255.255.255"
		[ "$FORM_metric" = "" ] && FORM_metric="0"
		FORM_rtname=$(echo -n $FORM_rtname | sed 's/ /_/g')
		uci_add "network" "route" "$FORM_rtname"
		uci_set "network" "$FORM_rtname" "target" "$FORM_target"
		uci_set "network" "$FORM_rtname" "gateway" "$FORM_gateway"
		uci_set "network" "$FORM_rtname" "netmask" "$FORM_netmask"
		uci_set "network" "$FORM_rtname" "metric" "$FORM_metric"
		uci_set "network" "$FORM_rtname" "interface" "$FORM_interface"
		FORM_target=""
		FORM_gateway=""
		FORM_netmask=""
		FORM_metric=""
		FORM_rtname=""
		FORM_interface=""
	}
fi

config_load "network"
for cfgsec in $CONFIG_SECTIONS; do
	eval "cfgtype=\$CONFIG_${cfgsec}_TYPE"
	[ "$cfgtype" = "interface" ] && {
		eval "type=\"\$CONFIG_${cfgsec}_type\""
		if [ "$type" =  "bridge" ]; then
			iface="br-${cfgsec}"
		else
			eval "iface=\"\$CONFIG_${cfgsec}_ifname\""
		fi
		[ -n "$iface" ] && {
 			ifaces="$ifaces ${cfgsec}:$iface"
 			ifacelist="$ifacelist ${cfgsec}"
		}
	}
done

# Copy just pre fills the form with the values from a specific route.
if [ "$FORM_route_copy" != "" ]
then
	FORM_rtname="$FORM_route_copy"
	config_get FORM_target "$FORM_rtname" "target"
	config_get FORM_gateway "$FORM_rtname" "gateway"
	config_get FORM_netmask "$FORM_rtname" "netmask"
	config_get FORM_metric "$FORM_rtname" "metric"
	config_get FORM_interface "$FORM_rtname" "interface"
fi

display_config_routes() {
	local ipv="$1"
	[ -z "$ipv" ] && return
	local route_ver cfgsec cfgtype route_count interface target netmask gateway metric
	local odd=1
	[ "$ipv" = "6" ] && route_ver="route6" ||  route_ver="route"
	cat <<EOF
<div class="settings">
<h3><strong>@TR<<network_routes_static_Configured_IPv${ipv}_Static_Routes#Configured IPv${ipv} Static Routes>></strong></h3>
<form enctype="multipart/form-data" action="/cgi-bin/webif/network-routes.sh" method="post"><input type="hidden" name="submit" value="1" />
<table>
<tbody>
<tr>
	<th>@TR<<network_routes_static_th_Destination#Destination>></th>
	<th>@TR<<network_routes_static_th_Gateway#Gateway>></th>
EOF
	[ "$ipv" = "4" ] && echo "	<th>@TR<<network_routes_static_th_Netmask#Netmask>></th>"
	cat <<EOF
	<th>@TR<<network_routes_static_th_Metric#Metric>></th>
	<th>@TR<<network_routes_static_th_Use_With#Use With>></th>
	<th>@TR<<network_routes_static_th_Name#Name>></th>
</tr>
EOF
	for cfgsec in $CONFIG_SECTIONS; do
		eval "cfgtype=\$CONFIG_${cfgsec}_TYPE"
		[ "$cfgtype" = "$route_ver" ] && {
			route_count=$(( $route_count + 1 ))
			eval "interface=\"\$CONFIG_${cfgsec}_interface\""
			eval "target=\"\$CONFIG_${cfgsec}_target\""
			[ "$ipv" = 4 ] && eval "netmask=\"\$CONFIG_${cfgsec}_netmask\""
			eval "gateway=\"\$CONFIG_${cfgsec}_gateway\""
			eval "metric=\"\$CONFIG_${cfgsec}_metric\""
			metric="${metric:-0}"
			if [ "$odd" = "1" ]; then
				echo "<tr>"
				odd=0
			else
				echo "<tr class=\"odd\">"
				odd=1
			fi
			echo "	<td>$target</td>"
			echo "	<td>$gateway</td>"
			[ "$ipv" = 4 ] && echo "	<td>$netmask</td>"
			echo "	<td>$metric</td>"
			echo "	<td>$interface</td>"
			if $(echo "$cfgsec" | grep -q "^cfg[[:digit:]]*"); then
				echo "	<td>&nbsp;</td>"
			else
				echo "	<td>$cfgsec</td>"
			fi
			[ "$ipv" = "4" ] && echo "<td><a href=\"?route_remove=$cfgsec\">X</a></td>
			<td><a href=\"?route_copy=$cfgsec\">@TR<<rt_copy#Edit>></td>"
			echo "</tr>"
		}
	done
	if [ "$ipv" = "4" ]
 	then
		cat <<EOF
<tr>
	<td><input type="text" size="15" name="target" value="$FORM_target" /></td>
	<td><input type="text" size="15" name="gateway" value="$FORM_gateway" /></td>
	<td><input type="text" size="15" name="netmask" value="$FORM_netmask" /></td>
	<td><input type="text" size="3" name="metric" value="$FORM_metric" /></td>
	<td><select id="interface" name="interface">
EOF
		for iface in $ifacelist
		do
			echo -n "	<option value=\"$iface\""
			[ "$FORM_interface" = "$iface" ] && echo -n "selected=\"selected\""
			echo ">$iface</option>"
		done
		#echo $ifacelist | awk -vseliface="$iface" -F":" 'BEGIN { RS=" " } { print "<option value=\"" $1 ($1 == seliface ? "selected=\"selected\"" : "") "\">" $1 "</option>" }'

		cat <<EOF
	</select></td>
	<td><input type="text" size="15" name="rtname" value="$FORM_rtname" /></td>
	<td><input type="submit" name="add_route" value="Add" /></td>
</tr>
EOF
	fi

	cat <<EOF
</tbody>
</table>
</form>
<div class="clearfix">&nbsp;</div>
</div>
EOF
}

display_kernel_routes() {
	local ipv="$1"
	[ -z "$ipv" ] && return
	local route_cmd="route -n"
	local table_cols="8"
	[ "$ipv" = "6" ] && {
		route_cmd="$route_cmd -A inet6"
		table_cols="7"
	}
	cat <<EOF
<div class="settings">
<h3><strong>@TR<<network_routes_IPv${ipv}_routing_table#Kernel IPv${ipv} Routing Table>></strong></h3>
<table>
<tbody>
<tr>
	<th>@TR<<network_routes_static_th_Destination#Destination>></th>
EOF
	if [ "$ipv" = "4" ]; then
		cat <<EOF
	<th>@TR<<network_routes_static_th_Gateway#Gateway>></th>
	<th>@TR<<network_routes_static_th_Genmask#Genmask>></th>
EOF
	else
		cat <<EOF
	<th>@TR<<network_routes_static_th_Next_Hop#Next Hop>></th>
EOF
	fi
	cat <<EOF
	<th>@TR<<network_routes_static_th_Flags#Flags>></th>
	<th>@TR<<network_routes_static_th_Metric#Metric>></th>
	<th>@TR<<network_routes_static_th_Ref#Ref>></th>
	<th>@TR<<network_routes_static_th_Use#Use>></th>
	<th>@TR<<network_routes_static_th_Interface#Interface>></th>
</tr>
EOF
	$route_cmd 2>/dev/null | awk -v "ifaces=$ifaces" -v "cols=$table_cols" '
BEGIN {
	nic = split(ifaces, pairs)
	for (i = 1; i <= nic; i++) {
		npt = split(pairs[i], parts, ":")
		if (npt > 1) _ifaceifs[parts[2]] = parts[1]
	}
	odd = 1
	count = 0
}
($1 ~ /^[[:digit:]]/) || ($1 !~ /Kernel|Destination/) {
	if (odd == 1) {
		print "<tr>"
		odd--
	} else {
		print "<tr class=\"odd\">"
		odd++
	}
	for (i = 1; i <= NF; i++) {
		if (i == cols) {
			print "<td>" ((_ifaceifs[$(i)] == "") ? "@TR<<network_routes_unknown_iface#unknown>>" : _ifaceifs[$(i)]) " (" $(i) ")</td>"
		} else printf "<td>" $(i) "</td>"
	}
	count++
	print "</tr>"
}
END {
	if (count == 0) print "<tr>\n" td_ind "<td colspan=\""cols"\">@TR<<network_routes_No_kernel_routes#There are no IP routes in the kernel'\''s table?!>></td>\n</tr>"
}
'
	cat <<EOF
</tbody>
</table>
<div class="clearfix">&nbsp;</div>
</div>
EOF
}

header_inject_head=$(cat <<EOF
<style type="text/css">
<!--
table {
	text-align: left;
	margin: 0px;
	padding: 0px;
}
td, th {
	padding: 1px;
	vertical-align: center;
}
.rightcell {
	text-align: right;
}
-->
</style>
EOF
)

header "Network" "Routes" "@TR<<network_routes_Static_Routes#Static Routes>>"

uci_load network

display_config_routes "4"
display_config_routes "6"

display_kernel_routes "4"
[ -f /proc/net/ipv6_route ] && display_kernel_routes "6"

footer %>
<!--
##WEBIF:name:Network:500:Routes
-->
