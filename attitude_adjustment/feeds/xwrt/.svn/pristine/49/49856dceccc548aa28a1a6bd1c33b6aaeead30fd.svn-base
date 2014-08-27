#!/usr/bin/webif-page
<?
#########################################
# Applications AirCrack
#
# Author(s) [in order of work date]:
#        Dmytro Dykhman <dmytro@iroot.ca>
#
. /usr/lib/webif/webif.sh
. /www/cgi-bin/webif/applications-shell.sh

echo "$HEADER"

if ! empty "$FORM_package"; then

        App_package_install "AirCrack" "" "aircrack-ng"
        
        if  is_package_installed "aircrack-ng" && is_package_installed "libpthread" ; then 
        mkdir /wep 2> /dev/null
        ##### Make config file

        if [ ! -s "/etc/config/app.aircrack" ] ; then 
        echo "config aircrack set
        option path        '/wep'
        option cpath        '/wep'
        option ivs        'checked'
        option key        '128'" > /etc/config/app.aircrack
        fi
        echo_install_complete
        fi
exit
fi

if ! empty "$FORM_remove"; then
        App_package_remove "AirCrack" "app.aircrack" "aircrack-ng" "libpthread"
        rmdir /wep 2> /dev/null
exit
fi

if  is_package_installed "aircrack-ng" && is_package_installed "libpthread" ; then 

        cat <<EOF
        $HTMLHEAD</head><body bgcolor="#eceeec">
        <div id="mainmenu"><ul>
        <li><a href="$SCRIPT_NAME">Status</a></li>
        <li class="separator">-</li>
        <li><a href="$SCRIPT_NAME?page=aircrack">AirCrack</a></li>
        <li><a href="$SCRIPT_NAME?page=airodump">AiroDump</a></li>
        <li><a href="$SCRIPT_NAME?page=airoplay">AiroPlay</a></li>
        </ul></div><br/>
EOF
        ######## Read config
        uci_load "app.aircrack"
        CFG_PATH="$CONFIG_set_path"
        CFG_CPATH="$CONFIG_set_cpath"
        CFG_WPATH="$CONFIG_set_wpath"
        CFG_CHANNEL="$CONFIG_set_channel"
        CFG_BOOTAIR="$CONFIG_set_bootaircrack"
        CFG_BOOTDUMP="$CONFIG_set_bootairodump"
        #CFG_BOOTPLAY="$CONFIG_set_bootairoplay"
        CFG_IVS="$CONFIG_set_ivs"
        CFG_KEY="$CONFIG_set_key"

        if equal $CFG_IVS "checked" ; then hIVS="--ivs" ; fi

        ######## Run/Stop Airodump
        if ! empty "$FORM_run_airodump"; then
                APP_Refresh_Page $SCRIPT_NAME?page=airodump "<br/>Starting Airodump ...</br/>"
                killall -q airodump-ng 2> /dev/null
                wlc monitor 1
                airodump-ng $hIVS -w $CFG_PATH/key prism0 > /dev/null 2>&1
                exit
        fi
        if ! empty "$FORM_stop_airodump"; then
                APP_Refresh_Page $SCRIPT_NAME?page=airodump "<br/>Stopping Airodump ...</br/>"
                killall -q airodump-ng
                exit
        fi

        ######## Run/Stop Aircrack
        if ! empty "$FORM_run_aircrack"; then
                APP_Refresh_Page $SCRIPT_NAME?page=aircrack "<br/>Starting Aircrack ...</br/>"
                killall -q aircrack-ng 2> /dev/null
                if ! equal $CFG_KEY "" ; then hN="-n "$CFG_KEY ; fi
                        if [ $(echo $FORM_selectMAC | awk '{ print $2 }') -lt 50000 ] ; then
                                echo "<script type='text/javascript'>alert (\"Not enought IVs captured\")</script>"
                        else aircrack-ng -q -s $hN -f 4 -x1 -b $(echo $FORM_selectMAC | awk '{ print $1 }') $CFG_CPATH > $CFG_PATH/aircrack.key &
                        fi
                exit
        fi
        if ! empty "$FORM_stop_aircrack"; then
                APP_Refresh_Page $SCRIPT_NAME?page=aircrack "<br/>Stopping Aircrack ...</br/>"
                killall -q aircrack-ng
                exit
        fi

        ######## Save AirCrack
        if ! empty "$FORM_save_aircrack"; then

                APP_Refresh_Page $SCRIPT_NAME?page=$FORM_page "<br/>saving ...</br/>"
                uci_set "app.aircrack" "set" "cpath" "$FORM_aircpath"
                uci_set "app.aircrack" "set" "wpath" "$FORM_wpapath"
                uci_set "app.aircrack" "set" "key" "$FORM_keytype"

                #---- If checkbox on startup AirCrack ----
                if ! empty "$FORM_aircrack_startup"; then
                uci_set "app.aircrack" "set" "bootaircrack" "checked"

                if  [ -s "/etc/init.d/aircrack" ] ; then rm /etc/init.d/aircrack ; fi
