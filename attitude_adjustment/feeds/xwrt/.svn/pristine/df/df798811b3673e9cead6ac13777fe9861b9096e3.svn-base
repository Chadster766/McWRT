#!/usr/bin/webif-page "-U /tmp -u 4096"
<?
# add haserl args in double quotes it has very ugly
# command line parsing code!

. /usr/lib/webif/webif.sh

config_cb() {
	local cfg_type="$1"
	local cfg_name="$2"

	case "$cfg_type" in
		webifopenvpn)
			append openvpnconfigs "$cfg_name" "$N"
		;;
	esac
}

#FIXME: uci_load bug
#uci_load will pass the same config twice when there is a section to be added by using uci_add before a uci_commit happens
#we will use uniq so we don't try to parse the same config section twice.
openvpnconfigs=$(echo "$openvpnconfigs" |uniq)

openvpncfg_number=$(echo "$openvpnconfigs" |wc -l)
let "openvpncfg_number+=1"

# Add Openvpn Section
if ! empty "$FORM_add_openvpncfg_number"; then
	[ -e /etc/config/webifopenvpn ] || touch /etc/config/webifopenvpn
	uci_add webifopenvpn webifopenvpn
	uci_set webifopenvpn "$CONFIG_SECTION" "mode" ""
	uci_set webifopenvpn "$CONFIG_SECTION" "enabled"  "1"
	uci_set webifopenvpn "$CONFIG_SECTION" "port" "1194"
	uci_set webifopenvpn "$CONFIG_SECTION" "auth" "cert"
	uci_set webifopenvpn "$CONFIG_SECTION" "proto" "udp"
	uci_set webifopenvpn "$CONFIG_SECTION" "complzo" "1"
	uci_set webifopenvpn "$CONFIG_SECTION" "ping" ""
	uci_set webifopenvpn "$CONFIG_SECTION" "pingrestart" ""
	uci_set webifopenvpn "$CONFIG_SECTION" "persisttun" ""
	uci_set webifopenvpn "$CONFIG_SECTION" "persistkey" ""
	uci_set webifopenvpn "$CONFIG_SECTION" "ipaddr" ""
	uci_set webifopenvpn "$CONFIG_SECTION" "client_to_client" "0"
	uci_set webifopenvpn "$CONFIG_SECTION" "cmdline" ""
	uci_set webifopenvpn "$CONFIG_SECTION" "local" ""
	uci_set webifopenvpn "$CONFIG_SECTION" "remote" ""
	uci_set webifopenvpn "$CONFIG_SECTION" "pull" "1"
	uci_set webifopenvpn "$CONFIG_SECTION" "verb" "1"
	#Create Config dir
	if [ -e  /etc/openvpn/webifopenvpn* ]; then
		number=$(ls -1d /etc/openvpn/webifopenvpn* | sed -e '$!d' -e 's/.*\([0-9]\+\)$/\1/')
		let "number+=1"
	else
		number="1"
	fi
	config_dir="/etc/openvpn/webifopenvpn$number"
	uci_set webifopenvpn "$CONFIG_SECTION" "dir" "$config_dir"
	mkdir -p "$config_dir"
	uci_load
fi

# Remove Openvpn Section
if ! empty "$FORM_remove_openvpncfg"; then
	uci_remove webifopenvpn "$FORM_remove_openvpncfg"
fi

uci_load "webifopenvpn"

header "VPN" "OpenVPN" "@TR<<OpenVPN>>" ' onload="modechange()" ' "$SCRIPT_NAME"

if ! empty "$FORM_install_package"; then
	echo "@TR<<vpn_openvpn_Installing_package#Installing openvpn package ...>><pre>"
	install_package "openvpn"
	echo "</pre>"
fi

install_package_button=""
! is_package_installed "openvpn" &&
	install_package_button="string|<div class=warning>@TR<<vpn_openvpn_warn#VPN will not work until you install OpenVPN:>> </div>
		submit|install_package| @TR<<vpn_openvpn_install_package#Install OpenVPN Package>> |"

