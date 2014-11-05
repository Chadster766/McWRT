hostapd_set_bss_options() {
	local var="$1"
	local vif="$2"
	local enc wep_rekey wpa_group_rekey wpa_pair_rekey wpa_master_rekey wps_possible

	config_get enc "$vif" encryption "none"
	config_get wep_rekey        "$vif" wep_rekey        # 300
	config_get wpa_group_rekey  "$vif" wpa_group_rekey  # 300
	config_get wpa_pair_rekey   "$vif" wpa_pair_rekey   # 300
	config_get wpa_master_rekey "$vif" wpa_master_rekey # 640
	config_get_bool ap_isolate "$vif" isolate 0
	config_get_bool disassoc_low_ack "$vif" disassoc_low_ack 1
	config_get max_num_sta "$vif" max_num_sta 0
	config_get max_inactivity "$vif" max_inactivity 0
	config_get_bool preamble "$vif" short_preamble 1

	config_get device "$vif" device
	config_get hwmode "$device" hwmode
	config_get phy "$device" phy

	append "$var" "ctrl_interface=/var/run/hostapd-$phy" "$N"

	if [ "$ap_isolate" -gt 0 ]; then
		append "$var" "ap_isolate=$ap_isolate" "$N"
	fi
	if [ "$max_num_sta" -gt 0 ]; then
		append "$var" "max_num_sta=$max_num_sta" "$N"
	fi
	if [ "$max_inactivity" -gt 0 ]; then
		append "$var" "ap_max_inactivity=$max_inactivity" "$N"
	fi
	append "$var" "disassoc_low_ack=$disassoc_low_ack" "$N"
	if [ "$preamble" -gt 0 ]; then
		append "$var" "preamble=$preamble" "$N"
	fi

	# Examples:
	# psk-mixed/tkip 	=> WPA1+2 PSK, TKIP
	# wpa-psk2/tkip+aes	=> WPA2 PSK, CCMP+TKIP
	# wpa2/tkip+aes 	=> WPA2 RADIUS, CCMP+TKIP
	# ...

	# TODO: move this parsing function somewhere generic, so that
	# later it can be reused by drivers that don't use hostapd

	# crypto defaults: WPA2 vs WPA1
	case "$enc" in
		wpa2*|*psk2*)
			wpa=2
			crypto="CCMP"
		;;
		*mixed*)
			wpa=3
			crypto="CCMP TKIP"
		;;
		*)
			wpa=1
			crypto="TKIP"
		;;
	esac

	# explicit override for crypto setting
	case "$enc" in
		*tkip+aes|*tkip+ccmp|*aes+tkip|*ccmp+tkip) crypto="CCMP TKIP";;
		*aes|*ccmp) crypto="CCMP";;
		*tkip) crypto="TKIP";;
	esac

	# enforce CCMP for 11ng and 11na
	case "$hwmode:$crypto" in
		*ng:TKIP|*na:TKIP) crypto="CCMP TKIP";;
	esac

	# use crypto/auth settings for building the hostapd config
	case "$enc" in
		none)
			wps_possible=1
			wpa=0
			crypto=
			# Here we make the assumption that if we're in open mode
			# with WPS enabled, we got to be in unconfigured state.
			wps_not_configured=1
		;;
		*psk*)
			config_get psk "$vif" key
			if [ ${#psk} -eq 64 ]; then
				append "$var" "wpa_psk=$psk" "$N"
			else
				append "$var" "wpa_passphrase=$psk" "$N"
			fi
			wps_possible=1
			[ -n "$wpa_group_rekey"  ] && append "$var" "wpa_group_rekey=$wpa_group_rekey" "$N"
			[ -n "$wpa_pair_rekey"   ] && append "$var" "wpa_ptk_rekey=$wpa_pair_rekey"    "$N"
			[ -n "$wpa_master_rekey" ] && append "$var" "wpa_gmk_rekey=$wpa_master_rekey"  "$N"
		;;
		*wpa*|*8021x*)
			# required fields? formats?
			# hostapd is particular, maybe a default configuration for failures
			config_get auth_server "$vif" auth_server
			[ -z "$auth_server" ] && config_get auth_server "$vif" server
			append "$var" "auth_server_addr=$auth_server" "$N"
			config_get auth_port "$vif" auth_port
			[ -z "$auth_port" ] && config_get auth_port "$vif" port
			auth_port=${auth_port:-1812}
			append "$var" "auth_server_port=$auth_port" "$N"
			config_get auth_secret "$vif" auth_secret
			[ -z "$auth_secret" ] && config_get auth_secret "$vif" key
			append "$var" "auth_server_shared_secret=$auth_secret" "$N"
			# You don't really want to enable this unless you are doing
			# some corner case testing or are using OpenWrt as a work around
			# for some systematic issues.
			config_get_bool auth_cache "$vif" auth_cache 0
			config_get rsn_preauth "$vif" rsn_preauth
			[ "$auth_cache" -gt 0 ] || [[ "$rsn_preauth" = 1 ]] || append "$var" "disable_pmksa_caching=1" "$N"
			[ "$auth_cache" -gt 0 ] || [[ "$rsn_preauth" = 1 ]] || append "$var" "okc=0" "$N"
			config_get acct_server "$vif" acct_server
			[ -n "$acct_server" ] && append "$var" "acct_server_addr=$acct_server" "$N"
			config_get acct_port "$vif" acct_port
			[ -n "$acct_port" ] && acct_port=${acct_port:-1813}
			[ -n "$acct_port" ] && append "$var" "acct_server_port=$acct_port" "$N"
			config_get acct_secret "$vif" acct_secret
			[ -n "$acct_secret" ] && append "$var" "acct_server_shared_secret=$acct_secret" "$N"
			config_get eap_reauth_period "$vif" eap_reauth_period
			[ -n "$eap_reauth_period" ] && append "$var" "eap_reauth_period=$eap_reauth_period" "$N"
			config_get dae_client "$vif" dae_client
			config_get dae_secret "$vif" dae_secret
			[ -n "$dae_client" -a -n "$dae_secret" ] && {
				config_get dae_port  "$vif" dae_port
				append "$var" "radius_das_port=${dae_port:-3799}" "$N"
				append "$var" "radius_das_client=$dae_client $dae_secret" "$N"
			}
			config_get nasid "$vif" nasid
			config_get ownip "$vif" ownip
			append "$var" "nas_identifier=$nasid" "$N"
			append "$var" "own_ip_addr=$ownip" "$N"
			append "$var" "eapol_key_index_workaround=1" "$N"
			append "$var" "ieee8021x=1" "$N"
			append "$var" "wpa_key_mgmt=WPA-EAP" "$N"
			[ -n "$wpa_group_rekey"  ] && append "$var" "wpa_group_rekey=$wpa_group_rekey" "$N"
			[ -n "$wpa_pair_rekey"   ] && append "$var" "wpa_ptk_rekey=$wpa_pair_rekey"    "$N"
			[ -n "$wpa_master_rekey" ] && append "$var" "wpa_gmk_rekey=$wpa_master_rekey"  "$N"
		;;
		*wep*)
			config_get key "$vif" key
			key="${key:-1}"
			case "$key" in
				[1234])
					for idx in 1 2 3 4; do
						local zidx
						zidx=$(($idx - 1))
						config_get ckey "$vif" "key${idx}"
						[ -n "$ckey" ] && \
							append "$var" "wep_key${zidx}=$(prepare_key_wep "$ckey")" "$N"
					done
					append "$var" "wep_default_key=$((key - 1))"  "$N"
				;;
				*)
					append "$var" "wep_key0=$(prepare_key_wep "$key")" "$N"
					append "$var" "wep_default_key=0" "$N"
					[ -n "$wep_rekey" ] && append "$var" "wep_rekey_period=$wep_rekey" "$N"
				;;
			esac
			case "$enc" in
				*shared*)
					auth_algs=2
				;;
				*mixed*)
					auth_algs=3
				;;
			esac
			wpa=0
			crypto=
		;;
		*)
			wpa=0
			crypto=
		;;
	esac
	append "$var" "auth_algs=${auth_algs:-1}" "$N"
	append "$var" "wpa=$wpa" "$N"
	[ -n "$crypto" ] && append "$var" "wpa_pairwise=$crypto" "$N"
	[ -n "$wpa_group_rekey" ] && append "$var" "wpa_group_rekey=$wpa_group_rekey" "$N"

	config_get ssid "$vif" ssid
	config_get bridge "$vif" bridge
	config_get ieee80211d "$vif" ieee80211d
	config_get iapp_interface "$vif" iapp_interface

	config_get_bool wps_pbc "$vif" wps_pushbutton 0
	config_get_bool wps_label "$vif" wps_label 0

	config_get config_methods "$vif" wps_config
	[ "$wps_pbc" -gt 0 ] && append config_methods push_button

	[ -n "$wps_possible" -a -n "$config_methods" ] && {
		config_get device_type "$vif" wps_device_type "6-0050F204-1"
		config_get device_name "$vif" wps_device_name "OpenWrt AP"
		config_get manufacturer "$vif" wps_manufacturer "openwrt.org"
		config_get wps_pin "$vif" wps_pin

		config_get_bool ext_registrar "$vif" ext_registrar 0
		[ "$ext_registrar" -gt 0 -a -n "$bridge" ] && append "$var" "upnp_iface=$bridge" "$N"

		append "$var" "eap_server=1" "$N"
		[ -n "$wps_pin" ] && append "$var" "ap_pin=$wps_pin" "$N"
		append "$var" "wps_state=${wps_not_configured:-2}" "$N"
		append "$var" "ap_setup_locked=0" "$N"
		append "$var" "device_type=$device_type" "$N"
		append "$var" "device_name=$device_name" "$N"
		append "$var" "manufacturer=$manufacturer" "$N"
		append "$var" "config_methods=$config_methods" "$N"
	}

	append "$var" "ssid=$ssid" "$N"
	[ -n "$bridge" ] && append "$var" "bridge=$bridge" "$N"
	[ -n "$ieee80211d" ] && append "$var" "ieee80211d=$ieee80211d" "$N"
	[ -n "$iapp_interface" ] && append "$var" iapp_interface=$(uci_get_state network "$iapp_interface" ifname "$iapp_interface") "$N"

	if [ "$wpa" -ge "2" ]
	then
		# RSN -> allow preauthentication. You have two
		# options, rsn_preauth for production or rsn_preauth_testing
		# for validation / testing.
		if [ -n "$bridge" -a "$rsn_preauth" = 1 ]
		then
			append "$var" "rsn_preauth=1" "$N"
			append "$var" "rsn_preauth_interfaces=$bridge" "$N"
			append "$var" "okc=1" "$N"
		else
			# RSN preauthentication testings hould disable
			# Opportunistic Key Caching (okc) as otherwise the PMKSA
			# entry for a test could come from the Opportunistic Key Caching
			config_get rsn_preauth_testing "$vif" rsn_preauth_testing
			if [ -n "$bridge" -a "$rsn_preauth_testing" = 1 ]
			then
				append "$var" "rsn_preauth=1" "$N"
				append "$var" "rsn_preauth_interfaces=$bridge" "$N"
				append "$var" "okc=0" "$N"
			fi
		fi

		# RSN -> allow management frame protection
		config_get ieee80211w "$vif" ieee80211w
		case "$ieee80211w" in
			[012])
				append "$var" "ieee80211w=$ieee80211w" "$N"
				[ "$ieee80211w" -gt "0" ] && {
					config_get ieee80211w_max_timeout "$vif" ieee80211w_max_timeout
					config_get ieee80211w_retry_timeout "$vif" ieee80211w_retry_timeout
					[ -n "$ieee80211w_max_timeout" ] && \
						append "$var" "assoc_sa_query_max_timeout=$ieee80211w_max_timeout" "$N"
					[ -n "$ieee80211w_retry_timeout" ] && \
						append "$var" "assoc_sa_query_retry_timeout=$ieee80211w_retry_timeout" "$N"
				}
			;;
		esac
	fi

	config_get macfile "$vif" macfile
	config_get maclist "$vif" maclist
	if [ -z "$macfile" ]
	then
		# if no macfile has been specified, fallback to the default name
		# and truncate file to avoid aggregating entries over time
		macfile="/var/run/hostapd-$ifname.maclist"
		echo "" > "$macfile"
	else
		if [ -n "$maclist" ]
		then
			# to avoid to overwrite the original file, make a copy
			# before appending the entries specified by the maclist
			# option
			cp $macfile $macfile.maclist
			macfile=$macfile.maclist
		fi
	fi

	if [ -n "$maclist" ]
	then
		for mac in $maclist; do
			echo "$mac" >> $macfile
		done
	fi

	config_get macfilter "$vif" macfilter
	case "$macfilter" in
		allow)
			append "$var" "macaddr_acl=1" "$N"
			append "$var" "accept_mac_file=$macfile" "$N"
			;;
		deny)
			append "$var" "macaddr_acl=0" "$N"
			append "$var" "deny_mac_file=$macfile" "$N"
			;;
	esac
}

