#!/bin/sh
# Copyright (C) 2014 OpenWrt.org

. /lib/functions/leds.sh
. /lib/mvebu.sh

get_status_led() {
	case $(mvebu_board_name) in
	armada-xp-mamba)
		status_led="mamba:white:power"
		;;
	esac
}

set_state() {
	get_status_led

	case "$1" in
	preinit)
		status_led_blink_preinit
		;;
	failsafe)
		status_led_blink_failsafe
		;;
	done)
		status_led_on
		;;
	esac
}
