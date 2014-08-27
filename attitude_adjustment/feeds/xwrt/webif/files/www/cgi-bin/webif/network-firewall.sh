#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
###################################################################
# Firewall configuration
#
# Description:
#	Firewall configuration.
#
# Author(s) [in order of work date]:
#	Original webif authors.
#	Travis Kemen	<kemen04@gmail.com>
# Major revisions:
#
# UCI variables referenced:
#
# Configuration files referenced:
#	firewall
#

#remove rule
if ! empty "$FORM_remove_vcfg"; then
	uci_remove "firewall" "$FORM_remove_vcfg"
fi
#The following check needs to remain or we have no good way of knowing if a rule should be added.
[ -z "$FORM_port_select_rule" ] && FORM_port_select_rule=custom
#Add new rules
if [ -n "$FORM_port_rule" -o "$FORM_port_select_rule" != "custom" ]; then
	validate <<EOF
string|FORM_name|@TR<<Name>>|nospaces|$FORM_name
ip|FORM_src_ip_rule|@TR<<Source IP Address>>||$FORM_src_ip_rule
ip|FORM_dest_ip_rule|@TR<<Destination IP Address>>||$FORM_dest_ip_rule
ports|FORM_port_rule|@TR<<Destination Port>>||$FORM_port_rule
EOF
	equal "$?" 0 && {
		[ "$FORM_port_select_rule" != "custom" ] && FORM_port_rule="$FORM_port_select_rule"
		uci_add firewall rule "$FORM_name"; add_rule_cfg="$CONFIG_SECTION"
		uci_set firewall "$add_rule_cfg" proto "$FORM_protocol_rule"
		uci_set firewall "$add_rule_cfg" src "$FORM_src_rule"
		uci_set firewall "$add_rule_cfg" dest "$FORM_dest_rule"
		uci_set firewall "$add_rule_cfg" src_ip "$FORM_src_ip_rule"
		uci_set firewall "$add_rule_cfg" dest_ip "$FORM_dest_ip_rule"
		uci_set firewall "$add_rule_cfg" dest_port "$FORM_port_rule"
		uci_set firewall "$add_rule_cfg" target "$FORM_target_rule"
		unset FORM_port_rule FORM_dest_ip_rule FORM_src_ip_rule FORM_protocol_rule FORM_name FORM_src FORM_dest
		FORM_port_select_rule=custom
		FORM_src=wan
	}
fi
if [ -n "$FORM_dest_ip_redirect" ]; then
	validate <<EOF
string|FORM_name_redirect|@TR<<Name>>|nospaces|$FORM_name_redirect
ip|FORM_src_ip_redirect|@TR<<Source IP Address>>||$FORM_src_ip_redirect
ports|FORM_src_dport_redirect|@TR<<Destination Port>>|required|$FORM_src_dport_redirect
ip|FORM_dest_ip_redirect|@TR<<To IP Address>>|required|$FORM_dest_ip_redirect
ports|FORM_dest_port_redirect|@TR<<To Port>>||$FORM_dest_port_redirect
EOF
	equal "$?" 0 && {
		[ "$FORM_port_select_redirect" != "custom" ] && FORM_port_rule="$FORM_port_select_redirect"
		uci_add firewall redirect "$FORM_name_redirect"; add_redirect_cfg="$CONFIG_SECTION"
		uci_set firewall "$add_redirect_cfg" src wan
		uci_set firewall "$add_redirect_cfg" proto "$FORM_protocol_redirect"
		uci_set firewall "$add_redirect_cfg" src_ip "$FORM_src_ip_redirect"
		uci_set firewall "$add_redirect_cfg" src_dport "$FORM_src_dport_redirect"
		uci_set firewall "$add_redirect_cfg" dest_ip "$FORM_dest_ip_redirect"
		[ "$FORM_dest_port_redirect" = "" -a "0" != "$(echo "$FORM_src_dport_redirect" |grep -q "-" -e ":";echo "$?")" ] && FORM_dest_port_redirect="$FORM_src_dport_redirect"
		uci_set firewall "$add_redirect_cfg" dest_port "$FORM_dest_port_redirect"
		unset FORM_dest_port_redirect FORM_dest_ip_redirect FORM_src_dport_redirect FORM_src_ip_redirect FORM_protocol_redirect FORM_name_redirect
		FORM_port_select_redirect=custom
	}
