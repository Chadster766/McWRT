#!/bin/sh

[ "${ACTION}" = "released" ] || exit 0

. /lib/functions.sh

case "${BUTTON}" in
	BTN_0)
		logger "reset pressed"
		sync
		reboot
		;;
	BTN_1)
		logger "factory pressed"
		jffs2_mark_erase "rootfs_data"
		sync
		reboot
		;;
	*)
		logger "unknown button ${BUTTON}"
		;;
esac
