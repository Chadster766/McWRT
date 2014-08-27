#!/usr/bin/webif-page
<?
#########################################
# Applications ipkg
#
# Author(s) [in order of work date]:
#        Dmytro Dykhman <dmytro@iroot.ca>
#

. /usr/lib/webif/functions.sh
. /lib/config/uci.sh
. /www/cgi-bin/webif/applications-shell.sh

echo "$HEADER"

if ! empty "$FORM_remove" ; then
	echo "<html><header><META http-equiv="refresh" content='4;URL=$SCRIPT_NAME'></header><body>Restoring ipkg configuration ...<br/><br/><pre>"

	if [ -s "/etc/profile.bak" ] ; then

	rm /etc/profile 
	mv /etc/profile.bak /etc/profile 

	if [ -s "/etc/ipkg.conf.bak" ] ; then rm /etc/ipkg.conf ; mv /etc/ipkg.conf.bak /etc/ipkg.conf ; fi

	rm /etc/config/app.ipkg

	else
	echo "Sorry cannot restore ... Try manually."
	fi
	echo "</pre>Done.</body></html>"
exit
fi

if [ -s "/etc/config/app.ipkg" ] ; then

	uci_load "app.ipkg"
	location="$CONFIG_int_location"

cat <<EOF
$HTMLHEAD</head><body bgcolor="#eceeec">
<strong>Status</strong><br/><br/><hr/>
EOF

######### Save ipkg
if ! empty "$FORM_save_ipkg"; then
echo "<META http-equiv="refresh" content='2;URL=$SCRIPT_NAME'>"
echo "<br/>saving...."

uci_set "app.ipkg" "int" "location" "$FORM_location"
uci_commit "app.ipkg"

exit
fi 

echo "<form method="post" action='$SCRIPT_NAME'><font color="#33CC00">ipkg is configured ($location)</font>&nbsp;&nbsp;<input type="submit" name="remove" value='Restore Default' /></form><br/><br/>"

cat <<EOF
<strong>ipkg Configuration</strong><br/>
<br/>
<form action='$SCRIPT_NAME' method='post'>
<table width="100%" border="0" cellspacing="1">
<tr><td colspan="2" height="1" bgcolor="#333333"></td></tr>
<tr><td>&nbsp;</td><td>&nbsp;</td></tr>
<tr><td width="100"><a href="#" rel="b1">Location</a></td><td><input name="location" type="text" value="$location" /></td></tr>
<tr><td>&nbsp;</td><td>&nbsp;</td></tr>
<tr> <td>&nbsp;</td>
<td><input type="submit" style='border: 1px solid #000000;' name="save_ipkg" value="Save" /></td>
</tr></table></form>
EOF

TIP 450 "Path to where Applications will install, if router is out of RAM<br/><br/>Example: /mnt/usb"
echo "</body></html>"

fi
?>