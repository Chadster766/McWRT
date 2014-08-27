#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
. /www/cgi-bin/webif/ast_functions.sh

header "Asterisk" "Dev" "<img src="/images/mars1.jpg"> @TR<<MARS Development>> <img src="/images/mars1.jpg">"

asterisk_calls="/var/log/asterisk/cdr-csv/Master.csv"
asterisk_cdr='"${CDR(accountcode)","${CDR(src)}","${CDR(dst)}","${CDR(dcontext)}","${CDR(clid)}","${CDR(channel)}","${CDR(dstchannel)}","${CDR(lastapp)}","${CDR(lastdata)}","${CDR(start)}","${CDR(answer)}","${CDR(end)}","${CDR(duration)}","${CDR(billsec)}","${CDR(disposition)}","${CDR(amaflags)}"'
asterisk_logs="/var/log/asterisk/messages"
show_call_reverse="yes"
ONLY_ONE_VOIP="FALSE"
NO_SIP_TRUNK=TRUE
MARS_VERSION="MARS V1.1 by TELE DATA Inc. 04/2007"

options=""

MNGR_LOGON="http://192.168.1.10:8088/asterisk/manager?action=login&username=MARS_Mngr&secret=Q1w2E3r4T5y"
MNGR_LOGOF="http://192.168.1.10:8088/asterisk/manager?action=logoff"
MNGR_SIP_PEERS="http://192.168.1.10:8088/asterisk/manager?action=SipPeers"


#--------------------------------------------------------------------------#
#--------------------------------------------------------------------------#
#
#
#--------------------------------------------------------------------------#

if [ -e /var/run/asterisk.pid ]; then
  ast_pid=$(cat /var/run/asterisk.pid)
  ast_proc_info="$(cat /proc/$ast_pid/cmdline | sed 's/\0/ /g')"
  asterisk_exec="$(echo $ast_proc_info | awk '{print $1}')"
  ast_conf_path=/etc/asterisk

  echo '<script type="text/javascript" src="/mars.js"></script>'

  echo '<center>'
  echo '<table><tr><td align=center border=0 colspan=6>'
  echo '<h3>+---------------------------------------------------------------------------------------------------+</h3></td></tr>'

  echo '<tr><td align=center><a href="'$SCRIPT_NAME'?action=applications">Applications</a></td>'
  echo '<td align=center><a href="'$SCRIPT_NAME'?action=functions">Functions</a></td>'
  echo '<td align=center><a href="'$SCRIPT_NAME'?action=manager_commands">Mngr Cmd</a></td>'
  echo '<td align=center><a href="'$SCRIPT_NAME'?action=agi">AGI Cmd</a></td>'
  echo '<td align=center><a href="'$SCRIPT_NAME'?action=modules">Modules</a><br></td>'
  echo '<td align=center><a href="'$SCRIPT_NAME'?action=reload">Reload</a><br></td></tr>'

  echo '<tr><td align=center><a href="'$SCRIPT_NAME'?action=formats">Formats</a></td>'
  echo '<td align=center><a href="'$SCRIPT_NAME'?action=translation">Trans</a></td>'
  echo '<td align=center><a href="'$SCRIPT_NAME'?action=uptime">UpTime</a></td>'
  echo '<td align=center><a href="'$SCRIPT_NAME'?action=database">DataBase</a></td>'
  echo '<td align=center><a href="'$SCRIPT_NAME'?action=dyndns_restart">DynDNS Restart</a></td>'
  echo '<td align=center><a href="'$SCRIPT_NAME'?action=restart">Restart</a></td><tr>'

  echo '<tr><td align=center><a href="'$SCRIPT_NAME'?action=globals">Globals</a></td>'
  echo '<td align=center><a href="'$SCRIPT_NAME'?action=nvram">AST nvram</a></td>'
  echo '<td align=center><a href="'$SCRIPT_NAME'?action=editor">.conf Editor</a></td>'
  echo '<td align=center><a href="'$SCRIPT_NAME'?action=dialplan">Dialplan</a></td>'
#  echo '<td align=center><a href="'$SCRIPT_NAME'?action=screen">Screen (Beta)</td>'
  echo '<td align=center></td>'
  echo '<td align=center><a href="'$SCRIPT_NAME'?action=cust_com">Cmd</a></td></tr>'

