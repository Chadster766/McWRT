#!/bin/sh

admin_email=$(uci get mars.system.AST_ADMIN_Email)
smtp_server=$(uci get mars.system.AST_SMTP_Server)
server_name=$(uci get mars.system.AST_ThisServerName)

if [ "$admin_email" != "" ]; then
# Build call log HTML file
  echo 'From: '$admin_email >/tmp/wrtlog.html
  echo 'Subject: Wrt log' >>/tmp/wrtlog.html
  echo 'MIME-Version: 1.0' >>/tmp/wrtlog.html
  echo -e 'Content-Type: text/html\n' >>/tmp/wrtlog.html

  echo '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">' >>/tmp/wrtlog.html
  echo '<html xmlns="http://www.w3.org/1999/xhtml" lang="en" xml:lang="en">' >>/tmp/wrtlog.html
  echo '<xml version="1.0" encoding="ISO-8859-1"' >>/tmp/wrtlog.html

  echo '<body>' >>/tmp/wrtlog.html
  echo '<b>Wrt Logs ('$server_name')</b><p>' >>/tmp/wrtlog.html
  echo '<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">' >>/tmp/wrtlog.html
  echo '<tr><td><pre>'  >>/tmp/wrtlog.html
  logread >>/tmp/wrtlog.html
  echo '</pre></tr></td><br>'  >>/tmp/wrtlog.html
  echo -e "\n<tr><td><h3><b>free</b></h3></td></tr><tr><td><pre>"  >>/tmp/wrtlog.html
  free  >>/tmp/wrtlog.html 
  echo -e "\n</pre><tr><td><h3><b>df</h3></b></td></tr><tr><td><pre>"  >>/tmp/wrtlog.html
  df  >>/tmp/wrtlog.html
  echo -e "\n</pre><tr><td><h3><b>mount</h3></b></td></tr><tr><td><pre>"  >>/tmp/wrtlog.html
  mount  >>/tmp/wrtlog.html
  echo -e "\n</pre><tr><td><h3><b>ps</h3></b></td></tr><tr><td><br><pre>"  >>/tmp/wrtlog.html
  ps  >>/tmp/wrtlog.html
  echo '</pre></td></tr>'  >>/tmp/wrtlog.html
  echo '<tr><td><br><b>Ethernet/Wireless Physical Connections</b></td></tr>'  >>/tmp/wrtlog.html
  echo '<tr><td><pre>'  >>/tmp/wrtlog.html
  cat /proc/net/arp  >>/tmp/wrtlog.html
  echo '</pre></td></tr><tr><td><br /><br /></td></tr>'  >>/tmp/wrtlog.html
  echo '<tr><td><b>Routing Table</b></td></tr>'  >>/tmp/wrtlog.html
  echo '<tr><td><pre>'  >>/tmp/wrtlog.html
  netstat -rn  >>/tmp/wrtlog.html
  echo '</pre></td></tr><tr><td><br /><br /></td></tr>'  >>/tmp/wrtlog.html
  echo '<tr><td><b>Router Listening Ports</b></td></tr>'  >>/tmp/wrtlog.html
  echo '<tr><td><pre>'  >>/tmp/wrtlog.html
  netstat -ln 2>&- | awk '$0 ~ /^Active UNIX/ {ignore = 1}; ignore != 1 { print $0 }'  >>/tmp/wrtlog.html
  echo '</pre></td></tr><tr><td><br /><br /></td></tr>'  >>/tmp/wrtlog.html
  echo '<tr><td><b>Connections to the Router</b></td></tr>'  >>/tmp/wrtlog.html
  echo '<tr><td><pre>'  >>/tmp/wrtlog.html
  netstat -n 2>&- | awk '$0 ~ /^Active UNIX/ {ignore = 1}; ignore != 1 { print $0 }'  >>/tmp/wrtlog.html
  echo '</pre></td></tr></tbody></table></body></html>'  >>/tmp/wrtlog.html

#  echo -e '\n+===================================================================================+\n' >> /tmp/callslog.html
#  cat /var/log/asterisk/webcall >> /tmp/wrtlog.html
  
# Mailing Calls Log ...
if [ "$1" = "-v" ]; then
  /usr/sbin/mini_sendmail -t -v -s$smtp_server $admin_email </tmp/wrtlog.html
else
  /usr/sbin/mini_sendmail -t -s$smtp_server $admin_email </tmp/wrtlog.html
fi
#  rm /tmp/wrtlog.html
fi
