#!/usr/bin/webif-page
<?
. "/usr/lib/webif/webif.sh"

###################################################################
# system configuration page
#
# Description:
#	Configures general system settings.
#
# Author(s) [in order of work date]:
#   	Original webif developers -- todo
#	Markus Wigge <markus@freewrt.org>
#   	Jeremy Collake <jeremy.collake@gmail.com>
#	Travis Kemen <kemen04@gmail.com>
#
# Major revisions:
#
# Configuration files referenced:
#   none
#

# ntpcliconf variable left here incase name changes again.
ntpcliconf="ntpclient"

# Add NTP Server
if ! empty "$FORM_add_ntpcfg_number"; then
	uci_add "$ntpcliconf" "ntpserver"
	uci_set "$ntpcliconf" "$CONFIG_SECTION" "hostname" ""
	uci_set "$ntpcliconf" "$CONFIG_SECTION" "port" "123"
	FORM_add_ntpcfg=""
fi

# Remove NTP Server
if ! empty "$FORM_remove_ntpcfg"; then
	uci_remove "$ntpcliconf" "$FORM_remove_ntpcfg"
fi

config_cb() {
	local cfg_type="$1"
	local cfg_name="$2"

	case "$cfg_type" in
		system)
			hostname_cfg="$cfg_name"
		;;
		timezone)
			timezone_cfg="$cfg_name"
		;;
		ntpserver)
			append ntpservers "$cfg_name" "$N"
		;;
		ntpclient)
			append ntpclient_cfgs "$cfg_name" "$N"
		;;
		rdate)
			append rdate_cfgs "$cfg_name" "$N"
		;;
	esac
}

uci_load "webif"
uci_load "system"
#We have to load the system host name setting here because ntp_client also uses the hostname setting.
eval CONFIG_systemhostname="\$CONFIG_${hostname_cfg}_hostname"
FORM_hostname="${FORM_hostname:-$CONFIG_systemhostname}"
FORM_hostname="${FORM_hostname:-OpenWrt}"
config_clear "$hostname_cfg"
uci_load "network"
uci_load timezone
[ "$?" != "0" ] && {
	uci_set_default timezone <<EOF
config 'timezone'
	option 'posixtz' 'UTC+0'
	option 'zoneinfo' ''
EOF
	uci_load timezone

}
uci_load "$ntpcliconf"

#FIXME: uci_load bug
#uci_load will pass the same config twice when there is a section to be added by using uci_add before a uci_commit happens
#we will use uniq so we don't try to parse the same config section twice.
ntpservers=$(echo "$ntpservers" |uniq)

ntpcfg_number=$(echo "$ntpservers" |wc -l)
let "ntpcfg_number+=1"

#####################################################################
header "System" "Settings" "@TR<<System Settings>>" ' onload="modechange()" ' "$SCRIPT_NAME"

generate_ssl_key() {
	local inst_packages inst_links llib llink libsymlinks
	is_package_installed "px5g"
	if [ "$?" = "0" ]; then
		for llib in $inst_links; do
			llink=$(echo "$llib" | sed 's/\/tmp//')
			ln -s $llib $llink
			[ "$?" = "0" ] && libsymlinks="$libsymlinks $llink"
		done
		if [  -z "$(ps | grep "[n]tpd\>")" ]; then
			is_package_installed "ntpclient"
			[ "$?" != "0" ] && {
				echo "@TR<<system_settings_Updating_time#Updating time>> ..."
				rdate -s time.fu-berlin.de
				[ "$?" != "0" ] && rdate -s ptbtime1.ptb.de
				[ "$?" != "0" ] && rdate -s ac-ntp0.net.cmu.edu
				[ "$?" != "0" ] && echo "@TR<<system_time_update_error#Updating time failed, generating cert anyhow>>"
			}
		fi
		/tmp/usr/sbin/px5g selfsigned -der -days ${days:-730} -newkey rsa:${bits:-1024} -keyout "/etc/uhttpd.key" -out "/etc/uhttpd.crt" -subj /C=${country:-DE}/ST=${state:-Saxony}/L=${location:-Leipzig}/CN=${commonname:-OpenWrt}
	fi
	[ -n "$libsymlinks" ] && rm -f $libsymlinks
	[ -n "$inst_packages" ] && opkg remove $inst_packages
}

