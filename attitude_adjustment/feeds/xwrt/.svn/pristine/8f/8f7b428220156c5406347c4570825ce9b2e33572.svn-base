#!/usr/bin/webif-page "-U /tmp -u 4096"
<?
#########################################
# Applications Hydra
#
# Author(s) [in order of work date]:
#        Dmytro Dykhman <dmytro@iroot.ca>
#
. /usr/lib/webif/functions.sh
. /lib/config/uci.sh
. /www/cgi-bin/webif/applications-shell.sh

echo "$HEADER"


if ! empty "$FORM_package"; then

        App_package_install "Hydra 4.5" "" ""
        wget -q http://www.hackerpimps.com/fairuzawrt/bin/hydra -P /usr/sbin/
        wget -q http://www.phenoelit.de/obiwan/common-passwords.txt -P /etc/
        mv /etc/common-passwords.txt /etc/pwd.lst
        
        if  [ -s "/usr/sbin/hydra" ]  ; then
        chmod 755 /usr/sbin/hydra
        echo "config hydra conf
        option ip        ''
        option service        'http'
        option path        ''
        option usr        'admin'
        option pwd        '/etc/pwd.lst'" > /etc/config/hydra

        echo_install_complete 
        fi
exit
fi

if ! empty "$FORM_remove"; then
        App_package_remove "Hydra 4.5" "hydra"
        rm /usr/sbin/hydra
        rm /etc/pwd.lst 2> /dev/null
exit
fi

if  [ -s "/usr/sbin/hydra" ]  ; then 

echo "$HTMLHEAD<META http-equiv='refresh' content='30;URL=$SCRIPT_NAME' /></head><body bgcolor='#eceeec'><strong>Status</strong><br/><br/><hr/>"

if equal $FORM_action  "upload" ; then echo "... File sucessfully uploaded</br>"

        if [ $FORM_usrorpwd = "usr" ] ; then
        mv $FORM_uploadfile /etc/usr.lst
        uci_set "hydra" "conf" "usr" "/etc/usr.lst"
        else mv $FORM_uploadfile /etc/pwd.lst
        uci_set "hydra" "conf" "pwd" "/etc/pwd.lst"
        fi
        uci_commit "hydra"
fi

######### Save Hydra
if ! empty "$FORM_save_hydra"; then
echo "<META http-equiv="refresh" content='3;URL=$SCRIPT_NAME'>"
echo "<br/>saving...."

uci_set "hydra" "conf" "ip" "$FORM_h_ip"
uci_set "hydra" "conf" "service" "$FORM_service"
uci_set "hydra" "conf" "port" "$FORM_h_port"
uci_set "hydra" "conf" "path" "$FORM_h_path"
uci_set "hydra" "conf" "usr" "$FORM_h_usr"
uci_set "hydra" "conf" "pwd" "$FORM_h_pwd"
uci_set "hydra" "conf" "wait" "$FORM_h_wait"

        ### If  session checkbox on 
        if ! empty "$FORM_chksession"; then
                uci_set "hydra" "conf" "session" "checked"
        else         uci_set "hydra" "conf" "session" "" ; fi

