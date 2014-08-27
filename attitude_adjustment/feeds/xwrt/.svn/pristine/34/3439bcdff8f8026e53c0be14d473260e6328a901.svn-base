#!/usr/bin/webif-page
<?
###################################################################
# qos-scripts configuration page
# (c)2007 X-Wrt project (http://www.x-wrt.org)
# (c)2007 Jeremy Collake
#
# Description:
#	Configures the qos-scripts package.
#
# Author(s) [in order of work date]:
#	Jeremy Collake <jeremy.collake@gmail.com>
#
# Major revisions:
#
# NVRAM variables referenced:
#   none
#
# Configuration files referenced:
#   /etc/config/qos
#
#
. /usr/lib/webif/webif.sh

header "Network" "QoS" "@TR<<QOS Configuration>>" ' onload="modechange()" ' "$SCRIPT_NAME"

if ! empty "$FORM_install_nbd"; then
	echo "@TR<<qos_installing#Installing Nbd's QoS scripts>> ...<pre>"
	install_package qos-scripts
	echo "</pre>"
fi

is_package_installed "qos-re" && {
	echo "<div class=\"warning\">@TR<<qos_found_rudys#Rudy's QoS scripts are found installed. Be sure to uninstall Rudy's scripts before using the new qos-scripts package.>></div>"
}

# TODO: move this to shared functions somewhere
# set an option, or remove it if the value is empty
uci_set_value_remove_if_empty() {
	local _package="$1"
	local _config="$2"
	local _option="$3"
	local _value="$4"
	if ! empty "$_value"; then
		uci_set "$_package" "$_config" "$_option" "$_value"
	else
		uci_remove "$_package" "$_config" "$_option"
	fi
}

#########################################################################################
# if qos-scripts installed ... (encapsulates most of remainder)
#
if is_package_installed "qos-scripts"; then

uci_load "webif"

#
# if form submit, then ...
# else ...
#
! empty "$FORM_submit" && empty "$FORM_install_nbd" && {
	current_qos_item="$FORM_current_rule_index"
	! empty "$current_qos_item" && {
		# for validation purposes, replace non-numeric stuff in
		# ports list and port range with integer.
		ports_validate=$(echo "$FORM_current_ports" | sed s/','/'0'/g)
		portrange_validate=$(echo "$FORM_current_portrange" | sed s/'-'/'0'/g)
validate <<EOF
int|ports_validate|@TR<<Port Listing>>||$ports_validate
int|portrange_validate|@TR<<Port Range>>||$portrange_validate
ip|FORM_current_srchost|@TR<<Source IP>>||$FORM_current_srchost
ip|FORM_current_dsthost|@TR<<Dest IP>>||$FORM_current_dsthost
EOF
		if ! equal "$?" "0"; then
			echo "<div class=\"warning\">@TR<<qos_validation_failed#Validation of one or more fields failed! Not saving.>></div>"
		else
			SAVED=1
			uci_set "qos" "$current_qos_item" "target" "$FORM_current_target"
			uci_set_value_remove_if_empty "qos" "$current_qos_item" "srchost" "$FORM_current_srchost"
			uci_set_value_remove_if_empty "qos" "$current_qos_item" "dsthost" "$FORM_current_dsthost"
			uci_set_value_remove_if_empty "qos" "$current_qos_item" "proto" "$FORM_current_proto"
			uci_set_value_remove_if_empty "qos" "$current_qos_item" "ports" "$FORM_current_ports"
			uci_set_value_remove_if_empty "qos" "$current_qos_item" "portrange" "$FORM_current_portrange"
			uci_set_value_remove_if_empty "qos" "$current_qos_item" "layer7" "$FORM_current_layer7"
			uci_set_value_remove_if_empty "qos" "$current_qos_item" "ipp2p" "$FORM_current_ipp2p"
			uci_set_value_remove_if_empty "qos" "$current_qos_item" "mark" "$FORM_current_mark"
			uci_set_value_remove_if_empty "qos" "$current_qos_item" "tcpflags" "$FORM_current_tcpflags"
			uci_set_value_remove_if_empty "qos" "$current_qos_item" "pktsize" "$FORM_current_pktsize"
		fi
	}

	validate <<EOF
int|FORM_wan_dowload|@TR<<WAN Download Speed>>||$FORM_wan_download
int|FORM_wan_upload|@TR<<WAN Upload Speed>>||$FORM_wan_upload
EOF
	equal "$?" "0" && {
		SAVED=1
		uci_load qos # to check existing variables
		! equal "$FORM_wan_enabled" "$CONFIG_wan_enabled" && {
		 	uci_set "qos" "wan" "enabled" "$FORM_wan_enabled"
		}
		! equal "FORM_wan_overhead" "$CONFIG_wan_overhead" && {
			uci_set "qos" "wan" "overhead" "$FORM_wan_overhead"
		}
		! empty "$FORM_wan_download" && ! equal "$FORM_wan_download" "$CONFIG_wan_download" && {
			uci_set "qos" "wan" "download" "$FORM_wan_download"
		}
		! empty "$FORM_wan_upload" && ! equal "$FORM_wan_upload" "$CONFIG_wan_upload" && {
			uci_set "qos" "wan" "upload" "$FORM_wan_upload"
		}
		! empty "$FORM_webif_advanced" && ! equal "$FORM_webif_advanced" "$CONFIG_qos_show_advanced_rules" && {
			uci_set "webif" "qos" "show_advanced_rules" "$FORM_webif_advanced"
		}
	}
}

