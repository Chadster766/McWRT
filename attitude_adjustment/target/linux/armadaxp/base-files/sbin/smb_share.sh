#!/bin/sh
source /etc/functions.sh
source /etc/init.d/samba 
smb_share_lock_file=/tmp/smb_share

smb_share_reload_config (){
	config_load samba
	config_foreach smb_header samba
	config_foreach smb_add_share sambashare
	killall -HUP smbd nmbd
}

smb_share_add (){
	# "device" is /dev/sdaX for example
	local device=$1
	target_dev_name=`echo $device | sed -e "s/\/dev\///"`
	lock ${smb_share_lock_file}
	# check if it already exists
	total_smbshare=`uci show samba | grep "=sambashare" | wc -l`
	total_smbshare=$((total_smbshare - 1))
	for i in `seq 0 ${total_smbshare}`
	do
		dev_name=`uci get samba.@sambashare[$i].name`
		if [ "${dev_name}" = "${target_dev_name}" ]
		then
			lock -u ${smb_share_lock_file}
			return
		fi
	done
	# check if path really exists
	if [ -d /mnt/${target_dev_name} ]
	then
		chmod 777 /mnt/${target_dev_name}
	else
		lock -u ${smb_share_lock_file}
		return
	fi
	# add new sambashare config
	uci add samba sambashare > /dev/null
	uci set samba.@sambashare[-1]=sambashare
	uci set samba.@sambashare[-1].name=${target_dev_name}
	uci set samba.@sambashare[-1].path=/mnt/${target_dev_name}
	uci set samba.@sambashare[-1].read_only=no
	uci set samba.@sambashare[-1].guest_ok=yes
	uci set samba.@sambashare[-1].create_mask='0777'
	uci set samba.@sambashare[-1].dir_mask='0777'
	uci commit samba
	smb_share_reload_config
	lock -u ${smb_share_lock_file}
}

smb_share_remove (){
	# "device" is /dev/sdaX for example
	local device=$1
	target_dev_name=`echo $device | sed -e "s/\/dev\///"`
	lock ${smb_share_lock_file}
	total_smbshare=`uci show samba | grep "=sambashare" | wc -l`
	total_smbshare=$((total_smbshare - 1))
	for i in `seq 0 ${total_smbshare}`
	do
		dev_name=`uci get samba.@sambashare[$i].name`
		if [ "${dev_name}" = "${target_dev_name}" ]
		then
			uci delete samba.@sambashare[$i]
			uci commit samba
			smb_share_reload_config
			break
		fi
	done
	lock -u ${smb_share_lock_file}
}

if [ $# -lt 2 ]
then
        echo "smb_share add|remove dev_path"
        echo "dev_path is /dev/sda2 for example"
        exit 0
fi

case "$1" in
add)
	smb_share_add $2
	;;
remove)
	smb_share_remove $2
	;;
*)
	echo "smb_share add|remove dev_path"
	echo "dev_path is /dev/sda2 for example"
esac
