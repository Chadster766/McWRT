#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
. /www/cgi-bin/webif/ast_functions.sh

header "Asterisk" "Management" "<center><img src="/images/mars1.jpg">  @TR<<Asterisk Management>> <img src="/images/mars1.jpg"></center>"

asterisk_calls="/var/log/asterisk/cdr-csv/Master.csv"
asterisk_cdr='"${CDR(accountcode)","${CDR(src)}","${CDR(dst)}","${CDR(dcontext)}","${CDR(clid)}","${CDR(channel)}","${CDR(dstchannel)}","${CDR(lastapp)}","${CDR(lastdata)}","${CDR(start)}","${CDR(answer)}","${CDR(end)}","${CDR(duration)}","${CDR(billsec)}","${CDR(disposition)}","${CDR(amaflags)},"${CDR(userfield)}"'
asterisk_logs="/var/log/asterisk/messages"
show_call_reverse="yes"
ONLY_ONE_VOIP="FALSE"
NO_SIP_TRUNK=TRUE

MARS_VERSION=$(uci get mars.system.Version)
if [ -z "$MARS_VERSION"  ]; then
  MARS_VERSION="MARS V2.21 by TELE DATA Inc. 02/2009"
fi

options=""

#--------------------------------------------------------------------------#
#--------------------------------------------------------------------------#
#
#
#--------------------------------------------------------------------------#

if [ -e /var/run/asterisk.pid ]; then
###  has_pkgs asterisk
  ast_pid=$(cat /var/run/asterisk.pid)
  ast_proc_info="$(cat /proc/$ast_pid/cmdline | sed 's/\0/ /g')"
  asterisk_exec="$(echo $ast_proc_info | awk '{print $1}')"
fi

ast_conf_path=/etc/asterisk

### echo '<img src="/images/mars1.jpg">'
echo '<center>'
echo '<table border="0" cellspacing="8" cellpadding="8"><tr><td align=center border=0 colspan=6>'
echo '<h3>+---------------------------------------------------------------------------------------------------+</h3></td></tr>'
#  echo '<td align=center><a href="'$SCRIPT_NAME'?action=ast_conf">Asterisk Config</a></td>'
echo '<tr><td align=center><a href="'$SCRIPT_NAME'">Version</a></td>'
echo '<td align=center><a href="'$SCRIPT_NAME'?action=sip_peers">SIP Peers</a></td>'
echo '<td align=center><a href="'$SCRIPT_NAME'?action=sip_users">SIP Users</a></td>'
echo '<td align=center><a href="'$SCRIPT_NAME'?action=sip_channels">SIP Channels</a></td>'
echo '<td align=center><a href="'$SCRIPT_NAME'?action=sip_registry">SIP Registry</a></td>'
echo '<td align=center><a href="'$SCRIPT_NAME'?action=sip_reload">SIP Reload</a></td></tr>'

echo '<tr><td align=center><a href="'$SCRIPT_NAME'?action=uptime">UpTime</a></td>'
echo '<td align=center><a href="'$SCRIPT_NAME'?action=iax_peers">IAX Peers</a></td>'
echo '<td align=center><a href="'$SCRIPT_NAME'?action=iax_users">IAX Users</a></td>'
echo '<td align=center><a href="'$SCRIPT_NAME'?action=iax_channels">IAX Channels</a></td>'
echo '<td align=center><a href="'$SCRIPT_NAME'?action=iax_registry">IAX Registry</a></td>'
echo '<td align=center><a href="'$SCRIPT_NAME'?action=iax_reload">IAX Reload</a></td></tr>'

echo '<tr><td align=center><a href="'$SCRIPT_NAME'?action=ast_log">Asterisk Log</a></td>'
echo '<td align=center><a href="'$SCRIPT_NAME'?action=netstat">Net Stats</a></td>'
echo '<td align=center><a href="'$SCRIPT_NAME'?action=system_status">Sys. Status</a></td>'
echo '<td align=center><a href="'$SCRIPT_NAME'?action=webcalllog">WebCall Log</a></td>'
echo '<td align=center><a href="'$SCRIPT_NAME'?action=calls">Show Calls Log</a></td>'
echo '<td align=center><a href="'$SCRIPT_NAME'?action=reload">Reload Dialplan</a></td></tr>'