fi
if [ -n "$FORM_add_rule_add" ]; then
	uci_add firewall forwarding ""; add_forward_cfg="$CONFIG_SECTION"
	uci_set firewall "$add_forward_cfg" src "$FORM_src_add"
	uci_set firewall "$add_forward_cfg" dest "$FORM_dest_add"
fi
config_cb() {
	local cfg_type="$1"
	local cfg_name="$2"

	case "$cfg_type" in
		forwarding)
			append forwarding_cfgs "$cfg_name"
		;;
		defaults)
			append default_cfgs "$cfg_name" "$N"
		;;
		zone)
			append zone_cfgs "$cfg_name" "$N"
		;;
		rule)
			append rule_cfgs "$cfg_name" "$N"
		;;
		redirect)
			append redirect_cfgs "$cfg_name" "$N"
		;;
		interface)
			if [ "$cfg_name" != "loopback" ]; then
				append networks "option|$cfg_name" "$N"
			fi
		;;
	esac
}
cur_color="odd"
get_tr() {
	if equal "$cur_color" "odd"; then
		cur_color="even"
		tr="string|<tr>"
	else
		cur_color="odd"
		tr="string|<tr class=\"odd\">"
	fi
}

uci_load firewall
uci_load network
append forms "start_form|@TR<<Forwarding Configuration>>" "$N"
for zone in $forwarding_cfgs; do
	eval FORM_remove="\$FORM_remove_rule_$zone"
	if [ "$FORM_remove" != "" ]; then
		uci_remove firewall "$zone"
	fi
	if [ "$FORM_submit" = "" -o "$add_forward_cfg" = "$zone" ]; then
		config_get FORM_src "$zone" src
		config_get FORM_dest "$zone" dest
	else
		eval FORM_src="\$FORM_src_$zone"
		eval FORM_dest="\$FORM_dest_$zone"
		eval FORM_mss="\$FORM_mss_$zone"
		uci_set firewall "$zone" src "$FORM_src"
		uci_set firewall "$zone" dest "$FORM_dest"
	fi
	if [ "$FORM_remove" = "" ]; then
		form="field|@TR<<Allow traffic originating from>>
			select|src_$zone|$FORM_src
			$networks
			string|@TR<<to>>
			select|dest_$zone|$FORM_dest
			$networks
			submit|remove_rule_$zone|@TR<<Remove Rule>>"
		append forms "$form" "$N"
	fi
done
form="field|@TR<<Allow traffic originating from>>
	select|src_add
	$networks
	string|@TR<<to>>
	select|dest_add
	$networks
	submit|add_rule_add|@TR<<Add Rule>>
	end_form"
append forms "$form" "$N"

for zone in $default_cfgs; do
	if [ "$FORM_submit" = "" ]; then
		config_get FORM_input $zone input
		config_get FORM_output $zone output
		config_get FORM_forward $zone forward
		config_get_bool FORM_syn_flood $zone syn_flood 1
		config_get_bool FORM_disable_ipv6 $zone disable_ipv6 0
	else
		eval FORM_input="\$FORM_input_$zone"
		eval FORM_output="\$FORM_output_$zone"
		eval FORM_forward="\$FORM_forward_$zone"
		eval FORM_syn_flood="\$FORM_syn_flood_$zone"
		eval FORM_disable_ipv6="\$FORM_disable_ipv6_$zone"
		uci_set firewall "$zone" disable_ipv6 "$FORM_disable_ipv6"
		uci_set firewall "$zone" output "$FORM_output"
		uci_set firewall "$zone" input "$FORM_input"
		uci_set firewall "$zone" forward "$FORM_forward"
		[ "$FORM_syn_flood" = "" ] && FORM_syn_flood=0
		uci_set firewall "$zone" syn_flood "$FORM_syn_flood"
	fi

	form="start_form|@TR<<Default Zone Settings>>
		field|@TR<<Input>>
		select|input_$zone|$FORM_input
		option|ACCEPT|@TR<<Accept>>
		option|REJECT|@TR<<Reject>>
		option|DROP|@TR<<Drop>>
		field|@TR<<Output>>
		select|output_$zone|$FORM_output
		option|ACCEPT|@TR<<Accept>>
		option|REJECT|@TR<<Reject>>
		option|DROP|@TR<<Drop>>
		field|@TR<<Forward>>
		select|forward_$zone|$FORM_forward
		option|ACCEPT|@TR<<Accept>>
		option|REJECT|@TR<<Reject>>
		option|DROP|@TR<<Drop>>
		field|@TR<<Disable IPv6>>
		checkbox|disable_ipv6_$zone|$FORM_disable_ipv6|1
		field|@TR<<SYN Flood Protection>>
		checkbox|syn_flood_$zone|$FORM_syn_flood|1
		end_form"
		append forms "$form" "$N"
