#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh

config_cb() {
	local cfg_type="$1"
	local cfg_name="$2"

	case "$cfg_type" in
		chilli)
			chilli_cfg="$cfg_name"
		;;
		chillispot)
			chillispot_cfg="$cfg_name"
		;;
	esac
}

[ -n "$FORM_install_package" ] && unset FORM_submit

uci_load "hotspot"

if empty "$FORM_submit"; then
	eval "FORM_service_enable=\"\$CONFIG_${chillispot_cfg}_enable\""
	FORM_service_enable="${FORM_service_enable:-0}"
	eval "FORM_chilli_debug=\"\$CONFIG_${chilli_cfg}_debug\""
	eval "FORM_chilli_net=\"\$CONFIG_${chilli_cfg}_net\""
	eval "FORM_chilli_dns1=\"\$CONFIG_${chilli_cfg}_dns1\""
	eval "FORM_chilli_dns2=\"\$CONFIG_${chilli_cfg}_dns2\""
	eval "FORM_chilli_dhcpif=\"\$CONFIG_${chilli_cfg}_dhcpif\""
	eval "FORM_chilli_dhcpmac=\"\$CONFIG_${chilli_cfg}_dhcpmac\""
	eval "FORM_chilli_lease=\"\$CONFIG_${chilli_cfg}_lease\""
	eval "FORM_chilli_pidfile=\"\$CONFIG_${chilli_cfg}_pidfile\""
	eval "FORM_chilli_interval=\"\$CONFIG_${chilli_cfg}_interval\""
	eval "FORM_chilli_domain=\"\$CONFIG_${chilli_cfg}_domain\""
	eval "FORM_chilli_dynip=\"\$CONFIG_${chilli_cfg}_dynip\""
	eval "FORM_chilli_statip=\"\$CONFIG_${chilli_cfg}_statip\""
else
	SAVED=1
	validate <<EOF
int|FORM_service_enable|@TR<<hotspot_core_Service#Service>>||$FORM_service_enable
int|FORM_chilli_debug|@TR<<hotspot_core_Debug#Debug>>||$FORM_chilli_debug
string|FORM_chilli_net|@TR<<hotspot_core_DHCP_Network#DHCP Network>>||$FORM_chilli_net
string|FORM_chilli_dhcpif|@TR<<hotspot_core_DHCP_Interface#DHCP Interface>>||$FORM_chilli_dhcpif
string|FORM_chilli_dhcpmac|@TR<<hotspot_core_DHCP_MAC#DHCP MAC>>||$FORM_chilli_dhcpmac
string|FORM_chilli_lease|@TR<<hotspot_core_DHCP_Lease#DHCP Lease>>||$FORM_chilli_lease
string|FORM_chilli_dns1|hotspot_core_DNS1#DNS1||$FORM_chilli_dns1
string|FORM_chilli_dns2|hotspot_core_DNS2#DNS2||$FORM_chilli_dns2
string|FORM_chilli_domain|@TR<<hotspot_core_Domain#Domain>>||$FORM_chilli_domain
string|FORM_chilli_interval|@TR<<hotspot_core_Interval#Interval||$FORM_chilli_interval
string|FORM_chilli_pidfile|@TR<<hotspot_core_Pidfile#Pidfile>>||$FORM_chilli_pidfile
string|FORM_chilli_dynip|@TR<<hotspot_core_Dynamic_IP_Pool#Dynamic IP Pool||$FORM_chilli_dynip
string|FORM_chilli_statip|@TR<<hotspot_core_Static_IP_Pool#Static IP Pool>>||$FORM_chilli_statip
EOF
	equal "$?" 0 && {
		uci_reload=0
		[ "$chillispot_cfg" = "" ] && {
			uci_add hotspot chillispot
			uci_reload=1
		}
		[ "$chilli_cfg" = "" ] && {
			uci_add hotspot chilli
			uci_reload=1
		}
		[ 1 -eq "$uci_reload" ] && uci_load "hotspot"
		uci_set hotspot "$chillispot_cfg" enable "$FORM_service_enable"
		uci_set hotspot "$chilli_cfg" debug "$FORM_chilli_debug"
		uci_set hotspot "$chilli_cfg" dns1 "$FORM_chilli_dns1"
		uci_set hotspot "$chilli_cfg" dns2 "$FORM_chilli_dns2"
		uci_set hotspot "$chilli_cfg" lease "$FORM_chilli_lease"
		uci_set hotspot "$chilli_cfg" interval "$FORM_chilli_interval"
		uci_set hotspot "$chilli_cfg" domain "$FORM_chilli_domain"
		uci_set hotspot "$chilli_cfg" pidfile "$FORM_chilli_pidfile"
		uci_set hotspot "$chilli_cfg" statip "$FORM_chilli_statip"
		uci_set hotspot "$chilli_cfg" dynip "$FORM_chilli_dynip"
		uci_set hotspot "$chilli_cfg" dhcpif "$FORM_chilli_dhcpif"
		uci_set hotspot "$chilli_cfg" dhcpmac "$FORM_chilli_dhcpmac"
	}
