#!/bin/sh
# Copyright (C) 2011 Openwrt.org
# Copyright (C) 2011 Daniel Dickinson <openwrt@cshore.neomailbox.net>

BXBK_CONFIG_PATH=/var/etc/boxbackup
PID_FILE=/var/run/bbstored.pid

EXTERNAL_CONFIG=0

EXTERNAL_CONF_FILE=
RUNAS_USER=
RUNAS_GROUP=
BXBK_RAIDFILE_CONF=

bbstored_global() {
	local cfg="$1"
	local get_config="$2"

	local external_config
	local raidfileconf
	local user
	local group
	local config_path
	local pidfile
	local accountdb
	local extended_logging
	local housekeep_time
	local address
	local certfile
	local keyfile
	local cafile

	config_get external_config "$cfg" external_config
	if [ -n "$external_config" ]; then
		EXTERNAL_CONFIG=1
		EXTERNAL_CONF_FILE="$external_config"
	fi

	config_get config_path "$cfg" config_path
	[ -n "$config_path" ] && {
		BXBK_CONFIG_PATH="$config_path"
	}

	[ "$get_config" = "1" ] && return 0
	[ -z "$BXBK_CONFIG_FILE" ] && return 1
	
	rm -f "$BXBK_CONFIG_FILE"
	touch "$BXBK_CONFIG_FILE"

	config_get raidfileconf "$cfg" raidfileconf "$BXBK_CONFIG_PATH/raidfile.conf"
	echo "RaidFileConf = $raidfileconf" >>$BXBK_CONFIG_FILE
	BXBK_RAIDFILE_CONF="$raidfileconf"

	config_get accountdb "$cfg" accountdb "/etc/bbstored/accounts.txt"
	echo "AccountDatabase = $accountdb" >>$BXBK_CONFIG_FILE
	[ ! -r "$accountdb" ] && {
		echo "Account database missing"
		NORUN=1
		return 1
	}

	local extlog
	config_get extended_logging "$cfg" extended_logging 1
	if [ "$extended_logging" = "1" ]; then
		extlog=yes
	else
		extlog=no
	fi
	echo "ExtendedLogging = $extlog" >>$BXBK_CONFIG_FILE

	config_get housekeep_time "$cfg" housekeep_time 900
	echo "TimeBetweenHousekeeping = $housekeep_time" >>$BXBK_CONFIG_FILE	

	echo "" >>$BXBK_CONFIG_FILE
	echo "Server" >>$BXBK_CONFIG_FILE
	echo "{" >>$BXBK_CONFIG_FILE

	config_get user "$cfg" user 
	config_get group "$cfg" group
	[ -n "$user" ] && [ "$group" ] && {
		RUNAS_USER=$user
		RUNAS_GROUP=$group
	}
	echo "    User = ${RUNAS_USER:-nobody}" >>$BXBK_CONFIG_FILE
	
	config_get address "$cfg" address
	[ -z "$address" ] && NORUN=1

	echo "    ListenAddresses = inet:$address" >>$BXBK_CONFIG_FILE
	
	config_get certfile "$cfg" certfile
	[ -z  "$certfile" ] && NORUN=1

	echo "    CertificateFile = $certfile" >>$BXBK_CONFIG_FILE

	config_get keyfile "$cfg" keyfile
	[ -z  "$keyfile" ] && NORUN=1

	echo "    PrivateKeyFile = $keyfile" >>$BXBK_CONFIG_FILE

	config_get cafile "$cfg" cafile
	[ -z  "$cafile" ] && NORUN=1

	echo "    TrustedCAsFile = $cafile" >>$BXBK_CONFIG_FILE

	config_get pidfile "$cfg" pidfile 
	[ -n "$pidfile" ] && {
		PID_FILE="$pidfile"
	}
	echo "    PidFile = $PID_FILE" >>$BXBK_CONFIG_FILE

	echo "}" >>$BXBK_CONFIG_FILE
}

raidfile_section() {
	local cfg="$1"

	local setnum
	local blocksize
	local path
	
	config_get setnum "$cfg" setnum
	[ -z "$setnum" ] && return 1

	config_get blocksize "$cfg" blocksize

	config_get path "$cfg" path
	[ -z "$path" ] && return 1

	echo "disc${setnum}" >>$BXBK_RAIDFILE_CONF
	echo "{" >>$BXBK_RAIDFILE_CONF
	echo "    SetNumber = ${setnum}" >>$BXBK_RAIDFILE_CONF
	[ -n "$blocksize" ] && echo "    BlockSize = ${blocksize}" >>$BXBK_RAIDFILE_CONF
	for i in 0 1 2; do
		echo "    Dir${i} = $path" >>$BXBK_RAIDFILE_CONF
	done

	echo "}" >>$BXBK_RAIDFILE_CONF

}

create_config() {
	config_load bbstored
	config_foreach bbstored_global bbstored 1

	if [ "$EXTERNAL_CONFIG" -eq 0 ]
	then
		mkdir -p "$BXBK_CONFIG_PATH/bbstored"
		BXBK_CONFIG_FILE="$BXBK_CONFIG_PATH/bbstored.conf"
		touch "$BXBK_CONFIG_FILE"

		config_load bbstored
		config_foreach bbstored_global bbstored

		[ -z "$BXBK_RAIDFILE_CONF" ] && return 1
		rm -f "$BXBK_RAIDFILE_CONF"
		touch "$BXBK_RAIDFILE_CONF"
	
		config_foreach raidfile_section raidfile

		chown -R ${RUNAS_USER:-nobody}:${RUNAS_GROUP:-nogroup} "$BXBK_CONFIG_PATH"
	fi
}

