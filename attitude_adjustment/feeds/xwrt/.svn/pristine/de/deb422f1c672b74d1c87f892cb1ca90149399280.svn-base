#!/usr/bin/webif-page
<?
#########################################
# About page
#
# Author(s) [in order of work date]:
#        Dmytro Dykhman <dmytro@iroot.ca>
#

. /usr/lib/webif/functions.sh
. /lib/config/uci.sh
. /www/cgi-bin/webif/applications-shell.sh

echo "$HEADER"

if ! empty "$FORM_package"; then

	#App_package_install "FTP Server" "http://ftp.osuosl.org/pub/nslu2/feeds/unslung/wl500g/" "openssl_0.9.7l-3_mipsel.ipk" "proftpd_1.2.10-5_mipsel.ipk"

	App_package_install "FTP Server" "http://192.168.0.7/" "openssl_0.9.7l-3_mipsel.ipk" "proftpd_1.2.10-5_mipsel.ipk"
	#ipkg $loc install $url1"proftpd_1.3.0a-1_mipsel.ipk" -force-overwrite -force-defaults

	if  is_package_installed "openssl"  &&  is_package_installed "proftpd"  ; then 
	
	#### Small patches to get it working with webif^2
	#cp $loc/opt/etc/proftpd.conf /etc/proftpd.conf

echo "ServerName			\"ProFTPD Webif^2 Installation\"
ServerType			standalone
DefaultServer			on
WtmpLog			off
Port				21
Umask				022
MaxInstances			10
PassivePorts			49873 49873

MaxClients                    3 \"550 Too Many Users (Limit=%m)\"
MaxClientsPerHost             1 \"551 One connection per IP\"

DirFakeUser on ~
DirFakeGroup on ~

<Global>
  RootLogin On
  RequireValidShell off
  #AuthUserFile /etc/passwd
  AuthUserFile		/etc/passwd
 
  AllowStoreRestart on
</Global>

DefaultRoot	/www/

<Directory /www>
    <Limit ALL>
      AllowAll
    </Limit>
</Directory>

User				root
Group				root

AllowOverwrite		on" > /etc/proftpd.conf

	#if equal $(grep "nobody:" < /etc/group) "" ; then echo "nobody:x:65535:" >> /etc/group ; fi
	mkdir /opt
	mkdir /opt/var
	mkdir /opt/var/proftpd
	ln -s $loc/opt/sbin/proftpd /usr/sbin/proftpd

	#### Check if first time /etc/config exists

	if [ -s "/etc/config/app.proftpd" ] ; then
	echo ""
	else
	echo "config proftpd net
	option port	'21'" > /etc/config/app.proftpd
	fi

	Load_remote_libs
	echo_install_complete
fi
exit
fi

if ! empty "$FORM_remove"; then
	echo "<html><header></header><body><font size=3>Removing FTP Server packages ...<br><br><pre>"
	remove_package "proftpd"
	remove_package "openssl"
	rm /etc/config/app.proftpd
	rm /etc/proftpd.conf
	echo "</pre>Done.</font></body></html>"
exit
fi

if  is_package_installed "openssl"  &&  is_package_installed "proftpd"  ; then 


######## Read config

uci_load "app.proftpd"
NET_PORT="$CONFIG_net_port"


HEADER="<link rel=stylesheet type=text/css href=/themes/active/webif.css>
<script type="text/javascript" src="/js/balloontip.js">
</script>"

cat <<EOF
$HTMLHEAD</head><body bgcolor="#eceeec">
<strong>Status</strong><br><br><hr>
EOF

######## Try start Proftpd
if ! empty "$FORM_startftp"; then
echo "<META http-equiv="refresh" content='4;URL=$SCRIPT_NAME'>"
echo "<br>Starting FTP Server ...<br/>"
proftpd -c /etc/proftpd.conf &
exit
fi

######### Save Proftpd
if ! empty "$FORM_save_proftpd"; then
echo "<META http-equiv="refresh" content='2;URL=$SCRIPT_NAME'>"
echo "<br>saving...."

uci_set "app.proftpd" "net" "port" "$FORM_ftp_port"
uci_commit "app.proftpd"

exit
fi 

if ! empty "$FORM_stopftp"; then
echo "<META http-equiv="refresh" content='4;URL=$SCRIPT_NAME'>"
echo "<br>Stopping FTP Server ..."
killall -q proftpd
exit
fi

####### Check if proftpd is running
#echo "'"$(ps | grep -c proftpd)"'"
if [ $(ps | grep -c proftpd) = "1" ] ; then

cat <<EOF
<form method="post" action='$SCRIPT_NAME'>
<div class=warning>FTP Server is not running</div>&nbsp;&nbsp;<input type="submit" name="startftp" value="Start"><br/>
</form>
<br>
EOF

else

echo "<form method="post" action='$SCRIPT_NAME'><font color="#33CC00">FTP Server is succesfully started</font>&nbsp;&nbsp;<input type="submit" name="stopftp" value='Stop'></form><br><br>"
fi

cat <<EOF
<strong>FTP Server Configuration</strong><br>
<br>
<form action='$SCRIPT_NAME' method='post'>
<table width="100%" border="0" cellspacing="1">
<tr><td colspan="2" height="1"  bgcolor="#333333"></td></tr>
<tr><td>&nbsp;</td><td>&nbsp;</td></tr>
<tr> 
<td width="100"><a href="#" rel="b1">Port</a></td>
<td><input name="ftp_port" type="text" value=$NET_PORT></td>
</tr>
<tr><td>&nbsp;</td><td>&nbsp;</td></tr>
<tr><td>&nbsp;</td>
<td><input type="submit" style='border: 1px solid #000000;' name="save_proftpd" value="Save"></td>
</tr>
</table>
</form>
EOF

TIP 450 "Port number<br><br>Default: 21"
echo "</body></html>"

fi
?>