#
# handle 'add new rule'
#
! empty "$FORM_qos_add" && {
	# todo: this add needs to be in the save area, causes instant save here	of
	#       an empty rule here. However, requires more work than a simple move ;).
	uci_add "qos" "classify" ""
}

#
# handle 'remove' (qos rule)
#
! empty "$FORM_qos_remove" && {
	current_qos_item=$(echo "$QUERY_STRING" | grep "qos_remove=" | cut -d'=' -f2)
	! empty "$current_qos_item" && {
		# also manually clear the other options so they are immediately empty
		uci_remove "qos" "$current_qos_item"
	}
}

# copy a rule to another - used by swap_rule()
copy_rule()
{
	local section_src=$1
	local section_dest=$2
	local _target
	local _srchost
	local _dsthost
	local _proto
	local _ports
	local _portrange
	local _layer7
	local _ipp2p
	local _mark
	local _tcpflags
	local _pktsize
	config_get _target "${section_dest}" "target"
	config_get _srchost "${section_dest}" "srchost"
	config_get _dsthost "${section_dest}" "dsthost"
	config_get _proto "${section_dest}" "proto"
	config_get _ports "${section_dest}" "ports"
	config_get _portrange "${section_dest}" "portrange"
	config_get _layer7 "${section_dest}" "layer7"
	config_get _ipp2p "${section_dest}" "ipp2p"
	config_get _mark "${section_dest}" "mark"
	config_get _tcpflags "${section_dest}" "tcpflags"
	config_get _pktsize "${section_dest}" "pktsize"
	uci_set_value_remove_if_empty "qos" "$section_src" "target" "$_target"
	uci_set_value_remove_if_empty "qos" "$section_src" "srchost" "$_srchost"
	uci_set_value_remove_if_empty "qos" "$section_src" "dsthost" "$_dsthost"
	uci_set_value_remove_if_empty "qos" "$section_src" "proto" "$_proto"
	uci_set_value_remove_if_empty "qos" "$section_src" "layer7" "$_layer7"
	uci_set_value_remove_if_empty "qos" "$section_src" "ipp2p" "$_ipp2p"
	uci_set_value_remove_if_empty "qos" "$section_src" "ports" "$_ports"
	uci_set_value_remove_if_empty "qos" "$section_src" "portrange" "$_portrange"
	uci_set_value_remove_if_empty "qos" "$section_src" "mark" "$_mark"
	uci_set_value_remove_if_empty "qos" "$section_src" "tcpflags" "$_tcpflags"
	uci_set_value_remove_if_empty "qos" "$section_src" "pktsize" "$_pktsize"
}

# swap a rule with another - for up/down
swap_rule()
{
	local section_src=$1
	local section_dest=$2
	copy_rule "$1" "$2"
	copy_rule "$2" "$1"
	# now a uci_load will reload swapped rules
}

