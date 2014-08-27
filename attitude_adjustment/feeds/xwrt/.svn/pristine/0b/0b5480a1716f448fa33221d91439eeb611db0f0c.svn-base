#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
. /www/cgi-bin/webif/ast_functions.sh

uci_load "mars"

header "Asterisk" "System" "<center><img src="/images/mars1.jpg"> @TR<<Asterisk System>> <img src="/images/mars1.jpg"></center>" '' "$SCRIPT_NAME"

if [ "$DEBUG" = "1" ]; then
  echo 'Form_action='$FORM_action'<br>'
  echo 'Form_submit='$FORM_submit'<br>'
fi

#--------------------------------------------------------------------------#

# ===================================================================

 if [ -e /var/run/asterisk.pid ]; then
   ast_pid=$(cat /var/run/asterisk.pid)
   ast_proc_info="$(cat /proc/$ast_pid/cmdline | sed 's/\0/ /g')"
   asterisk_exec="$(echo $ast_proc_info | awk '{print $1}')"
else
  echo "@TR<<Asterisk is not running.>>"
fi
    echo '<script type="text/javascript" src="/mars.js"></script>'

  if [ "$FORM_action" = "EMail Test" ]; then
    FORM_admin_email=$CONFIG_system_AST_ADMIN_Email
    FORM_smtp_server=$CONFIG_system_AST_SMTP_Server
    echo "<script type=\"text/javascript\">test_email(\"$FORM_admin_email\",\"$FORM_smtp_server\");</script>"
    FORM_action="" 
  fi

  if [ "$FORM_action" = "" ]; then
    # ADMIN Get Admin variables
    FORM_server_id=$CONFIG_system_AST_ServerId
    FORM_serv_language=$CONFIG_system_AST_ServiceLanguage
    FORM_admin_email=$CONFIG_system_AST_ADMIN_Email
    FORM_smtp_server=$CONFIG_system_AST_SMTP_Server
    FORM_local_prefixes_disabled=$CONFIG_system_AST_LocalPrefixesDisabled
    FORM_local_area_code=$CONFIG_system_AST_LocalAreaCode
    FORM_local_prefix=$CONFIG_system_AST_LocalPrefix
    FORM_webcall_disabled=$CONFIG_system_AST_WebCallDisabled
    FORM_load_prompts_in_ram=$CONFIG_system_AST_LoadPromptsInRAM
    FORM_cfg_password=$CONFIG_system_AST_CGPassword

    # IAX Queue Form
    FORM_queue_usage=${AST_QueueUsage:-$CONFIG_queue_AST_QueueUsage}
    FORM_queue_usage=${FORM_queue_usage:-'0'}
    FORM_queue_strategy=${AST_QueueStrategy:-$CONFIG_queue_AST_QueueStrategy}
    FORM_queue_strategy=${FORM_queue_strategy:-'0'}
    FORM_queue_0=$CONFIG_queue_AST_Queue0
    FORM_queue_1=$CONFIG_queue_AST_Queue1
    FORM_queue_2=$CONFIG_queue_AST_Queue2
    FORM_queue_3=$CONFIG_queue_AST_Queue3
    FORM_queue_4=$CONFIG_queue_AST_Queue4
    FORM_queue_5=$CONFIG_queue_AST_Queue5
    FORM_queue_6=$CONFIG_queue_AST_Queue6
    FORM_queue_7=$CONFIG_queue_AST_Queue7

    FORM_time_enabled=$CONFIG_system_AST_TimeEnabled
    FORM_time_from=$CONFIG_system_AST_TimeFrom
    FORM_time_to=$CONFIG_system_AST_TimeTo
    FORM_time_days=$CONFIG_system_AST_TimeDays
fi

