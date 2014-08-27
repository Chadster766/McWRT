#!/usr/bin/webif-page
<?
#########################################
# Applications Swap
#
# Author(s) [in order of work date]:
#        Dmytro Dykhman <dmytro@iroot.ca>
#

. /usr/lib/webif/functions.sh
. /lib/config/uci.sh
. /www/cgi-bin/webif/applications-shell.sh

echo "$HEADER"

if ! empty "$FORM_package"; then

	App_package_install "Swap" "" "kmod-loop" "losetup" "swap-utils"

	if  is_package_installed "kmod-loop" && is_package_installed "losetup" &&  is_package_installed "swap-utils" ; then 
	if [ ! -s "/etc/config/swap" ] ; then
	echo "config swap set
	option path	'/tmp'
	option ram	'2000'" > /etc/config/swap
	fi
	echo_install_complete
	fi

exit
fi

if ! empty "$FORM_remove"; then
	App_package_remove "Swap" "swap" "losetup" "kmod-loop" "swap-utils"
exit
fi

if  is_package_installed "kmod-loop" && is_package_installed "losetup" &&  is_package_installed "swap-utils" ; then 

###### Read config

uci_load "swap"
SET_PATH="$CONFIG_set_path"
SET_RAM="$CONFIG_set_ram"
SET_BOOT="$CONFIG_set_startup"

cat <<EOF
$HTMLHEAD</head>
<body bgcolor="#eceeec"><strong>Status</strong><br/><br/><hr/>
EOF

####### Try to SWAP
if ! empty "$FORM_swapon"; then

dd if=/dev/zero of=/$SET_PATH/swapfile count=$SET_RAM
losetup /dev/loop/0 /$SET_PATH/swapfile
mkswap /dev/loop/0
swapon /dev/loop/0
echo "<META http-equiv="refresh" content='2;URL=$SCRIPT_NAME'>"
echo "<br/><br/>Changing current RAM ..."
exit
fi

if ! empty "$FORM_swapoff"; then
swapoff /dev/loop/0
fi

####### Save SWAP
if ! empty "$FORM_save_swap"; then
echo "<META http-equiv="refresh" content='2;URL=$SCRIPT_NAME'>"
echo "<br/>saving ..."

uci_set "swap" "set" "path" "$FORM_swap_path"
uci_set "swap" "set" "ram" "$FORM_swap_ram"

	### If checkbox on startup
	if ! empty "$FORM_chkstartup"; then
		uci_set "swap" "set" "startup" "checked"
		if  [ -s "/etc/init.d/swap" ]  ; then rm /etc/init.d/swap ; fi
	echo "#!/bin/sh

START=98
sleep 5
dd if=/dev/zero of=/$SET_PATH/swapfile count=$SET_RAM
losetup /dev/loop/0 /$SET_PATH/swapfile
mkswap /dev/loop/0
swapon /dev/loop/0" > /etc/init.d/swap
		ln -s /etc/init.d/swap /etc/rc.d/S98swap
		chmod 755 /etc/init.d/swap
	else 	### else
		uci_set "swap" "set" "startup" ""
		if [ -s "/etc/init.d/swap" ] ; then rm /etc/init.d/swap ; rm /etc/rc.d/S98swap; fi
	fi 	### end if checkbox on startup

uci_commit "swap"
exit
fi

####### Check if already swapped
if [ $(free |grep "Swap" | sed -e s/'Swap:'//g -e s/' '//g) = "000" ]; then
cat <<EOF
<div class=warning>Swap is not mounted.</div><br/><br/> 
<form method="post" action='$SCRIPT_NAME'>
<input type="submit" name="swapon" value="Swap RAM" />
</form><br/>
EOF

else
echo "<form method="post" action='$SCRIPT_NAME'><font color="#33CC00"><br/>Swap is succesfully maped in $SET_PATH</font>&nbsp;&nbsp;<input type="submit" name="swapoff" value="UnSwap RAM" /></form><br/><br/>"
fi

cat <<EOF
<strong>Swap Configuration</strong><br/>
<br/><form action='$SCRIPT_NAME' method='post'>
<table width="100%" border="0" cellspacing="1">
<tr><td colspan="2" height="1"  bgcolor="#333333"></td></tr>
<tr><td>&nbsp;</td><td>&nbsp;</td></tr>
<tr><td width="200">&nbsp;<a href="#" rel="b1">Swap file Location:</a></td>
<td><input name="swap_path" type="text" value=$SET_PATH /></td></tr>
<tr><td width="200">&nbsp;<a href="#" rel="b2">Size (blocks):</a></td>
<td><input name="swap_ram" type="text" value=$SET_RAM /></td></tr>
<tr><td>&nbsp;</td><td>&nbsp;</td></tr>
<tr><td colspan="2" height="1" bgcolor="#333333"></td></tr>
<tr><td>&nbsp;</td><td><input type="checkbox" name="chkstartup" $SET_BOOT />&nbsp;mount on boot<br/><br/></td></tr>
<tr><td>&nbsp;</td><td><input type="submit" style='border: 1px solid #000000;' name="save_swap" value="Save" /></td>
</tr></table></form>
EOF

TIP 450 "Location where swap file will be stored.<br/><br/>Example: /mnt"
TIP 450 "Size in blocks: 1000 blocks = 512 Kbytes | 1 Megabyte = 2000 blocks"
echo "</body></html>"

fi
?>