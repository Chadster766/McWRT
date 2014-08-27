#!/usr/bin/webif-page
<?
. "/usr/lib/webif/webif.sh"
###################################################################
# Services configuration page
#
# Description:
#	Configures services not configured elsewhere.
#
# Author(s) [in order of work date]:
#	Jeremy Collake <jeremy.collake@gmail.com>
#
# NVRAM variables referenced:
#	none
#
# Configuration files referenced:
#	none
#

header "Network" "UPnP" "@TR<<UPnP Configuration>>" ' onload="modechange()" ' "$SCRIPT_NAME"

uci_load "upnpd"

if ! empty "$FORM_install_miniupnp"; then
	echo "@TR<<Installing>> miniUPNPd ...<pre>"
	install_package miniupnpd
	uci_set upnpd general enable 1
	echo "</pre>"
fi

if ! empty "$FORM_install_linuxigd"; then
	echo "@TR<<Installing>> linuxigd ...<pre>"
	install_package libpthread
	install_package libupnp
	install_package linuxigd
	# if config file doesn't exist, create it since it doesn't come with above pkg at present
	! exists "/etc/config/upnpd" && {
		uci_add upnpd upnpd config
		uci_set upnpd config enabled 1
	}
	uci_load upnpd
	echo "</pre>"
fi

if ! empty "$FORM_remove_miniupnpd"; then
	echo "@TR<<Removing>> miniUPNPd ...<pre>"
	remove_package miniupnpd
	#uci_set "upnpd" "general" "enable" "0"
	echo "</pre>"
fi

if ! empty "$FORM_remove_linuxigd"; then
	echo "@TR<<Removing>> linuxigd UPNPd ...<pre>"
	remove_package linuxigd
	remove_package libupnp
	remove_package libpthread
	#uci_set "upnpd" "general" "enable" "0"
	echo "</pre>"
fi

ipkg_listinst=$(opkg list_installed 2>/dev/null | grep "^\(miniupnpd \|linuxigd \)")
upnp_installed="0"

echo "$ipkg_listinst" | grep -q "^miniupnpd "
equal "$?" "0" && {
	upnp_installed="1"
	upnp_miniupnpd="1"
	remove_upnpd_button="field|@TR<<Remove miniupnpd>>
	submit|remove_miniupnpd| @TR<<Remove>> |"
}

echo "$ipkg_listinst" | grep -q "^linuxigd "
equal "$?" "0" && {
	upnp_installed="1"
	upnp_miniupnpd="0"
	remove_upnpd_button="field|@TR<<Remove linuxigd>>
	submit|remove_linuxigd| @TR<<Remove>> |"
}

if empty "$FORM_submit"; then
	# initialize all defaults
	FORM_upnp_enable="$CONFIG_config_enabled"
	FORM_upnpd_log_output="$CONFIG_config_log_output"
	FORM_upnpd_up_bitspeed="$CONFIG_config_upload"
	FORM_upnpd_down_bitspeed="$CONFIG_config_download"
else
	# save form
	uci_set upnpd config enabled "$FORM_upnp_enable"
	if equal "$upnp_miniupnpd" "1" ; then
		uci_set upnpd config log_output "$FORM_upnpd_log_output"
		uci_set upnpd config download "$FORM_upnpd_down_bitspeed"
		uci_set upnpd config upload "$FORM_upnpd_up_bitspeed"
	fi
fi

#####################################################################s
cat <<EOF
<script type="text/javascript" src="/webif.js"></script>
<script type="text/javascript">

function modechange()
{
EOF
if equal "$upnp_miniupnpd" "1" ; then
	cat <<EOF
	if(isset('upnp_enable','1'))
	{
		document.getElementById('upnpd_up_bitspeed').disabled = false;
		document.getElementById('upnpd_down_bitspeed').disabled = false;
		document.getElementById('upnpd_log_output').disabled = false;
	}
	else
	{
		document.getElementById('upnpd_up_bitspeed').disabled = true;
		document.getElementById('upnpd_down_bitspeed').disabled = true;
		document.getElementById('upnpd_log_output').disabled = true;
	}
EOF
fi
	cat <<EOF
}
</script>
EOF
#####################################################################

if equal "$upnp_installed" "1" ; then
	primary_upnpd_form="field|@TR<<UPNP Daemon>>
	select|upnp_enable|$FORM_upnp_enable
	option|0|@TR<<Disabled>>
	option|1|@TR<<Enabled>>"
	if equal "$upnp_miniupnpd" "1" ; then
		primary_upnpd_form="$primary_upnpd_form
		field|@TR<<WAN Upload (bits/sec)>>
		text|upnpd_up_bitspeed|$FORM_upnpd_up_bitspeed| @TR<<kilobits>>
		field|@TR<<WAN Download (bits/sec)>>
		text|upnpd_down_bitspeed|$FORM_upnpd_down_bitspeed| @TR<<kilobits>>
		helpitem|WAN Speeds
		helptext|HelpText upnpd_wan_speeds#Set your WAN speeds here, in kilobits. This is for reporting to upnp clients that request it only.
		field|@TR<<Log Debug Output>>
		select|upnpd_log_output|$FORM_upnpd_log_output
		option|0|@TR<<Disabled>>
		option|1|@TR<<Enabled>>"
	fi
	primary_upnpd_form="$primary_upnpd_form
	$remove_upnpd_button
	helpitem|Remove UPNPd
	helptext|HelpText remove_upnpd_help#If you have problems you can remove your current UPNPd and try the other one to see if it works better for you."
else
	install_miniupnp_button="field|@TR<<miniupnpd>>
submit|install_miniupnp| @TR<<Install>> |"
	install_linuxigd_button="field|@TR<<linuxigd>>
submit|install_linuxigd| @TR<<Install>> |"
	install_help="helpitem|Which UPNPd to choose
helptext|HelPText install_upnpd_help#There are two UPNP daemons to choose from: miniupnpd and linuxigd. Try miniupnpd first, but it if does not work for you, then remove that package and try linuxigd."
fi

display_form <<EOF
onchange|modechange
start_form|@TR<<UPNP>>
$primary_upnpd_form
$install_miniupnp_button
$install_linuxigd_button
$install_help
end_form
EOF

footer ?>
<!--
##WEBIF:name:Network:550:UPnP
-->