echo "#!/bin/sh
start() {
}" > /etc/init.d/aircrack

                ln -s /etc/init.d/aircrack /etc/rc.d/S95aircrack
                chmod 755 /etc/init.d/aircrack
                else
                uci_set "app.aircrack" "set" "bootaircrack" ""
                if [ -s "/etc/init.d/aircrack" ] ; then rm /etc/init.d/aircrack ; rm /etc/rc.d/S95aircrack; fi
                fi        #-----------------------------------

                uci_commit "app.aircrack"
                rm /tmp/aircrack.ivs 2> /dev/null
                exit
        fi

        ######## Save AiroDump
        if ! empty "$FORM_save_airodump"; then

                APP_Refresh_Page $SCRIPT_NAME?page=$FORM_page "<br/>saving ...</br/>"
                uci_set "app.aircrack" "set" "path" "$FORM_airdpath"

                #---- If checkbox on startup AiroDump ----
                if ! empty "$FORM_airodump_startup"; then
                uci_set "app.aircrack" "set" "bootairodump" "checked"

                if  [ -s "/etc/init.d/airodump" ]  ; then rm /etc/init.d/airodump ; fi
        
echo "#!/bin/sh

wlc monitor 1
airodump-ng $hIVS -w $CFG_PATH/key prism0 &" > /etc/init.d/airodump

                ln -s /etc/init.d/airodump /etc/rc.d/S96airodump
                chmod 755 /etc/init.d/airodump
                else
                uci_set "app.aircrack" "set" "bootairodump" ""
                if [ -s "/etc/init.d/airodump" ] ; then rm /etc/init.d/airodump ; rm /etc/rc.d/S96airodump; fi
                fi        #-----------------------------------

                if ! empty "$FORM_chkivs"; then
                        uci_set "app.aircrack" "set" "ivs" "checked"
                else        uci_set "app.aircrack" "set" "ivs" "" ; fi

                uci_commit "app.aircrack"
                exit
        fi

if [ "$FORM_page" = "aircrack" ]; then ########################
                
####### Check if airodump is running
        if [ $(ps | grep -c aircrack-ng) = "1" ] ; then

        cat <<EOF
        <form method="post" action='$SCRIPT_NAME'>
        <div class=warning>Aircrack is not running</div>&nbsp;&nbsp;<input type="submit" name="run_aircrack" class='flatbtn' value="Run Aircrack" /><br/>

EOF
if [ -s "$CFG_PATH/aircrack.key" ]; then 
        echo "<br/><center><table border="0" cellspacing="1" bgcolor="#000000"><tr bgcolor='#FF0000'><td>"
        grep '' < $CFG_PATH/aircrack.key
        echo "</td></tr></table></center>"        
fi

if [ -s "/tmp/aircrack.ivs" ]; then 
        echo "<br/><table width="85%" border="0" cellspacing="1" bgcolor="#000000" align='center'><tr bgcolor='#FFFFFF'><td><table width='100%' border='0'>"
        echo "<tr bgcolor='#999999'><td width='50%' colspan=2><center><b>MAC Address</b></center></td><td width='40%'><center><b>SSID</b></center></td><td><center><b>Encryption</b></center></td><td><center><b>IVs</b></center></td></tr>"

        exec 0</tmp/aircrack.ivs
        while read line
        do
