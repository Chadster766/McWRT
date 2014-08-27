#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
vcfg_number=0
config_cb() {
	local cfg_type="$1"
	local cfg_name="$2"

	case "$cfg_type" in
		webifopenvpn)
			append openvpnconfigs "$cfg_name" "$N"
			config_get enabled "$cfg_name" enabled
			[ -n "$enabled" ] && openvpn_enabled=1
		;;
	esac
}

uci_load webifopenvpn

header "Status" "OpenVPN" "@TR<<OpenVPN Status>>"

for config in $openvpnconfigs; do
	config_get enabled "$config" enabled
	[ -n "$enabled" ] && openvpn_enabled=1
done

empty "$openvpn_enabled" || {
	case "$FORM_action" in
		start)
			ps | grep -q '[o]penvpn' || {
				echo -n "@TR<<status_openvpn_Starting_OpenVPN#Starting OpenVPN ...>>"
				/etc/init.d/webifopenvpn start
				echo " @TR<<status_openvpn_done#done.>>"
			}
		;;
		stop)
			ps | grep -q '[o]penvpn' && {
				echo -n "@TR<<status_openvpn_Stopping_OpenVPN#Stopping OpenVPN ...>>"
				/etc/init.d/webifopenvpn stop
				echo " @TR<<status_openvpn_done#done.>>"
			}
		;;
	esac
	for config in $openvpnconfigs; do
		config_get CONFIG_auth "$config" "auth"
		config_get dir_name $config "dir"
		case "$CONFIG_auth" in
			cert)
				[ -f "$dir_name/certificate.p12" ] || ERROR="@TR<<status_openvpn_Err_cert_missing#Error, certificate is missing!>>"
			;;
			psk)
				[ -f "$dir_name/shared.key" ] || ERROR="@TR<<status_openvpn_Err_keyfile_missing#Error, keyfile is missing!>>"
			;;
			pem)
				[ -f $dir_name/ca.crt ] || ERROR="@TR<<status_openvpn_Err_root_ca_missing#Error, root ca is missing!>>"
				[ -f $dir_name/client.crt ] || ERROR="@TR<<status_openvpn_Err_client_cert_missing#Error, client certificate is missing!>>"
				[ -f $dir_name/client.key ] || ERROR="@TR<<status_openvpn_Err_client_keyfile_missing#Error, client keyfile is missing!>>"
				[ -f $dir_name/dh.pem ] || ERROR="@TR<<status_openvpn_Err_client_dh_keyfile_missing#Error, client diffie hellmann keyfile is missing!>>"
			;;
			*)
				ERROR="@TR<<status_openvpn_Err_unknown_authtype#error in OpenVPN configuration, unknown authtype>>"
			;;
		esac

		empty "$ERROR" && {
			DEVICES=$(egrep "(tun$vcfg_number)" /proc/net/dev | cut -d: -f1 | tr -d ' ')
			empty "$DEVICES" && {
				echo "@TR<<status_openvpn_no_active_tunnel#no active tunnel found>>"
			} || {
				echo "@TR<<status_openvpn_active_tunnel#found the following active tunnel:>>"
				echo "<pre>"
				for DEV in $DEVICES;do
					ifconfig $DEV
				done
				echo "</pre>"
			}
			echo "<br/>"

			ps | grep -q '[o]penvpn' && {
				echo '@TR<<status_openvpn_OpenVPN_running#OpenVPN process is running>> <a href="?action=stop">@TR<<status_openvpn_stop_now#[stop now]>></a>'
			} || {
				echo '@TR<<status_openvpn_OpenVPN_not_running#OpenVPN is not running>> <a href="?action=start">@TR<<status_openvpn_start_now#[start now]>></a>'
			}
			echo "<br/>"
			let "vcfg_number+=1"
		} || {
			echo "$ERROR"
		}
	done
} || {
	echo "<br />@TR<<status_openvpn_OpenVPN_disabled#OpenVPN is disabled>>"
}

footer ?>
<!--
##WEBIF:name:Status:910:OpenVPN
-->
