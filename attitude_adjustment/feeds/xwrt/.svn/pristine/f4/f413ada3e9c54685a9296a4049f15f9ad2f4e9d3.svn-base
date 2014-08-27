#!/usr/bin/webif-page
<?
. "/usr/lib/webif/webif.sh"
LOAD_STATE=""
###################################################################
# WAN and LAN configuration page
#
# Description:
#	Configures basic WAN and LAN interface settings.
#
# Author(s) [in order of work date]:
#       Original webif authors of wan.sh and lan.sh
#	Jeremy Collake <jeremy.collake@gmail.com>
#	Travis Kemen <kemen04@gmail.com>
#
# Major revisions:
#
# UCI variables referenced:
#   todo
# Configuration files referenced:
#   /etc/config/network
#

is_package_installed kmod-ipv6
equal "$?" "0" && ipv6_installed="1"

is_package_installed aiccu
equal "$?" "0" && aiccu_installed="1"

is_package_installed radvd
equal "$?" "0" && radvd_installed="1"

#Add new network
if [ "$FORM_button_add_network" != "" ]; then
	if [ "$FORM_add_network" = "" ]; then
		append validate_error "string|<h3>@TR<<Please add a network name>></h3><br />"
	else
		uci_add "network" "interface" "$FORM_add_network"
		uci_set "network" "$FORM_add_network" "proto" "none"
		uci_add "firewall" "zone" ""; add_forward_cfg="$CONFIG_SECTION"
		uci_set "firewall" "$add_forward_cfg" "name" "$FORM_add_network"
		uci_set "firewall" "$add_forward_cfg" "input" "ACCEPT"
		uci_set "firewall" "$add_forward_cfg" "output" "ACCEPT"
		uci_set "firewall" "$add_forward_cfg" "forward" "REJECT"
	fi
fi

config_cb() {
	local cfg_type="$1"
	local cfg_name_dhcp="$2"

	case "$cfg_type" in
		interface)
			append network "$cfg_name_dhcp" "$N"
		;;
		dhcp|zone)
			option_cb() {
				case "$1" in
					interface)
						[ "$2" = "$FORM_remove_network" ] && uci_remove "dhcp" "$cfg_name_dhcp"
					;;
					name)
						[ "$2" = "$FORM_remove_network" ] && uci_remove "zone" "$CONFIG_SECTION"
					;;
				esac
			}
		;;
		aiccu)
			append aiccu_sections "$cfg_name_dhcp" "$N"
		;;
	esac
}

#remove network
if ! empty "$FORM_remove_network"; then
	uci_remove "network" "$FORM_remove_network"
	uci_load dhcp
fi

uci_load network
network=$(echo "$network" |uniq)

WWAN_COUNTRY_LIST=$(
		awk '	BEGIN{FS=":"}
			$1 ~ /[ \t]*#/ {next}
			{print "option|" $1 "|@TR<<" $2 ">>"}' < /usr/lib/webif/apn.csv
	)
	JS_APN=$(
		awk '	BEGIN{FS=":"}
			$1 ~ /[ \t]*#/ {next}
			{print "	apnDB." $1 " = new Object;"
			 print "	apnDB." $1 ".name = \"" $3 "\";"
			 print "	apnDB." $1 ".user = \"" $4 "\";"
			 print "	apnDB." $1 ".pass = \"" $5 "\";\n"}' < /usr/lib/webif/apn.csv
	)
append JS_APN_DB "$JS_APN" "$N"