if [ "$FORM_action" = "Save Changes" ]; then
    # Save values to Config FILE
    uci set mars.system.AST_ServerId="$FORM_server_id"
    uci set mars.system.AST_ServiceLanguage="$FORM_serv_language"
    uci set mars.system.AST_ADMIN_Email="$FORM_admin_email"
    uci set mars.system.AST_SMTP_Server="$FORM_smtp_server"
    uci set mars.system.AST_CGPassword="$FORM_cfg_password"
    uci set mars.system.AST_WebCallDisabled="$FORM_webcall_disabled"
    uci set mars.system.AST_LoadPromptsInRAM="$FORM_load_prompts_in_ram"
    uci set mars.system.AST_LocalPrefixesDisabled="$FORM_local_prefixes_disabled"

    uci set mars.system.AST_LocalAreaCode="$FORM_local_area_code"
    uci set mars.system.AST_LocalPrefix="$FORM_local_prefix"

    uci set mars.trunks.AST_TrunkProto="$FORM_trunks_proto"
    uci set mars.queue.AST_QueueUsage="$FORM_queue_usage"
    uci set mars.queue.AST_QueueStrategy="$FORM_queue_strategy"

    uci set mars.queue.AST_Queue0="$FORM_queue_0"
    uci set mars.queue.AST_Queue1="$FORM_queue_1"
    uci set mars.queue.AST_Queue2="$FORM_queue_2"
    uci set mars.queue.AST_Queue3="$FORM_queue_3"
    uci set mars.queue.AST_Queue4="$FORM_queue_4"
    uci set mars.queue.AST_Queue5="$FORM_queue_5"
    uci set mars.queue.AST_Queue6="$FORM_queue_6"
    uci set mars.queue.AST_Queue7="$FORM_queue_7"

    uci set mars.system.AST_TimeFrom="$FORM_time_from"
    uci set mars.system.AST_TimeTo="$FORM_time_to"
    uci set mars.system.AST_TimeDays="$FORM_time_days"
    uci set mars.system.AST_TimeEnabled="$FORM_time_enabled"
    SAVED=1
    export SAVE_QUEUE=1

###TMP In apply-mars ...     write_queue
  fi

    echo '<center><table><tr><td colspan=1><h3>'
    echo 'System</h3></td><td  colspan=3 align=center><a href="/MngrManual_en.html#System" target="_blank"><h3>Help</h3></a></td></tr>'

    echo '<tr><th>System ID (Server No): </th>'
    echo '<td colspan=3>'
    select_2 server_id $FORM_server_id
    option_2 200 "1 - Ext: 200-207"
    option_2 300 "2 - Ext: 300-307"
    option_2 400 "3 - Ext: 400-407"
    option_2 500 "4 - Ext: 500-507"
    echo '</select></td></tr>'

    echo '<tr><th>Service Main Language (E/F): </th>'
    echo '<td colspan=3><INPUT type=text name="serv_language" value="'$FORM_serv_language'" size="1" maxlength="1"></td></tr>'
    echo '<tr><th>Admin Email: </th>'
    echo '<td colspan=3><INPUT type=text name="admin_email" value="'$FORM_admin_email'" size="48" maxlength="128"></td></tr>'
    echo '<tr><th>SMTP Server: </th>'
    echo '<td colspan=2><INPUT type=text name="smtp_server" value="'$FORM_smtp_server'" size="48" maxlength="128"></td>'
    echo "<td colspan=4 id=\"EMail_Test\"><center><input type=\"button\" value=\"@TR<<EMail Test>>\" onClick=\"return ConfirmEMailTest ('$FORM_admin_email','$FORM_smtp_server');\" ></center></td></tr>"

    echo '<tr><th>Cell Phone Gateway Pwd: </th>'
    echo '<td colspan=3><INPUT type=text name="cfg_password" value="'$FORM_cfg_password'" size="8" maxlength="8"></td></tr>'
###     echo '<tr><td colspan=4><br><h3></h3></td></tr>'

###     echo '<tr><th>WebCall Disabled: </th>'
###     if [ "$FORM_webcall_disabled"  = "" ]; then
###       opts=""
###     else
###       opts="checked=\"checked\"" 
###     fi
###     echo '<td colspan=3><input type="checkbox" '$opts' value="1" name="webcall_disabled" id="field_webcall_disabled"></td></tr>'
    echo '<tr><th>Load Prompts in RAM: </th>'
    if [ "$FORM_load_prompts_in_ram"  = "" ]; then
      opts=""
    else
      opts="checked=\"checked\"" 
    fi
    echo '<td colspan=3><input type="checkbox" '$opts' value="1" name="load_prompts_in_ram" id="field_load_prompts_in_ram"></td></tr>'
    echo '<tr><td colspan="4"><h3></h3></td></tr>'
    echo '<tr><th>Local Prefixes Disable:</th><td colspan=3>'
    checkbox local_prefixes_disabled "$FORM_local_prefixes_disabled" 1 
    echo '</td></tr>'

    if [ "$FORM_local_prefixes_disabled" = "" ]; then
      echo '<tr><th>Local Prefixes: </th>'
      echo '<td>Area Code: <INPUT type=text name="local_area_code" value="'$FORM_local_area_code'" size="3" maxlength="3">'
      echo '<td>TelNo. Prefix: <INPUT type=text name="local_prefix" value="'$FORM_local_prefix'" size="3" maxlength="3"></td>'
      echo "<td><INPUT type=\"button\" name=\"get_local_codes\" value=\"Get Codes\" title=\"You need to save and Apply first.\" onClick=\"GetLocalAreaCodes ('$FORM_local_area_code','$FORM_local_prefix');\"> (Press to fetch from WWW)</td>"
      echo '<tr><th>Local Prefixes List: </th>'
      echo '<td colspan=3><textarea readonly="readonly" cols="6" rows="4" name="LocalPrefixes">'
      if [ -e "/usr/lib/asterisk/agi-bin/local.txt" ]; then
        cat /usr/lib/asterisk/agi-bin/local.txt | sort
      fi
      echo '</textarea></td></tr>'
    fi
    echo '<tr><td colspan=4><h3></h3></td></tr>'

