#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
. /www/cgi-bin/webif/ast_functions.sh

uci_load "mars"

header "Asterisk" "WebCall" "<center><img src="/images/mars1.jpg"> @TR<<Web Call Configuration>> <img src="/images/mars1.jpg"></center>" '' "$SCRIPT_NAME"
DEBUG=0

if [ "$DEBUG" = "1" ]; then
  echo "FORM_action= "$FORM_action
fi


#--------------------------------------------------------------------------#

if [ -e /var/run/asterisk.pid ]; then
  ast_pid=$(cat /var/run/asterisk.pid)
  ast_proc_info="$(cat /proc/$ast_pid/cmdline | sed 's/\0/ /g')"
  asterisk_exec="$(echo $ast_proc_info | awk '{print $1}')"
else
  echo "@TR<<Asterisk is not running.>>"
fi

#--------------------------------------------------------------------------#
# Load values from config file (/etc/config/mars)

if [ "$FORM_action" = "" ]; then
  # Remote Server Form
  server_id=$(uci get mars.system.AST_ServerId)
  FORM_AST_WebCallEnabled=$CONFIG_webcall_AST_WebCallEnabled
  FORM_AST_WebCallRetries=$CONFIG_webcall_AST_WebCallRetries
  FORM_AST_WebCallInterval=$CONFIG_webcall_AST_WebCallInterval
  FORM_AST_WC_OnSuccess=$CONFIG_webcall_AST_WC_OnSuccess
  FORM_AST_WC_OnTelNoError=$CONFIG_webcall_AST_WC_OnTelNoError
  FORM_AST_WC_OnDisabled=$CONFIG_webcall_AST_WC_OnDisabled
  FORM_AST_WC_OnBlackList=$CONFIG_webcall_AST_WC_OnBlackList
fi

#--------------------------------------------------------------------------#
# Write values to (temp) config file (/etc/.uci/mars) until commit

if [ "$FORM_action" = "Save Changes" ]; then
  uci_set mars webcall AST_WebCallEnabled $FORM_AST_WebCallEnabled
  uci_set mars webcall AST_WebCallRetries $FORM_AST_WebCallRetries
  uci_set mars webcall AST_WebCallInterval $FORM_AST_WebCallInterval
  uci_set mars webcall AST_WC_OnSuccess $FORM_AST_WC_OnSuccess
  uci_set mars webcall AST_WC_OnTelNoError $FORM_AST_WC_OnTelNoError 
  uci_set mars webcall AST_WC_OnDisabled $FORM_AST_WC_OnDisabled 
  uci_set mars webcall AST_WC_OnBlackList $FORM_AST_WC_OnBlackList
SAVED=1
fi

#--------------------------------------------------------------------------#
# Display Form 

   echo '<center><table cellspacing="4"><tr><td colspan=2>'
   echo '<h3>Web Call</h3></td>'
   echo '<td align=center><a href="/MngrManual_en.html#WebCall" target="_blank"><h3>Help</h3></a></td></tr>'
###   echo '<tr><td></td><th>Server UserName</th><th>Password</th><th>Host (IP or dyndns)</th>'

  echo '<tr><th>WebCall Enable: </th>'
  if [ "$FORM_AST_WebCallEnabled"  = "" ]; then
    opts=""
  else
    opts="checked=\"checked\"" 
  fi
  echo '<td><input type="checkbox" '$opts' value="1" name="AST_WebCallEnabled" id="field_webcall_enabled"></td>'
  echo '<td class="settings-help">Check to enable the Web Call Service (Changing requires reboot) </td></tr>'
  echo '<tr><th>Nb Retries: </th>'
  echo '<td><INPUT type=text name="AST_WebCallRetries" value="'$FORM_AST_WebCallRetries'" size="2" maxlength="2"></td>'
  echo '<td class="settings-help">Number of times the call will be retried when extension busy</td></tr>'
  echo '<tr><th>Interval: </th>'
  echo '<td><INPUT type=text name="AST_WebCallInterval" value="'$FORM_AST_WebCallInterval'" size="2" maxlength="2"></td>'
  echo '<td class="settings-help">Number of minutes to wait to retry calling extension</td></tr>'

  echo '<tr><th>On Success</th>'
  echo '<td><INPUT type=text name="AST_WC_OnSuccess" value="'$FORM_AST_WC_OnSuccess'" size="32" maxlength="128"></td>'
  echo '<td class="settings-help">Url to send the WebCall user on success</td></tr>'
  echo '<tr><th>On Disabled</th>'
  echo '<td><INPUT type=text name="AST_WC_OnDisabled" value="'$FORM_AST_WC_OnDisabled'" size="32" maxlength="128"></td>'
  echo '<td class="settings-help">Url to send the WebCall user when WC disabled</td></tr>'
  echo '<tr><th>On Black Listed</th>'
  echo '<td><INPUT type=text name="AST_WC_OnBlackList" value="'$FORM_AST_WC_OnBlackList'" size="32" maxlength="128"></td>'
  echo '<td class="settings-help">When user is not allowed to use the service anymore</td></tr>'
  echo '<tr><th>On TelNo Error</th>'
  echo '<td><INPUT type=text name="AST_WC_OnTelNoError" value="'$FORM_AST_WC_OnTelNoError'" size="32" maxlength="128"></td>'
  echo '<td class="settings-help">Url to send the WebCall user on wrong Tel. No.</td></tr>'
  echo '</table></center>'

if [ "$DEBUG" = "1" ]; then
echo '<pre>'
env | sort
echo '</pre>'
fi

footer ?>

<!--
##WEBIF:name:Asterisk:170:WebCall
-->