if [ "$aiccu_installed" = "1" ]; then
	uci_load aiccu
	for cfgs_section in $aiccu_sections; do
		if ! empty "$FORM_submit"; then
			eval FORM_aiccu_username="\$FORM_${cfgs_section}_username"
			eval FORM_aiccu_provider="\$FORM_${cfgs_section}_provider"
			eval FORM_aiccu_protocol="\$FORM_${cfgs_section}_protocol"
			eval FORM_aiccu_server="\$FORM_${cfgs_section}_server"
			eval FORM_aiccu_tls="\$FORM_${cfgs_section}_tls"
			[ "$FORM_aiccu_tls" = "" ] && FORM_aiccu_tls="0"
			eval FORM_aiccu_password="\$FORM_${cfgs_section}_password"
			eval FORM_aiccu_tunnel_id="\$FORM_${cfgs_section}_tunnel_id"
			eval FORM_aiccu_default_route="\$FORM_${cfgs_section}_default_route"
			[ "$FORM_aiccu_default_route" = "" ] && FORM_aiccu_default_route="0"
			eval FORM_aiccu_nat="\$FORM_${cfgs_section}_nat"
			[ "$FORM_aiccu_nat" = "" ] && FORM_aiccu_nat="0"
			eval FORM_aiccu_heartbeat="\$FORM_${cfgs_section}_heartbeat"
			[ "$FORM_aiccu_heartbeat" = "" ] && FORM_aiccu_heartbeat="0"
			case  "$FORM_aiccu_provider" in
				other)
					uci_set "aiccu" "$cfgs_section" "protocol" "$FORM_aiccu_protocol"
					uci_set "aiccu" "$cfgs_section" "server" "$FORM_aiccu_server"
					uci_set "aiccu" "$cfgs_section" "requiretls" "$FORM_aiccu_tls"
				;;
			esac
			uci_set "aiccu" "$cfgs_section" "provider" "$FORM_aiccu_provider"
			uci_set "aiccu" "$cfgs_section" "username" "$FORM_aiccu_username"
			uci_set "aiccu" "$cfgs_section" "password" "$FORM_aiccu_password"
			uci_set "aiccu" "$cfgs_section" "tunnel_id" "$FORM_aiccu_tunnel_id"
			uci_set "aiccu" "$cfgs_section" "defaultroute" "$FORM_aiccu_default_route"
			uci_set "aiccu" "$cfgs_section" "nat" "$FORM_aiccu_nat"
			uci_set "aiccu" "$cfgs_section" "heartbeat" "$FORM_aiccu_heartbeat"
			config_get FORM_aiccu_interface $cfgs_section interface
			[ "$FORM_aiccu_interface" = "" ] && uci_set "aiccu" "$cfgs_section" "interface" "aiccu"
		else
			config_get FORM_aiccu_provider $cfgs_section provider
			config_get FORM_aiccu_username $cfgs_section username
			config_get FORM_aiccu_password $cfgs_section password
			config_get FORM_aiccu_server $cfgs_section server
			config_get FORM_aiccu_protocol $cfgs_section protocol
			config_get FORM_aiccu_tunnel_id $cfgs_section tunnel_id
			config_get_bool FORM_aiccu_tls $cfgs_section requiretls 0
			config_get_bool FORM_aiccu_default_route $cfgs_section defaultroute 1
			config_get_bool FORM_aiccu_nat $cfgs_section nat 1
			config_get_bool FORM_aiccu_heartbeat $cfgs_section heatbeat 1
		fi
		aiccu_forms="start_form|Aiccu @TR<<Configuration>>
			field|@TR<<Tunnel Broker>>
			select|${cfgs_section}_provider|$FORM_aiccu_provider
			option|sixxs|@TR<<SixXS>>
			option|other|@TR<<Other>>
			field|@TR<<Username>>
			text|${cfgs_section}_username|$FORM_aiccu_username
			field|@TR<<Password>>
			text|${cfgs_section}_password|$FORM_aiccu_password
			field|@TR<<Server>>|field_${cfgs_section}_server|hidden
			text|${cfgs_section}_server|$FORM_aiccu_server
			field|@TR<<Protocol>>|field_${cfgs_section}_protocol|hidden
			select|${cfgs_section}_protocol|$FORM_aiccu_protocol
			option|tic|@TR<<tic>>
			option|tsp|@TR<<tsp>>
			option|l2tp|@TR<<l2tp>>
			option|none|@TR<<None>>
			field|@TR<<Tunnel ID>>
			text|${cfgs_section}_tunnel_id|$FORM_aiccu_tunnel_id
			field|@TR<<Require TLS>>|field_${cfgs_section}_tls|hidden
			checkbox|${cfgs_section}_tls|$FORM_aiccu_tls|1
			field|@TR<<Default IPv6 Route>>
			checkbox|${cfgs_section}_default_route|$FORM_aiccu_default_route|1
			field|@TR<<Behind NAT>>
			checkbox|${cfgs_section}_nat|$FORM_aiccu_nat|1
			field|@TR<<Heartbeat>>
			checkbox|${cfgs_section}_heartbeat|$FORM_aiccu_heartbeat|1
			end_form"
		append forms "$aiccu_forms" "$N"
		javascript_forms="
			v = (isset('${cfgs_section}_provider', 'other'));
			set_visible('field_${cfgs_section}_protocol', v);
			set_visible('field_${cfgs_section}_tls', v);
			set_visible('field_${cfgs_section}_server', v);"
		append js "$javascript_forms" "$N"
	done