#===================================================================
  
    echo '<tr><td align=left border=0 colspan=1>'
    echo '<h3>Queues</h3></td>'
    echo '<td colspan=3 align=center><a href="/MngrManual_en.html#Queue" target="_blank"><h3>Help</h3></a></td></tr>'
    echo '<tr><th>Use Queue: </th>'
    echo '<td colspan=3>'
    radio_button queue_usage $FORM_queue_usage 0 "When Auto-Attendant Off"
    radio_button queue_usage $FORM_queue_usage 1 "Always (No AA)"
    radio_button queue_usage $FORM_queue_usage 2 "Never (AA or Ext:200)"
    echo '</td></tr>'
    echo '<tr><th>Queue Strategy: </th>'
    echo '<td colspan=3>'
    radio_button queue_strategy $FORM_queue_strategy 0 "Round Robin"
    radio_button queue_strategy $FORM_queue_strategy 1 "Ring All"
    echo '</td></tr>'
    echo '<tr><th>Members: </th>'
    echo '<td colspan=3>'

    queue_id=$CONFIG_system_AST_ServerId
    checkbox queue_0 "$FORM_queue_0" 1 "$queue_id"
    let "queue_id+=1"
    checkbox queue_1 "$FORM_queue_1" 1 "$queue_id"
    let "queue_id+=1"
    checkbox queue_2 "$FORM_queue_2" 1 "$queue_id"
    let "queue_id+=1"
    checkbox queue_3 "$FORM_queue_3" 1 "$queue_id"
    let "queue_id+=1"
    checkbox queue_4 "$FORM_queue_4" 1 "$queue_id"
    let "queue_id+=1"
    checkbox queue_5 "$FORM_queue_5" 1 "$queue_id"
    let "queue_id+=1"
    checkbox queue_6 "$FORM_queue_6" 1 "$queue_id"
    let "queue_id+=1"
    checkbox queue_7 "$FORM_queue_7" 1 "$queue_id"
    echo '</td></tr>'
    echo '</center></td></tr>'

#===================================================================

    echo '<tr><td colspan=1><h3>Time Conditions</h3></td>'
    echo '<td  colspan=3 align=center><a href="/MngrManual_en.html#TimeConditions" target="_blank"><h3>Help</h3></a></td></tr>'
    echo '<tr><th>Enable:</th><td colspan=4>'
    checkbox time_enabled "$FORM_time_enabled" 1 
    echo '</td></tr><tr><td colspan=4><h3></h3></td></tr>'
    echo '<tr><td></td><td colspan=1 style="text-align: center">From Hours</td>'
    echo '<td style="text-align: center">To Hours</td><td style="text-align: center">Days</td></tr>'
    echo '<tr><td></td><td colspan=1 style="text-align: center">(hh:mm)</td><td style="text-align: center">(hh:mm)</td>'
    echo '<td style="text-align: center">(ddd-ddd)</td></tr>'
    echo '<tr><td></td><td colspan=1 style="text-align: center"><INPUT type="text" name="time_from" value="'$FORM_time_from'" size="5" maxlength="5" title="Enter the opening time in 24 hour format (Ex: 09:00)"></td>'
    echo '<td style="text-align: center"><INPUT type="text" name="time_to" value="'$FORM_time_to'" size="5" maxlength="5" title="Enter the closing time in 24 hour format (Ex: 18:00)"></td>'
    echo '<td style="text-align: center"><INPUT type="text" name="time_days" value="'$FORM_time_days'" size="7" maxlength="7"  title="Enter the days span (ddd-ddd) (mon/tue/wed/thu/fri/sat/sun)"></td></tr>'
    echo '<tr><td colspan=4><h3></h3></td></tr>'
    echo '</td></tr></table></center>'

#===================================================================

if [ "$DEBUG" = "1" ]; then
echo '<pre>'
env | sort
echo '</pre>'
fi

footer ?>

<!--
##WEBIF:name:Asterisk:120:System
-->