done

for zone in $zone_cfgs; do
	if [ "$FORM_submit" = "" ]; then
		config_get FORM_name $zone name
		config_get FORM_input $zone input
		config_get FORM_output $zone output
		config_get FORM_forward $zone forward
		config_get_bool FORM_masq $zone masq 0
		config_get_bool FORM_mtu_fix $zone mtu_fix 0
	else
		config_get FORM_name $zone name
		eval FORM_input="\$FORM_input_$zone"
		eval FORM_output="\$FORM_output_$zone"
		eval FORM_forward="\$FORM_forward_$zone"
		eval FORM_masq="\$FORM_masq_$zone"
		eval FORM_mtu_fix="\$FORM_mtu_fix_$zone"
		uci_set firewall "$zone" input "$FORM_input"
		uci_set firewall "$zone" output "$FORM_output"
		uci_set firewall "$zone" forward "$FORM_forward"
		uci_set firewall "$zone" masq "$FORM_masq"
		uci_set firewall "$zone" mtu_fix "$FORM_mtu_fix"
	fi
	form="start_form|@TR<<$FORM_name Zone Settings>>
		field|@TR<<Input>>
		select|input_$zone|$FORM_input
		option|ACCEPT|@TR<<Accept>>
		option|REJECT|@TR<<Reject>>
		option|DROP|@TR<<Drop>>
		field|@TR<<Output>>
		select|output_$zone|$FORM_output
		option|ACCEPT|@TR<<Accept>>
		option|REJECT|@TR<<Reject>>
		option|DROP|@TR<<Drop>>
		field|@TR<<Forward>>
		select|forward_$zone|$FORM_forward
		option|ACCEPT|@TR<<Accept>>
		option|REJECT|@TR<<Reject>>
		option|DROP|@TR<<Drop>>
		field|@TR<<NAT>>
		checkbox|masq_$zone|$FORM_masq|1
		field|@TR<<MTU Fix>>
		checkbox|mtu_fix_$zone|$FORM_mtu_fix|1
		end_form"
	append forms "$form" "$N"
done

get_tr
form="string|<div class=\"settings\">
	string|<h3><strong>@TR<<Incoming Ports>></strong></h3>
	string|<table style=\"width: 90%; margin-left: 2.5em; text-align: left; font-size: 0.8em;\" border=\"0\" cellpadding=\"3\" cellspacing=\"2\" summary=\"@TR<<Incomimg Ports>>\">
	$tr
	string|<th>@TR<<Name>></th>
	string|<th>@TR<<Source>></th>
	string|<th>@TR<<Destination>></th>
	string|<th>@TR<<Protocol>></th>
	string|<th>@TR<<Source IP>></th>
	string|<th>@TR<<Destination IP>></th>
	string|<th>@TR<<Port>></th>
	string|</tr>"
append forms "$form" "$N"
for rule in $rule_cfgs; do
	if [ "$FORM_submit" = "" -o "$add_rule_cfg" = "$rule" ]; then
		config_get FORM_src "$rule" src "wan"
		config_get FORM_dest "$rule" dest
		config_get FORM_protocol "$rule" proto
		config_get FORM_src_ip "$rule" src_ip
		config_get FORM_dest_ip "$rule" dest_ip
		config_get FORM_target "$rule" target "ACCEPT"
		config_get FORM_port "$rule" dest_port
		FORM_port_select_rule=custom
	else
		eval FORM_src="\$FORM_src_$rule"
		eval FORM_dest="\$FORM_dest_$rule"
		eval FORM_protocol="\$FORM_protocol_$rule"
		eval FORM_src_ip="\$FORM_src_ip_$rule"
		eval FORM_dest_ip="\$FORM_dest_ip_$rule"
		eval FORM_port="\$FORM_port_$rule"
		eval FORM_target="\$FORM_target_$rule"
		validate <<EOF