if ! empty "$FORM_install_ssl"; then
	echo "@TR<<system_settings_Installing_STunnel_package#Installing uhttpd-mod-tls package>> ...<pre>"
	install_package "uhttpd-mod-tls"
	is_package_installed "uhttpd-mod-tls"
	[ "$?" = "0" ] && [ ! -e /etc/uhttpd.key -o ! -e /etc/uhttpd.crt ] && {
		echo "@TR<<system_settings_Generating_SSL_certificate#Generating SSL certificate>> ..."
		generate_ssl_key
	}
	echo "</pre><br />"
fi

#####################################################################
# initialize forms
if empty "$FORM_submit"; then
	# initialize all defaults
	eval time_zone_part="\$CONFIG_${timezone_cfg}_posixtz"
	eval time_zoneinfo_part="\$CONFIG_${timezone_cfg}_zoneinfo"
	time_zone_part="${time_zone_part:-"UTC+0"}"
	time_zoneinfo_part="${time_zoneinfo_part:-"-"}"
	FORM_system_timezone="${time_zoneinfo_part}@${time_zone_part}"

	has_nvram_support && {
		FORM_boot_wait="${boot_wait:-$(nvram get boot_wait)}"
		FORM_boot_wait="${FORM_boot_wait:-off}"
		FORM_wait_time="${wait_time:-$(nvram get wait_time)}"
		FORM_wait_time="${FORM_wait_time:-1}"
	}
	# webif settings
	FORM_effect="${CONFIG_general_use_progressbar}"		# -- effects checkbox
	if equal $FORM_effect "1" ; then FORM_effect="checked" ; fi	# -- effects checkbox
	FORM_language="${CONFIG_general_lang:-en}"	
	FORM_theme="${CONFIG_theme_id:-xwrt}"
else
#####################################################################
# save forms
	SAVED=1
	validate <<EOF
hostname|FORM_hostname|@TR<<Host Name>>|nodots required|$FORM_hostname
EOF
	if equal "$?" 0 ; then
		time_zone_part="${FORM_system_timezone#*@}"
		time_zoneinfo_part="${FORM_system_timezone%@*}"
		empty "$hostname_cfg" && {
			uci_add system system
			hostname_cfg="$CONFIG_SECTION"
		}
		uci_set "system" "$hostname_cfg" "hostname" "$FORM_hostname"
		empty "$timezone_cfg" && {
			uci_add timezone timezone
			timezone_cfg="$CONFIG_SECTION"
		}
		uci_set timezone "$timezone_cfg" posixtz "$time_zone_part"
		uci_set timezone "$timezone_cfg" zoneinfo "$time_zoneinfo_part"
		for server in $ntpservers; do
			eval FORM_ntp_server="\$FORM_ntp_server_$server"
			eval FORM_ntp_port="\$FORM_ntp_port_$server"
			uci_set "$ntpcliconf" "$server" hostname "$FORM_ntp_server"
			uci_set "$ntpcliconf" "$server" port "$FORM_ntp_port"
		done

		has_nvram_support && {
			case "$FORM_boot_wait" in
				on|off) save_setting system boot_wait "$FORM_boot_wait";;
			esac
			! empty "$FORM_wait_time" &&
			{
				save_setting system wait_time "$FORM_wait_time"
			}
		}
		# webif settings
		uci_set "webif" "theme" "id" "$FORM_theme"
		uci_set "webif" "general" "lang" "$FORM_language"
		uci_set "webif" "general" "use_progressbar" "$FORM_effect_enable"
		FORM_effect=$FORM_effect_enable ; if equal $FORM_effect "1" ; then FORM_effect="checked" ; fi
	else
		echo "<br /><div class=\"warning\">@TR<<Warning>>: @TR<<system_settings_Hostname_failed_validation#Hostname failed validation. Can not be saved.>></div><br />"
	fi
fi

WEBIF_SSL="field|@TR<<system_settings_Webif_SSL#Webif&sup2; SSL>>"
is_package_installed "uhttpd-mod-tls"
if [ "$?" != "0" ]; then
	WEBIF_SSL="$WEBIF_SSL
string|<div class=\"warning\">@TR<<system_settings_Feature_requires_ssl#uhttpd-mod-tls package is not installed. You need to install it for ssl support>>:</div>
submit|install_ssl| @TR<<system_settings_Install_SSL#Install uhttpd-mod-tls>> |"
else
	WEBIF_SSL="$WEBIF_SSL
	string|<div class=\"warning\">@TR<<system_settings_Feature_ssl_enabled#SSL is enabled>></div>"
