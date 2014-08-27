#!/usr/bin/webif-page
<?
. "/usr/lib/webif/webif.sh"
. "/usr/lib/webif/switchinfo.sh"

###################################################################
# VLAN configuration page
#
# Description:
#	Configures any number of VLANs.
#
# Author(s) [in order of work date]:
#	Fabian Omar Franzotti <fofware@gmail.com>
#
# Major revisions:
#
# NVRAM variables referenced:
#	none
#
# Configuration files referenced:
#   network
#
uci_get() {
	uci -P /var/state get "$1" 2>/dev/null
}

config_cb() {
	cfg_type="$1"
	cfg_name="$2"
	case "$cfg_type" in
	        switch)
		        append switch_interfaces "$cfg_name"
	        ;;
	        switch_vlan)
		        append vlan_interfaces "$cfg_name" "$N"
	        ;;
	esac
}

option_cb() {
	local var_name="$1"; shift
	local var_value="$*"
	[ "$var_name" = "ifname" ] && [ "$cfg_type" = "interface" ] && eval "ifnames_${cfg_name}=\"$var_value\""
}

cgi_process()
{
	uci_load "network"
	for vlans in $vlan_interfaces; do
		eval bt_rmv="\$FORM_remove_$vlans"
		if [ "$bt_rmv" != "" ]; then
			uci_remove "network" "$vlans"
		fi
	done
	for switch in $switch_interfaces; do
		switch_info $switch
		local bt_add
		eval bt_add="\$FORM_add_vlan_${switch}"
		if [ "$bt_add" != "" ]; then
			eval value="\$FORM_newvlan_${switch}"
			if [ $value -gt 0 ]; then
				varname="${switch}_${value}"
				config_get vlan "$varname" "vlan"
				if [ "$vlan" = "" ]; then
					uci_add "network" "switch_vlan" "$varname"
					uci_set "network" "$varname" "device" "$switch"
					uci_set "network" "$varname" "vlan" "$value"
					uci_load "network"
				fi
			fi
		fi
		if [ "$FORM_submit" != "" ]; then
			local enable 
			local enable_vlan 
			local vreset

			eval enable="\$FORM_enable_${switch}"
			eval enable_vlan="\$FORM_enable_vlan_${switch}"
			eval vreset="\$FORM_reset_${switch}"

			uci_set "network" "$switch" "enable" "$enable"
			uci_set "network" "$switch" "enable_vlan" "$enable_vlan"
			uci_set "network" "$switch" "reset" "$vreset"
			
			for vlans in $vlan_interfaces; do
				config_get vlan $vlans vlan
				config_get device $vlans device
				if [ "$device" = "$switch" ]; then
					ports=""
					for port in $(seq $SW_PORT_BASE $SW_PORT_MAX); do
						eval value="\"\$FORM_${switch}_${vlan}_port_${port}\""
						if [ "$value" != "" ]; then
							append ports "$port"
						fi
					done
					uci_set "network" "$vlans" ports "$ports"
				fi
			done
		fi
	done
	unset vlan_interfaces
	unset switch_interfaces
}

formVlans ()
{
	switch_data_form=""
	if [ "$SW_PORT_MAX" = "0" ]; then
		return
	fi
	switch=$1
	vlan_headers="string|<tr><th>&nbsp;</th>"
	for current_port in $(seq $SW_PORT_BASE $SW_PORT_MAX); do
		vlan_headers="${vlan_headers}<th>$current_port</th>"
	done
	vlan_headers="${vlan_headers}<td>&nbsp;</td></tr>"
	for vlans in $vlan_interfaces; do
		config_get ports $vlans ports
		config_get vlan $vlans vlan
		config_get device $vlans device
		if [ "$device" = "$switch" ]; then
			vlan_data="string|<tr><th>VLAN $vlan&nbsp;&nbsp;($device.$vlan)</th>"
			for current_port in $(seq $SW_PORT_BASE $SW_PORT_MAX); do
				port_included=0
				variable_name="${switch}_${vlan}_port_${current_port}"
				if [ "$FORM_submit" = "" ] ; then
					echo "$ports" | grep "$current_port" >> "/dev/null"  2>&1
					if equal "$?" "0" ; then
						port_included=1
					fi
				else
					eval port_included="\$FORM_$variable_name"
				fi
				checkbox_string="checkbox|$variable_name|$port_included|1|&nbsp;"
				vlan_data="$vlan_data
				string|<td>	
				$checkbox_string
				string|</td>"
			done
			vlan_data="$vlan_data
			string|<td ><input TYPE='submit' name='remove_${device}_${vlan}' value='Delete It!' />
			string|</td>
			string|</tr>"
			append setted_ports "$vlan_data" "$N"
		fi
	done
	if [ "$setted_ports" != "" ]; then
		switch_data_form="$vlan_headers
		$setted_ports"
	fi
}

switchForm ()
{
	local FORM_enable
	local FORM_enable_vlan
	local FORM_reset
	
	local FORM_enable=$(uci_get "network.$SW_DEVICE.enable")
	local FORM_enable_vlan=$(uci_get "network.$SW_DEVICE.enable_vlan")
	local FORM_reset=$(uci_get "network.$SW_DEVICE.reset")

	FORM_enable=${FORM_enable:-0}
	FORM_enable_vlan=${FORM_enable_vlan:-0}
	FORM_reset=${FORM_reset:-0}

	switch_form=""
	append switch_form "start_form|@TR<<Interface>> $switch ($SW_DRIVER)"   "$N"
	append switch_form "
		field|@TR<<Enable this switch>>
		checkbox|enable_${SW_DEVICE}|$FORM_enable|1
		field|@TR<<Enable VLAN functionality>>
		checkbox|enable_vlan_${SW_DEVICE}|$FORM_enable_vlan|1
		field|@TR<<Reset switch during setup>>
		checkbox|reset_${SW_DEVICE}|$FORM_reset|1
		"
	if [ $SW_VLAN_MAX -gt 0 ]; then
		append switch_form "
			field|@TR<<VLAN Number>>
			text|newvlan_${SW_DEVICE}
			submit|add_vlan_${SW_DEVICE}|@TR<<Add>> @TR<<Number between>> ($SW_VLAN_BASE - $SW_VLAN_MAX)
		"
	fi
	append switch_form "end_form" "$N"
	if [ "$SW_PORT_MAX" = "0" ]; then
		return
	fi
	formVlans $SW_DEVICE
	if [ "$switch_data_form" != "" ]; then
		append switch_form "start_form|@TR<<Virtual Lans of>> $SW_DEVICE"   "$N"
		append switch_form "$switch_data_form" "$N"
		append switch_form "end_form" "$N"
	fi
}

header "Network" "Switch" "@TR<<Switches>>" '' "$SCRIPT_NAME"
cgi_process
uci_load "network"
for switch in $switch_interfaces; do
	vlans_available=""
	switch_info $switch
	switchForm $switch
	append interface_forms "$switch_form" "$N"
done

###################################################################
# show form
#
display_form <<EOF
onchange|modechange
$vlan_forms
$interface_forms
EOF

 footer ?>
<!--
##WEBIF:name:Network:250:Switch
-->
