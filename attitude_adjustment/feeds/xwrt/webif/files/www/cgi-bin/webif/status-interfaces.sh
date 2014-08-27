#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh

header "Status" "Interfaces" "@TR<<Interfaces>>"

config_cb() {
	cfg_type="$1"
	cfg_name="$2"
}
option_cb() {
	local var_name="$1"; shift
	local var_value="$*"

	case "$cfg_type" in
		dnsmasq)
			case "$var_name" in
				resolvfile) resolvfile="$var_value" ;;
			esac
		;;
		aiccu)
			case "$var_name" in
				interface)  aiccu_iface="$var_value" ;;
			esac
		;;
	esac
}
config_load dhcp
config_load aiccu
reset_cb

config_load network
for cfgsec in $CONFIG_SECTIONS; do
	eval "cfgtype=\$CONFIG_${cfgsec}_TYPE"
	[ "$cfgtype" = "interface" ] && {
		iflow=$(echo "$cfgsec" | tr [A-Z] [a-z])
		ifupr=$(echo "$cfgsec" | tr [a-z] [A-Z])
		eval "${iflow}_namen=\"$ifupr\""
		eval "typebr=\"\$CONFIG_${cfgsec}_type\""
		if [ "$typebr" =  "bridge" ]; then
			eval "${iflow}_ifacen=\"br-${cfgsec}\""
			eval "${iflow}_bridgen=\"1\""
		else
			eval "${iflow}_ifacen=\"\$CONFIG_${cfgsec}_ifname\""
		fi
		if [ "$iflow" != "wan" -a "$iflow" != "lan" ]; then
			frm_ifaces="$frm_ifaces $iflow"
		fi
	}
done

config_load wireless
for cfgsec in $CONFIG_SECTIONS; do
	eval "cfgtype=\$CONFIG_${cfgsec}_TYPE"
	[ "$cfgtype" = "wifi-iface" ] && {
		eval "wdevice=\"\$CONFIG_${cfgsec}_device\""
		eval "manuf=\"\$CONFIG_${wdevice}_type\""
		case "$manuf" in
			atheros)
				ath_cnt=$(( $ath_cnt + 1 ))
				cur_iface=$(printf "ath%d" "$(( $ath_cnt - 1))")
			;;
			*)
				eval "wdcnt=\$${wdevice}_cnt"
				wdcnt=$(( $wdcnt + 1 ))
				eval "${wdevice}_cnt=$wdcnt"
				if [ "$wdcnt" -gt 1 ]; then
					cur_iface=$(printf "$wdevice.%d" "$(( $wdcnt - 1))")
				else
					cur_iface="$wdevice"
				fi
			;;
		esac
		eval "cfgnet=\"\$CONFIG_${cfgsec}_network\""
		eval "isbridge=\"\$${cfgnet}_bridgen\""
		if [ "$isbridge" != "1" ]; then
			eval "${cfgnet}_ifacen=\"${cur_iface}\""
		fi
		frm_wifaces="$frm_wifaces $cur_iface"
	}
done

displaydns() {
	local ifpar="$1"; [ -z "$ifpar" ] && return
	local resconf="${resolvfile}"
	case "$ifpar" in
		wan) [ -z "$resconf" ] && resconf=$(cat /etc/dnsmasq.conf 2>/dev/null | grep "^resolv-file=" | cut -d'=' -f 2) ;;
		lan) resconf="/etc/resolv.conf" ;;
		*) return ;;
	esac
	local iname
	eval iname="\$${ifpar}_namen"; [ -z "$iname" ] && iname="$ifpar"
	cat "$resconf" 2>/dev/null | sed '/^nameserver/!d' | awk -v iname="$iname" '
BEGIN {
	counter = 0
	print "start_form|@TR<<DNS Servers>> ("iname")"
}
{
	counter = counter + 1
	print "field|@TR<<DNS Server>> "counter"|dns_server_"iname"_"counter
	print "string|"$2
}
END {
	if (counter == 0) {
		print "field|@TR<<DNS Server>>|dns_server_"iname"_"counter
		print "string|@TR<<status_interfaces_no_DNS_server#No DNS Server found.>>"
	}
	print "end_form"
}' | display_form
}

