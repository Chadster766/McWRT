#!/usr/bin/webif-page
<?
#########################################
# Applications CIFS
#
# Author(s) [in order of work date]:
#        Dmytro Dykhman <dmytro@iroot.ca>
#

. /usr/lib/webif/functions.sh
. /lib/config/uci.sh
. /www/cgi-bin/webif/applications-shell.sh

echo "$HEADER"

if ! empty "$FORM_package"; then

	App_package_install "Samba Client" "" "kmod-fs-cifs -force-depends" "cifsmount"
	
	if  is_package_installed "kmod-fs-cifs"  &&  is_package_installed "cifsmount"  ; then 
	
	insmod cifs 2> /dev/null
	##### Make config file

	if [ ! -s "/etc/config/cifs" ] ; then 
echo "config samba net
	option ip	''
	option pc	''
	option grp	'workgroup'

config samba login
	option usr	'guest'
	option pwd	''

config samba mount
	option startup	'0'
	option share	'smb'
	option mnt	'/mnt'" > /etc/config/cifs
	fi
	echo_install_complete
	fi
	
exit
fi

if ! empty "$FORM_remove"; then
	App_package_remove "Samba Client" "cifs" "kmod-fs-cifs" "cifsmount"
	rmmod cifs 2> /dev/null
exit
fi

if  is_package_installed "kmod-fs-cifs"  &&  is_package_installed "cifsmount"  ; then 

######## Read config

uci_load "cifs"
NET_IP="$CONFIG_net_ip"
NET_PC="$CONFIG_net_pc"
NET_GRP="$CONFIG_net_grp"

LOGIN_USR="$CONFIG_login_usr"
LOGIN_PWD="$CONFIG_login_pwd"

MOUNT_SRT="$CONFIG_mount_startup"
MOUNT_SHARE="$CONFIG_mount_share"
MOUNT_MNT="$CONFIG_mount_mnt"

cat <<EOF
$HTMLHEAD
</head><body bgcolor="#eceeec"><strong>Status</strong><br/><br/><hr/>
EOF


######## Try to map CIFS
if ! empty "$FORM_mapcifs"; then
echo "<META http-equiv="refresh" content='3;URL=$SCRIPT_NAME'>"
echo "<br/>Mounting file system...."
#insmod cifs
mount -t cifs //$NET_IP/$MOUNT_SHARE /$MOUNT_MNT -o unc=\\\\$NET_PC\\$MOUNT_SHARE,ip=$NET_IP,user=$LOGIN_USR,pass=$LOGIN_PWD,dom=$NET_GRP
exit
fi

######### Save CIFS
if ! empty "$FORM_save_cifs"; then
echo "<META http-equiv="refresh" content='2;URL=$SCRIPT_NAME'>"
echo "<br/>saving...."

uci_set "cifs" "net" "ip" "$FORM_smb_ip"
uci_set "cifs" "net" "pc" "$FORM_smb_pc"
uci_set "cifs" "net" "grp" "$FORM_smb_wrkgrp"
uci_set "cifs" "login" "usr" "$FORM_smb_usr"
uci_set "cifs" "login" "pwd" "$FORM_smb_pwd"

	### If checkbox on startup
	if ! empty "$FORM_chkstartup"; then
		uci_set "cifs" "mount" "startup" "checked"
		if  [ -s "/etc/init.d/cifs" ]  ; then rm /etc/init.d/cifs ; fi
	echo "#!/bin/sh

START=96
mount -t cifs //$NET_IP/$MOUNT_SHARE /$MOUNT_MNT -o unc=\\\\\\\\$NET_PC\\\\$MOUNT_SHARE,ip=$NET_IP,user=$LOGIN_USR,pass=$LOGIN_PWD,dom=$NET_GRP" > /etc/init.d/cifs
		ln -s /etc/init.d/cifs /etc/rc.d/S96cifs
		chmod 755 /etc/init.d/cifs
	else 	### else
		uci_set "cifs" "mount" "startup" ""
		if [ -s "/etc/init.d/cifs" ] ; then rm /etc/init.d/cifs ; rm /etc/rc.d/S96cifs; fi
	fi 	### end if checkbox on startup