echo '<tr><td align=center><a href="'$SCRIPT_NAME'?action=vmailusers">VMail Users</a></td>'
echo '<td align=center><a href="'$SCRIPT_NAME'?action=queues">Call Queue</a></td>'
echo '<td align=center></td>'
echo '<td align=center><a href="'$SCRIPT_NAME'?action=editor">Asterisk Conf. Edit</a></td>'
echo '<td align=center><a href="'$SCRIPT_NAME'?action=email_calls">EMail Calls Log</a><br></td>'
echo '<td align=center><a href="'$SCRIPT_NAME'?action=restart">Restart</a></td><tr>'
echo '<tr><td align=center></td>'
#  echo '<td align=center><a href="http://teledata.qc.ca/MARS/UserManual_en.html" target="_blank">User Manual</a></td>'
echo '<td align=center><a href="/UserManual_en.html" target="_blank">User Manual</a></td>'
#  echo '<td align=center><a href="http://teledata.qc.ca/MARS/MngrManual_en.html" target="_blank">Mngr Manual</a></td>'
echo '<td align=center><a href="/MngrManual_en.html" target="_blank">Mngr Manual</a></td>'
#  echo '<td align=center><a href="http://teledata.qc.ca/MARS/PhoneSetup_en.html" target="_blank">GS Phone Setup</a></td>'
echo '<td align=center><a href="/PhoneSetup_en.html" target="_blank">Phones Setup</a></td>'
echo '<td align=center></td>'
echo '<td align=center><a href="'$SCRIPT_NAME'?action=update_manuals">Update Manuals</a></td><tr>'
echo '<tr><td align=center border=0 colspan=6>'
echo '<h3>+---------------------------------------------------------------------------------------------------+</h3></td></tr>'

###   echo '<tr><td align=center></td>'
### #  echo '<td align=center><a href="/mysql-essential-5.0.45-win32.msi" target="_blank">Install MySQL 5.0</a></td>'
###   echo '<td align=center></td>'
###   echo '<td align=center></td>'
###   echo '<td align=center><a href="/MARS_Commander.zip" target="_blank">Install MARS Commander</a></td>'
###   echo '<td align=center></td>'
###   echo '<td align=center></td></tr>'

echo '</table>'
echo '<table><tr><td align=left border=0>'
echo '<pre><strong>'

if [ "$FORM_action" = "" ]; then
  echo "<h3>Credits / Version</h3>"
  echo $MARS_VERSION
  if [ "$asterisk_exec" != "" ]; then
    $asterisk_exec -r -x 'show version' | grep "Asterisk"
    echo -e "$( uname -srv )\n"
    cat /proc/cpuinfo | head -n 4
  else
    echo "@TR<<Asterisk is not running.>>"
  fi
elif [ "$FORM_action" = "reload" ]; then
  echo "<h3>Reloading...</h3>"
  $asterisk_exec -r -x 'reload'
elif [ "$FORM_action" = "sip_peers" ]; then
  echo "<h3>SIP/Peers</h3>"
  $asterisk_exec -r -x 'sip show peers'
elif [ "$FORM_action" = "sip_users" ]; then
  echo "<h3>SIP/Users</h3>"
  $asterisk_exec -r -x 'sip show users'
elif [ "$FORM_action" = "sip_channels" ]; then
  echo "<h3>SIP/Channels</h3>"
  $asterisk_exec -r -x 'sip show channels'
elif [ "$FORM_action" = "sip_registry" ]; then
  echo "<h3>SIP/Registry</h3>"
  $asterisk_exec -r -x 'sip show registry'
elif [ "$FORM_action" = "sip_reload" ]; then
  echo "<h3>SIP Reload</h3>"
  $asterisk_exec -r -x 'sip reload'
elif [ "$FORM_action" = "iax_peers" ]; then
  echo "<h3>IAX/Peers</h3>"
  $asterisk_exec -r -x 'iax2 show peers'