displayiface() {
	local ifpar="$1"; [ -z "$ifpar" ] && return
	local iface iname config
	if [ "$2" != "aiccu" ]; then
		eval iface="\$${ifpar}_ifacen"
		[ -z "$iface" ] && return
		eval iname="\$${ifpar}_namen"
		[ -z "iname" ] && iname="$iface"
	else
		iface="$ifpar"
		iname="aiccu"
	fi
	config=$(ifconfig "$iface" 2>/dev/null)
	[ -n "$config" ] && echo "$config" | awk -v iface="$iface" -v iname="$iname" '
function colonstr(strc, nparts, colparts) {
	if ((length(strc) == 0) || (strc !~ /:/)) return ""
	nparts = split(strc, colparts, ":")
	if (nparts != 2) return ""
	else return colparts[2]
}
function int2human(num, pref) {
	if (num == "") return num
	pref = 1000*1000*1000*1000
	if (int(num/pref) > 0) return sprintf("%.2f@TR<<int2human_tera#t>>", num/pref)
	pref = pref / 1000
	if (int(num/pref) > 0) return sprintf("%.2f@TR<<int2human_giga#g>>", num/pref)
	pref = pref / 1000
	if (int(num/pref) > 0) return sprintf("%.2f@TR<<int2human_mega#m>>", num/pref)
	pref = pref / 1000
	if (int(num/pref) > 0) return sprintf("%.2f@TR<<int2human_kilo#k>>", num/pref)
	return sprintf("%d", num)
}
function hardspace(parm) {
	if (parm == "") return "&nbsp;"
	else return parm
}
{
	if ($0 ~ /Link encap:/)	_if["mac"] = hardspace($5)
	else if ($0 ~ /inet addr:/) _if["ip"] = hardspace(colonstr($2))
	else if ($0 ~ /inet6 addr:/) _if6[colonstr($4)] = hardspace($3)
	else if ($0 ~ /MTU:/) {
		for (i=1; i <= NF; i++) {
			if ($i ~ /MTU:/) {
				_if["mtu"] = hardspace(colonstr($i))
				break
			}
		}
	}
	else if ($0 ~ /RX packets:/) _if["rxp"] = hardspace(int2human(colonstr($2)))
	else if ($0 ~ /TX packets:/) _if["txp"] = hardspace(int2human(colonstr($2)))
	else if ($0 ~ /RX bytes:/) {
		_if["rxh"] = $3" "$4
		_if["txh"] = $7" "$8
	}
}
END {
	if (_if["mac"] || _if["ip"]) {
		print "start_form|@TR<<" iname ">>"
		print "field|@TR<<MAC Address>>|" iname "_mac_addr"
		print "string|" _if["mac"]
		print "field|@TR<<IP Address>>|" iname "_ip_addr"
		print "string|" _if["ip"]
		cntr = 0
		for (var in _if6) {
			print "field|@TR<<IPv6 Address>> (@TR<<status_interfaces_ipv6_scope_"var"#"var">>)|"iname"_ipv6_addr_"cntr"_"var
			print "string|" _if6[var]
			cntr = cntr + 1
		}
		print "field|@TR<<Received>>|" iname "_rx"
		print "string|" _if["rxp"] " @TR<<units_packets_pkts#pkts>>&nbsp;" _if["rxh"]
		print "field|@TR<<Transmitted>>|" iname "_tx"
		print "string|" _if["txp"] " @TR<<units_packets_pkts#pkts>>&nbsp;" _if["txh"]
		print "field|@TR<<MTU>>|" iname "_mtu"
		print "string|" _if["mtu"]
		if (iname == "WAN") {
			print "helpitem|WAN"
			print "helptext|WAN WAN#WAN stands for Wide Area Network and is usually the upstream connection to the internet."
			if (wan_timestamp != "") {
				print "helpitem|Duration"
				print "helptext|Duration_helptext#The field displays the time of the connection in case the time was known shortly after establishing the link (+/- several seconds)."
			}
		} else if (iname == "LAN") {
			print "helpitem|LAN"
			print "helptext|LAN LAN#LAN stands for Local Area Network."
		} else if (iname == "LOOPBACK") {
			print "helpitem|LOOPBACK"
			print "helptext|LOOPBACK_helptext#A loopback interface is a type of '\''circuitless IP address'\'' or '\''virtual IP'\'' address, as the IP address is not associated with any one particular interface (or circuit) on the host or router. Any traffic that a computer program sends on the loopback network is addressed to the same computer."
		} else if (iname == "aiccu") {
			print "helpitem|aiccu"
			print "helptext|aiccu_helptext#Automatic IPv6 Connectivity Client Utility used to connect to a IPv6 tunnel broker."
		}
		print "end_form"
	}
}' | display_form
}

