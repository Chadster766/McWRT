#!/usr/bin/webif-page "-U /tmp -u 4096"
<?
# from: http://coova.org
# 03/05/2007 port of pptp page to Kamikaze by Liran Tal
# *unfinished port still...

. /usr/lib/webif/webif.sh
. /etc/functions-net.sh

load_settings "pptp"

if empty "$FORM_submit"; then
	FORM_pptp_cli=$CONFIG_pptp_cli
	FORM_pptp_srv=$CONFIG_pptp_srv
else
	uci_set pptp pptp pptp_cli "$FORM_pptp_cli"
	uci_set pptp pptp pptp_srv "$FORM_pptp_srv"
fi

header "VPN" "PPTP" "@TR<<PPTP>>" ' onLoad="modechange()" ' "$SCRIPT_NAME"
ShowNotUpdatedWarning

if [ ! -e /etc/ppp/functions.sh ]; then
	has_pkgs pptp pptpd
else

. /etc/ppp/functions.sh

empty "$FORM_add_user" || {
	vip=
	equal "$FORM_ip" "*" || vip="ip|FORM_ip|@TR<<IP Address>>|required|$FORM_ip"
	validate <<EOF
$vip
hostname|FORM_user|@TR<<Username>>|required|$FORM_user
string|FORM_pass|@TR<<Password>>|required|$FORM_pass
EOF
	equal "$?" 0 && ppp_add_user pptpd "$FORM_user" "$FORM_pass" "$FORM_ip"
}

empty "$FORM_add_peer" || {
	vip=
	equal "$FORM_ip" "*" || vip="ip|FORM_ip|@TR<<IP Address>>|required|$FORM_ip"
	validate <<EOF
$vip
hostname|FORM_peer|@TR<<Peername>>|required|$FORM_peer
hostname|FORM_host|@TR<<Hostname>>|required|$FORM_host
hostname|FORM_user|@TR<<Username>>|required|$FORM_user
string|FORM_pass|@TR<<Password>>|required|$FORM_pass
EOF
	equal "$?" 0 && ppp_add_peer pptp "$FORM_peer" "$FORM_host" "$FORM_user" "$FORM_pass" "$FORM_ip"
}

empty "$FORM_del_user" || {
	validate <<EOF
hostname|FORM_user|@TR<<Username>>|required|$FORM_user
EOF
	equal "$?" 0 && ppp_del_user pptpd "$FORM_user"
}

empty "$FORM_del_peer" || {
	validate <<EOF
hostname|FORM_peer|@TR<<Peername>>|required|$FORM_peer
EOF
	equal "$?" 0 && ppp_del_peer pptp "$FORM_peer"
}

build_chap_secrets

cat <<EOF
<script type="text/javascript" src="/webif.js "></script>
<script type="text/javascript">
<!--
function modechange()
{
	var v;
	v = isset('pptp_cli', '1');
	set_visible('pptppeers', v);

	v = isset('pptp_cli_auth', 'psk');
	set_visible('psk_status', v);
	set_visible('psk', v);

	v = isset('pptp_cli_auth', 'cert');
	set_visible('certificate_status', v);
	set_visible('certificate', v);
	set_visible('pkcs12pass', v);

	v = isset('pptp_srv', '1');
	set_visible('server_settings', v);
	set_visible('pptpusers', v);

	hide('save');
	show('save');
}
-->
</script>
EOF

has_pkgs pptp && {

display_form <<EOF
onchange|modechange
start_form|@TR<<PPTP Client>>
field|@TR<<PPTP Client Connection>>
select|pptp_cli|$FORM_pptp_cli
option|0|@TR<<Disabled>>
option|1|@TR<<Enabled>>
onchange|
end_form
EOF

awk -v "url=$SCRIPT_NAME" \
	-v "peer=$FORM_peer" \
	-v "host=$FORM_host" \
	-v "user=$FORM_user" \
	-v "pass=$FORM_pass" \
	-v "ip=$FORM_ip" \
	-f /usr/lib/webif/common.awk -f - /etc/ppp/users.pptp /etc/ppp/peers.pptp <<EOF
BEGIN {
	FS="[ \\t]"
	print "<form enctype=\\"multipart/form-data\\" method=\\"post\\">"
	start_form("@TR<<PPTP Client Connections>>"," style=\"display:none;\" id=\"pptppeers\"")
	print "<table width=\\"70%\\" summary=\\"Settings\\">"
	print "<tr><th>@TR<<Peername>></th><th>@TR<<Hostname>></th><th>@TR<<Username>></th><th>@TR<<Password>></th><th>@TR<<IP Address>></th><th></th></tr>"
}
(\$4 != "") {
	user[\$1]=\$2
	pass[\$1]=\$3
	ip[\$1]=\$4
}
(\$4 == "") {
	print "<tr><td>" \$1 "</td><td>" \$2 "</td><td>" \$3 "</td><td>" pass[\$1] "</td><td>" ip[\$1] "</td><td align=\\"right\\" width=\\"10%\\"><a href=\\"" url "?del_peer=1&peer=" \$1 "\\">@TR<<Remove>></a></td></tr>"
}
END {
	print "<tr style\"text-size: 80%\"><td>" textinput2("peer", peer, 6) "</td><td>" textinput2("host", host, 8) "</td><td>" textinput2("user", user, 8) "</td><td>" textinput2("pass", pass, 8) "</td><td>" textinput2("ip", ip, 8) "</td><td style=\\"width: 10em\\">" button("add_peer", "Add") "</td></tr>"
	print "</table>"
	print "</form>"
	end_form();
}
EOF

}

has_pkgs pptpd && {

display_form <<EOF
onchange|modechange
start_form|@TR<<PPTPD Server>>
field|@TR<<PPTP Server>>
select|pptp_srv|$FORM_pptp_srv
option|0|@TR<<Disabled>>
option|1|@TR<<Enabled>>
onchange|
end_form
EOF

awk -v "url=$SCRIPT_NAME" \
	-v "user=$FORM_user" \
	-v "pass=$FORM_pass" \
	-v "ip=$FORM_ip" \
	-f /usr/lib/webif/common.awk -f - /etc/ppp/users.pptpd <<EOF
BEGIN {
	FS="[ \\t]"
	print "<form enctype=\\"multipart/form-data\\" method=\\"post\\">"
	start_form("@TR<<PPTP VPN Users>>"," style=\"display:none;\" id=\"pptpusers\"")
	print "<table width=\\"70%\\" summary=\\"Settings\\">"
	print "<tr><th>@TR<<Username>></th><th>@TR<<Password>></th><th>@TR<<IP Address>></th><th></th></tr>"
}

{
	print "<tr><td>" \$1 "</td><td>" \$2 "</td><td>" \$3 "</td><td align=\\"right\\" width=\\"10%\\"><a href=\\"" url "?del_user=1&user=" \$1 "\\">@TR<<Remove>></a></td></tr>"
}

END {
	print "<tr><td>" textinput("user", user) "</td><td>" textinput("pass", pass) "</td><td>" textinput("ip", ip) "</td><td style=\\"width: 10em\\">" button("add_user", "Add") "</td></tr>"
	print "</table>"
	print "</form>"
	print "<b>Note</b>: The PPTP VPN network will be on the 192.168.200.0 network with gateway IP 192.168.200.1. Assign IP addresses in that network, e.g. 192.168.200.10"
	end_form();
}
EOF

}

fi

footer
?>
<!--
##WEBIF:name:VPN:6:PPTP
-->