##  echo "<tr><td><input type=\"button\" value=\"MngrLogin\" onClick=\"MngrLogin ('$MNGR_LOGON');\" ></td>"
  echo '<tr><td align=center><a href='"$MNGR_LOGON"' target="_blank">Mngr Login</a></td>'
  echo '<td align=center><a href='"$MNGR_LOGOFF"'>Mngr Logoff</a></td>'
  echo '<td align=center><a href='"$MNGR_SIP_PEERS"' target="_blank">SIP Peers</a></td>'
  echo '<td align=center></td>'
  echo '<td align=center><a href="'$SCRIPT_NAME'?action=asterisk-status">Status (Beta)</a></td>'
  echo '<td align=center><a href="/cp_headers.html">Manchettes</a></td></tr>'

  echo '<tr><td align=center border=0 colspan=6>'
  echo '<br><h3>+---------------------------------------------------------------------------------------------------+</h3></td></tr>'
  echo '</table>'

  echo '<table><tr><td align=left border=0>'
  echo '<pre><strong>'

  if [ "$FORM_action" = "" ]; then
    echo "<h3>Credits / Version</h3>"
    echo $MARS_VERSION
    $asterisk_exec -r -x 'show version' | grep "Asterisk"
    echo "$( uname -srv )"
  elif [ "$FORM_action" = "reload" ]; then
    echo "<h3>Reloading...</h3>"
    $asterisk_exec -r -x 'reload'
  elif [ "$FORM_action" = "modules" ]; then
    echo "<h3>Global/Modules</h3>"
    $asterisk_exec -r -x 'module show' | sort
  elif [ "$FORM_action" = "netstat" ]; then
  echo '<table style="width: 90%; text-align: left;" border="0" cellpadding="2" cellspacing="2" align="center">'
  echo '<tbody><tr><th><b>@TR<<Physical Connections|Ethernet/Wireless Physical Connections>></b></th></tr>'
  echo '<tr><td><pre>'
  cat /proc/net/arp 
  echo '</pre></td></tr><tr><td><br /><br /></td></tr>'
  echo '<tr><th><b>@TR<<Routing Table|Routing Table>></b></th></tr>'
  echo '<tr><td><pre>'
  netstat -rn 
  echo '</pre></td></tr><tr><td><br /><br /></td></tr>'
  echo '<tr><th><b>@TR<<Router Listening Ports|Router Listening Ports>></b></th></tr>'
  echo '<tr><td><pre>'
  netstat -ln 2>&- | awk '$0 ~ /^Active UNIX/ {ignore = 1}; ignore != 1 { print $0 }'
  echo '</pre></td></tr><tr><td><br /><br /></td></tr>'
  echo '<tr><th><b>@TR<<Router Connections|Connections to the Router>></b></th></tr>'
  echo '<tr><td><pre>'
  netstat -n 2>&- | awk '$0 ~ /^Active UNIX/ {ignore = 1}; ignore != 1 { print $0 }'
  echo '</pre></td></tr></tbody></table>'
 
    elif [ "$FORM_action" = "execute" ]; then
    echo "<h3>$FORM_exec_com</h3>"
    $asterisk_exec -r -x "$FORM_exec_com"
  elif [ "$FORM_action" = "dialplan" ]; then
     echo "<h3>Dialplan</h3>"
     asterisk -r -x 'dialplan show'
  elif [ "$FORM_action" = "screen" ]; then
     cat /tmp/astlog.txt
  elif [ "$FORM_action" = "applications" ]; then
     echo "<h3>Applications</h3>"
     asterisk -r -x 'show applications'
  elif [ "$FORM_action" = "functions" ]; then
     echo "<h3>Functions</h3>"
     asterisk -r -x 'show functions' | sort
  elif [ "$FORM_action" = "agi" ]; then
     echo "<h3>AGI Commands</h3>"
     asterisk -r -x 'agi show'
  elif [ "$FORM_action" = "manager_commands" ]; then
     echo "<h3>Manager Commands</h3>"
     asterisk -r -x 'manager show commands' | sort
  elif [ "$FORM_action" = "nvram" ]; then
     echo "<h3>Asterisk NVRAM Variables</h3>"
     nvram show | grep AST* | sort
  elif [ "$FORM_action" = "globals" ]; then
     echo "<h3>Asterisk Globals Variables</h3>"
     asterisk -rx 'core show globals' | sort