displaywiface() {
	local wifpar="$1"; [ -z "$wifpar" ] && return
	local wnum="$2"; [ -z "$wnum" ] && return
	local wconfig=$(iwconfig "$wifpar" 2>/dev/null)
	[ -n "$wconfig" ] && echo "$wconfig" | awk -v wnum="$wnum" '
function colonstr(strc, nparts, colparts) {
	if ((length(strc) == 0) || (strc !~ /:/)) return ""
	nparts = split(strc, colparts, ":")
	if (nparts != 2) return ""
	else return colparts[2]
}
function equalstr(strc, nparts, colparts) {
	if ((length(strc) == 0) || (strc !~ /=/)) return ""
	nparts = split(strc, colparts, "=")
	if (nparts != 2) return ""
	else return colparts[2]
}
function hardspace(parm) {
	if (parm == "") return "&nbsp;"
	else return parm
}
{
	if ($0 ~ /ESSID:/) {
		for (i=1; i <= NF; i++) {
			if ($i ~ /ESSID:/) {
				_wlan["essid"] = hardspace(colonstr($i))
				sub(/^"/, "", _wlan["essid"])
				sub(/"$/, "", _wlan["essid"])
				_wlan["essid"] = hardspace(_wlan["essid"])
				break
			}
		}
	} else if ($0 ~ /Mode:/) {
		_wlan["mode"] = hardspace(colonstr($1))
		_wlan["freq"] = colonstr($2); if (_wlan["freq"] == "") _wlan["freq"] = 0
		_wlan["ap"] = hardspace($6)
	} else if ($0 ~ /Tx-Power=/) {
		for (i=1; i <= NF; i++) {
			if ($i ~ /Rate:/) {
				_wlan["bitrate"] = colonstr($i)
				if (_wlan["bitrate"] == "") _wlan["bitrate"] = 0
			} else if ($i ~ /Tx-Power=/) {
				_wlan["txpwr"] = equalstr($i)
				if (_wlan["txpwr"] == "") _wlan["txpwr"] = 0
			} else if ($i ~ /Sensitivity=/) {
				_wlan["sensitivity"] = equalstr($i)
			}
		}
	} else if ($0 ~ /Retry:/) {
		_wlan["retry"] = hardspace(colonstr($1))
		_wlan["rts"] = hardspace(colonstr($3))
		_wlan["fragment"] = hardspace(colonstr($5))
	} else if ($0 ~ /Encryption key:/) {
		_wlan["key"] = colonstr($2)
		_wlan["secmode"] = colonstr($4)
	} else if ($0 ~ /Power Management:/) {
		_wlan["powermanagement"] = hardspace(colonstr($2))
	} else if ($0 ~ /Link Quality=/) {
		_wlan["quality"] = equalstr($2)
		_wlan["signal"] = equalstr($4)
		if (_wlan["signal"] == "") _wlan["signal"] = 0
		_wlan["noise"] = equalstr($7)
		if (_wlan["noise"] == "") _wlan["noise"] = 0
	} else if ($0 ~ /Rx invalid nwid:/) {
		_wlan["rxinvnwid"] = colonstr($3); if (_wlan["rxinvnwid"] == "") _wlan["rxinvnwid"] = 0
		_wlan["rxinvcrypt"] = colonstr($6); if (_wlan["rxinvcrypt"] == "") _wlan["rxinvcrypt"] = 0
		_wlan["rxinvfrag"] = colonstr($9); if (_wlan["rxinvfrag"] == "") _wlan["rxinvcrypt"] = 0
	} else if ($0 ~ /Tx excessive retries:/) {
		_wlan["txretries"] = colonstr($3); if (_wlan["txretries"] == "") _wlan["txretries"] = 0
		_wlan["txinvalid"] = colonstr($5); if (_wlan["txinvalid"] == "") _wlan["txinvalid"] = 0
		_wlan["txmissed"] = colonstr($7); if (_wlan["txmissed"] == "") _wlan["txmissed"] = 0
	}
}
END {
	print "start_form|@TR<<WLAN>>" ((wnum > 1) ? " "wnum :"")
	print "field|@TR<<ESSID>>|wlan_"wnum"_ssid"
	print "string|" _wlan["essid"]
	print "field|@TR<<Mode>>|wlan_"wnum"_mode"
	print "string|" _wlan["mode"]
	print "field|@TR<<Frequency>>|wlan_"wnum"_freq"
	print "string|" _wlan["freq"] " @TR<<units_GHz#GHz>>"
	print "field|@TR<<Access Point>>|wlan_"wnum"_ap"
	print "string|" _wlan["ap"]
	print "field|@TR<<Bit Rate>>|wlan_"wnum"_bitrate"
	print "string|" _wlan["bitrate"] " @TR<<units_Mbps#Mbps>>"
	print "field|@TR<<Transmit Power>>|wlan_"wnum"_txpwr"
	print "string|" _wlan["txpwr"] " @TR<<units_dBm#dBm>>"
	print "field|@TR<<Sensitivity>>|wlan_"wnum"_sensitivity"
	print "string|" _wlan["sensitivity"]
	print "field|@TR<<Retry>>|wlan_"wnum"_retry"
	print "string|" _wlan["retry"]
	print "field|@TR<<RTS>>|wlan_"wnum"_rts"
	print "string|" _wlan["rts"]
	print "field|@TR<<Fragmentation>>|wlan_"wnum"_fragment"
	print "string|" _wlan["fragment"]
	print "field|@TR<<Encryption Key>>|wlan_"wnum"_key"
	print "string|<div class=\"smalltext\">" _wlan["key"] "</div>"
	print "field|@TR<<Security mode>>|wlan_"wnum"_secmode"
	print "string|" _wlan["secmode"]
	print "field|@TR<<Power Management>>|wlan_"wnum"_powermanagement"
	print "string|" _wlan["powermanagement"]
	print "field|@TR<<Link Quality>>|wlan_"wnum"_quality"
	print "string|" _wlan["quality"]
	print "field|@TR<<Signal Level>>|wlan_"wnum"_signal"
	print "string|" _wlan["signal"] " @TR<<units_dBm#dBm>>"
	print "field|@TR<<Noise Level>>|wlan_"wnum"_noise"
	print "string|" _wlan["noise"] " @TR<<units_dBm#dBm>>"
	print "field|@TR<<Rx Invalid nwid>>|wlan_"wnum"_rx_invalid_nwid"
	print "string|" _wlan["rxinvnwid"]
	print "field|@TR<<Rx Invalid Encryption>>|wlan_"wnum"_rx_invalid_crypt"
	print "string|" _wlan["rxinvcrypt"]
	print "field|@TR<<Tx Retries in Excess>>|wlan_"wnum"_tx_retries"
	print "string|" _wlan["txretries"]
	print "field|@TR<<Tx Invalid>>|wlan_"wnum"_tx_invalid"
	print "string|" _wlan["txinvalid"]
	print "field|@TR<<Tx Missed Beacon>>|wlan_"wnum"_tx_missed"
	print "string|" _wlan["txmissed"]
	print "helpitem|WLAN"
	print "helptext|WLAN LAN#WLAN stands for Wireless Local Area Network."
	print "end_form"
}' | display_form
}

displayiface wan
displaydns wan
displayiface lan
displaydns lan
displayiface $aiccu_iface aiccu
for iface in $frm_ifaces; do
	displayiface $iface
done
cntr=0
for wiface in $frm_wifaces; do
	displaywiface $wiface $cntr
	cntr=$(( $cntr +1 ))
done

#########################################
# raw stats
preinterface() {
	local iface="$1"
	local iname="$2"
	equal "$iface" "" || equal "$iname" "" && return 1
	echo "<tr>"
	case "$iname" in
		WAN) echo "	<th><b>@TR<<Interfaces Status WAN|WAN Interface>></b></th>" ;;
		LAN) echo "	<th><b>@TR<<Interfaces Status LAN|LAN Interface>></b></th>" ;;
		*)   echo "	<th><b>@TR<<Interfaces Status Other|Interface>> $iname</b></th>" ;;
	esac
	echo "</tr>"
	echo "<tr>"
	echo "	<td><div class=\"smalltext\"><pre>"
	ifconfig "$iface" 2>/dev/null
	echo "</pre></div></td>"
	echo "</tr>"
}

