#!/bin/ash
#
#
# Handler for config application of all types.
#
#   Types supported:
#	uci-*		UCI config files
#	file-*		Undefined format for whatever
#	edited-files-*  raw edited files
#
#
. /usr/lib/webif/functions.sh
. /lib/config/uci.sh

config_cb() {
	[ "$1" = "system" ] && system_cfg="$2"
	[ "$1" = "server" ] && l2tpns_cfg="$2"
	[ "$1" = "updatedd" ] && updatedd_cfg="$2"
}

# this line is for compatibility with webif-lua
#LUA="/usr/lib/webif/LUA/xwrt-apply.lua"
#if [ -e $LUA ]; then
#  /usr/lib/webif/LUA/xwrt-apply.lua 
#fi

HANDLERS_file='
	hosts) rm -f /etc/hosts; mv $config /etc/hosts; killall -HUP dnsmasq ;;
	ethers) rm -f /etc/ethers; mv $config /etc/ethers; killall -HUP dnsmasq ;;
	dnsmasq.conf) mv /tmp/.webif/file-dnsmasq.conf /etc/dnsmasq.conf && /etc/init.d/dnsmasq restart;;
	httpd.conf) mv -f /tmp/.webif/file-httpd.conf /etc/httpd.conf && HTTP_RESTART=1 ;;
'

# for some reason a for loop with "." doesn't work
eval "$(cat /usr/lib/webif/apply-*.sh 2>&-)"

mkdir -p "/tmp/.webif"
_pushed_dir=$(pwd)
cd "/tmp/.webif"