ip|FORM_src_ip|@TR<<Source IP Address>>||$FORM_src_ip
ip|FORM_dest_ip|@TR<<Destination IP Address>>||$FORM_dest_ip
ports|FORM_port|@TR<<Destination Port>>||$FORM_port
EOF
		equal "$?" 0 && {
			uci_set firewall "$rule" src "$FORM_src"
			uci_set firewall "$rule" dest "$FORM_dest"
			uci_set firewall "$rule" proto "$FORM_protocol"
			uci_set firewall "$rule" src_ip "$FORM_src_ip"
			uci_set firewall "$rule" dest_ip "$FORM_dest_ip"
			uci_set firewall "$rule" dest_port "$FORM_port"
			uci_set firewall "$rule" target "$FORM_target"
		}
	fi

	echo "$rule" |grep -q "cfg*****" && name="" || name="$rule"
	get_tr
	form="$tr
		string|<td>$name</td>
		string|<td>
		select|src_$rule|$FORM_src
		option||@TR<<Router>>
		$networks
		string|</td>
		string|<td>
		select|dest_$rule|$FORM_dest
		option||@TR<<Router>>
		$networks
		string|</td>
		string|<td>
		select|protocol_$rule|$FORM_protocol
		option|tcp|TCP
		option|udp|UDP
		option|tcpudp|Both
		option|icmp|ICMP
		string|</td>
		string|<td>
		text|src_ip_$rule|$FORM_src_ip
		string|</td>
		string|<td>
		text|dest_ip_$rule|$FORM_dest_ip
		string|</td>
		string|<td>
		text|port_$rule|$FORM_port
		string|</td>
		string|<td>
		select|target_$rule|$FORM_target
		option|ACCEPT|@TR<<Accept>>
		option|DROP|@TR<<Drop>>
		option|REJECT|@TR<<Reject>>
		string|</td>
		string|<td>
		string|<a href=\"$SCRIPT_NAME?remove_vcfg=$rule\">@TR<<Remove Rule>></a>
		string|</td>
		string|</tr>"
	append forms "$form" "$N"
done
get_tr
form="$tr
	string|<td>
	text|name|$FORM_name
	string|</td>
	string|<td>
	select|src_rule|wan
	option||@TR<<Router>>
	$networks
	string|</td>
	string|<td>
	select|dest_rule|
	option||@TR<<Router>>
	$networks
	string|</td>
	string|<td>
	select|protocol_rule|$FORM_protocol_rule
	option|tcp|TCP
	option|udp|UDP
	option|tcpudp|Both
	option|icmp|ICMP
	string|</td>
	string|<td>
	text|src_ip_rule|$FORM_src_ip_rule
	string|</td>
	string|<td>
	text|dest_ip_rule|$FORM_dest_ip_rule
	string|</td>
	string|<td>
	select|port_select_rule|$FORM_port_select_rule
	option|custom|@TR<<Custom>>
	option|22|SSH
	option|25|SMTP
	option|110|POP3
	option|143|IMAP
	option|80|HTTP
	option|443|HTTPS
	text|port_rule|$FORM_port_rule
	string|</td>
	string|<td>
	select|target_rule|ACCEPT
	option|ACCEPT|@TR<<Accept>>
	option|DROP|@TR<<Drop>>
	option|REJECT|@TR<<Reject>>
	string|</td>
	string|<td>
	string|&nbsp;
	string|</td>
	string|</tr>
	string|</table></div>"
append forms "$form" "$N"

#PORT Forwarding
cur_color="odd"
get_tr
form="string|<div class=\"settings\">
	string|<h3><strong>@TR<<Port Forwarding>></strong></h3>
	string|<table style=\"width: 90%; margin-left: 2.5em; text-align: left; font-size: 0.8em;\" border=\"0\" cellpadding=\"3\" cellspacing=\"2\" summary=\"@TR<<Port Forwarding>>\">
	$tr
	string|<th>@TR<<Name>></th>
	string|<th>@TR<<Protocol>></th>
	string|<th>@TR<<Source IP>></th>
	string|<th>@TR<<Destination Port>></th>
	string|<th>@TR<<To IP Address>></th>
	string|<th>@TR<<To Port>></th>
	string|</tr>"
append forms "$form" "$N"

