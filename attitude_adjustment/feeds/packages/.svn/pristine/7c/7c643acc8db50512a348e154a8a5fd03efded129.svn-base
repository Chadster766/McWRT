#!/bin/sh

. /etc/functions.sh
. ../netifd-proto.sh
init_proto "$@"

proto_wing_init_config() {
	no_device=1
	available=1
        proto_config_add_string "ipaddr"
	proto_config_add_string "netmask"
}

proto_wing_teardown() {
	local config="$1"
	local link="wing-$config"
	[ -f "/var/run/$link.pid" ] && {
		kill -9 $(cat /var/run/$link.pid)
		rm /var/run/$link.pid
	}
	env -i ACTION="ifdown" INTERFACE="$config" DEVICE="$link" PROTO=wing /sbin/hotplug-call "link" &
}

proto_wing_setup() {

	local iface="$2"
	local config="$1"
	local link="wing-$config"

	local hwmodes=""
	local freqs=""
	local ifnames=""
	local hwaddrs=""

	# temporary hack waiting for a way to delay wing interfaces until the
	# wifi sub-system has been brought up
	sleep 15

	config_load wireless
	config_foreach wing_list_interfaces wifi-iface

	# start click router
	if [ "$hwmodes" = "" -o "$freqs" = "" -o "$ifnames" = "" -o "$hwaddrs" = "" ]; then
		logger -t "$config" "No raw interfaces available. Exiting."
		exit 1
	fi

	local profile rc ls metric prefix period tau debug

	config_load network
	config_get profile $config profile "bulk"
	config_get rc $config rc "minstrel"
	config_get ls $config ls "fcfs"
	config_get metric $config metric "wcett"
	config_get prefix $config prefix "6"
	config_get period $config period "10000"
	config_get tau $config tau "100000"
	config_get_bool debug $config debug "false"

	local hwaddr=$(echo $hwaddrs | sed 's/ .*//');
	local ipaddr=$(printf "$prefix.%d.%d.%d" $(echo $hwaddr | awk -F: '{printf "0x%s 0x%s 0x%s",$4,$5,$6}'))
	local bcast="$prefix.255.255.255"
	local netmask=255.0.0.0

	if ! wing_template_available "profile" "$profile"; then
		logger -t "$config" "Unable to configure router. Exiting."
		exit 1
	fi

	if ! wing_template_available "rc" "$rc"; then
		logger -t "$config" "Unable to configure rate control. Exiting."
		exit 1
	fi

	if ! wing_template_available "ls" "$ls"; then
		logger -t "$config" "Unable to configure link scheduler. Exiting."
		exit 1
	fi

	if [ "$profile" = "" -o "$rc" = "" ]; then
		logger -t "$config" "Unable to generate template. Exiting."
		exit 1
	fi

	[ "$debug" == 0 ] && dbg="" || dbg="-d"

	export CLICK_BACKTRACE=1

	/usr/bin/click_config -p $profile -r $rc -s $ls -l $metric \
		-m "$hwmodes" -c "$freqs" -n "$ifnames" -a "$hwaddrs" $dbg \
		| sed -e "s/__XR_IFNAME__/$link/g" \
		| sed -e "s/__XR_IP__/$ipaddr/g" \
		| sed -e "s/__XR_BCAST__/$bcast/g" \
		| sed -e "s/__XR_NM__/$netmask/g" \
		| sed -e "s/__XR_PERIOD__/$period/g" \
		| sed -e "s/__XR_TAU__/$tau/g" > /tmp/$link.click

	/usr/bin/click-align /tmp/$link.click > /tmp/$link-aligned.click 2>/var/log/$link.log
	[ ! -c /dev/net/tun ] && {
		mkdir -p /dev/net/
		mknod /dev/net/tun c 10 200
		if [ ! -c /dev/net/tun ]; then
			logger -t "$config" "Device not available (/dev/net/tun). Exiting."
			exit 1
		fi
	}

	(/usr/bin/click /tmp/$link-aligned.click >> /var/log/$link.log 2>&1 &) &
	sleep 2
	ps | grep /usr/bin/click | grep -q -v grep || {
		logger -t "$config" "Unable to start click. Exiting."
		exit 1
	}

	ps | grep /usr/bin/click | grep -v grep | awk '{print $1}' > /var/run/$link.pid

        uci_set_state network $config ifname "$iface"
        uci_set_state network $config ipaddr "$ipaddr"
        uci_set_state network $config netmask "$netmask"
        uci_set_state network $config gateway "0.0.0.0"

	env -i ACTION="ifup" INTERFACE="$config" DEVICE="$link" PROTO=wing /sbin/hotplug-call "link" &

	proto_init_update "$link" 1
	proto_add_ipv4_address "$ipaddr" "$netmask"
	proto_add_ipv4_route "$prefix.0.0.0" "255.0.0.0" "$iface"

	route -n | grep -q '^0.0.0.0' || {
		proto_add_ipv4_route "0.0.0.0" "0" "$iface"
	}
	
	proto_send_update "$config"

}

wing_template_available() { # prefix, template, default
	local template="/etc/wing/$1.$2.click"
	[ ! -f $template ] && {
		return 1
	}
	return 0
}

wing_list_interfaces() {
	local channel freq hwmode hwaddr ifname mode
	config_get mode $1 mode
	config_get_bool up $1 up
	[ "$up" = "1" -a "$mode" = "monitor" ] || return 0
	config_get ifname $1 ifname
	config_get device $1 device
	config_get hwmode $device hwmode "11bg"
	config_get channel $device channel "0"
	[ "$channel" = "0" -o "$channel" = "auto" ] && {
		logger -t "$device" "Channel not specified. Ignoring."
		return 0
	}
	freq=$(iwlist $ifname freq | sed -n "s/^.*Channel 0*$channel : \([0-9.]*\).*/\1/p" | awk '{print $1*1000}')
	hwaddr=$(/sbin/ifconfig $ifname 2>&1 | sed -n 's/^.*HWaddr \([0-9A-Za-z\-]*\).*/\1/p' | sed -e 's/\-/:/g' | cut -c1-17)
	freqs=${freqs:+"$freqs "}$freq
	hwmodes=${hwmodes:+"$hwmodes "}$hwmode
	hwaddrs=${hwaddrs:+"$hwaddrs "}$hwaddr
	ifnames=${ifnames:+"$ifnames "}$ifname
	/sbin/ifconfig $ifname mtu 1900
	/sbin/ifconfig $ifname txqueuelen 5
	/sbin/ifconfig $ifname up
}

add_protocol wing