uci_set "cifs" "mount" "share" "$FORM_smb_dir"
uci_set "cifs" "mount" "mnt" "$FORM_smb_mnt"
uci_commit "cifs"

exit
fi 

if ! empty "$FORM_unmount"; then
echo "<META http-equiv="refresh" content='3;URL=$SCRIPT_NAME'>"
echo "<br/>Unmounting file system...."
umount -l -r -f $MOUNT_MNT
exit
fi

#echo "'"$(ps | grep -c cifs)"'"
####### Check if df is mounted
if equal $( df |grep "$MOUNT_MNT" ) "" ; then 

cat <<EOF
<div class=warning>Network drive is not mounted.</div><br/><br/> 
<form method="post" action='$SCRIPT_NAME'>
<input type="submit" name="mapcifs" value="Map Network Drive" /><br/>
</form>
<br/>
EOF
else echo "<form method="post" action='$SCRIPT_NAME'><font color="#33CC00">Network drive is succesfully maped in $MOUNT_MNT</font>&nbsp;&nbsp;<input type="submit" name="unmount" value='Unmount Drive' /></form><br/><br/>"
fi

cat <<EOF
<strong>Samba Configuration</strong><br/>
<br/><form action='$SCRIPT_NAME' method='post'>
<table width="100%" border="0" cellspacing="1">
<tr><td colspan="2" height="1"  bgcolor="#333333"></td></tr>
<tr><td width="200"><a href="#" rel="b1">IP Address</a></td><td><input name="smb_ip" type="text" value="$NET_IP" /></td></tr>
<tr><td><a href="#" rel="b2">PC name</a></td><td><input name="smb_pc" type="text" value="$NET_PC" /></td></tr>
<tr><td><a href="#" rel="b3">Workgroup</a></td><td><input name="smb_wrkgrp" type="text" value="$NET_GRP" /></td></tr>
<tr><td>&nbsp;</td><td>&nbsp;</td></tr>
<tr><td colspan="2" height="1" bgcolor="#333333"></td></tr>
<tr><td><a href="#" rel="b6">Username</a></td><td><input name="smb_usr" type="text" value="$LOGIN_USR" /></td></tr>
<tr><td><a href="#" rel="b7">Password</a></td><td><input name="smb_pwd" type="password" value="$LOGIN_PWD" /></td></tr>
<tr><td>&nbsp;</td><td>&nbsp;</td></tr>
<tr><td colspan="2" height="1" bgcolor="#333333"></td></tr>
<tr><td>&nbsp;</td><td><input type="checkbox" name="chkstartup" $MOUNT_SRT />&nbsp;mount on boot</td></tr>
<tr><td><a href="#" rel="b4">Shared Folder</a></td><td><input name="smb_dir" type="text" value="$MOUNT_SHARE" /></td></tr>
<tr><td><a href="#" rel="b5">Mount Path</a></td><td><input name="smb_mnt" type="text" value="$MOUNT_MNT" /></td></tr>
<tr><td>&nbsp;</td><td>&nbsp;</td></tr>
<tr><td>&nbsp;</td><td><input type="submit" style='border: 1px solid #000000;' name="save_cifs" value="Save" /></td>
</tr></table></form>
EOF

TIP 450 "An IP address (Internet Protocol address) is a unique address that devices use in order to identify and communicate with each other on a network.<br/><br/>Example: 192.168.1.5"
TIP 450 "A Computer Name is a unique name assigned to each computer on a network.<br/><br/>Example: Tux"
TIP 450 "Workgroup is a group of computers participating on a network.<br/><br/>Example: Workgroup"
TIP 450 "Shared Folder is a directory which can be viewd by any person connected to that client at that time.<br/><br/>Example: documents/todays"
TIP 450 "Location on the router to create virtual directory from shared folder<br/><br/>Example: /mnt"
TIP 450 "A user in computing context is one who uses a computer system.<br/><br/>Example: joesmith"
TIP 450 "A password is a form of secret authentication data that is used to control access to a resource.<br/><br/>Example: hBjGX56"
echo "<br/></body></html>"

fi
?>