#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh

header "Mesh" "BATMAN" "@TR<<BATMAN status and configuration>>" ' onload="modechange()" ' "$SCRIPT_NAME"

uci_load "batman"


available_interfaces=$(ifconfig | grep inet -B1 | awk '/HWaddr/ {printf "%s ",$1}')

#####################################################################
# If this is a form submit..
! empty "$FORM_submit" && {
	# TODO: validate input 
	#validate << EOF
	#string|FORM_reponame|@TR<<system_ipkg_reponame#Repo. Name>>|min=4 max=40 required #nospaces|$FORM_reponame
	#string|FORM_repourl|@TR<<system_ipkg_repourl#Repo. URL>>|min=4 max=4096 #required|$FORM_repourl
	#EOF

	# Get interfaces selected in the form
	interfaces_to_configure=""
	for interface in $available_interfaces; do
		sane_iface=$(echo "$interface" | tr '.-' 'PH')
		eval checkbox_status="\$FORM_batman_interface_${sane_iface}"
		if equal "$checkbox_status" "1";then
			interfaces_to_configure=$interfaces_to_configure" "$interface
		fi
	done

	# Make primary interface ordered first and clean up whitespace
	first_interface=$(echo "$FORM_batman_primary_interface" | tr 'PH' '.-')
	if echo $interfaces_to_configure | grep -q $first_interface; then
		a="$interfaces_to_configure"
		f="$first_interface"
		interfaces_to_configure="${f} ${a%%$f*}${a#*$f}"
	fi
	interfaces_to_configure=$(echo $interfaces_to_configure)

	# Get current configured values
	batman_interfaces=$(uci get batman.general.interface)
	batman_announce=$(uci get batman.general.announce)
	batman_gateway_class=$(uci get batman.general.gateway_class)
	if equal $batman_gateway_class ""; then 
		batman_gateway_class="0"
	fi
	batman_originator_interval=$(uci get batman.general.originator_interval)
	batman_preferred_gateway=$(uci get batman.general.preferred_gateway)
	batman_routing_class=$(uci get batman.general.routing_class)
	if equal $batman_routing_class ""; then 
		batman_routing_class="0"
	fi
	batman_visualisation_srv=$(uci get batman.general.visualisation_srv)

	# Find differences and apply
	if ! equal "$batman_interfaces" "$interfaces_to_configure"; then
		uci set batman.general.interface="$interfaces_to_configure"
	fi
	if ! equal "$FORM_batman_announce" "$batman_announce"; then
		uci set batman.general.announce="$FORM_batman_announce"
	fi
	if ! equal "$FORM_batman_gateway_class" "$batman_gateway_class"; then
		uci set batman.general.gateway_class="$FORM_batman_gateway_class"
	fi
	if ! equal "$FORM_batman_originator_interval" "$batman_originator_interval"; then
		uci set batman.general.originator_interval="$FORM_batman_originator_interval"
	fi
	if ! equal "$FORM_batman_preferred_gateway" "$batman_preferred_gateway"; then
		uci set batman.general.preferred_gateway="$FORM_batman_preferred_gateway"
	fi
	if ! equal "$FORM_batman_routing_class" "$batman_routing_class"; then
		uci set batman.general.routing_class="$FORM_batman_routing_class"
	fi
	if ! equal "$FORM_batman_visualisation_srv" "$batman_visualisation_srv"; then
		uci set batman.general.visualisation_srv="$FORM_batman_visualisation_srv"
	fi
}

#####################################################################
# Prepare form Batman status: batman nodes and gateways

batmanstatus=""
if [ -e "/var/run/batmand.socket" ]; then
	batmantmpfile=$(mktemp "/tmp/webif-XXXXXX")
	batmand -c -b -d 1 | grep -v WARNING > "$batmantmpfile"
	# leave here, just for testing purpose :o)
	#cat /www/cgi-bin/webif/output | grep -v WARNING > "$batmantmpfile"
	
	if equal "$( cat "$batmantmpfile" | grep "No batman")" ""; then
		batmanstatus=$batmanstatus"string|<div><table border=0 cellpadding=0 cellspacing=0 width=\"100%\">"
		batmanstatus=$batmanstatus"<tr><td>BATMAN node</td><td>Gateway</td><td>Through</td></tr>"
		batmanstatus=$batmanstatus$(cat "$batmantmpfile" | awk ' {printf "%s %s %s %s %s","<tr><td width=\"30%\">",$1,"</td><td width=\"35%\">",$3,"</td><td width=\"35%\">"} {for (i = 5; i <= NF; i++) printf " %s <br>",$i } { printf "<br></td></tr>"}')
		batmanstatus=$batmanstatus"</table>"
	else
		batmanstatus="string|<tr><td>No BATMAN nodes in range</td></tr>"
	fi