fi

header "HotSpot" "hotspot_core_Core#Core" "@TR<<hotspot_core_Core_Settings#Core Settings>>" '' "$SCRIPT_NAME"

if ! empty "$FORM_install_package"; then
	echo "@TR<<hotspot_common_Installing_package#Installing chillispot package>>...<pre>"
	install_package "chillispot"
	echo "</pre>"
fi

display_form <<EOF
start_form|@TR<<hotspot_core_Core_Settings#Core Settings>>
EOF

is_package_installed chillispot
equal "$?" "0" && chillispot_installed="1"

if [ "$chillispot_installed" = "1" ]; then
	display_form <<EOF
field|
string|<div class=warning>@TR<<hotspot_common_package_required#HotSpot will not work until you install ChilliSpot>>:</div>
submit|install_package| @TR<<hotspot_common_install_package#Install ChilliSpot Package>> |
helplink|http://www.chillispot.org/
end_form
EOF
else
	display_form <<EOF
field|@TR<<hotspot_core_Service#Service>>
select|service_enable|$FORM_service_enable
option|0|@TR<<hotspot_core_Disable#Disable>>
option|1|@TR<<hotspot_core_Enable#Enable>>

field|@TR<<hotspot_core_Debug#Debug>>
checkbox|chilli_debug|$FORM_chilli_debug|1
helpitem|hotspot_core_Debug#Debug
helptext|hotspot_core_Debug_helptext#Enable/Disable debugging.
field|@TR<<hotspot_core_DHCP_Network#DHCP Network>>
text|chilli_net|$FORM_chilli_net
helpitem|hotspot_core_DHCP_Network#DHCP Network
helptext|hotspot_core_DHCP_Network_helptext#Client's DHCP Network IP Subnet (192.168.182.0/24 by default).
field|@TR<<hotspot_core_DHCP_Interface#DHCP Interface>>
text|chilli_dhcpif|$FORM_chilli_dhcpif
field|@TR<<hotspot_core_DHCP_MAC#DHCP MAC>>
text|chilli_dhcpmac|$FORM_chilli_dhcpmac
field|@TR<<hotspot_core_DHCP_Lease#DHCP Lease>>
text|chilli_lease|$FORM_chilli_lease
helpitem|hotspot_core_DHCP_Lease#DHCP Lease
helptext|hotspot_core_DHCP_Lease_helptext#DHCP Lease time for clients before expires (default is 600).
field|@TR<<hotspot_core_DNS1#DNS1>>
text|chilli_dns1|$FORM_chilli_dns1
field|@TR<<hotspot_core_DNS2#DNS2>>
text|chilli_dns2|$FORM_chilli_dns2
field|@TR<<hotspot_core_Domain#Domain>>
text|chilli_domain|$FORM_chilli_domain
helpitem|hotspot_core_DHCP_DNS#DHCP DNS
helptext|hotspot_core_DHCP_DNS_helptext#DNS Servers offered to clients (if omitted system default will be used).
field|@TR<<hotspot_core_Interval#Interval>>
text|chilli_interval|$FORM_chilli_interval
field|@TR<<hotspot_core_Pidfile#Pidfile>>
text|chilli_pidfile|$FORM_chilli_pidfile
helpitem|hotspot_core_Pidfile#Pidfile
helptext|hotspot_core_Pidfile_helptext#File to store information about the process id.
field|@TR<<hotspot_core_Dynamic_IP_Pool#Dynamic IP Pool>>
text|chilli_dynip|$FORM_chilli_dynip
helpitem|hotspot_core_Dynamic_IP_Pool#Dynamic IP Pool
helptext|hotspot_core_Dynamic_IP_Pool_helptext#Allocation of dynamic IP Addresses to clients.
field|@TR<<hotspot_core_Static_IP_Pool#Static IP Pool>>
text|chilli_statip|$FORM_chilli_statip
helpitem|hotspot_core_Static_IP_Pool#Static IP Pool
helptext|hotspot_core_Static_IP_Pool_helptext#Allocation of static IP Addresses.
helplink|http://www.chillispot.org/
end_form
EOF
fi
footer ?>
<!--
##WEBIF:name:HotSpot:1:hotspot_core_Core#Core
-->
