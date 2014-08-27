#!/bin/sh
#
# Copyright (C) 2007 OpenWrt.org
#
#

board_name=""
status_led=""
status_led2=""
sys_mtd_part=""
brcm63xx_has_reset_button=""

brcm63xx_detect() {
	board_name=$(awk 'BEGIN{FS="[ \t:/]+"} /system type/ {print $4}' /proc/cpuinfo)

	if [ "$board_name" = "96358VW" ] && [ -e /proc/switch/eth1/enable ]; then
		board_name="DVAG3810BN"
	fi

	case "$board_name" in
	96328avng)
		status_led="96328avng::power"
		ifname=eth0
		;;
	963281TAN)
		status_led="963281TAN::power"
		ifname=eth0
		;;
	96348GW)
		status_led="power"
		brcm63xx_has_reset_button="true"
		ifname=eth1
		;;
	GW6000)
		brcm63xx_has_reset_button="true"
		ifname=eth1
		;;
	GW6200)
		status_led="line1"
		status_led2="tel"
		brcm63xx_has_reset_button="true"
		ifname=eth1
		;;
	96348GW-11)
		status_led="power"
		brcm63xx_has_reset_button="true"
		ifname=eth1
		;;
	CT536_CT5621)
		status_led="power"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	SPW500V)
		status_led="power:green"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	AW4139 |\
	AW4339U)
		status_led="dsl-274xb:green:power"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	96358-502V)
		status_led="spw303v:green:power+adsl"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	V2110)
		status_led="V2110:power:green"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	CPVA642)
		status_led="power:green"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	D-4P-W)
		status_led="power:green"
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	NB6)
		brcm63xx_has_reset_button="true"
		ifname=eth0
		;;
	*)
		;;
	esac
}

brcm63xx_detect