elif [ "$FORM_action" = "iax_users" ]; then
  echo "<h3>IAX/Users</h3>"
  $asterisk_exec -r -x 'iax2 show users'
elif [ "$FORM_action" = "iax_channels" ]; then
  echo "<h3>IAX/Channels</h3>"
  $asterisk_exec -r -x 'iax2 show channels'
elif [ "$FORM_action" = "iax_registry" ]; then
  echo "<h3>IAX/Registration</h3>"
  $asterisk_exec -r -x 'iax2 show registry'
elif [ "$FORM_action" = "iax_reload" ]; then
  echo "<h3>IAX2 Reload</h3>"
  $asterisk_exec -r -x 'iax2 reload'
elif [ "$FORM_action" = "system_status" ]; then
#    echo "<h3>System Status</h3>"
  echo -e "\n<pre><h3><b>free</h3></b>"
  free
  echo -e "\n<h3><b>df</h3></b>"
  df
  echo -e "\n<h3><b>mount</h3></b>"
  mount
  echo -e "\n<h3><b>ps</h3></b>"
  ps
  echo -e "\n<h3><b>ifconfig</h3></b>"
  ifconfig
  echo ''
elif [ "$FORM_action" = "ast_log" ]; then
  echo '<h3>Logs</h3>'
  echo -n '<pre>'
  cat "$asterisk_logs"
  echo '</pre>'
elif [ "$FORM_action" = "modules" ]; then
  echo "<h3>Global/Modules</h3>"
  $asterisk_exec -r -x 'show modules' | sort
elif [ "$FORM_action" = "netstat" ]; then
#    echo "<h3>Net Stats</h3>"
#    echo "<pre>"
#    netstat -nat 2>&- | awk '$0 ~ /^Active UNIX/ {ignore = 1}; ignore != 1 { print $0 }'
#    echo "</pre>"

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
   asterisk -r -x 'show dialplan'
elif [ "$FORM_action" = "applications" ]; then
   echo "<h3>Applications</h3>"
   asterisk -r -x 'show applications'
elif [ "$FORM_action" = "functions" ]; then
   echo "<h3>Functions</h3>"
   asterisk -r -x 'show functions' | sort
elif [ "$FORM_action" = "agi" ]; then
   echo "<h3>AGI Commands</h3>"
   asterisk -r -x 'show agi'
elif [ "$FORM_action" = "manager_commands" ]; then
   echo "<h3>Manager Commands</h3>"
   asterisk -r -x 'show manager commands' | sort
elif [ "$FORM_action" = "nvram" ]; then
   echo "<h3>Asterisk NVRAM Variables</h3>"
   nvram show | grep AST* | sort
elif [ "$FORM_action" = "modules" ]; then
   echo "<h3>Modules</h3>"
   asterisk -r -x 'show modules'
elif [ "$FORM_action" = "formats" ]; then
   echo "<h3>Formats</h3>"
   asterisk -r -x 'show file formats'
elif [ "$FORM_action" = "translation" ]; then
   echo "<h3>Translations</h3>"
   asterisk -rx 'show translation recalc 10'
elif [ "$FORM_action" = "uptime" ]; then
   echo "<h3>UpTime</h3>"
   asterisk -r -x 'core show uptime'
elif [ "$FORM_action" = "vmailusers" ]; then
   echo "<h3>VMail Users</h3>"
   asterisk -r -x 'voicemail show users'
elif [ "$FORM_action" = "queues" ]; then
   echo "<h3>Call Queue</h3>"
   asterisk -r -x 'show queues'
elif [ "$FORM_action" = "webcalllog" ]; then
   echo "<h3>WebCall Log</h3>"
   if [ -f "/var/log/asterisk/webcall" ]; then
     echo "WebCall log empty ..."
   else
     cat /var/log/asterisk/webcall
   fi
elif [ "$FORM_action" = "calls" ]; then
   echo '' | ./parse_asterisk_calls.awk "$asterisk_calls" "$asterisk_cdr" "$show_call_reverse"
elif [ "$FORM_action" = "database" ]; then
   echo "<h3>DataBase</h3>"
   asterisk -r -x 'database show'
