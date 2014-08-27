#!/usr/bin/webif-page
<?
# from: http://coova.org
. /usr/lib/webif/webif.sh

header "Status" "PPTP" "@TR<<PPTP Status>>"
ShowNotUpdatedWarning

if [ ! -e /etc/ppp/functions.sh ]; then
	has_pkgs pptp pptpd
else

. /etc/ppp/functions.sh
build_chap_secrets

cat<<EOF
<div class="settings">
<div class="settings-title"><h3><strong>@TR<<PPTP Client>></strong></h3></div>
<div class="settings-content">
<table><tbody>
EOF

if equal "$(nvram get pptp_cli)" "1"; then

	case "$FORM_cli_action" in
	start)
		if [ "$FORM_peer" != "" ]; then
		echo "<p>Starting VPN for peer $FORM_peer</p><pre>"
		/etc/init.d/S??pptp start "$FORM_peer"
		echo "</pre>"
		fi
		;;
	stop)
		if [ "$FORM_peer" != "" ]; then
		/etc/init.d/S??pptp stop "$FORM_peer"
		echo "<p>Stopped VPN for peer $FORM_peer</p>"
		fi
		;;
	esac

	for peer in $(cut -f1 -d' ' /etc/ppp/peers.pptp 2>&-); do
		pid=$(/etc/init.d/S??pptp status "$peer")
		[ -z "$pid" ] && {
			echo "<p>The '$peer' tunnel is not running. <a href=\"?cli_action=start&peer=$peer\">start</a></p>"
		}
	done


	echo "<h3>Active PPTP Tunnels:</h3>"
	ifconfig_info raw | awk '$6 ~ "pptp:"' | awk '
BEGIN {
	print "<table width=\"80%\"><tbody><tr><th>Peername</th><th>Interface</th><th>IP Address</th><th>Netmask</th><th>Protocol</th><th>Link</th><th>RX Bytes</th><th>TX Bytes</th></tr>"
}
{
	print "<tr><td>" substr($6,6) "</td><td>" $1 "</td><td>" $2 "</td><td>" $3 "</td><td>" $4 "</td><td>" $5 "</td><td align=right>" $7 " " $8 "</td><td align=right>" $9 " " $10 "</td><td><a href=\"?cli_action=stop&peer=" substr($6,6) "\">stop</a></td></tr>"
}
END {
	print "</tbody></table>"
}
'

else
	echo "pptp client disabled."
fi
cat<<EOF
</tbody></table>
</div>
</div>
EOF

cat<<EOF
<br /><br />
<div class="settings">
<div class="settings-title"><h3><strong>@TR<<PPTP Server>></strong></h3></div>
<div class="settings-content">
<table><tbody>
EOF

if equal "$(nvram get pptp_srv)" "1"; then

	case "$FORM_srv_action" in
	start)
		/etc/init.d/S50pptpd start
		echo "<p>pptpd started</p>"
		;;
	stop)
		/etc/init.d/S50pptpd stop
		echo "<p>pptpd stopped</p>"
		;;
	stopvpn)
		if [ "$FORM_clientip" != "" ]; then
		pid=`ps |grep pptpd|grep $FORM_clientip|grep -v grep|awk '{print $1}'`
		[ "$pid" != "" ] && kill $pid
		echo "<p>stopped VPN for client ip $FORM_clientip</p>"
		fi
		;;
	esac

	ps | grep -v grep | grep -v options.pptpd | grep -v '\[' | grep -q 'pptpd' && {
	echo '<p>pptpd process is running. <a href="?srv_action=stop">[stop now]</a></p>'
	} || {
	echo '<p>pptpd is not running. <a href="?srv_action=start">[start now]</a></p>'
	}

	echo "<br><h3>Established VPN Connections</h3>"
	if [ -e $SRV_USR.ppp ] && [ -e $SRV_IPS.pptp ]; then
		echo "<table style=\"margin-top: 10px; width: 80%; text-align: left;\" border=\"0\" cellpadding=\"2\" cellspacing=\"2\"><tbody>"
		echo "<tr><th>Interface</th><th>Username</th><th>TTY</th><th>Tunnel IP</th><th>Client IP</th><th>Time</th><th></th></tr>"
		cat $SRV_USR.ppp $SRV_IPS.pptp | awk '
			($6 != "") {
				user[$1]=$3
				user_login[$1]=$4 " " $5 " " $6 " " $7 " " $8 " " $9
			}
			($1 != "" && $6 == "") {
				print "<tr><td>" $1 "</td><td>" user[$1] "</td><td>" $2 "</td><td>" $4 "</td><td>" $5 "</td><td>" user_login[$1] "</td><td><a href=\"?srv_action=stopvpn&clientip=" $5 "\">Stop VPN</a></td></tr>"
			}
		'
		echo "</tbody></table>"
	fi
else
	echo "<p>pptp server disabled.</p>"
fi

cat<<EOF
</tbody></table>
</div>
</div>
EOF

fi

footer ?>
<!--
##WEBIF:name:Status:600:PPTP
-->
