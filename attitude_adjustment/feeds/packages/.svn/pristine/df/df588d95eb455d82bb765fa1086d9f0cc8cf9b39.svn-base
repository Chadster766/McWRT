#!/bin/sh /etc/rc.common
# Copyright (C) 2006-2012 OpenWrt.org

START=50

section_enabled() {
	local enabled
	config_get_bool enabled "$1" 'enabled' 0
	[ $enabled -gt 0 ]
}

append_interface() {
	local name="$1"
	local device
	network_get_device device "$name"
	append args "${device:-$name}"
}

start_instance() {
	local section="$1"
	local permanent
	local interfaces
	local args=""

	section_enabled "$section" || return 1

	config_get_bool permanent "$section" 'permanent' 0
	[ $permanent -eq 0 ] || append args "-p"

	config_list_foreach "$section" 'interfaces' append_interface

	service_start /usr/sbin/parprouted $args
}

start() {
	. /lib/functions/network.sh

	config_load 'parprouted'
	config_foreach start_instance 'parprouted'
}

stop() {
	service_stop /usr/sbin/parprouted
}