for rule in $redirect_cfgs; do
	if [ "$FORM_submit" = "" -o "$add_redirect_cfg" = "$rule" ]; then
		config_get FORM_protocol "$rule" proto
		config_get FORM_src_ip "$rule" src_ip
		config_get FORM_dest_ip "$rule" dest_ip
		config_get FORM_src_dport "$rule" src_dport
		config_get FORM_dest_port "$rule" dest_port
		FORM_port_select_redirect=custom
	else
		eval FORM_protocol="\$FORM_protocol_$rule"
		eval FORM_src_ip="\$FORM_src_ip_$rule"
		eval FORM_dest_ip="\$FORM_dest_ip_$rule"
		eval FORM_dest_port="\$FORM_dest_port_$rule"
		eval FORM_src_dport="\$FORM_src_dport_$rule"
		validate <<EOF
ip|FORM_src_ip_rule|@TR<<Source IP Address>>||$FORM_src_ip_rule
ip|FORM_dest_ip_rule|@TR<<Destination IP Address>>||$FORM_dest_ip_rule
ports|FORM_src_dport|@TR<<Destination Port>>|required|$FORM_src_dport
ip|FORM_dest_ip|@TR<<To IP>>|required|$FORM_dest_ip
ports|FORM_dest_dport|@TR<<To Port>>||$FORM_dest_dport
EOF
		equal "$?" 0 && {
			uci_set firewall "$rule" proto "$FORM_protocol"
			uci_set firewall "$rule" src_ip "$FORM_src_ip"
			uci_set firewall "$rule" dest_ip "$FORM_dest_ip"
			uci_set firewall "$rule" src_dport "$FORM_src_dport"
			[ "$FORM_dest_port" = "" -a "0" != "$(echo "$FORM_src_dport_redirect" |grep -q "-" -e ":";echo "$?")" ] && FORM_dest_port="$FORM_src_dport"
			uci_set firewall "$rule" dest_port "$FORM_dest_port"
		}
	fi

	echo "$rule" |grep -q "cfg*****" && name="" || name="$rule"
	get_tr
	form="$tr
		string|<td>$name</td>
		string|<td>
		select|protocol_$rule|$FORM_protocol
		option|tcp|TCP
		option|udp|UDP
		option|tcpudp|Both
		string|</td>
		string|<td>
		text|src_ip_$rule|$FORM_src_ip
		string|</td>
		string|<td>
		text|src_dport_$rule|$FORM_src_dport
		string|</td>
		string|<td>
		text|dest_ip_$rule|$FORM_dest_ip
		string|</td>
		string|<td>
		text|dest_port_$rule|$FORM_dest_port
		string|</td>
		string|<td>
		string|<a href=\"$SCRIPT_NAME?remove_vcfg=$rule\">@TR<<Remove Rule>></a>
		string|</td>
		string|</tr>"
	append forms "$form" "$N"
done
get_tr
form="$tr
	string|<td>
	text|name_redirect|$FORM_name_redirect
	string|</td>
	string|<td>
	select|protocol_redirect|$FORM_protocol_redirect
	option|tcp|TCP
	option|udp|UDP
	option|tcpudp|Both
	string|</td>
	string|<td>
	text|src_ip_redirect|$FORM_src_ip_redirect
	string|</td>
	string|<td>
	text|src_dport_redirect|$FORM_src_dport_redirect
	string|</td>
	string|<td>
	text|dest_ip_redirect|$FORM_dest_ip_redirect
	string|</td>
	string|<td>
	select|port_select_redirect|$FORM_port_select_redirect
	option|custom|@TR<<Custom>>
	option|22|SSH
	option|25|SMTP
	option|110|POP3
	option|143|IMAP
	option|80|HTTP
	option|443|HTTPS
	text|dest_port_redirect|$FORM_dest_port_redirect
	string|</td>
	string|<td>
	string|&nbsp;
	string|</td>
	string|</tr>
	string|</table></div>"
append forms "$form" "$N"

header_inject_head="<script type=\"text/javascript\" src=\"/webif.js\"></script>"

header "Network" "Firewall" "@TR<<Firewall>>" 'onload="modechange()" onkeydown="return processKey(event)"' "$SCRIPT_NAME"
#####################################################################
# modechange script
#
cat <<EOF
<script type="text/javascript">
<!--
function modechange()
{
	var v;
	$js

	v = (isset('port_select_rule','custom'));
	set_visible('port_rule', v);
	v = (isset('port_select_redirect','custom'));
	set_visible('port_redirect', v);

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
##WEBIF:name:Network:415:Firewall
-->
