#
# Copyright (C) 2011 OpenWrt.org
#

. /lib/functions.sh

get_update_kernel_label() {
	cur_boot_part=`/usr/sbin/fw_printenv -n boot_part`
	kernel_label=""
	if [ "$cur_boot_part" -eq 1 ]
	then
		# current primary boot - update alt boot
		kernel_label="alt_kernel"
		fw_setenv boot_part 2
		fw_setenv bootcmd "run altnandboot"
	elif [ "$cur_boot_part" -eq 2 ]
	then
		# current alt boot - update primary boot
		kernel_label="kernel"
		fw_setenv boot_part 1
		fw_setenv bootcmd "run nandboot"
	else
		# try to guess from bootarg, should not come here
		grep -q "mtdblock5" /proc/cmdline && next_boot_part=2 \
			&& kernel_label="alt_kernel" && next_bootcmd="run altnandboot"
		grep -q "mtdblock7" /proc/cmdline && next_boot_part=1 \
			&& kernel_label="kernel" && next_bootcmd="run nandboot"
		fw_setenv boot_part $next_boot_part
		fw_setenv bootcmd $next_bootcmd
	fi
	echo "$kernel_label"
}

platform_do_upgrade () {
	mkdir -p /var/lock
	touch /var/lock/fw_printenv.lock
	local kern_label=$(get_update_kernel_label)

	if [ ! -n "$kern_label" ]
	then
		echo "cannot find kernel partition"
		exit 1
	fi
	echo "Mamba do upgrade on $kern_label"
	mtdpart="$(find_mtd_part $kern_label)"
	mtdpart_idx="$(echo $mtdpart | tr -d "/dev/mtdblock")"
	flash_erase /dev/mtd${mtdpart_idx} 0 0
	get_image "$1" | nandwrite /dev/mtd${mtdpart_idx} -
}

platform_check_image() {
	# return 0 on valid image, 1 otherwise
	echo "Mamba check image"
	return 0
}

mamba_preupgrade() {
	echo "Mamba preupgrade called..."
	export RAMFS_COPY_BIN="${RAMFS_COPY_BIN} /usr/sbin/fw_printenv /usr/sbin/fw_setenv"
	export RAMFS_COPY_BIN="${RAMFS_COPY_BIN} /usr/sbin/flash_erase /usr/sbin/nandwrite /usr/bin/tr"
	export RAMFS_COPY_BIN="${RAMFS_COPY_BIN} /bin/mkdir /bin/touch"
	export RAMFS_COPY_DATA="${RAMFS_COPY_DATA} /etc/fw_env.config /var/lock/fw_printenv.lock"

	rm -rf /overlay/*
	# populate the backup configuration files to overlay ubifs
	if [ -f "$CONF_TAR" -a "$SAVE_CONFIG" -eq 1 ]
	then
		tar -C /overlay/ -x${TAR_V}zf "$CONF_TAR"
	fi

	# umount overlay partition now
	umount -l /overlay
	umount -l /tmp/syscfg
}

append sysupgrade_pre_upgrade mamba_preupgrade
