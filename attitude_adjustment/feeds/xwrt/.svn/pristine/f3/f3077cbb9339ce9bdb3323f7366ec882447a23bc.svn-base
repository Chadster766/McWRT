#!/usr/bin/webif-page
<?
# Adopted from vpn-openvpn.sh
# July 2007 - Authored by Liran Tal <liran@enginx.com>

. /usr/lib/webif/webif.sh

config_cb() {
	local cfg_type="$1"
	local cfg_name="$2"

	case "$cfg_type" in
		server) server_cfg="$cfg_name" ;;
	esac
}

uci_load "l2tpns"

if ! empty "$FORM_install_package"; then
	echo "@TR<<l2tpns_Installing_package#Installing l2tpns package ...>><pre>"
	install_package "l2tpns"
	echo "</pre>"
fi

install_package_button=""
! is_package_installed "l2tpns" &&
	install_package_button="string|<div class=\"warning\">@TR<<l2tpns_warn#VPN will not work until you install L2TPns:>> </div>
		submit|install_package| @TR<<l2tpns_install_package#Install L2TPns Package>> |"

if empty "$FORM_submit"; then
	config_get_bool FORM_server_mode "$server_cfg" mode 0
	eval "FORM_server_debug=\"\$CONFIG_${server_cfg}_debug\""
	eval "FORM_server_pid_file=\"\$CONFIG_${server_cfg}_pid_file\"" 
	eval "FORM_server_log_file=\"\$CONFIG_${server_cfg}_log_file\""
	eval "FORM_server_bind_address=\"\$CONFIG_${server_cfg}_bind_address\""
	eval "FORM_server_primary_dns=\"\$CONFIG_${server_cfg}_primary_dns\""
	eval "FORM_server_secondary_dns=\"\$CONFIG_${server_cfg}_secondary_dns\""
	eval "FORM_server_primary_radius=\"\$CONFIG_${server_cfg}_primary_radius\"" 
	eval "FORM_server_primary_radius_port=\"\$CONFIG_${server_cfg}_primary_radius_port\"" 
	eval "FORM_server_secondary_radius=\"\$CONFIG_${server_cfg}_secondary_radius\"" 
	eval "FORM_server_secondary_radius_port=\"\$CONFIG_${server_cfg}_secondary_radius_port\"" 
	eval "FORM_server_radius_accounting=\"\$CONFIG_${server_cfg}_radius_accounting\""
	eval "FORM_server_radius_secret=\"\$CONFIG_${server_cfg}_radius_secret\"" 
else
	SAVED=1
	validate <<EOF
int|FORM_server_mode|@TR<<l2tpns_Start_L2TPns_Connection#Start L2TPns Connection>>||$FORM_server_mode
int|FORM_server_debug|@TR<<l2tpns_Debug_Level#Debug Level>>||$FORM_server_debug
string|FORM_server_pid_file|@TR<<l2tpns_Pid_File#Pid File>>||$FORM_server_pid_file
string|FORM_server_log_file|@TR<<l2tpns_Log_File#Log File>>||$FORM_server_log_file
ip|FORM_server_bind_address|@TR<<l2tpns_Bind_Address#Bind Address>>||$FORM_server_bind_address
ip|FORM_server_primary_dns|@TR<<l2tpns_Primary_DNS#Primary DNS>>||$FORM_server_primary_dns
ip|FORM_server_secondary_dns|@TR<<l2tpns_Secondary_DNS#Secondary DNS>>||$FORM_server_secondary_dns
ip|FORM_server_primary_radius|@TR<<l2tpns_Primary_RADIUS#Primary RADIUS>>||$FORM_server_primary_radius
port|FORM_server_primary_radius_port|@TR<<l2tpns_Primary_RADIUS_Port#Primary RADIUS Port>>||$FORM_server_primary_radius_port
ip|FORM_server_secondary_radius|@TR<<l2tpns_Secondary_RADIUS#Secondary RADIUS>>||$FORM_server_secondary_radius
port|FORM_server_secondary_radius_port|@TR<<l2tpns_Secondary_RADIUS_Port#Secondary RADIUS Port>>||$FORM_server_secondary_radius_port
string|FORM_server_radius_accounting|@TR<<l2tpns_RADIUS_Accounting#RADIUS Accounting>>||$FORM_server_radius_accounting
string|FORM_server_radius_secret|@TR<<l2tpns_RADIUS_Secret#RADIUS Secret>>||$FORM_server_radius_secret
EOF
	equal "$?" 0 && {
		[ -e "/etc/config/l2tpns" ] || touch "/etc/config/l2tpns"
		[ "$server_cfg" = "" ] && {
			uci_add "l2tpns" "server"
			uci_load "l2tpns"
		}
		uci_set "l2tpns" "$server_cfg" "mode" "$FORM_server_mode"
		uci_set "l2tpns" "$server_cfg" "debug" "$FORM_server_debug"
		uci_set "l2tpns" "$server_cfg" "pid_file" "$FORM_server_pid_file"
		uci_set "l2tpns" "$server_cfg" "log_file" "$FORM_server_log_file"
		uci_set "l2tpns" "$server_cfg" "bind_address" "$FORM_server_bind_address"
		uci_set "l2tpns" "$server_cfg" "primary_dns" "$FORM_server_primary_dns"
		uci_set "l2tpns" "$server_cfg" "secondary_dns" "$FORM_server_secondary_dns"
		uci_set "l2tpns" "$server_cfg" "primary_radius" "$FORM_server_primary_radius"
		uci_set "l2tpns" "$server_cfg" "primary_radius_port" "$FORM_server_primary_radius_port"
		uci_set "l2tpns" "$server_cfg" "secondary_radius" "$FORM_server_secondary_radius"
		uci_set "l2tpns" "$server_cfg" "secondary_radius_port" "$FORM_server_secondary_radius_port"
		uci_set "l2tpns" "$server_cfg" "radius_accounting" "$FORM_server_radius_accounting"
		uci_set "l2tpns" "$server_cfg" "radius_secret" "$FORM_server_radius_secret"
	}
