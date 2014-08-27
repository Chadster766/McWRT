#!/bin/sh /etc/rc.common
# Copyright (C) 2006 OpenWrt.org

START=50

config_cb() {
	local cfg="$CONFIG_SECTION"
	local cfgt
	config_get cfgt "$cfg" TYPE

	case "$cfgt" in
		ftpd-topfield)
			config_get turbo $cfg turbo
			config_get port $cfg port
			config_get elpf $cfg elpf

			case "$turbo" in
				yes|on|enabled|1) turbo=1;;
			esac
			case "$elpf" in
				yes|on|enabled|1) elpf=1;;
			esac
			TOPFIELD_ARGS="-D ${turbo:+--turbo }${port:+-P $port }${elpf:+-E}"
		;;
	esac
}
                                                                                                                                                                                                            
start() {
	config_load ftpd-topfield
	/usr/sbin/ftpd-topfield $TOPFIELD_ARGS 
}
  
stop() {
    killall ftpd-topfield
}