elif [ "$FORM_action" = "stop" ]; then
   echo "<h3>Stop</h3>"
   asterisk -r -x 'stop now'
elif [ "$FORM_action" = "restart" ]; then
   echo "<h3>Restart</h3>"
#     asterisk -r -x 'stop now'
   echo "Stoping Asterisk"
   /etc/init.d/asterisk stop
   sleep 3
   echo "Restarting Asterisk"
   /etc/init.d/asterisk start
elif [ "$FORM_action" = "update_manuals" ]; then
  echo "<h3>Updating Manuals</h3>"
  wget -O /tmp/UserManual_en.html "http://teledata.qc.ca/MARS/UserManual_en.html"
  if [ -f /tmp/UserManual_en.html ]; then
     rm /mnt/disc0_1/manuals/UserManual_en.html
     mv /tmp/UserManual_en.html /mnt/disc0_1/manuals/
     echo '<strong>Updated User Manual<br></strong>'
  fi
  wget -O /tmp/MngrManual_en.html "http://teledata.qc.ca/MARS/MngrManual_en.html"
  if [ -f /tmp/MngrManual_en.html ]; then
     rm /mnt/disc0_1/manuals/MngrManual_en.html
     mv /tmp/MngrManual_en.html /mnt/disc0_1/manuals/
     echo '<strong>Updated Mngr Manual<br></strong>'
  fi
  wget -O /tmp/PhoneSetup_en.html "http://teledata.qc.ca/MARS/PhoneSetup_en.html"
  if [ -f /tmp/PhoneSetup_en.html ]; then
     rm /mnt/disc0_1/manuals/PhoneSetup_en.html
     mv /tmp/PhoneSetup_en.html /mnt/disc0_1/manuals/
     echo '<strong>Updated Phone Setup Manual<br></strong>'
  fi

elif [ "$FORM_action" = "email_calls" ]; then
  admin_email=$(uci get mars.system.AST_ADMIN_Email)
  smtp_server=$(uci get mars.system.AST_SMTP_Server)

  if [ "$admin_email" = "" ]; then
      echo "<h3>Please set Admin Email and SMTP Server first ...</h3>"
  else
    echo "Calls Log. Please wait ..."
    echo 'From: '$admin_email >/tmp/callslog.html
    echo 'Subject: Asterisk Call log' >>/tmp/callslog.html
    echo 'MIME-Version: 1.0' >>/tmp/callslog.html
    echo -e 'Content-Type: text/html\n' >>/tmp/callslog.html

    echo '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">' >>/tmp/callslog.html
    echo '<html xmlns="http://www.w3.org/1999/xhtml" lang="en" xml:lang="en">' >>/tmp/callslog.html
    echo '<xml version="1.0" encoding="ISO-8859-1"' >>/tmp/callslog.html

    echo '<body>' >>/tmp/callslog.html
    echo '<b>Calls</b><p>' >>/tmp/callslog.html
    echo '<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">' >>/tmp/callslog.html

    echo '' | ./parse_asterisk_calls.awk "$asterisk_calls" "$asterisk_cdr" "$show_call_reverse" >>/tmp/callslog.html
    echo '</table></body></html>' >>/tmp/callslog.html
    echo "Mailing Calls Log ..."
    /usr/sbin/mini_sendmail -t -s$smtp_server $admin_email </tmp/callslog.html
    echo "Call Log mailed to: "$admin_email
    rm /tmp/callslog.html
    #     rm asterisk_calls
  fi
fi
echo '</strong></pre>'
echo '</td></tr></table>'
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
### fi

footer ?>
<!--
##WEBIF:name:Asterisk:100:Management
-->

<!--
# List all local prefixes
# wget -O- "http://www.localcallingguide.com/xmllocalprefix.php?npa=514&nxx=723" | grep "<n..>" | sed -e 's/<npa>//; s/<\/npa>//; s/<nxx>//; s/<\/nxx>//'
# Test if Local / Long dist
# wget -O- "http://www.localcallingguide.com/xmlrcdist.php?npa1=416&nxx1=423&npa2=212&nxx2=733" | grep "<islocal>" | sed -e 's/<islocal>//; s/<\/islocal>//'
-->