fi

	effect_field=$(cat <<EOF
field| 
string|<input type="checkbox" name="effect_enable" value="1" $FORM_effect />&nbsp;@TR<<Enable visual effects>><br/><br/>
EOF
)

#####################################################################
# boot wait time
#
has_nvram_support && {
	#####################################################################
	# Initialize wait_time form
	for wtime in $(seq 1 30); do
		FORM_wait_time="$FORM_wait_time
			option|$wtime"
	done
}

#####################################################################
# Initialize THEMES form
#
#
# start with list of available installable theme packages
#
! exists "/etc/themes.lst" && {
	# create list if it doesn't exist ..
	/usr/lib/webif/webif-mkthemelist.sh	
}
THEMES=$(cat "/etc/themes.lst")
for str in $temp_t; do
	THEME="$THEME
		option|$str"
done

# enumerate installed themes by finding all subdirectories of /www/theme
# this lets users install themes not built into packages.
#
for curtheme in /www/themes/*; do
	curtheme=$(echo "$curtheme" | sed s/'\/www\/themes\/'//g)
	if exists "/www/themes/$curtheme/name"; then
		theme_name=$(cat "/www/themes/$curtheme/name")
	else
		theme_name="$curtheme"
	fi
	! equal "$curtheme" "active" && {
		THEMES="$THEMES
option|$curtheme|$theme_name"
	}
done
#
# sort list and remove dupes
#
THEMES=$(echo "$THEMES" | sort -u)

#####################################################################
# Initialize LANGUAGES form
# create list if it doesn't exist ..
/usr/lib/webif/webif-mklanglist.sh
LANGUAGES=$(cat "/etc/languages.lst")

has_nvram_support && {
	bootwait_form="field|@TR<<Boot Wait>>
	select|boot_wait|$FORM_boot_wait
	option|on|@TR<<Enabled>>
	option|off|@TR<<Disabled>>
	helpitem|Boot Wait
	helptext|HelpText boot_wait#Boot wait causes the boot loader of some devices to wait a few seconds at bootup for a TFTP transfer of a new firmware image. This is a security risk to be left on."

	waittime_form="field|@TR<<Wait Time>>
	select|wait_time|$FORM_wait_time
	helpitem|Wait Time
	helptext|HelpText wait_time#Number of seconds the boot loader should wait for a TFTP transfer if Boot Wait is on."

}
#####################################################################
# rdate form

for cfg in $rdate_cfgs; do
	if empty "$FORM_submit"; then
		config_get FORM_rdate $cfg server
		eval FORM_rdateremove="\$FORM_${cfg}_rdateremove"
		if [ "$FORM_rdateremove" != "" ]; then
			list_remove FORM_rdate "$FORM_rdateremove"
			uci_set "system" "$cfg" "server" "$FORM_rdate"
		fi
	else
		eval FORM_rdateadd="\$FORM_${cfg}_rdateadd"
		config_get FORM_rdate $cfg server
		[ $FORM_rdateadd != "" ] && FORM_rdate="$FORM_rdate $FORM_rdateadd"
		uci_set "system" "$cfg" "server" "$FORM_rdate"
	fi

	rdate_forms="start_form|@TR<<rdate Servers>>|field_${cfg}_rdate
	listedit|${cfg}_rdate|$SCRIPT_NAME?|$FORM_rdate
	end_form"
	append rdate_form "$rdate_forms" "$N"
done


#####################################################################
# ntp form
if [ -z "$(has_pkgs ntpclient)" ]; then
	for server in $ntpservers; do
		if empty "$FORM_submit"; then
			config_get FORM_ntp_server $server hostname
			config_get FORM_ntp_port $server port
		else
			eval FORM_ntp_server="\$FORM_ntp_server_$server"
			eval FORM_ntp_port="\$FORM_ntp_port_$server"
		fi
		#add check for blank config, the only time it will be seen is when config section is waitings to be removed
		if [ "$FORM_ntp_port" != "" -o "$FORM_ntp_server" != "" ]; then
			if [ "$FORM_ntp_port" = "" ]; then
				FORM_ntp_port=123
			fi
			ntp_form="field|@TR<<NTP Server>>
			text|ntp_server_$server|$FORM_ntp_server
			field|@TR<<NTP Server Port>>
			text|ntp_port_$server|$FORM_ntp_port
			field|
			string|<a href=\"$SCRIPT_NAME?remove_ntpcfg=$server\">@TR<<Remove NTP Server>></a>"
			append NTP "$ntp_form" "$N"
		fi
	done
fi
add_ntpcfg="field|
string|<a href=\"$SCRIPT_NAME?add_ntpcfg_number=$ntpcfg_number\">@TR<<Add NTP Server>></a>"
append NTP "$add_ntpcfg" "$N"


#####################################################################
# initialize time zones

TIMEZONE_OPTS=$(
	awk -v timezoneinfo="$FORM_system_timezone" '
		BEGIN {
			FS="	"
			last_group=""
			defined = 0
		}
		/^(#.*)?$/ {next}
		$1 != last_group {
			last_group=$1
			print "optgroup|" $1
		}
		{
			list_timezone = $4 "@" $3
			if (list_timezone == timezoneinfo)
				defined = defined + 1
			print "option|" list_timezone "|@TR<<" $2 ">>"
		}
		END {
			if (defined == 0) {
				split(timezoneinfo, oldtz, "@")
				print "optgroup|@TR<<system_settings_group_unknown_TZ#Unknown>>"
				if (oldtz[1] == "-") oldtz[1] = "@TR<<system_settings_User_or_old_TZ#User defined (or out of date)>>"
				print "option|" timezoneinfo "|" oldtz[1]
			}
		}' < /usr/lib/webif/timezones.csv 2>/dev/null

)
#######################################################
# Web Services Form
uci_load uhttpd

if empty "$FORM_submit"; then
  config_get FORM_http_port "main" listen_http
  [ "$FORM_http_port" = "0.0.0.0:80" ] && FORM_http_port="80"
  config_get FORM_https_port "main" listen_https
  [ "$FORM_https_port" = "0.0.0.0:443" ] && FORM_https_port="443"
else
  uci_set "uhttpd" "main" "listen_http" "$FORM_http_port"
  uci_set "uhttpd" "main" "listen_https" "$FORM_https_port"
fi

cat <<EOF
<script type="text/javascript" src="/webif.js"></script>
<script type="text/javascript">
<!--
function modechange()
{
EOF
has_nvram_support && cat <<EOF
	if(isset('boot_wait','on'))
	{
		document.getElementById('wait_time').disabled = false;
	}
	else
	{
		document.getElementById('wait_time').disabled = true;
	}

EOF
cat <<EOF
	var tz_info = value('system_timezone');
	if ((tz_info=='') || (tz_info==null)){
		set_value('show_TZ', tz_info);
	}
	else {
		var tz_split = tz_info.split('@');
		set_value('show_TZ', tz_split[1]);
	}
}
-->
</script>
EOF
#######################################################
# Show form
display_form <<EOF
onchange|modechange
start_form|@TR<<System Settings>>
field|@TR<<Host Name>>
text|hostname|$FORM_hostname
$bootwait_form
$waittime_form
end_form
start_form|@TR<<Time Settings>>
field|@TR<<Timezone>>
select|system_timezone|$FORM_system_timezone
$TIMEZONE_OPTS
field|@TR<<system_settings_POSIX_TZ_String#POSIX TZ String>>|view_tz_string|
string|<input id="show_TZ" type="text" style="width: 96%; height: 1.2em; color: #2f2f2f; background: #ececec; " name="show_TZ" readonly="readonly" value="@TR<<system_settings_js_required#This field requires the JavaScript support.>>" />
helpitem|Timezone
helptext|Timezone_helptext#Set up your time zone according to the nearest city of your region from the predefined list.
$NTP
end_form
$rdate_form
##########################
# webif settings
start_form|@TR<<Webif&sup2; Settings>>
$effect_field
field|@TR<<Language>>
select|language|$FORM_language
$LANGUAGES
field|@TR<<Theme>>
select|theme|$FORM_theme
$THEMES
$WEBIF_SSL
end_form
start_form|@TR<<Web Configurator Settings>>
field|@TR<<HTTP Port>>
text|http_port|$FORM_http_port
field|@TR<<HTTPS Port>>
text|https_port|$FORM_https_port
end_form
EOF

footer ?>

<!--
##WEBIF:name:System:010:Settings
-->