#
# handle 'up' or 'down' (qos rule)
#
! empty "$FORM_qos_swap_dest" && ! empty "$FORM_qos_swap_src" && {
	uci_load "qos"
	swap_rule "$FORM_qos_swap_dest" "$FORM_qos_swap_src"
}

#
# show advanced
#
FORM_webif_advanced=${FORM_webif_advanced:-$CONFIG_qos_show_advanced_rules}

#
# load qos-scripts config
#
uci_load "qos"

FORM_wan_enabled="$CONFIG_wan_enabled"
FORM_wan_download="$CONFIG_wan_download"
FORM_wan_upload="$CONFIG_wan_upload"
FORM_wan_overhead="$CONFIG_wan_overhead"

######################################################################
cat <<EOF
<script type="text/javascript" src="/webif.js"></script>
<script type="text/javascript">

function modechange()
{
	if(isset('wan_enabled','1'))
	{
		document.getElementById('wan_upload').disabled = false;
		document.getElementById('wan_download').disabled = false;
	}
	else
	{
		document.getElementById('wan_upload').disabled = true;
		document.getElementById('wan_download').disabled = true;
	}
}
</script>
EOF
######################################################################

display_form <<EOF
onchange|modechange
start_form|@TR<<QoS Options>>
field|@TR<<QoS Service>>|field_n_enabled
select|wan_enabled|$FORM_wan_enabled
option|1|@TR<<qos_enabled#Enabled>>
option|0|@TR<<qos_disabled#Disabled>>
field|@TR<<QoS Overhead Calculation>>|field_wan_overhead
select|wan_overhead|$FORM_wan_overhead
option|1|@TR<<qos_overhead_enabled#Enabled>>
option|0|@TR<<qos_overhead_disabled#Disabled>>
field|@TR<<WAN Upload Speed>>|field_n_wan_upload
text|wan_upload|$FORM_wan_upload| @TR<<kilobits>>
helpitem|Maximum Upload/Download
helptext|HelpText Maximum Upload#Your maximum sustained upload and download speeds, in kilobits.
field|@TR<<WAN Download Speed>>|field_n_wan_download
text|wan_download|$FORM_wan_download| @TR<<kilobits>>
field|@TR<<Show Advanced Rules>>|field_webif_advanced
select|webif_advanced|$FORM_webif_advanced
option|1|@TR<<qos_adv_enabled#Enabled>>
option|0|@TR<<qos_adv_disabled#Disabled>>
helpitem|Advanced
helptext|HelpText Advanced#Normally users just use the form below to configure QoS. Some people may need access to the more advanced settings. Alternatively, you can <a href="./system-editor.sh?path=/etc/config&amp;edit=qos">manually edit the config</a>.
end_form
EOF

# show the current ruleset in a table
cat <<EOF
<div class="settings">
<h3><strong>@TR<<QoS Traffic Classification Rules>></strong></h3>
<table style="width: 90%; margin-left: 2.5em; text-align: left; font-size: 0.8em;" border="0" cellpadding="3" cellspacing="2" summary="QoS Traffic Classification Rules">
<tbody>
<tr>
<th>@TR<<Group>></th>
EOF
equal "$FORM_webif_advanced" "1" && {
	cat <<EOF
	<th>@TR<<Type>></th>
EOF
}
cat <<EOF
<th>@TR<<Source IP>></th>
<th>@TR<<Dest. IP>></th>
<th>@TR<<Protocol>></th>
<th>@TR<<Layer-7>></th>
<th>@TR<<Port range>></th>
<th>@TR<<Ports>></th>
EOF
equal "$FORM_webif_advanced" "1" && {
	cat <<EOF
	<th>@TR<<Flags>></th>
	<th>@TR<<PktSize>></th>
	<th>@TR<<Mark>></th>
EOF
}
cat <<EOF
<th></th>
</tr>
EOF

# outputs variable to a column
show_column()
{
	# section name
	# option name
	# over-ride text (if config option is empty)
	local _val
	# config_get returns TYPE if OPTION ($2) is empty, else returns value
	config_get _val "$1" "$2"
	echo "<td>${_val:-$4}</td>"
}

