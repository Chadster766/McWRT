#!/bin/sh

[ -f "/etc/tinc/chaos/rsa_key.pub" -a -f "/etc/tinc/chaos/rsa_key.priv" ] || {
	echo "please generate rsa key pair"
	echo "tincd -n chaos --generate-keys=2048"
	exit 1
}

C=`grep unconfigured_please_change_me /etc/tinc/chaosvpn.conf  | wc -l`
[ "$C" = "0" ] || {
	echo "/etc/tinc/chaosvpn.conf is not configured yet"
	exit 1
}
exit 0
