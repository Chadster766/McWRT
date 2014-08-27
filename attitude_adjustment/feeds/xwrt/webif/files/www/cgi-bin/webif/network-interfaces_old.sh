#!/usr/bin/webif-page
<?
. "/usr/lib/webif/webif.sh"
###################################################################
# VLAN configuration page
#
# Description:
#	Configures any number of VLANs.
#
# Author(s) [in order of work date]:
#	Jeremy Collake <jeremy.collake@gmail.com>
#	pier11<pier11@operamail.com> port to UCI
#
# Major revisions:
#
# NVRAM variables referenced:
#	none
#
# Configuration files referenced:
#   network
#

config_cb() {
	cfg_type="$1"
	cfg_name="$2"

	case "$cfg_type" in
	        interface)
		        append networks "$cfg_name" "$N"
	        ;;
	        switch)
		        append switch_interfaces "$cfg_name"
	        ;;
	esac
}
option_cb() {
	local var_name="$1"; shift
	local var_value="$*"

	[ "$var_name" = "ifname" ] && [ "$cfg_type" = "interface" ] && eval "ifnames_${cfg_name}=\"$var_value\""
}

#Load settings from the network config file.	
uci_load "network"
networks=$(echo "$networks" |uniq)
#Create network options list
for network in $networks; do
	append network_options "option|$network" "$N"
done


header "Network" "Interfaces" "@TR<<Interfaces>>" '' "$SCRIPT_NAME"

