#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
. /www/cgi-bin/webif/ast_functions.sh

uci_load "mars"

header "Asterisk" "Servers" "<center><img src="/images/mars1.jpg"> @TR<<Remote MARS Servers>> <img src="/images/mars1.jpg"></center>" '' "$SCRIPT_NAME"

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

  FORM_server1_username=$CONFIG_servers_AST_Server1UserName
  FORM_server1_pwd=$CONFIG_servers_AST_Server1Pwd
  FORM_server1_host=$CONFIG_servers_AST_Server1Host

  FORM_server2_username=$CONFIG_servers_AST_Server2UserName
  FORM_server2_pwd=$CONFIG_servers_AST_Server2Pwd
  FORM_server2_host=$CONFIG_servers_AST_Server2Host

  FORM_server3_username=$CONFIG_servers_AST_Server3UserName
  FORM_server3_pwd=$CONFIG_servers_AST_Server3Pwd
  FORM_server3_host=$CONFIG_servers_AST_Server3Host

  FORM_server4_username=$CONFIG_servers_AST_Server4UserName
  FORM_server4_pwd=$CONFIG_servers_AST_Server4Pwd
  FORM_server4_host=$CONFIG_servers_AST_Server4Host

  FORM_tts_server=$CONFIG_system_AST_TTS_Server
  FORM_tts_format=$CONFIG_system_AST_TTS_Format
fi

#--------------------------------------------------------------------------#
# Write values to (temp) config file (/etc/.uci/mars) until commit

if [ "$FORM_action" = "Save Changes" ]; then
#  if [ "$FORM_action" = "SaveServers" ]; then
# echo '<center><table><tr><td>'

  COUNT=1
  while [  $COUNT -lt 5 ]; do

 # Save values to Config File
#   echo "Saving Server "$COUNT" ...<br>"
    VNAME=FORM_server"$COUNT"_username
    eval VVALUE=\$$VNAME 
    echo "Saving Server "$COUNT" "$VNAME" "$VVALUE" ...<br>"
    uci_set mars servers AST_Server"$COUNT"UserName "$VVALUE"

    VNAME=FORM_server"$COUNT"_pwd
    eval VVALUE=\$$VNAME 
    uci_set mars servers "AST_Server"$COUNT"Pwd" "$VVALUE"

    VNAME=FORM_server"$COUNT"_host
    eval VVALUE=\$$VNAME 
    uci_set mars servers "AST_Server"$COUNT"Host" "$VVALUE"

  let COUNT=$COUNT+1
  done

  uci_set mars system AST_TTS_Server "$FORM_tts_server"
  uci_set mars system AST_TTS_Format "$FORM_tts_format"
  SAVED=1
fi

#--------------------------------------------------------------------------#
# Display Form 

  echo '<center><table cellspacing="4"><tr><td colspan=3>'
  echo '<h3>Remote MARS Servers</h3></td>'
  echo '<td align=center><a href="/MngrManual_en.html#Servers" target="_blank"><h3>Help</h3></a></td></tr>'
  echo '<tr><td></td><th>Server UserName</th><th>Password</th><th>Host (IP or dyndns)</th>'

  COUNT=1
  while [  $COUNT -lt 5 ]; do
    echo '<tr><th>MARS'$COUNT': </th>'
    VNAME=FORM_server"$COUNT"_username
    eval VVALUE=\$$VNAME 
    echo '<td><INPUT type=text name="server'$COUNT'_username" value="'$VVALUE'" size="32" maxlength="128"></td>'
    VNAME=FORM_server"$COUNT"_pwd
    eval VVALUE=\$$VNAME 
    echo '<td><INPUT type=text name="server'$COUNT'_pwd" value="'$VVALUE'" size="32" maxlength="128"></td>'
    VNAME=FORM_server"$COUNT"_host
    eval VVALUE=\$$VNAME 
    echo '<td><INPUT type=text name="server'$COUNT'_host" value="'$VVALUE'" size="32" maxlength="128"></td></tr>'
    let COUNT=$COUNT+1
  done

###   echo '<tr><th>MARS1: </th>'
###   echo '<td><INPUT type=text name="server1_username" value="'$FORM_server1_username'" size="32" maxlength="128"></td>'
###   echo '<td><INPUT type=text name="server1_pwd" value="'$FORM_server1_pwd'" size="32" maxlength="128"></td>'
###   echo '<td><INPUT type=text name="server1_host" value="'$FORM_server1_host'" size="32" maxlength="128"></td></tr>'
###   echo '<tr><th>MARS2: </th>'

  echo '<tr><td colspan=4><h3>Remote TTS Server</h3></td></tr>'
  echo '<tr><th>TTS Host</th>'
  echo '<td colspan=3><INPUT type=text name="tts_server" value="'$FORM_tts_server'" size="32" maxlength="128"></td>'
  echo '<tr><th>Format (ul/gsm)</th>'
  echo '<td colspan=3><INPUT type=text name="tts_format" value="'$FORM_tts_format'" size="5" maxlength="5"></td></tr>'
  echo '<tr><td colspan=4><h3></h3></td></tr>'

  echo '</td></tr>'
  echo '</table></center>'

if [ "$DEBUG" = "1" ]; then
echo '<pre>'
env | sort
echo '</pre>'
fi

footer ?>

<!--
##WEBIF:name:Asterisk:150:Servers
-->