fi

for interface in $network; do
	config_get delete_check $interface proto
	if [ "$interface" != "loopback" ]; then
	if [ "$delete_check" != "" ]; then
	if empty "$FORM_submit"; then
		config_get FORM_proto $interface proto
		config_get FORM_type $interface type
		config_get FORM_ifname $interface ifname
		config_get FORM_ipaddr $interface ipaddr
		config_get FORM_netmask $interface netmask
		config_get FORM_gateway $interface gateway
		config_get FORM_pptp_server $interface server
		config_get FORM_service $interface service
		config_get FORM_pincode $interface pincode
		config_get FORM_country $interface country
		config_get FORM_apn $interface apn
		config_get FORM_username $interface username
		config_get FORM_passwd $interface password
		config_get FORM_mtu $interface mtu
		config_get FORM_ppp_redial $interface ppp_redial
		config_get FORM_keepalive $interface keepalive
		config_get FORM_demand $interface demand
		config_get FORM_vci $interface vci
		config_get FORM_vpi $interface vpi
		config_get FORM_macaddr $interface macaddr
		config_get_bool FORM_defaultroute $interface defaultroute 1
		config_get FORM_ip6addr $interface ip6addr
		config_get FORM_gateway6 $interface ip6gw
		config_get FORM_peeraddr $interface peeraddr
		config_get FORM_ttl $interface ttl
		config_get FORM_tunnelid $interface tunnelid
		config_get FORM_dns $interface dns
		config_get FORM_device $interface device
		eval FORM_dnsremove="\$FORM_${interface}_dnsremove"
		if [ "$FORM_dnsremove" != "" ]; then
			list_remove FORM_dns "$FORM_dnsremove"
			uci_set "network" "$interface" "dns" "$FORM_dns"
		fi
	else
		eval FORM_proto="\$FORM_${interface}_proto"
		eval FORM_type="\$FORM_${interface}_type"
		eval FORM_ifname="\$FORM_${interface}_ifname"
		eval FORM_ipaddr="\$FORM_${interface}_ipaddr"
		eval FORM_netmask="\$FORM_${interface}_netmask"
		eval FORM_gateway="\$FORM_${interface}_gateway"
		eval FORM_pptp_server="\$FORM_${interface}_pptp_server"
		eval FORM_service="\$FORM_${interface}_service"
		eval FORM_pincode="\$FORM_${interface}_pincode"
		eval FORM_country="\$FORM_${interface}_country"
		eval FORM_apn="\$FORM_${interface}_apn"
		eval FORM_username="\$FORM_${interface}_username"
		eval FORM_passwd="\$FORM_${interface}_passwd"
		eval FORM_mtu="\$FORM_${interface}_mtu"
		eval FORM_ppp_redial="\$FORM_${interface}_ppp_redial"
		eval FORM_demand="\$FORM_${interface}_demand"
		eval FORM_keepalive="\$FORM_${interface}_keepalive"
		eval FORM_vci="\$FORM_${interface}_vci"
		eval FORM_vpi="\$FORM_${interface}_vpi"
		eval FORM_macaddr="\$FORM_${interface}_macaddr"
		eval FORM_defaultroute="\$FORM_${interface}_defaultroute"
		eval FORM_ip6addr="\$FORM_${interface}_ip6addr"
		eval FORM_gateway6="\$FORM_${interface}_gateway6"
		eval FORM_peerid="\$FORM_${interface}_peeraddr"
		eval FORM_ttl="\$FORM_${interface}_ttl"
		eval FORM_tunnelid="\$FORM_${interface}_tunnelid"
		eval FORM_device="\$FORM_${interface}_device"
		eval FORM_dnsadd="\$FORM_${interface}_dnsadd"
		config_get FORM_dns $interface dns
		[ $FORM_dnsadd != "" ] && FORM_dns="$FORM_dns $FORM_dnsadd"
		if [ "$FORM_defaultroute" = "" ]; then
			FORM_defaultroute=0
		fi
		validate <<EOF