for config in $openvpnconfigs; do
	if empty "$FORM_submit"; then
		config_get dir_name $config "dir"
		[ -s "$dir_name/certificate.p12" ] ||
			NOCERT=1
		[ -s "$dir_name/shared.key" ] ||
			NOPSK=1
		[ -s "$dir_name/ca.crt" ] ||
			NOROOTCACERT=1
		[ -s "$dir_name/client.crt" ] ||
			NOCLIENTCERT=1
		[ -s "$dir_name/client.key" ] ||
			NOCLIENTKEY=1
		[ -s "$dir_name/dh.pem" ] ||
			NODH=1
		[ -s "$dir_name/tlsauth.key" ] ||
			NOTLSAUTH=1

		# general settings
		config_get FORM_ovpn_mode $config "mode"
		config_get FORM_ovpn_enabled $config "enabled"
		config_get FORM_ovpn_port $config "port"
		config_get FORM_ovpn_auth $config "auth"
		config_get FORM_ovpn_proto $config "proto"
		config_get FORM_ovpn_complzo $config "complzo"
		config_get FORM_ovpn_ping $config "ping"
		config_get FORM_ovpn_pingrestart $config "pingrestart"
		config_get FORM_ovpn_persisttun $config "persisttun"
		config_get FORM_ovpn_persistkey $config "persistkey"
		config_get FORM_ovpn_ipaddr $config "ipaddr"
		config_get FORM_ovpn_client_to_client $config "client_to_client"
		config_get FORM_ovpn_cmdline $config "cmdline"
		config_get FORM_ovpn_local $config "local"
		config_get FORM_ovpn_remote  $config "remote"
		config_get FORM_ovpn_pull $config "pull"
		config_get FORM_ovpn_verb $config "verb"
		[ -z "$FORM_ovpn_verb" ] && FORM_ovpn_verb=1

	else
		config_get dir_name $config "dir"
		#PKCS12
		[ -s "$FORM_openvpn_pkcs12file" ] && {
			cp "$FORM_openvpn_pkcs12file" "$dir_name/certificate.p12" &&
				UPLOAD_CERT=1
		}
		#PreShared Key
		[ -s "$FORM_openvpn_pskfile" ] && {
			cp "$FORM_openvpn_pskfile" "$dir_name/shared.key" &&
				UPLOAD_PSK=1
		}
		#PEM Cert
		[ -s "$FORM_openvpn_rootcafile" ] && {
			cp "$FORM_openvpn_rootcafile" "$dir_name/ca.crt" &&
				UPLOAD_ROOTCACERT=1
		}
		[ -s "$FORM_openvpn_clientcertfile" ] && {
			cp "$FORM_openvpn_clientcertfile" "$dir_name/client.crt" &&
				UPLOAD_CLIENTCERT=1
		}
		[ -s "$FORM_openvpn_clientkeyfile" ] && {
			cp "$FORM_openvpn_clientkeyfile" "$dir_name/client.key" &&
				UPLOAD_CLIENTKEY=1
		}
		[ -s "$FORM_openvpn_dh" ] && {
			cp "$FORM_openvpn_dh" "$dir_name/dh.pem" &&
				UPLOAD_DH=1
		}
		[ -s "$FORM_openvpn_tlsauth" ] && {
			cp "$FORM_openvpn_tlsauth" "$dir_name/tlsauth.key" &&
				UPLOAD_TLSAUTH=1
		}

		eval FORM_ovpn_mode="\$FORM_ovpn_mode_$config"
		eval FORM_ovpn_enabled="\$FORM_ovpn_enabled_$config"
		[ -z "$FORM_ovpn_enabled" ] && FORM_ovpn_enabled=0
		eval FORM_ovpn_port="\$FORM_ovpn_port_$config"
		eval FORM_ovpn_auth="\$FORM_ovpn_auth_$config"
		eval FORM_ovpn_proto="\$FORM_ovpn_proto_$config"
		eval FORM_ovpn_complzo="\$FORM_ovpn_complzo_$config"
		eval FORM_ovpn_ping="\$FORM_ovpn_ping_$config"
		eval FORM_ovpn_pingrestart="\$FORM_ovpn_pingrestart_$config"
		eval FORM_ovpn_persisttun="\$FORM_ovpn_persisttun_$config"
		eval FORM_ovpn_persistkey="\$FORM_ovpn_persistkey_$config"
		eval FORM_ovpn_ipaddr="\$FORM_ovpn_ipaddr_$config"
		eval FORM_ovpn_client_to_client="\$FORM_ovpn_client_to_client_$config"
		eval FORM_ovpn_cmdline="\$FORM_ovpn_cmdline_$config"
		eval FORM_ovpn_local="\$FORM_ovpn_local_$config"
		eval FORM_ovpn_remote="\$FORM_ovpn_remote_$config"
		eval FORM_ovpn_pull="\$FORM_ovpn_pull_$config"
		eval FORM_ovpn_verb="\$FORM_ovpn_verb_$config"

		uci_set webifopenvpn "$config" "mode" "$FORM_ovpn_mode"
		uci_set webifopenvpn "$config" "enabled" "$FORM_ovpn_enabled"
		uci_set webifopenvpn "$config" "port" "$FORM_ovpn_port"
		uci_set webifopenvpn "$config" "auth" "$FORM_ovpn_auth"
		uci_set webifopenvpn "$config" "proto" "$FORM_ovpn_proto"
		uci_set webifopenvpn "$config" "complzo" "$FORM_ovpn_complzo"
		uci_set webifopenvpn "$config" "ping" "$FORM_ovpn_ping"
		uci_set webifopenvpn "$config" "pingrestart" "$FORM_ovpn_pingrestart"
		uci_set webifopenvpn "$config" "persisttun" "$FORM_ovpn_persisttun"
		uci_set webifopenvpn "$config" "persistkey" "$FORM_ovpn_persistkey"
		uci_set webifopenvpn "$config" "ipaddr" "$FORM_ovpn_ipaddr"
		uci_set webifopenvpn "$config" "client_to_client" "$FORM_ovpn_client_to_client"
		uci_set webifopenvpn "$config" "cmdline" "$FORM_ovpn_cmdline"
		uci_set webifopenvpn "$config" "local" "$FORM_ovpn_local"
		uci_set webifopenvpn "$config" "remote" "$FORM_ovpn_remote"
		uci_set webifopenvpn "$config" "pull" "$FORM_ovpn_pull"
		uci_set webifopenvpn "$config" "verb" "$FORM_ovpn_verb"
	fi
	ovpn_form="start_form|@TR<<OpenVPN Config>>
	field|@TR<<Enabled>>
	checkbox|ovpn_enabled_$config|$FORM_ovpn_enabled|1
	field|@TR<<VPN Connection Type>>|mode_$config
	select|ovpn_mode_$config|$FORM_ovpn_mode
	option|client|@TR<<Client>>
	option|server|@TR<<Server>>
	field|@TR<<Accept Server Options>>|pull_$config|hidden
	checkbox|ovpn_pull_$config|$FORM_ovpn_pull|1
	field|@TR<<Server Address>>|ipaddr_$config|hidden
	text|ovpn_ipaddr_$config|$FORM_ovpn_ipaddr
	field|@TR<<Local Address>>|local_$config|hidden
	text|ovpn_local_$config|$FORM_ovpn_local
	field|@TR<<Remote Address>>|remote_$config|hidden
	text|ovpn_remote_$config|$FORM_ovpn_remote
	field|@TR<<Protocol>>|proto_$config
	select|ovpn_proto_$config|$FORM_ovpn_proto
	option|udp|UDP
	option|tcp|TCP
	field|@TR<<Server Port (default: 1194)>>|port_$config
	text|ovpn_port_$config|$FORM_ovpn_port
	field|@TR<<Authentication Method>>|auth_$config
	onchange|modechange
	select|ovpn_auth_$config|$FORM_ovpn_auth
	option|psk|@TR<<Preshared Key>>
	option|cert|@TR<<Certificate (PKCS12)>>
	option|pem|@TR<<Certificate (PEM)>>
	field|@TR<<Advanced Options>>|advanced_option_$config|hidden
	checkbox|ovpn_advanced_$config|$FORM_ovpn_advanced_$config|1
	end_form

	start_form|@TR<<Authentication>>|authentication_$config
	field|@TR<<Preshared Key Status>>|psk_status_$config|hidden
	$(empty "$NOPSK" || echo 'string|<span style="color:red">@TR<<No Keyfile uploaded yet!>></span>')
	$(empty "$UPLOAD_PSK" || echo 'string|<span style="color:green">@TR<<Upload Successful>><br/></span>')
	$(empty "$NOPSK" && echo 'string|@TR<<Found Installed Keyfile>>')
	field|@TR<<Upload Preshared Key>>|psk_$config|hidden
	upload|openvpn_pskfile

	# PKCS12 Cert
	field|@TR<<Certificate Status>>|certificate_status_$config|hidden
	$(empty "$NOCERT" || echo 'string|<span style="color:red">@TR<<No Certificate uploaded yet!>></span>')
	$(empty "$UPLOAD_CERT" || echo 'string|<span style="color:green">@TR<<Upload Successful>><br/></span>')
	$(empty "$NOCERT" && echo 'string|@TR<<Found Installed Certificate.>>')
	field|@TR<<Upload PKCS12 Certificate>>|certificate_$config|hidden
	upload|openvpn_pkcs12file

	# PEM cert
	field|@TR<<Certificate Status>>|root_ca_status_$config|hidden
	$(empty "$NOROOTCACERT" || echo 'string|<span style="color:red">@TR<<No Root CA certificate uploaded yet!>></span>')
	$(empty "$UPLOAD_ROOTCACERT" || echo 'string|<span style="color:green">@TR<<Upload Successful>><br/></span>')
	$(empty "$NOROOTCACERT" && echo 'string|@TR<<Found Installed Certificate.>>')
	field|@TR<<Upload Root CA certificate>>|root_ca_$config|hidden
	upload|openvpn_rootcafile

	field|@TR<<Certificate Status>>|client_certificate_status_$config|hidden
	$(empty "$NOCLIENTCERT" || echo 'string|<span style="color:red">@TR<<No client certificate uploaded yet!>></span>')
	$(empty "$UPLOAD_CLIENTCERT" || echo 'string|<span style="color:green">@TR<<Upload Successful>><br/></span>')
	$(empty "$NOCLIENTCERT" && echo 'string|@TR<<Found Installed Certificate.>>')
	field|@TR<<Upload Client Certificate>>|client_certificate_$config|hidden
	upload|openvpn_clientcertfile

	field|@TR<<Certificate Status>>|client_key_status_$config|hidden
	$(empty "$NOCLIENTKEY" || echo 'string|<span style="color:red">@TR<<No client key uploaded yet!>></span>')
	$(empty "$UPLOAD_CLIENTKEY" || echo 'string|<span style="color:green">@TR<<Upload Successful>><br/></span>')
	$(empty "$NOCLIENTKEY" && echo 'string|@TR<<Found installed client key.>>')
	field|@TR<<Upload Client Key>>|client_key_$config|hidden
	upload|openvpn_clientkeyfile

	field|@TR<<Certificate Status>>|dh_status_$config|hidden
	$(empty "$NODH" || echo 'string|<span style="color:red">@TR<<No Diffie Hellman parameters uploaded yet!>></span>')
	$(empty "$UPLOAD_DH" || echo 'string|<span style="color:green">@TR<<Upload Successful>><br/></span>')
	$(empty "$NODH" && echo 'string|@TR<<Found installed Diffie Hellman parameters.>>')
	field|@TR<<Upload Diffie Hellman>>|dh_$config|hidden
	upload|openvpn_dh

	# TLS Auth key
	field|@TR<<Certificate Status>>|tlsauth_status_$config|hidden
	$(empty "$NOTLSAUTH" || echo 'string|<span style="color:red">@TR<<No TLS Auth key uploaded yet!>></span>')
	$(empty "$UPLOAD_TLSAUTH" || echo 'string|<span style="color:green">@TR<<Upload Successful>><br/></span>')
	$(empty "$NOTLSAUTH" && echo 'string|@TR<<Found installed TLS Auth key.>>')
	field|@TR<<Upload TLS Auth key>>|tlsauth_$config|hidden
	upload|openvpn_tlsauth

	helpitem|Uploading Keys
	helptext|HelpText Uploading Keys#Only one key/certificate may be uploaded at a time.
	end_form
	
	start_form|@TR<<Advanced Options>>|advanced_$config|hidden
	field|@TR<<LZO compression>>
	checkbox|ovpn_complzo_$config|$FORM_ovpn_complzo|1
	field|@TR<<Ping>>
	text|ovpn_ping_$config|$FORM_ovpn_ping
	helpitem|Ping
	helptext|HelpText Ping#Causes OpenVPN to exit after n seconds pass without reception of a ping or other packet from remote.
	field|@TR<<Ping restart>>
	text|ovpn_pingrestart_$config|$FORM_ovpn_pingrestart
	helpitem|Ping Restart
	helptext|HelpText Ping Restart#Causes OpenVPN to restart after n seconds pass without reception of a ping or other packet from remote.
	field|@TR<<Persistent Device>>
	checkbox|ovpn_persisttun_$config|$FORM_ovpn_persisttun|1
	helpitem|Persistent Device
	helptext|HelpText Persistent Device#Don't close and reopen TUN/TAP device or run up/down scripts across SIGUSR1 or ping-restart restarts. 
	field|@TR<<Persistent Key>>
	checkbox|ovpn_persistkey_$config|$FORM_ovpn_persistkey|1
	helpitem|Persistent Key
	helptext|HelpText Persistent Key#Don't re-read key files across SIGUSR1 or ping-restart.
	field|@TR<<Client to Client>>|field_client_to_client_$config|hidden
	checkbox|ovpn_client_to_client_$config|$FORM_ovpn_client_to_client|1
	helpitem|Client to Client
	helptext|HelpText Client to Client#When this option is used, each client will "see" the other clients which are currently connected. Otherwise, each client will only see the server.
	field|@TR<<Logging Verbosity>>
	select|ovpn_verb_$config|$FORM_ovpn_verb
	option|0|0
	option|1|1
	option|2|2
	option|3|3
	option|4|4
	helpitem|Logging Verbosity
	helptext|HelpText Logging Verbosity#0 -- only fatal errors, 1 to 4 -- normal usage, default 1.
	field|@TR<<Extra cmdline arguments>>
	text|ovpn_cmdline_$config|$FORM_ovpn_cmdline
	end_form

	start_form
	field|
	string|<a href=\"$SCRIPT_NAME?remove_openvpncfg=$config\">@TR<<Remove OpenVPN Config>></a>"
	append OVPN "$ovpn_form" "$N"

	javascript_forms="
	var v;
	var v_enabled = checked('ovpn_enabled_${config}_1');
	var v_client = isset('ovpn_mode_$config','client');
	var v_server = isset('ovpn_mode_$config','server');

	set_visible('mode_$config', v_enabled);
	set_visible('port_$config', v_enabled);
	set_visible('auth_$config', v_enabled);
	set_visible('proto_$config', v_enabled);
	set_visible('authentication_$config', v_enabled);
	set_visible('advanced_option_$config', v_enabled);
	set_visible('advanced_$config', v_enabled);
	set_visible('local_$config', v_enabled);
	set_visible('remote_$config', v_enabled);
	set_visible('advanced_option_$config', v_enabled);

	if (v_enabled) {
		set_visible('advanced_$config', checked('ovpn_advanced_${config}_1'));

		set_visible('field_client_to_client_$config', v_server);
		set_visible('ipaddr_$config', v_client);
		set_visible('pull_$config', v_client);

		v = isset('ovpn_auth_$config','psk');
		set_visible('psk_status_$config', v);
		set_visible('psk_$config', v);

		v = isset('ovpn_auth_$config','cert');
		set_visible('certificate_status_$config', v);
		set_visible('certificate_$config', v);

		v = isset('ovpn_auth_$config','pem');
		set_visible('root_ca_status_$config', v);
		set_visible('root_ca_$config', v);
		set_visible('client_certificate_status_$config', v);
		set_visible('client_certificate_$config', v);
		set_visible('client_key_status_$config', v);
		set_visible('client_key_$config', v);

		v = (isset('ovpn_auth_$config','cert') || isset('ovpn_auth_$config','pem'))
		set_visible('tlsauth_status_$config', v);
		set_visible('tlsauth_$config', v);
		v = (v && v_server)
		set_visible('dh_status_$config', v);
		set_visible('dh_$config', v);
	}"
	append js "$javascript_forms" "$N"
done

add_ovpncfg="field|
string|<a href=\"$SCRIPT_NAME?add_openvpncfg_number=$openvpncfg_number\">@TR<<Add OpenVPN Config>></a>
end_form"
append OVPN "$add_ovpncfg" "$N"

cat <<EOF
<script type="text/javascript" src="/webif.js"></script>
<script type="text/javascript">
<!--
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
$install_package_button
$OVPN

EOF

footer
?>
<!--
##WEBIF:name:VPN:1:OpenVPN
-->