#
# callback for sections
#
local last_shown_rule="-1"
callback_foreach_rule() {
	local section_name=$1
	config_get _type "$section_name" "TYPE"
	case $_type in
		"classify") ;;
		"reclassify") equal "$FORM_webif_advanced" "0" && return;;
		"default") equal "$FORM_webif_advanced" "0" && return;;
		*) return;;
	esac
	## finishing previous table entry
	# for 'down' since we didn't know index of next classify item.
	# if there is a last shown rule, show 'up' option for PREVIOUS rule
	! equal "$last_shown_rule" "-1" && {
		echo "<a href=\"$SCRIPT_NAME?qos_swap_dest=$section_name&amp;qos_swap_src=$last_shown_rule\"><img alt=\"@TR<<down>>\" src=\"/images/down.gif\" title=\"@TR<<down>>\" /></a>"
		echo "</td></tr>"
	}
	## end finishing last iteration
	if equal "$cur_color" "odd"; then
		cur_color="even"
		echo "<tr>"
	else
		cur_color="odd"
		echo "<tr class=\"$cur_color\">"
	fi
	show_column "$section_name" "target" "" "..."
	equal "$FORM_webif_advanced" "1" && show_column "$section_name" "TYPE" "" ""
	show_column "$section_name" "srchost" ""
	show_column "$section_name" "dsthost" ""
	eval _val="\"\$CONFIG_${section_name}_ipp2p\""
	if empty "$_val"; then
	 	show_column "$section_name" "proto" ""
	else
		equal "$_val" "all" && _val="peer-2-peer"
		show_column "$section_name" "proto" "" "$_val"
	fi
	show_column "$section_name" "layer7" ""
	show_column "$section_name" "portrange" ""
	show_column "$section_name" "ports" ""
	equal "$FORM_webif_advanced" "1" && show_column "$section_name" "tcpflags" "" ""
	equal "$FORM_webif_advanced" "1" && show_column "$section_name" "pktsize" "" ""
	equal "$FORM_webif_advanced" "1" && show_column "$section_name" "mark" "" ""
	echo "<td>"
	echo "<a href=\"$SCRIPT_NAME?qos_remove=$section_name\"><img alt=\"@TR<<delete>>\" src=\"/images/x.gif\" title=\"@TR<<delete>>\" /></a>"
	echo "<a href=\"$SCRIPT_NAME?qos_edit=$section_name\"><img alt=\"@TR<<edit>>\" src=\"/images/edit.gif\" title=\"@TR<<edit>>\" /></a>"
	# if there is a last shown rule, show 'up' option
	! equal "$last_shown_rule" "-1" && {
		echo "<a href=\"$SCRIPT_NAME?qos_swap_src=$section_name&amp;qos_swap_dest=$last_shown_rule\"><img alt=\"@TR<<up>>\" src=\"/images/up.gif\" title=\"@TR<<up>>\" /></a>"
	}
	# if we are adding, always keep last index in FORM_qos_edit
	! empty "$FORM_qos_add" && FORM_qos_edit="$section_name"
	last_shown_rule="$section_name"	
}

config_foreach callback_foreach_rule

# if we showed any rules, finish table row
! equal "$last_shown_rule" "-1" && {
	echo "</td></tr>"
}

cat <<EOF
<tr><td><a href="$SCRIPT_NAME?qos_add=1">@TR<<new rule>></a></td></tr>
</tbody></table>
<div class="clearfix">&nbsp;</div></div>
EOF