hostapd_set_log_options() {
	local var="$1"
	local cfg="$2"
	local log_level log_80211 log_8021x log_radius log_wpa log_driver log_iapp log_mlme

	config_get log_level "$cfg" log_level 2

	config_get_bool log_80211  "$cfg" log_80211  1
	config_get_bool log_8021x  "$cfg" log_8021x  1
	config_get_bool log_radius "$cfg" log_radius 1
	config_get_bool log_wpa    "$cfg" log_wpa    1
	config_get_bool log_driver "$cfg" log_driver 1
	config_get_bool log_iapp   "$cfg" log_iapp   1
	config_get_bool log_mlme   "$cfg" log_mlme   1

	local log_mask=$((       \
		($log_80211  << 0) | \
		($log_8021x  << 1) | \
		($log_radius << 2) | \
		($log_wpa    << 3) | \
		($log_driver << 4) | \
		($log_iapp   << 5) | \
		($log_mlme   << 6)   \
	))

	append "$var" "logger_syslog=$log_mask" "$N"
	append "$var" "logger_syslog_level=$log_level" "$N"
	append "$var" "logger_stdout=$log_mask" "$N"
	append "$var" "logger_stdout_level=$log_level" "$N"
}

hostapd_setup_vif() {
	local vif="$1"
	local driver="$2"
	local ifname device channel hwmode

	hostapd_cfg=

	config_get ifname "$vif" ifname
	config_get device "$vif" device
	config_get channel "$device" channel
	config_get hwmode "$device" hwmode

	hostapd_set_log_options hostapd_cfg "$device"
	hostapd_set_bss_options hostapd_cfg "$vif"

	case "$hwmode" in
		*bg|*gdt|*gst|*fh) hwmode=g;;
		*adt|*ast) hwmode=a;;
	esac
	[ "$channel" = auto ] && channel=
	[ -n "$channel" -a -z "$hwmode" ] && wifi_fixup_hwmode "$device"
	cat > /var/run/hostapd-$ifname.conf <<EOF
driver=$driver
interface=$ifname
${hwmode:+hw_mode=${hwmode#11}}
${channel:+channel=$channel}
$hostapd_cfg
EOF
	hostapd -P /var/run/wifi-$ifname.pid -B /var/run/hostapd-$ifname.conf
}