if ! equal $line "" ; then
        if [ "$color" = "#FFFFFF" ] ; then color="#E6E6E6" ; else color="#FFFFFF" ; fi
        a_bssid=$(echo $line | awk '{ print $2 }')
        a_ssid=$(echo $line | awk '{ print $3 }')
        a_enc=$(echo $line | awk '{ print $4 }')
        a_ivs=$(echo $line | awk '{ print $5 }')
        if equal $a_ssid "WEP" || equal $a_ssid "WEP" ; then a_ssid="" ; a_enc=$(echo $line | awk '{ print $3 }') ; a_ivs=$(echo $line | awk '{ print $4 }') ; fi
        echo "<tr bgcolor='$color'><td><input type='radio' name='selectMAC' value='$a_bssid $(echo $a_ivs | sed -e s/'('//g)'></td><td><center>$a_bssid</center></td><td><center>$a_ssid</center></td><td><center>$a_enc</center></td><td>$(echo $a_ivs | sed -e s/'('//g)</td></tr>"

fi        
        done
        echo "</table></td></tr></table>"
fi
        if [ -s $CFG_CPATH ] && [ ! -s "/tmp/aircrack.ivs" ]; then 
                APP_Refresh_Page $SCRIPT_NAME?page=$FORM_page "<br/>Reading IVs ...</br/>"
                aircrack-ng $CFG_CPATH | grep -vE 'Opening|#  BSSID|Read|Index' > /tmp/aircrack.ivs &
                sleep 1                
                killall -q aircrack-ng
        fi

        echo "</form><br/>"

        else echo "<form method="post" action='$SCRIPT_NAME'><font color="#33CC00">Aircrack is succesfully started</font>&nbsp;&nbsp;<input type="submit" name="stop_aircrack" class='flatbtn' value='Stop Aircrack' /></form><br/><br/>"
        fi

        cat <<EOF
        <strong>Aircrack Configuration</strong><br/>
        <script type='text/javascript' src='/js/forms.js'></script>
        <script type='text/javascript'>
        window.onload = function() {
        setupDependencies('aircrack_cfg');
        }
        </script>
        <br/><form action='$SCRIPT_NAME' method='post' name='aircrack_cfg'>

        <table width="100%" border="0" cellspacing="1">
EOF
        HTML_Table_Line 2
        echo "<tr><td><a href='#' rel='b1'><br/>Key Type:</a></td><td><br/><select name='keytype' STYLE='width: 150px'>"
        if [ $CFG_KEY = "64" ] ; then HTML_key_option "selected"
        elif [ $CFG_KEY = "128" ] ; then HTML_key_option "" "selected"
        elif [ $CFG_KEY = "wpa" ] ; then HTML_key_option "" "" "selected"
        fi
        echo "</select></td></tr>"

        HTML_Table_TR "b2" "CAP/IVS File:" "<input name='aircpath' type='text' value='$CFG_CPATH' />" 200
        HTML_Table_TR "b3" "<label>WPA Keys File:<input type='hidden' class='DEPENDS ON keytype BEING wpa' /></label>" "<input name='wpapath' type='text' value='$CFG_WPATH' class='DEPENDS ON keytype BEING wpa' /><br/>" 
        HTML_Table_Line 2
        HTML_Table_TR "" "" "<input type="checkbox" name="aircrack_startup" $CFG_BOOTAIR />&nbsp;Run on boot<br/><br/>"
        HTML_Table_TR "" "" "<input name='page' type='hidden' value='$FORM_page' /><input type='submit' class='flatbtn' name='save_aircrack' value='Save' />"

        echo "</table></form>"

        TIP 0 "Encryption key"
        TIP 0 "Path to captured .cap or .ivs file"
exit
elif [ "$FORM_page" = "airodump" ]; then ########################


        ####### Check if airodump is running
        if [ $(ps | grep -c airodump-ng) = "1" ] ; then

        cat <<EOF
        <form method="post" action='$SCRIPT_NAME'>
        <div class=warning>Airodump is not running</div>&nbsp;&nbsp;<input type="submit" class='flatbtn' name="run_airodump" value="Run Airodump" /><br/>
        </form><br/>