mac|FORM_${interface}_macaddr|$interface @TR<<MAC Address>>||$FORM_macaddr
ip|FORM_${interface}_ipaddr|$interface @TR<<IP Address>>||$FORM_ipaddr
netmask|FORM_${interface}_netmask|$interface @TR<<WAN Netmask>>||$FORM_netmask
ip|FORM_${interface}_gateway|$interface @TR<<Default Gateway>>||$FORM_gateway
ip|FORM_${interface}_pptp_server|$interface @TR<<PPTP Server IP>>||$FORM_pptp_server
ip|FORM_dnsadd|@TR<<DNS Address>>||$FORM_dnsadd
EOF
		[ "$?" = "0" -a "$interface" != "$FORM_add_network" ] && {
			uci_set "network" "$interface" "proto" "$FORM_proto"
			[ "$FORM_type" = "" ] && uci_remove "network" "$interface" "type"
			[ "$FORM_type" != "" ] && uci_set "network" "$interface" "type" "$FORM_type"
			[ "$FORM_device" != "" -a "$FORM_proto" != "pptp" ] && uci_remove "network" "$interface" "device"
			uci_set "network" "$interface" "macaddr" "$FORM_macaddr"
			case "$FORM_proto" in
				pptp)
					uci_set "network" "$interface" "server" "$FORM_pptp_server" 
					uci_set "network" "$interface" "device" "$FORM_device"
					;;
				3g)
					if ! equal "$FORM_pincode" "-@@-"; then
						uci_set "network" "$interface" "pincode" "$FORM_pincode"
					fi
					uci_set "network" "$interface" "service" "$FORM_service"
					uci_set "network" "$interface" "country" "$FORM_country"
					uci_set "network" "$interface" "apn" "$FORM_apn" ;;
				dhcp)
					uci_remove "network" "$interface" "gateway";;
				6in4)
					uci_set "network" "$interface" "peeraddr" "$FORM_peeraddr"
					uci_set "network" "$interface" "defaultroute" "$FORM_defaultroute"
					uci_set "network" "$interface" "ttl" "$FORM_ttl"
					uci_set "network" "$interface" "mtu" "$FORM_mtu"
					uci_set "network" "$interface" "tunnelid" "$FORM_tunnelid"
					uci_set "network" "$interface" "username" "$FORM_username"
					uci_set "network" "$interface" "password" "$FORM_passwd";;
			esac
			case "$FORM_proto" in
				pppoe|pppoa|pptp|3g)
					uci_set "network" "$interface" "username" "$FORM_username"
					uci_set "network" "$interface" "password" "$FORM_passwd"
					uci_set "network" "$interface" "vpi" "$FORM_vpi"
					uci_set "network" "$interface" "vci" "$FORM_vci"
					uci_set "network" "$interface" "mtu" "$FORM_mtu"
					if [ "$FORM_ppp_redial" = "persist" ]; then
						uci_set "network" "$interface" "keepalive" "$FORM_keepalive"
						uci_remove "network" "$interface" "demand"
					else
						uci_remove "network" "$interface" "keepalive"
						uci_set "network" "$interface" "demand" "$FORM_demand"
					fi
					uci_set "network" "$interface" "defaultroute" "$FORM_defaultroute"
					uci_set "network" "$interface" "ppp_redial" "$FORM_ppp_redial";;
			esac
			uci_set "network" "$interface" "ipaddr" "$FORM_ipaddr"
			uci_set "network" "$interface" "ip6addr" "$FORM_ip6addr"
			uci_set "network" "$interface" "netmask" "$FORM_netmask"
			uci_set "network" "$interface" "gateway" "$FORM_gateway"
			uci_set "network" "$interface" "ip6gw" "$FORM_gateway6"
			uci_set "network" "$interface" "dns" "$FORM_dns"
		}
	fi

	network_options="start_form|$interface @TR<<Configuration>>
	field|@TR<<Connection Type>>
	select|${interface}_proto|$FORM_proto
	option|none|@TR<<Disabled>>
	option|static|@TR<<Static IP>>
	option|dhcp|@TR<<DHCP>>
	option|pppoe|@TR<<PPPOE>>
	option|pppoa|@TR<<PPPOA>>
	option|pptp|@TR<<PPTP>>
	option|3g|@TR<<WWAN>>
	option|6in4|@TR<<6in4>>
	option|6to4|@TR<<6to4>>
	option|ahcp|@TR<<AHCP>>
	helpitem|Connection Type
	helptext|Helptext Connection Type#Static IP: IP address of the interface is statically set. DHCP: The interface will fetch its IP address from a dhcp server.

	field|@TR<<Interface>>
	text|${interface}_ifname|$FORM_ifname
	helpitem|Interface
	helptext|Virtual Interface used by this network, can have multiple interfaces separates by spaces.

	field|@TR<<Type>>
	select|${interface}_type|$FORM_type
	option||@TR<<None>>
	option|bridge|@TR<<Bridged>>
	field|@TR<<MAC Address>>
	text|${interface}_macaddr|$FORM_macaddr
	helpitem|MAC Address
	helptext|Helptext MAC Address#Used to enter a MAC address besides the default one.
	end_form

	start_form||${interface}_ip_settings|hidden
	field|@TR<<IP Address>>|field_${interface}_ipaddr|hidden
	text|${interface}_ipaddr|$FORM_ipaddr
	field|@TR<<Netmask>>|field_${interface}_netmask|hidden
	text|${interface}_netmask|$FORM_netmask
	field|@TR<<Default Gateway>>|field_${interface}_gateway|hidden
	text|${interface}_gateway|$FORM_gateway
	field|@TR<<IPv6 Address>>|field_${interface}_ip6addr|hidden
	text|${interface}_ip6addr|$FORM_ip6addr
	field|@TR<<Default IPv6 Gateway>>|field_${interface}_gateway6|hidden
	text|${interface}_gateway6|$FORM_gateway6
	field|@TR<<PPTP Server IP>>|field_${interface}_pptp_server|hidden
	text|${interface}_pptp_server|$FORM_pptp_server
	helpitem|IP Settings
	helptext|Helptext IP Settings#IP Settings are optional for DHCP and PPTP. They are used as defaults in case the DHCP server is unavailable.
	end_form

	start_form||${interface}_ppp_settings|hidden
	field|@TR<<Connection Type>>|field_${interface}_service|hidden
	select|${interface}_service|$FORM_service
	option|umts_first|@TR<<UMTS first>>
	option|umts_only|@TR<<UMTS only>>
	option|gprs_only|@TR<<GPRS only>>
	field|@TR<<PIN Code>>|field_${interface}_pincode|hidden
	password|${interface}_pincode|$FORM_pincode
	field|@TR<<Select Network>>|field_${interface}_network|hidden
	onchange|setAPN
	select|${interface}_country|$FORM_country
	$WWAN_COUNTRY_LIST
	onchange|
	field|@TR<<APN Name>>|field_${interface}_apn|hidden
	text|${interface}_apn|$FORM_apn
	field|@TR<<Username>>|field_${interface}_username|hidden
	text|${interface}_username|$FORM_username
	field|@TR<<Password>>|field_${interface}_passwd|hidden
	password|${interface}_passwd|$FORM_passwd
	onchange|modechange
	field|@TR<<Redial Policy>>|${interface}_redial|hidden
	select|${interface}_ppp_redial|$FORM_ppp_redial
	option|demand|@TR<<Connect on Demand>>
	option|persist|@TR<<Keep Alive>>
	field|@TR<<Maximum Idle Time>>|${interface}_demand_idletime|hidden
	text|${interface}_demand|$FORM_demand
	helpitem|Maximum Idle Time
	helptext|Helptext Idle Time#The number of seconds without internet traffic that the router should wait before disconnecting from the Internet (Connect on Demand only)
	field|@TR<<Redial Timeout>>|${interface}_persist_redialperiod|hidden
	text|${interface}_keepalive|$FORM_keepalive
	helpitem|Redial Timeout
	helptext|Helptext Redial Timeout#The number of seconds to wait after receiving no response from the provider before trying to reconnect
	field|@TR<<MTU>>|field_${interface}_mtu|hidden
	text|${interface}_mtu|$FORM_mtu
	field|VCI|field_${interface}_vci|hidden
	text|${interface}_vci|$FORM_vci
	field|VPI|field_${interface}_vpi|hidden
	text|${interface}_vpi|$FORM_vpi
	field|@TR<<Default Route>>
	checkbox|${interface}_defaultroute|$FORM_defaultroute|1
	field|@TR<<VPN>>|field_${interface}_vpn|hidden
	checkbox|${interface}_device|$FORM_device|vpn
	field|@TR<<Peer Address>>|field_${interface}_peeraddr|hidden
	text|${interface}_peeraddr|$FORM_peeraddr
	field|TTL|field_${interface}_ttl|hidden
	text|${interface}_ttl|$FORM_ttl
	field|@TR<<Tunnel ID>>|field_${interface}_tunnelid|hidden
	text|${interface}_tunnelid|$FORM_tunnelid
	end_form

	start_form|$interface @TR<<DNS Servers>>|field_${interface}_dns|hidden
	listedit|${interface}_dns|$SCRIPT_NAME?${interface}_proto=static&amp;|$FORM_dns
	end_form"

	append forms "$network_options" "$N"

	remove_network_form="string|<a href=\"$SCRIPT_NAME?remove_network=$interface\">@TR<<Remove Network>> $interface</a>"
	append forms "$remove_network_form" "$N"

	###################################################################
	# set JavaScript
	javascript_forms="
		v = (isset('${interface}_proto', 'pppoe') || isset('${interface}_proto', 'pptp') || isset('${interface}_proto', 'pppoa') || isset('${interface}_proto', '3g') || isset('${interface}_proto', '6in4'));
		set_visible('${interface}_ppp_settings', v);
		set_visible('field_${interface}_username', v);
		set_visible('field_${interface}_passwd', v);
		set_visible('field_${interface}_mtu', v);

		v = (isset('${interface}_proto', 'pppoe') || isset('${interface}_proto', 'pptp') || isset('${interface}_proto', 'pppoa') || isset('${interface}_proto', '3g'));
		set_visible('${interface}_redial', v);
		set_visible('${interface}_demand_idletime', v && isset('${interface}_ppp_redial', 'demand'));
		set_visible('${interface}_persist_redialperiod', v && !isset('${interface}_ppp_redial', 'demand'));

		v = (isset('${interface}_proto', 'static') || isset('${interface}_proto', 'pptp') || isset('${interface}_proto', 'dhcp'));
		set_visible('${interface}_ip_settings', v);
		set_visible('field_${interface}_ipaddr', v);
		set_visible('field_${interface}_netmask', v);

		v = (isset('${interface}_proto', 'static'));
		set_visible('field_${interface}_gateway', v);
		set_visible('field_${interface}_dns', v);
		
		v = (isset('${interface}_proto', 'static') && ('$ipv6_installed'=='1'));
		set_visible('field_${interface}_ip6addr', v);
		set_visible('field_${interface}_gateway6', v);

		v = (isset('${interface}_proto', 'pptp'));
		set_visible('field_${interface}_pptp_server', v);
		set_visible('field_${interface}_vpn', v);

		v = (isset('${interface}_proto', 'pppoa'));
		set_visible('field_${interface}_vci', v);
		set_visible('field_${interface}_vpi', v);

		v = (isset('${interface}_proto', '3g'));
		set_visible('field_${interface}_service', v);
		set_visible('field_${interface}_network', v);
		set_visible('field_${interface}_apn', v);
		set_visible('field_${interface}_pincode', v);

		v = (isset('${interface}_proto', '6in4'));
		set_visible('field_${interface}_ttl', v);
		set_visible('field_${interface}_tunnelid', v);
		set_visible('field_${interface}_peeraddr', v);"
	append js "$javascript_forms" "$N"

	wwan_js="document.getElementById(\"${interface}_apn\").value = apnDB[element.value].name;
	document.getElementById(\"${interface}_username\").value = apnDB[element.value].user;
	document.getElementById(\"${interface}_passwd\").value = apnDB[element.value].pass;"
	append JS_APN_DB "$wwan_js" "$N"

	fi
	fi
done

add_network_form="
start_form
field|@TR<<Add Network>>|field_add_network
text|add_network
submit|button_add_network| @TR<<Add Network>> |
end_form"
append forms "$add_network_form" "$N"

header "Network" "Networks" "@TR<<Network Configuration>>" ' onload="modechange()" ' "$SCRIPT_NAME"
#####################################################################
# modechange script
#
cat <<EOF
<script type="text/javascript" src="/webif.js"></script>
<script type="text/javascript">
<!--
function setAPN(element) {
	var apnDB = new Object();
	$JS_APN_DB
}
function modechange()
{
	var v;
	$js

	hide('save');
	show('save');
}
-->
</script>

EOF

display_form <<EOF
onchange|modechange
$validate_error
$forms
EOF

footer ?>
<!--
##WEBIF:name:Network:101:Networks
-->