#  elif [ "$FORM_action" = "modules" ]; then
#     echo "<h3>Modules</h3>"
#     asterisk -r -x 'module show'
  elif [ "$FORM_action" = "formats" ]; then
     echo "<h3>Formats</h3>"
     asterisk -r -x 'core show file formats'
  elif [ "$FORM_action" = "translation" ]; then
     echo "<h3>Translations</h3>"
     asterisk -r -x 'core show translation'
  elif [ "$FORM_action" = "uptime" ]; then
     echo "<h3>UpTime</h3>"
     asterisk -r -x 'core show uptime'
  elif [ "$FORM_action" = "database" ]; then
     echo "<h3>DataBase</h3>"
     asterisk -r -x 'database show'
  elif [ "$FORM_action" = "stop" ]; then
     echo "<h3>Stop</h3>"
     asterisk -r -x 'stop now'
  elif [ "$FORM_action" = "restart" ]; then
     echo "<h3>Restart</h3>"
     asterisk -r -x 'stop now'
     sleep 3
     /etc/init.d/asterisk start
  elif [ "$FORM_action" = "dyndns_restart" ]; then
     echo "<h3>Restarting DynDNS</h3>"
     /etc/init.d/S52ez-ipupdate restart
  fi
  if [ "$FORM_action" = "cust_com" ]; then
    echo '<form action="'$SCRIPT_NAME'" method=POST>'
    echo '<center>'
    echo '<INPUT type=text name="exec_com" value="'$FORM_exec_com'" size="25" maxlength="150">'
    echo '<br /><br /><INPUT type="submit" value="@TR<<Accept>>">'
    echo '<INPUT name="action" type="hidden" value="execute">'
    echo "<br /><br />Enter 'help' for commands details."
    echo '</center>'
    echo '</form>'
  fi
  if [ "$FORM_action" = "editor" ]; then
    echo '<center><form action="'$SCRIPT_NAME'" method=POST>'
    echo '<table style="width: 25%; text-align: left;" border="0" cellpadding="2" cellspacing="2" align="center">'
    ls $ast_conf_path/.  | awk -F' ' '
    {
      link=$1
      gsub(/\+/,"%2B",link)
      print "<tr><td><a href=\"'$SCRIPT_NAME'?action=edit&target=" link "\">@TR<<Edit>></td><td>" $1 "</td></tr></a>"
    }'
    echo '</table></form></center>'
  fi
  if [ "$FORM_conf" != "" ]; then
    echo "$FORM_conf" | tr -d '\r' > $ast_conf_path/$FORM_target
  fi
  if [ "$FORM_action" = "edit" ]; then
    conf_file="$( cat $ast_conf_path/$FORM_target )"
    echo '<form action="'$SCRIPT_NAME'" method=POST>'
    echo '<center>'
    echo '<TEXTAREA wrap="nowrap" name="conf" rows="40" cols="100">'
    echo -n "$conf_file"
    echo '</TEXTAREA>'
    echo '<INPUT name="target" type="hidden" value="'$FORM_target'">'
    echo '<br /><INPUT type="submit" value="@TR<<Save Changes>>">'
    echo '</center>'
    echo '</form>'
  fi

  if [ "$FORM_action" = "asterisk-status" ]; then
    MIP=$(ifconfig | grep "inet addr:" | grep -v "127" | cut -d: -f 2 | cut -d' ' -f 1)
    MarsAJAM="http://"$MIP":8088/asterisk/static/asterisk-status.html"
    echo '<script language="JavaScript">location.href="'$MarsAJAM'"</script>'
  fi

  echo '</strong></pre>'
  echo '</td></tr></table>'
else
  has_pkgs asterisk
  echo "@TR<<Asterisk is not running.>>"
fi

echo '<pre>'
env | sort
echo '</pre>'
footer ?>
<!--
##WEBIF:name:Asterisk:180:Dev
-->