if [ "$switch_interfaces" != "" ]; then
	###################################################################
	# toggles and default settings
	#
	ALLOW_VLAN_NUMBERING_GAPS=0 # toggle alowance of gaps, i.e. vlan0, vlan1, vlan5 are defined
					# note: allowing gaps makes for a much slower loading page since
					#  we have to search through MAX_VLANS instead of stopping at first
					#  unset vlan variable.
	PORT_BASE=0		# base number of the ports
	PORT_COUNT=6		# number of ports (todo: should determine dynamically)
	MAX_PORT=5		# maximum port number (todo: should determine dynamically)
	MAX_VLANS=16		# limit the switch can handle on the bcm947xx (todo: dynamically determine)
	MAX_VLANS_INDEX=15	# like MAX_VLANS, except starts at 0 instead of 1
	#for later use
	HELP_TEXT=
	
	###################################################################
	# CountNumberOfVLANsThatContainPortX ( ### )
	#
	# used to test if a port is in another vlan, so we know if we should
	# tag it or not. Returns count in RETURN_VAR.
	#
	# stops when it encounters more than 1...
	#
	CountNumberOfVLANsThatContainPortX ( )
	{
		RETURN_VAR=0
		for count2 in $(seq "0" "$MAX_VLANS_INDEX"); do
			eval current_vlan_value2=\$CONFIG_eth0_vlan${count2}
			if [ -z "$current_vlan_value2" ]; then
				break
			fi
			eval value="\"\$FORM_vlan_${count2}_port_${1}\""
			equal "$value" "1" &&
			{
				let "RETURN_VAR+=1"
				equal "$RETURN_VAR" "2" && break
			}
		done
	}

	###################################################################
	# save settings or handle input
	#
	if ! empty "$FORM_submit"; then
		SAVED=1

		#
		# handle add or remove
		#
		for count in $(seq 0 $MAX_VLANS_INDEX); do
			eval current_vlan_value=\$CONFIG_eth0_vlan"$count"
			if [ -z "$current_vlan_value" ]; then
				let "count-=1"
				break
			fi
		done

		#
		# now add or remove if appropriate. In WR we used vlanXhwname variable
		#  as indication of the existance of the vlan, to allow for
		#  empty vlans.
		#

		! empty "$FORM_remove_vlan" &&
		{
			uci_remove "network" "eth0" "vlan${count}"
			let "count-=1"
		}
		highest_vlan=$count

		#
		# save VLAN configuration (also do add or remove)
		#
		for count in $(seq 0 $highest_vlan); do
			current_vlan_opt_name=vlan"$count"
			current_vlan_ports=""
			for port_counter in $(seq $PORT_BASE $MAX_PORT); do

				eval value="\"\$FORM_vlan_${count}_port_${port_counter}\""
				if [ "$value" = "1" ]; then
					if empty "$current_vlan_ports" ; then
						current_vlan_ports="$port_counter"
					else
						current_vlan_ports="$current_vlan_ports $port_counter"
					fi

					#
					# does port exist in alternate VLANs?
					#
					CountNumberOfVLANsThatContainPortX "$port_counter"
					equal "$RETURN_VAR" "1" ||
					{
						current_vlan_ports="$current_vlan_ports*"
					}
				fi
			done
			uci_set "network" "eth0" "$current_vlan_opt_name" "$current_vlan_ports"
			eval FORM_network_vlan="\$FORM_network_vlan$count"
			for network in $networks; do
				changed_ifname=0
				interfaces=""
				eval network_ifnames="\$ifname_$network"
				echo $network_ifnames |grep -q "eth0.$count"
				if [ "$?" = "0" ]; then
					if [ "$FORM_network_vlan" != "$network" ]; then
						for interface in $network_ifnames; do
							if [ "$interface" != "eth0.$count" ]; then
								#prevent lots of spaces from being added to the config file
								if [ "$interfaces" != "" -a "$interface" != "" ]; then
									interfaces="$interfaces $interface"
									changed_ifname=1
								elif [ "$interfaces" = "" -a "$interface" != "" ]; then
									interfaces="$interface"
									changed_ifname=1
								fi
							fi
							changed_ifname=1
						done
					fi
				else
					if [ "$FORM_network_vlan" = "$network" ]; then
						#prevent lots of spaces from being added to the config file
						if [ "$network_ifnames" != "" ]; then
							interfaces="$network_ifnames eth0.$count"
							changed_ifname=1
						else
							interfaces="eth0.$count"
							changed_ifname=1
						fi
						export ifname_$network="$interfaces"
					fi
				fi
				if [ "$changed_ifname" = "1" ]; then
					uci_set "network" "$network" "ifname" "$interfaces"
				fi
			done

		done

		! empty "$FORM_add_vlan" &&
		{
			let "count+=1"
			uci_set "network" "eth0" "vlan${count}" "$MAX_PORT"
		}

		uci_load "network"
	fi

	####################################################################
	# add headers for the port numbers
	#
	FORM_port_headers="string|<tr><th>&nbsp;</th>"
	for current_port in $(seq $PORT_BASE $MAX_PORT); do
		FORM_port_headers="${FORM_port_headers}<th>$current_port</th>"
	done
	FORM_port_headers="${FORM_port_headers}<td>Network</td></tr>"

	####################################################################
	# now create the vlan rows, one for each set vlan variable, even
	#  if empty.
	#
	FORM_all_vlans="$FORM_port_headers"		# holds VLAN webif form we build
	for count in $(seq "0" "$MAX_VLANS_INDEX"); do
		vlanport="CONFIG_eth0_vlan${count}"
		FORM_current_vlan="string|<tr><th>VLAN $count&nbsp;&nbsp;</th>"
		#
		# for each port, create a checkbox and mark if
		#  port for in vlan
		#
		
		#TODO: revisit for Kamikaze
		#FORM_log_ipaddr=${log_ipaddr:-$(nvram get log_ipaddr)}
		eval ports="\$$vlanport"
		if [ -z "$ports" ]; then
			if [ $ALLOW_VLAN_NUMBERING_GAPS = 1 ]; then
				continue		# to allow vlan # gaps
			else
				break			# to disallow vlan # gaps
			fi
		fi
		for current_port in $(seq $PORT_BASE $MAX_PORT); do
			# if port in vlan, mark checkbox
			port_included=0
			# see if saved but uncommitted/applied or already set in form
			eval value="\"\$FORM_vlan_${count}_port_${current_port}\""
			eval value2="\"\$vlan_${count}_port_${current_port}\""
			# set if committed
			echo "$ports" | grep "$current_port" >> "/dev/null"  2>&1
			if equal "$?" "0" || equal "$value" "1" || equal "$value2" "1" ; then
				port_included=1
			fi
			variable_name="vlan_${count}_port_${current_port}"
			checkbox_string="checkbox|$variable_name|$port_included|1|&nbsp;"
			FORM_current_vlan="$FORM_current_vlan
				string|<td>
				$checkbox_string
				string|</td>"
		done

		eval FORM_network_vlan="\$FORM_network_vlan$count"
		if [ "$FORM_network_vlan" = "" ]; then
			for network in $networks; do
				config_get type $network type
				if [ "$type" = "bridge" ]; then
					#Use device to get the ifname of a bridged connection
					config_get network_ifname $network device
				else
					eval network_ifname="\$ifnames_$network"
				fi
				echo $network_ifname |grep -q "eth0.$count"
				if [ "$?" = "0" ]; then
					FORM_network_vlan="$network"
				fi
			done
		fi

		FORM_all_vlans="$FORM_all_vlans
			$FORM_current_vlan
			string|<td>
			select|network_vlan$count|$FORM_network_vlan
			option|none|@TR<<None>>
			$network_options
			string|</td>
			string|</tr>"
	done
	vlan_forms="start_form|@TR<<VLAN Configuration>>
			helpitem|VLAN
			helptext|Helptext VLAN#A virtual LAN is a set of ports that are bridged. In cases where a port belongs to more than one VLAN, a technique known as tagging is used to identify to which VLAN traffic on that port belongs.
			$FORM_all_vlans
			end_form
			start_form|
			submit|add_vlan|@TR<<Add New VLAN>>
			submit|remove_vlan|@TR<<Remove Last VLAN>>
			end_form"