# edited-files/*		user edited files - stored with directory tree in-tact
for edited_file in $(find "/tmp/.webif/edited-files/" -type f 2>&-); do
	target_file=$(echo "$edited_file" | sed s/'\/tmp\/.webif\/edited-files'//g)
	echo "@TR<<Processing>> $target_file"
	fix_symlink_hack "$target_file"
	if tr -d '\r' <"$edited_file" >"$target_file"; then
		rm "$edited_file" 2>&-
	else
		echo "@TR<<Critical Error>> : @TR<<Could not replace>> $target_file. @TR<<Media full>>?"
	fi
done
# leave if some files not applied
rm -r "/tmp/.webif/edited-files" 2>&-


config_allclear() {
	for var in $(set | grep "^CONFIG_" | sed -e 's/\(.*\)=.*$/\1/'); do
		unset "$var"
	done
}

reload_upnpd() {
	config_load upnpd
	config_get_bool test config enabled 0
	if [ 1 -eq "$test" ]; then
		echo '@TR<<Starting>> @TR<<upnpd>> ...'
		[ -f /etc/init.d/miniupnpd ] && {
			/etc/init.d/miniupnpd enable >&- 2>&- <&-
			/etc/init.d/miniupnpd start >&- 2>&- <&-
		}
		[ -f /etc/init.d/upnpd ] && {
			/etc/init.d/upnpd enable >&- 2>&- <&-
			/etc/init.d/upnpd restart >&- 2>&- <&-
		}
	else
		echo '@TR<<Stopping>> @TR<<upnpd>> ...'
		[ -f /etc/init.d/miniupnpd ] && {
			/etc/init.d/miniupnpd stop >&- 2>&- <&-
			/etc/init.d/miniupnpd disable >&- 2>&- <&-
		}
		[ -f /etc/init.d/upnpd ] && {
			/etc/init.d/upnpd stop >&- 2>&- <&-
			/etc/init.d/upnpd disable >&- 2>&- <&-
		}
	fi
	config_allclear
}


# file-*		other config files
for config in $(ls file-* 2>&-); do
	name=${config#file-}
	echo "@TR<<Processing>> @TR<<config file>>: $name"
	eval 'case "$name" in
		'"$HANDLERS_file"'
	esac'
done


# config-conntrack	  Conntrack Config file
for config in $(ls config-conntrack 2>&-); do
	echo '@TR<<Applying>> @TR<<conntrack settings>> ...'
	fix_symlink_hack "/etc/sysctl.conf"
	# set any and all net.ipv4.netfilter settings.
	for conntrack in $(grep ip_ /tmp/.webif/config-conntrack); do
		variable_name=$(echo "$conntrack" | cut -d '=' -f1)
		variable_value=$(echo "$conntrack" | cut -d '"' -f2)
		echo "&nbsp;@TR<<Setting>> $variable_name to $variable_value"
		remove_lines_from_file "/etc/sysctl.conf" "net.ipv4.netfilter.$variable_name"
		echo "net.ipv4.netfilter.$variable_name=$variable_value" >> /etc/sysctl.conf
	done
	sysctl -p 2>&- 1>&- # reload sysctl.conf
	rm -f /tmp/.webif/config-conntrack
	echo '@TR<<Done>>'
done

# clear all uci settings to free memory
# init_theme - initialize a new theme
init_theme() {
	echo '@TR<<Initializing theme ...>>'
	config_get newtheme theme id
	# if theme isn't present, then install it		
	! exists "/www/themes/$newtheme/webif.css" && {
		install_package "webif-theme-$newtheme"	
	}
	if ! exists "/www/themes/$newtheme/webif.css"; then
		# if theme still not installed, there was an error
		echo "@TR<<Error>>: @TR<<installing theme package>>."
	else
		# create symlink to new active theme if its not already set right
		current_theme=$(ls /www/themes/active -l | cut -d '>' -f 2 | sed s/'\/www\/themes\/'//g)
		! equal "$current_theme" "$newtheme" && {
			rm /www/themes/active
			ln -s /www/themes/$newtheme /www/themes/active
		}
	fi		
	echo '@TR<<Done>>'
}

# switch_language (old_lang)  - switches language if changed
switch_language() {
	oldlang="$1"
	config_get newlang general lang
	! equal "$newlang" "$oldlang" && {
		echo '@TR<<Applying>> @TR<<Installing language pack>> ...'
		# if not English then we install language pack
		! equal "$newlang" "en" && {
			# build URL for package
			#  since the original webif may be installed to, have to make sure we get latest ver
			webif_version=$(opkg status webif | awk '/Version:/ { print $2 }')
			xwrt_repo_url=$(cat /etc/opkg/xwrt.conf | grep X-Wrt | cut -d' ' -f3)
			# always install language pack, since it may have been updated without package version change
			opkg -force-reinstall -force-overwrite install "${xwrt_repo_url}/webif-lang-${newlang}_${webif_version}_all.ipk" | uniq
			# switch to it if installed, even old one, otherwise return to previous
			if equal "$(opkg status "webif-lang-${newlang}" |grep "Status:" |grep " installed" )" ""; then
				echo '@TR<<Error installing language pack>>!'
			fi
		}
		echo '@TR<<Done>>'
	}
}

reload_qos() {
	config_load qos
	config_get_bool wan_enabled wan enabled 0
	if [ 1 -eq "$wan_enabled" ]; then
		echo '@TR<<Starting>> @TR<<qos>> ...'
		[ -f /etc/init.d/qos ] && {
			/etc/init.d/qos enable >&- 2>&- <&-
			/etc/init.d/qos start >&- 2>&- <&-
		}
	else
		echo '@TR<<Stopping>> @TR<<qos>> ...'
		[ -f /etc/init.d/qos ] && {
			/etc/init.d/qos stop >&- 2>&- <&-
			/etc/init.d/qos disable >&- 2>&- <&-
		}
	fi
	config_allclear
}


# config-*		simple config files
(
	cd /proc/self
	cat /tmp/.webif/config-* 2>&- | grep '=' >&- 2>&- && {
		exists "/usr/sbin/nvram" && {
			cat /tmp/.webif/config-* 2>&- | tee fd/1 | xargs -n1 nvram set	
			echo "@TR<<Committing>> NVRAM ..."
			nvram commit
		}
	}
)

#
# now apply any UCI config changes
#
process_packages=""
for ucifile in $(ls /tmp/.uci/* 2>&-); do
	# do not process lock files
	[ "${ucifile%.lock}" != "${ucifile}" ] && continue

	package=${ucifile#/tmp/.uci/}
	process_packages="$process_packages $package"

	# get old/updated values for the package here
	case "$ucifile" in
		"/tmp/.uci/webif") 
			config_load "/etc/config/$package"
			config_get apply_webif_oldlang general lang
			config_get old_firewall_log firewall log
			;;
	esac
	config_allclear

	# commit settings
	echo "@TR<<Committing>> $package ..."
	uci_commit "$package"
done

[ -n "$process_packages" ] && echo "@TR<<Waiting for the commit to finish>>..."
LOCK=`which lock` || LOCK=:
for ucilock in $(ls /tmp/.uci/*.lock 2>&-); do
	$LOCK -w "$ucilock"
	rm "$ucilock" >&- 2>&-
done

# now process changes in UCI configs
for package in $process_packages; do
	# process settings
	case "$package" in
		"qos")
			reload_qos
			;;
		"webif")
			config_load webif
			switch_language "$apply_webif_oldlang"
			init_theme
			/etc/init.d/webif start
			config_get log_enabled firewall log
			if [ "$old_firewall_log" != "$log_enabled" ]; then
				[ "$log_enabled" = "1" ] && /etc/init.d/webiffirewalllog start
				[ "$log_enabled" = "0" ] && /etc/init.d/webiffirewalllog stop
			fi
			config_allclear
			;;
		"upnpd")
			reload_upnpd
			;;
		"network")
			echo '@TR<<Reloading>> @TR<<network>> ...'
			/etc/init.d/network restart
			sleep 3
			killall dnsmasq
			if [ -f /etc/rc.d/S??dnsmasq ]; then
				/etc/init.d/dnsmasq start
			fi
			;;
		"ntpclient")
			killall ntpclient
			ACTION="ifup" . /etc/hotplug.d/iface/??-ntpclient; [ -f /etc/rc.d/S??ntpclient ] && /etc/rc.d/S??ntpclient start &
			;;
		"dhcp")
			killall dnsmasq
			[ -z "$(ps | grep "[d]nsmasq ")" ] && /etc/init.d/dnsmasq start
			;;
		"wireless")
			echo '@TR<<Reloading>> @TR<<wireless>> ...'
			wifi ;;
		"webifopenvpn")
			echo '@TR<<Reloading>> @TR<<OpenVPN>> ...'
			if [ ! -e S??webifopenvpn ]; then
				/etc/init.d/webifopenvpn enable
			fi
			/etc/init.d/webifopenvpn restart ;;
		"system")
			unset system_cfg
			config_load system
			reset_cb
			config_get hostname "$system_cfg" hostname
			echo "${hostname:-OpenWrt}" > /proc/sys/kernel/hostname
			echo "If you made changes to the log settings please reboot for them to take effect!"
			sleep 5
			config_allclear
			;;
		"l2tpns")
			echo '@TR<<Exporting>> @TR<<l2tpns server settings>> ...'
			[ -x "/usr/lib/webif/l2tpns_apply.sh" ] && {
				/usr/lib/webif/l2tpns_apply.sh >&- 2>&-
			}

			unset l2tpns_cfg
			uci_load "l2tpns"
			reset_cb
			config_get test "$l2tpns_cfg" mode
			if [ "$test" = "enabled" ]; then
				echo '@TR<<Starting>> @TR<<l2tpns server>> ...'
				/etc/init.d/l2tpns enable >&- 2>&- <&-
				/etc/init.d/l2tpns start >&- 2>&- <&-
			else
				echo '@TR<<Stopping>> @TR<<l2tpns server>> ...'
				/etc/init.d/l2tpns disable >&- 2>&- <&-
				/etc/init.d/l2tpns stop >&- 2>&- <&-
			fi
			config_allclear
			;;
		"updatedd")
			uci_load "updatedd"
			config_get_bool test "$updatedd_cfg" update 0
			if [ 1 -eq "$test" ]; then
				/etc/init.d/updatedd enable >&- 2>&- <&-
				/etc/init.d/updatedd stop >&- 2>&- <&-
				/etc/init.d/updatedd start >&- 2>&- <&-
			else
				/etc/init.d/updatedd disable >&- 2>&- <&-
				/etc/init.d/updatedd stop >&- 2>&- <&-
			fi
			config_allclear
		 	;;
		"timezone")
			echo '@TR<<Exporting>> @TR<<TZ setting>> ...'
			[ ! -f /etc/rc.d/S??timezone ] && /etc/init.d/timezone enable >&- 2>&- <&-
			/etc/init.d/timezone restart
			;;
		"firewall")
			/etc/init.d/firewall restart && reload_upnpd
			;;
		"httpd")
			[ -e "/etc/init.d/httpd" ] && /etc/init.d/httpd restart
			[ -e "/etc/init.d/uhttpd" ] && /etc/init.d/uhttpd restart
#			/etc/init.d/httpd restart
			;;
		"uhttpd")
			/etc/init.d/uhttpd restart
			;;
		"webif_access_control")
			rm -fr /tmp/.webcache/*
		;;
	esac
done

#
# commit tarfs if exists
#
[ -f "/rom/local.tar" ] && config save

#
# cleanup
#
cd "$pushed_dir"
rm /tmp/.webif/* >&- 2>&-
rm /tmp/.uci/* >&- 2>&-
if [ "$HTTP_RESTART" = "1" ]; then
	[ -e "/etc/init.d/httpd" ] && /etc/init.d/httpd restart
	[ -e "/etc/init.d/uhttpd" ] && /etc/init.d/uhttpd restart
fi