prewinterface() {
	local wiface="$1"
	local wnum="$2"; [ "$wnum" -eq 0 ] && wnum=""
	echo "<tr>"
	echo "	<th><b>@TR<<Interfaces Status WLAN|Wireless Interface>> $wnum</b></th>"
	echo "</tr>"
	echo "<tr>"
	echo "	<td><div class=\"smalltext\"><pre>"
	iwconfig "$wiface" 2>/dev/null | grep -v "no wireless"
	echo "</pre></div></td>"
	echo "</tr>"
}

display_form <<EOF
start_form|@TR<<Raw Information>>
EOF
if empty "$FORM_show_raw_stats"; then
	display_form <<EOF
field||show_raw
formtag_begin|raw_stats|$SCRIPT_NAME
submit|show_raw_stats| @TR<<&nbsp;Show raw statistics&nbsp;>>
formtag_end
end_form
EOF
else
	preinterface "$wan_ifacen" "$wan_namen"
	preinterface "$lan_ifacen" "$lan_namen"
	for iface in $frm_ifaces; do
		eval "dispiface=\$${iface}_ifacen"
		[ -n "$dispiface" ] && {
			eval "if_name=\"\$${iface}_namen\""
			preinterface "$dispiface" "$if_name"
		}
	done
	cntr=0
	for wiface in $frm_wifaces; do
		[ -n "$wiface" ] && {
			[ "$cntr" -eq 0 ] && dcntr="" || dcntr=" $cntr"
			prewinterface "$wiface" "$cntr"
		}
		cntr=$(( $cntr +1 ))
	done

	display_form <<EOF
end_form
EOF
fi

footer ?>
<!--
##WEBIF:name:Status:150:Interfaces
-->