uci_commit "hydra"
exit
fi
        ######## Read config

        uci_load "hydra"
        CFG_IP="$CONFIG_conf_ip"
        CFG_SRV="$CONFIG_conf_service"
        CFG_PORT="$CONFIG_conf_port"
        CFG_RSESSION="$CONFIG_conf_session"
        CFG_PATH="$CONFIG_conf_path"
        CFG_USR="$CONFIG_conf_usr"
        CFG_PWD="$CONFIG_conf_pwd"
        CFG_WAIT="$CONFIG_conf_wait"

        ######## Run Hydra
        if ! empty "$FORM_runhydra"; then
        APP_Refresh_Page $SCRIPT_NAME "<br/>Starting Hydra ...</br/>"

        if [ -s "$CFG_USR" ]  ; then hL="-L" ; else  hL="-l" ; fi
        if [ -s "$CFG_PWD" ]  ; then hP="-P" ; else  hP="-p" ; fi
        if ! equal $CFG_PATH "" ; then hM="-m $CFG_PATH" ; fi
        if ! equal $CFG_WAIT "" ; then hW="-w $CFG_WAIT" ; fi
        if equal $CFG_RSESSION "checked" ; then hR="-R" ; fi

        if [ $CFG_SRV = "http" ] ; then
                hydra -f $hR $hL $CFG_USR $hP $CFG_PWD $hW -e ns $CFG_IP http $hM > /tmp/hydra.www
        elif [ $CFG_SRV = "ftp" ] ; then
                hydra -f $hR $hL $CFG_USR $hP $CFG_PWD $hW -e ns $CFG_IP ftp > /tmp/hydra.ftp
        elif [ $CFG_SRV = "telnet" ] ; then
                #NOT TESTED YET
                #hydra -f $hR $hL $CFG_USR $hP $CFG_PWD $hW -e ns $CFG_IP telnet > /tmp/hydra.telnet
                echo ""
        fi
        exit
        fi

        ######## Stop Hydra
        if ! empty "$FORM_stophydra"; then
        echo "<META http-equiv="refresh" content='3;URL=$SCRIPT_NAME' /><br/>Stopping Hydra ..."
        killall -q hydra
        exit
        fi
        ######## Reset Hydra
        if ! empty "$FORM_resethydra"; then
        rm /tmp/hydra.www 2> /dev/null
        rm /tmp/hydra.ftp 2> /dev/null
        rm /tmp/hydra.telnet 2> /dev/null
        echo "<META http-equiv="refresh" content='3;URL=$SCRIPT_NAME'>"
        echo "<br/>Reseting Hydra ..."
        exit
        fi

        ####### Check if hydra is running
        pscheck=$(ps | grep -c 'hydra -f')
        if [ $pscheck = "1" ] ; then
        if  [ -s "/tmp/hydra.www" ]  ; then pwd_www=$( grep "\[www\]" < /tmp/hydra.www) ; fi
        if  [ -s "/tmp/hydra.ftp" ]  ; then pwd_ftp=$( grep "\[ftp\]" < /tmp/hydra.ftp) ; fi
        if  [ -s "/tmp/hydra.telnet" ]  ; then pwd_telnet=$( grep "\[telnet\]" < /tmp/hydra.telnet) ; fi
                echo "<form method='post' action='$SCRIPT_NAME'><div class=warning>Hydra is not running</div>&nbsp;&nbsp;<input type='submit' name='runhydra' value=\"Run Hydra\" /></form>"
        else
                echo "<form method="post" action='$SCRIPT_NAME'><font color="#33CC00">Hydra is running as '$CFG_USR' : '$CFG_PWD'</font>&nbsp;&nbsp;<input type='submit' name='stophydra' value=\"Stop Hydra\" /></form>"
        fi

if equal $pwd_www "" && equal $pwd_ftp  "" && equal $pwd_telnet ""  ; then
        echo "<font color=red>No sucessfull logins found so far...</font><br/><br/>"
else
cat <<EOF
<font color="#33CC00"><b>Found sucessfull logins!</b></font><br/><br/><font color=red>WARNING: Change your passwords immediately!!!</font><br/><br/>
<center><table width="80%" border="0" cellspacing="1" bgcolor="#FF5959">
<tr><td bgcolor="#FFFFFF">
$pwd_www<br/>
$pwd_ftp<br/>
$pwd_telnet<br/>
</td></tr></table><br/><form method="post" action='$SCRIPT_NAME'><input type="submit" name="resethydra" value="OK I did change my passwords" /></form></center>
EOF
fi

cat <<EOF
<script type="text/javascript" src="/js/forms.js"></script>
<script type="text/javascript">
function java2(s) {
document.uploadform.usrorpwd.value = s
loadwindow(0,'$SCRIPT_NAME',300,100,0,30) ;
}
window.onload = function() {
setupDependencies('form1');
}
</script>
EOF

DIV_Windows_Header 0

