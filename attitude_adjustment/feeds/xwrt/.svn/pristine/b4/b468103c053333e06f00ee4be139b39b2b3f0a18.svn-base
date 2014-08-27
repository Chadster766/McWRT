#!/bin/sh

asterisk_calls="/var/log/asterisk/cdr-csv/Master.csv"
asterisk_cdr='"${CDR(accountcode)","${CDR(src)}","${CDR(dst)}","${CDR(dcontext)}","${CDR(clid)}","${CDR(channel)}","${CDR(dstchannel)}","${CDR(lastapp)}","${CDR(lastdata)}","${CDR(start)}","${CDR(answer)}","${CDR(end)}","${CDR(duration)}","${CDR(billsec)}","${CDR(disposition)}","${CDR(amaflags)}","${CDR(userfield)}"'
show_call_reverse="yes"
admin_email=$(uci get mars.system.AST_ADMIN_Email)
smtp_server=$(uci get mars.system.AST_SMTP_Server)
server_name=$(uci get mars.system.AST_ThisServerName)
webcall_disabled=$(uci get mars.system.AST_WebCallDisabled)

if [ "$admin_email" != "" ]; then
# Build call log HTML file
  echo 'From: '$admin_email >/tmp/callslog.html
  echo 'Subject: Asterisk Call log' >>/tmp/callslog.html
  echo 'MIME-Version: 1.0' >>/tmp/callslog.html
  echo -e 'Content-Type: text/html\n' >>/tmp/callslog.html

  echo '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">' >>/tmp/callslog.html
  echo '<html xmlns="http://www.w3.org/1999/xhtml" lang="en" xml:lang="en">' >>/tmp/callslog.html
  echo '<xml version="1.0" encoding="ISO-8859-1"' >>/tmp/callslog.html
  # Calls Logs
  echo '<body>' >>/tmp/callslog.html
  echo '<b>Calls ('$server_name')</b><p>' >>/tmp/callslog.html
  echo '<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">' >>/tmp/callslog.html
  echo '' | /www/cgi-bin/webif/parse_asterisk_calls.awk "$asterisk_calls" "$asterisk_cdr" "$show_call_reverse" >>/tmp/callslog.html
  echo '</table><br>' >>/tmp/callslog.html

  if [ -e "/var/log/asterisk/webcall" ]; then
    # WebCall log
    echo '<b>WebCall</b><p>' >>/tmp/callslog.html
    echo '<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">' >>/tmp/callslog.html
    echo '<tr><td><pre>' >>/tmp/callslog.html
    cat /var/log/asterisk/webcall >>/tmp/callslog.html
  fi
  echo '</pre></td></tr>' >>/tmp/callslog.html
  echo '</table></body></html>' >>/tmp/callslog.html

#  echo -e '\n+===================================================================================+\n' >> /tmp/callslog.html
#  cat /var/log/asterisk/webcall >> /tmp/callslog.html
  
# Mailing Calls Log ...
  /usr/sbin/mini_sendmail -t -s$smtp_server $admin_email </tmp/callslog.html
  rm /tmp/callslog.html
###  rm $asterisk_calls

  DMOD=$(date -Idate -r /var/log/asterisk/cdr-csv/Master.csv)
  mv $asterisk_calls /var/log/asterisk/cdr-csv/"$DMOD"_Master.csv
else  ### If not sent then rename the master.csv file
  DMOD=$(date -Idate -r /var/log/asterisk/cdr-csv/Master.csv)
  mv $asterisk_calls /var/log/asterisk/cdr-csv/"$DMOD"_Master.csv
fi
