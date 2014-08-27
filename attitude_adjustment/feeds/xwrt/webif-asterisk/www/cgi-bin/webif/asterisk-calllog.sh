#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
. /www/cgi-bin/webif/ast_functions.sh

header "Asterisk" "Calls Log" "<center><img src="/images/mars1.jpg"> @TR<<Calls Log>> <img src="/images/mars1.jpg"></center>" '' "$SCRIPT_NAME"

asterisk_calls="/var/log/asterisk/cdr-csv/Master.csv"
asterisk_cdr='"${CDR(accountcode)","${CDR(src)}","${CDR(dst)}","${CDR(dcontext)}","${CDR(clid)}","${CDR(channel)}","${CDR(dstchannel)}","${CDR(lastapp)}","${CDR(lastdata)}","${CDR(start)}","${CDR(answer)}","${CDR(end)}","${CDR(duration)}","${CDR(billsec)}","${CDR(disposition)}","${CDR(amaflags)},"${CDR(userfield)}"'
asterisk_logs="/var/log/asterisk/messages"
show_call_reverse="yes"

#--------------------------------------------------------------------------#

if [ -e /var/run/asterisk.pid ]; then
  ast_pid=$(cat /var/run/asterisk.pid)
  ast_proc_info="$(cat /proc/$ast_pid/cmdline | sed 's/\0/ /g')"
  asterisk_exec="$(echo $ast_proc_info | awk '{print $1}')"
else
  echo "@TR<<Asterisk is not running.>>"
fi

#--------------------------------------------------------------------------#

# Display Form 

echo '<center><table><tr><td align=left border=0>'
echo '<pre><strong>'
if [ -e "$asterisk_calls" ]; then
  echo '' | ./parse_asterisk_calls.awk "$asterisk_calls" "$asterisk_cdr" "$show_call_reverse"
else
 echo 'No calls in Master.csv'
fi
echo '</strong></pre>'
echo '</td></tr></table></center>'


if [ "$DEBUG" = "1" ]; then
echo '<pre>'
env | sort
echo '</pre>'
fi

footer ?>

<!--
##WEBIF:name:Asterisk:175:Calls Log
-->