cat <<EOF
<h3>Upload File</h3><br/>
<form method="post" name="uploadform" action="$SCRIPT_NAME" enctype="multipart/form-data" >
<input type="hidden" name="usrorpwd" value="" />
<table width="100%">
<tr>
<td><input type="file" class="flatbtn" name="uploadfile" /></td>
<td><input class="flatbtn" type="submit" name="submit" value="Upload" /></td>
</tr>
</table>
<input type="hidden" name="action" value="upload" /></form></div>
<strong>Hydra Configuration</strong><br/>
<br/>
<form action='$SCRIPT_NAME' method='post' name='form1'>
<table width="100%" border="0" cellspacing="1">
<tr><td colspan="2" height="1"  bgcolor="#333333"></td></tr>
<tr><td width="200"><a href="#" rel="b1">IP Address</a></td>
<td><input name="h_ip" type="text" value='$CFG_IP' /></td></tr>
<tr><td><a href="#" rel="b2">Service</a></td>
<td><select name='service' STYLE='width: 150px'>
EOF
#### TODO...Make it a loop or something
if [ $CFG_SRV = "http" ] ; then echo "<option value='http' selected>http</option><option value='ftp'>ftp</option><option>telnet</option>"
elif [ $CFG_SRV = "ftp" ] ; then echo "<option value='http'>http</option><option value='ftp' selected>ftp</option><option>telnet</option>"
elif [ $CFG_SRV = "telnet" ] ; then echo "<option value='http'>http</option><option value='ftp'>ftp</option><option selected>telnet</option>"
fi
cat <<EOF
</select>
<input name="h_srv" type="hidden" value=$CFG_SRV /></td></tr>
<tr><td><a href="#" rel="b3">Port</a></td><td><input name="h_port" type="text" value="$CFG_PORT" /></td></tr>
<tr><td height="1"><label><a href="#" rel="b4" >URL Path</a><input type='hidden' class="DEPENDS ON service BEING http" /></label></td><td height="1"><input name="h_path" type="text" value="$CFG_PATH" class="DEPENDS ON service BEING http" /></td></tr>
<tr><td><a href="#" rel="b5">Username</a></td>
<td><input name="h_usr" type="text" value="$CFG_USR" />&nbsp;&nbsp;<input type='button' class='flatbtn' value='@TR<<Upload>>' onClick="java2('usr')"></td></tr>
<tr><td><a href="#" rel="b6">Password List</a></td>
<td><input name="h_pwd" type="text" value="$CFG_PWD" />&nbsp;&nbsp;<input type='button' class='flatbtn' value='@TR<<Upload>>' onClick="java2('pwd')"></td></tr>
<tr><td><a href="#" rel="b7">Wait Time</a></td>
<td><input name="h_wait" type="text" value="$CFG_WAIT" /></td></tr>
<tr><td>&nbsp;</td><td>&nbsp;</td></tr>
<tr><td colspan="2" height="1" bgcolor="#333333"></td></tr>
<tr><td colspan="2"><input type="checkbox" name="chksession" $CFG_RSESSION />&nbsp;Restore a previous aborted/crashed session</td></tr>
<tr><td>&nbsp;</td><td>&nbsp;</td></tr>
<tr><td>&nbsp;</td>
<td><input type="submit" style='border: 1px solid #000000;' name="save_hydra" value="Save" /></td>
</tr></table></form>
EOF

TIP 450 "IP address to attack.<br/><br/>Example: 192.168.1.5"
TIP 450 "Service to attack.<br/><br/>Example: http,ftp,telnet,ssh"
TIP 0 "Port # of service of different from default"
TIP 450 "Aditional parameter.<br/><br/>Example:"
TIP 450 "Single username or text file with list of usernames.<br/><br/>Example: admin or /tmp/usr.lst"
TIP 450 "Single password or text file with list of passwords.<br/><br/>Example: password or /tmp/passwd.lst"
TIP 450 "Defines the max wait time in seconds for responses (default: 30)"
echo "</body></html>"

fi
?>