EOF
        else echo "<form method="post" action='$SCRIPT_NAME'><font color="#33CC00">Airodump is succesfully started</font>&nbsp;&nbsp;<input type="submit" class='flatbtn' name="stop_airodump" value='Stop Airodump' /></form><br/><br/>"
        fi

        cat <<EOF
        <strong>AiroDump Configuration</strong><br/>
        <br/><form action='$SCRIPT_NAME' method='post'>
        <table width="100%" border="0" cellspacing="1">
EOF
        HTML_Table_Line 2
        HTML_Table_TR "b1" "<br/>Storage Path:" "<br/><input name='airdpath' type='text' value='$CFG_PATH' />" 200
        HTML_Table_TR "b2" "Channel:" "<input name='airchannel' type='text' value='$CFG_CHANNEL' />"
        HTML_Table_TR "" "" "<input type='checkbox' name='chkivs' $CFG_IVS />&nbsp;Save only captured IVs"
        HTML_Table_Line 2
        HTML_Table_TR "" "" "<input type='checkbox' name='airodump_startup' $CFG_BOOTDUMP />&nbsp;Run on boot<br/><br/>"
        HTML_Table_TR "" "" "<input name='page' type='hidden' value='$FORM_page' /><input type='submit' class='flatbtn' name='save_airodump' value='Save' />"
        echo "</table></form>"

        TIP 0 "Path where airodump-ng will store IVS."
        TIP 0 "Set specific channel to scan. [nothing = all]"
exit
elif [ "$FORM_page" = "airoplay" ]; then ########################
        echo "in development..."
exit
else                                                ########################

        HTML_Table 1 "100%" "999999" "align='center'" 3 "Captured_File width='80%'" "IVS" "Action"

        if [ "$FORM_page" = "showstatus" ]; then
                echo "<tr><td bgcolor="#FFFFFF"><pre><font size=2>$(grep '' < $CFG_PATH/$FORM_file.txt)</font></pre></td></tr>"
        elif [ "$FORM_page" = "deletestatus" ]; then
                APP_Refresh_Page $SCRIPT_NAME "<br/>Deleting $FORM_file ...</br/>"
                rm $CFG_PATH/$FORM_file.txt 
                rm $CFG_PATH/$FORM_file.ivs
        elif [ "$FORM_page" = "aircrackit" ]; then
                rm /tmp/aircrack.ivs 2> /dev/null
                uci_set "app.aircrack" "set" "cpath" "$CFG_PATH/$FORM_file.ivs"
                uci_commit "app.aircrack"
                APP_Refresh_Page $SCRIPT_NAME?page=aircrack "<br/>Prepearing ($FORM_file) for Aircrack ...</br/>"
                exit
        elif [ "$FORM_page" = "savestatus" ]; then
                (cd $CFG_PATH; tar czf /www/$FORM_file.tgz $CFG_PATH/$FORM_file.* ) > /dev/null 2>&1
                APP_Refresh_Page "/$FORM_file.tgz" ""
                sleep 25 ; rm /www/$FORM_file.tgz
                exit
        else
                ls -a $CFG_PATH | grep ".txt" | sed -e s/'.txt'//g | awk '{ print $1 }' | while read output;
                do
                if [ "$color" = "#FFFFFF" ] ; then color="#E6E6E6" ; else color="#FFFFFF" ; fi
                echo "<tr bgcolor='$color'><td>&nbsp;<a href=\"$SCRIPT_NAME?page=showstatus&file=$output\">$output</a></td><td><center>?</center></td><td><center><a href='$SCRIPT_NAME?page=savestatus&file=$output' target='DLIVS'><img src='/images/action_sv.gif' alt='Save' /></a>&nbsp;<a href='$SCRIPT_NAME?page=aircrackit&file=$output'><img src='/images/action_ok.gif' alt='AirCrack' /></a>&nbsp;<a href='$SCRIPT_NAME?page=deletestatus&file=$output'><img src='/images/action_x.gif' alt='Delete' /></a></center></td></tr>"
                done
        fi
        echo "</table></td></tr></table><IFRAME STYLE=\"width:0px; height:0px;\" FRAMEBORDER='0' SCROLLING='no' name='DLIVS'></IFRAME>"
fi
echo "<br/></body></html>"
fi
?>