else
	batmanstatus="string|<tr><td>BATMAN daemon not running or daemon socket not available</td></tr>"
fi


#####################################################################
# Prepare form Batman configuration

form_batman_interface=""
form_batman_primary_interface=""
batman_primary_interface=""
for interface in $available_interfaces; do
	sane_iface=$(echo "$interface" | tr '.-' 'PH')
	uci_iface=$(uci get batman.general.interface)
	form_batman_interface=$form_batman_interface$(printf "field| - %s\ncheckbox|batman_interface_%s|"  $interface $sane_iface)
	if echo $uci_iface | grep -q $interface; then
		form_batman_interface=$form_batman_interface"1"
	else
		form_batman_interface=$form_batman_interface"0"
	fi
	form_batman_interface=$form_batman_interface$(printf "|1|\n ")
	form_batman_primary_interface=$form_batman_primary_interface$(printf "\noption|%s|%s"  $sane_iface $interface)
	if echo $uci_iface | awk '{print $1}' | grep -q $interface; then
		batman_primary_interface=$sane_iface
	fi
done

batman_announce=$(uci get batman.general.announce)
batman_gateway_class=$(uci get batman.general.gateway_class)
if equal $batman_gateway_class ""; then 
	batman_gateway_class="0"
fi
batman_originator_interval=$(uci get batman.general.originator_interval)
batman_preferred_gateway=$(uci get batman.general.preferred_gateway)
batman_routing_class=$(uci get batman.general.routing_class)
if equal $batman_routing_class ""; then 
	batman_routing_class="0"
fi
batman_visualisation_srv=$(uci get batman.general.visualisation_srv)

#####################################################################
# Page show
display_form <<EOF
onchange|modechange

start_form|@TR<<BATMAN status>>
$batmanstatus
end_form

start_form|@TR<<BATMAN configuration>>

field|@TR<<Batman interfaces>>
$form_batman_interface
helpitem|Batman interface
helptext| interface on which BATMAN will work.

field|@TR<<Primary interface>>
select|batman_primary_interface|$batman_primary_interface
option|none|@TR<<(none)>>$form_batman_primary_interface|
helpitem|Primary interface
helptext| Primary interface TTL=50, non-primary interface TTL=2

field|@TR<<Announce networks>>
text|batman_announce|$batman_announce|
helpitem|Announce networks
helptext|type here networks/netmasks that this node has to annonce to be a gateway for.

field|@TR<<Gateway class>>
select|batman_gateway_class|$batman_gateway_class
option|0|@TR<<0: Not an internet gateway (default)>>
option|1|@TR<<1: Modem line>>
option|2|@TR<<2: ISDN line>>
option|3|@TR<<3: Double ISDN>>
option|4|@TR<<4: 256 KBit>>
option|5|@TR<<5: UMTS / 0.5 MBit>>
option|6|@TR<<6: 1 MBit>>
option|7|@TR<<7: 2 MBit>>
option|8|@TR<<8: 3 MBit>>
option|9|@TR<<9: 5 MBit>>
option|10|@TR<<10:6 MBit>>
option|11|@TR<<>11: 6 MBit>>|	
helpitem|Gateway class
helptext| Specify here if this node offers a internet gateway and tell clients how much bandwidth is available.

field|@TR<<Originator interval>>
text|batman_originator_interval|$batman_originator_interval|
helpitem|Originator interval
helptext| milliseconds beetween originator packets sent by this node.

field|@TR<<Preferred gateway>>
text|batman_preferred_gateway|$batman_preferred_gateway|
helpitem|Preferred gateway
helptext| IP address of the preferred internet gateway, this will create an IP tunnel to the specified IP.

field|@TR<<Routing class>>
select|batman_routing_class|$batman_routing_class
option|0|@TR<<0: No default route>>
option|1|@TR<<1: Fast internet connection>>
option|2|@TR<<2: Stable internet connection>>
option|3|@TR<<3: Best statistic internet connection (olsr style)>>|
helpitem|Routing class
helptext| Only needed if this node is not an internet gateway (Gateway class). Option 0 is not really usable yet. Option 1 use best connection to gateway, faster gateway class will be    preferred if connection quality is similar. Option 3 let this node use nearest gateway.

field|@TR<<Visualisation server>>
text|batman_visualisation_srv|$batman_visualisation_srv|
helpitem|Visualisation server
helptext| IP of the server that collects the topology information for        topology-visualisation, default none.

end_form

EOF

footer ?>
<!--
##WEBIF:name:Mesh:750:BATMAN
-->