fi

header "VPN" "L2TPns" "@TR<<L2TPns>>" 'onload="modechange()"' "$SCRIPT_NAME"

cat <<EOF
<script type="text/javascript" src="/webif.js "></script>
<script type="text/javascript">
<!--
function modechange()
{
	var v;
	v = isset('server_mode', '1');
	set_visible('connection_settings', v);

	hide('save');
	show('save');
}
-->
</script>
EOF


display_form <<EOF
onchange|modechange
$install_package_button
start_form|@TR<<l2tpns_L2TPns#L2TPns>>
field|@TR<<l2tpns_Start_L2TPns_Connection#Start L2TPns Connection>>
select|server_mode|$FORM_server_mode
option|0|@TR<<l2tpns_Disabled#Disabled>>
option|1|@TR<<l2tpns_Enabled#Enabled>>
end_form

start_form|@TR<<l2tpns_Connection_Settings#Connection Settings>>|connection_settings|hidden
field|@TR<<l2tpns_Bind_Address#Bind Address>>
text|server_bind_address|$FORM_server_bind_address
helpitem|l2tpns_Bind_Address#Bind Address
helptext|l2tpns_Bind_Address_helptext#The IP Address on which the L2TPns server will be listening on.

field|@TR<<l2tpns_Primary_DNS#Primary DNS>>
text|server_primary_dns|$FORM_server_primary_dns
field|@TR<<l2tpns_Secondary_DNS#Secondary DNS>>
text|server_secondary_dns|$FORM_server_secondary_dns
helpitem|l2tpns_DNS_Addresses#DNS Addresses
helptext|l2tpns_DNS_Addresses_helptext#DNS Servers upon which clients will be provided with.

field|@TR<<l2tpns_Primary_RADIUS#Primary RADIUS>>
text|server_primary_radius|$FORM_server_primary_radius
field|@TR<<l2tpns_Primary_RADIUS_Port#Primary RADIUS Port>>
text|server_primary_radius_port|$FORM_server_primary_radius_port
field|@TR<<l2tpns_Secondary_RADIUS#Secondary RADIUS>>
text|server_secondary_radius|$FORM_server_secondary_radius
field|@TR<<l2tpns_Secondary_RADIUS_Port#Secondary RADIUS Port>>
text|server_secondary_radius_port|$FORM_server_secondary_radius_port
helpitem|l2tpns_RADIUS_Servers#RADIUS Servers
helptext|l2tpns_RADIUS_Servers_helptext#RADIUS Servers IP Addresses.
helpitem|l2tpns_RADIUS_Ports#RADIUS Ports
helptext|l2tpns_RADIUS_Ports_helptext#RADIUS Servers Ports for authentication.

field|@TR<<l2tpns_RADIUS_Secret#RADIUS Secret>>
text|server_radius_secret|$FORM_server_radius_secret
helpitem|l2tpns_RADIUS_Secret#RADIUS Secret
helptext|l2tpns_RADIUS_Secret_helptext#RADIUS Servers shared secret key.

field|@TR<<l2tpns_RADIUS_Accounting#RADIUS Accounting>>
select|server_radius_accounting|$FORM_server_radius_accounting
option|no|@TR<<l2tpns_No#No>>
option|yes|@TR<<l2tpns_Yes#Yes>>

field|@TR<<l2tpns_Debug_Level#Debug Level>>
select|server_debug|$FORM_server_debug
option|0|0
option|1|1
option|2|2
option|3|3

field|@TR<<l2tpns_Log_File#Log File>>
text|server_log_file|$FORM_server_log_file

field|@TR<<l2tpns_Pid_File#Pid File>>
text|server_pid_file|$FORM_server_pid_file
end_form
EOF

footer
?>
<!--
##WEBIF:name:VPN:3:L2TPns
-->