fi

system_ifaces="$(ifconfig -a|grep eth |cut -d" " -f1)"
for iface in $system_ifaces; do
	for vlan_iface in $switch_interfaces; do
		switching_iface=0
		echo "$iface" |grep -q "$vlan_iface"
		[ "$?" = "0" ] && switching_iface=1 ; break
	done
	if [ "$switching_iface" != "1" ]; then
		eval FORM_network_iface="\$FORM_network_$iface"
		if [ "$FORM_network_iface" = "" ]; then
			for network in $networks; do
				config_get type $network type
				if [ "$type" = "bridge" ]; then
					#Use device to get the ifname of a bridged connection
					config_get network_ifname $network device
				else
					eval network_ifname="\$ifnames_$network"
				fi
				echo $network_ifname |grep -q "$iface"
				if [ "$?" = "0" ]; then
					FORM_network_iface="$network"
				fi
			done
		fi
		if [ "$FORM_submit" != "" ]; then
			for network in $networks; do
				changed_ifname=0
				interfaces=""
				config_get type $network type
				if [ "$type" = "bridge" ]; then
					#Use device to get the ifname of a bridged connection
					config_get network_ifname $network device
				else
					eval network_ifname="\$ifnames_$network"
				fi
				echo $network_ifnames |grep -q "$iface"
				if [ "$?" = "0" ]; then
					if [ "$FORM_network_iface" != "$network" ]; then
						for interface in $network_ifnames; do
							if [ "$interface" != "$iface" ]; then
								#prevent lots of spaces from being added to the config file
								if [ "$interfaces" != "" -a "$interface" != "" ]; then
									interfaces="$interfaces $interface"
									changed_ifname=1
								elif [ "$interfaces" = "" -a "$interface" != "" ]; then
									interfaces="$interface"
									changed_ifname=1
								elif [ "$interfaces" = "" -a "$interface" = "" ]; then
									changed_ifname=1
								fi
							fi
							changed_ifname=1
						done
					fi
				else
					if [ "$FORM_network_iface" = "$network" ]; then
						#prevent lots of spaces from being added to the config file
						if [ "$network_ifnames" != "" ]; then
							interfaces="$network_ifnames $iface"
							changed_ifname=1
						else
							interfaces="$iface"
							changed_ifname=1
						fi
					fi
				fi
				if [ "$changed_ifname" = "1" ]; then
					uci_set "network" "$network" "ifname" "$interfaces"
				fi
			done
		fi
		interface_fields="
		field|@TR<<Interface>> $iface
		select|network_$iface|$FORM_network_iface
		option|none|@TR<<None>>
		$network_options"
		append interface_form "$interface_fields" "$N"
	fi
done
if [ "$interface_form" != "" ]; then
	append interface_forms "start_form|@TR<<Interface Configuration>>" "$N"
	append interface_forms "$interface_form" "$N"
	append interface_forms "end_form" "$N"
fi

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
######WEBIF:name:Network:249:Interfaces
-->