#
# handle 'edit' (qos rule)
#
#
! empty "$FORM_qos_edit" && {
	# for padding as if the qos table was encpasulated in std form
	display_form <<EOF
	start_form
	end_form
EOF
	#
	# build list of available L7-protocols
	#
	l7_protocols="option||None"
	for curfile in /etc/l7-protocols/*; do
		_l7basename=$(basename "$curfile" | sed s/'.pat'//g)
		l7_protocols="$l7_protocols
			option|$_l7basename|$_l7basename"
	done

	current_item="$FORM_qos_edit"
	config_get _target "${current_item}" "target"
	config_get _srchost "${current_item}" "srchost"
	config_get _dsthost "${current_item}" "dsthost"
	config_get _proto "${current_item}" "proto"
	config_get _ports "${current_item}" "ports"
	config_get _portrange "${current_item}" "portrange"
	config_get _layer7 "${current_item}" "layer7"
	config_get _ipp2p "${current_item}" "ipp2p"
	ADVANCED_FIELD_FORM=""
	equal "$FORM_webif_advanced" "1" && {
		# config_get returns TYPE if OPTION ($2) is empty, else returns value
		#config_get _type "${current_item}"
		config_get _mark "${current_item}" "mark"
		config_get _tcpflags "${current_item}" "tcpflags"
		config_get _pktsize "${current_item}" "pktsize"
		ADVANCED_FIELD_FORM1=""
		#ADVANCED_FIELD_FORM1="
		#	field|@TR<<Rule Type>>
		#	select|current_type|$_type
		#	option|classify|@TR<<Classify>>
		#	option|default|@TR<<Default>>
		#	option|reclassify|@TR<<Reclassify>>
		#"
		ADVANCED_FIELD_FORM2="
			field|@TR<<TCP Flags>>
			text|current_tcpflags|$_tcpflags
			field|@TR<<Mark>>
			text|current_mark|$_mark
			field|@TR<<Packet Size>>
			text|current_mark|$_pktsize"
	}

	display_form <<EOF
	start_form|@TR<<QoS Rule Edit>>
	field|@TR<<Rules Index>>|rule_number|hidden
	text|current_rule_index|$current_item|hidden
	$ADVANCED_FIELD_FORM1
	field|@TR<<Classify As>>|current_target
	select|current_target|$_target
	option|Bulk|@TR<<Bulk>>
	option|Normal|@TR<<Normal>>
	option|Priority|@TR<<Priority>>
	option|Express|@TR<<Express>>
	field|@TR<<Source IP>>|current_srchost
	text|current_srchost|$_srchost
	field|@TR<<Dest IP>>|current_dsthost
	text|current_dsthost|$_dsthost
	field|@TR<<Protocol>>|proto
	select|current_proto|$_proto
	option||@TR<<qos_prot_any#Any>>
	option|tcp|@TR<<qos_prot_tcp#TCP>>
	option|udp|@TR<<qos_prot_udp#UDP>>
	option|icmp|@TR<<qos_prot_icmp#ICMP>>
	$ADVANCED_FIELD_FORM2
	field|@TR<<Ports>>|current_ports
	text|current_ports|$_ports
	field|@TR<<Port Range>>|current_portrange
	text|current_portrange|$_portrange
	field|@TR<<Layer7>>|current_layer7
	select|current_layer7|$_layer7
	$l7_protocols
	field|@TR<<Peer-2-Peer>>|ipp2p
	select|current_ipp2p|$_ipp2p
	option||@TR<<qos_p2p_none#None>>
	option|all|@TR<<qos_p2p_all#All>>
	option|bit|@TR<<qos_p2p_bittorrent#bitTorrent>>
	option|dc|@TR<<qos_p2p_dc#DirectConnect>>
	option|edk|@TR<<qos_p2p_ed2k#eDonkey>>
	option|gnu|@TR<<qos_p2p_gnutella#Gnutella>>
	option|kazaa|@TR<<qos_p2p_kazaa#Kazaa>>
	helpitem|QoS Rule Edit
	helptext|HelPText qos_rule_edit_help#You need only set fields you wish to match traffic on. Leave the others blank.
	helpitem|Layer-7
	helptext|HelpText layer7_help#Layer-7 filters are used to identify types of traffic based on content inspection. Numerous layer-7 filters are available on the web, though not all are efficient and accurate. To install more filters, download them and put them in /etc/l7-protocols.
	helpitem|Peer-2-Peer
	helptext|HelpText p2p_help#The difference between the Peer-2-Peer field and layer-7 filters is simply that the Peer-2-Peer option uses a special tool, ipp2p, to match traffic of common p2p protocols. It is typically more efficient than layer-7 filters.
	end_form
EOF
}
#########################################################################################
# else if qos-scripts NOT installed
else
	echo "<div class=\"warning\">@TR<<qos_no_compatible#A compatible QOS package was not found to be installed.>></div>"
display_form <<EOF
onchange|modechange
start_form|@TR<<QoS Packages>>
field|@TR<<qos_nbds#Nbd's QoS Scripts (recommended)>>|nbd_qos
submit|install_nbd|@TR<<qos_nbds_install#Install>>
end_form
EOF
fi

footer ?>
<!--
##WEBIF:name:Network:600:QoS
-->
