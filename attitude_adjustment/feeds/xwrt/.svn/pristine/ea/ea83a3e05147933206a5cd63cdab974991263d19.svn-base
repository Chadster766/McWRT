#!/usr/bin/webif-page
<?
. "/usr/lib/webif/webif.sh"
. /www/cgi-bin/webif/ast_functions.sh


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
  echo '<table border=\"1" cellpadding=\"0\" cellspacing=\"0\">' >>/tmp/wrtlog.html
# echo '<tr><td><pre>'  >>/tmp/wrtlog.html
# logread >>/tmp/wrtlog.html
# echo '</pre></tr></td><br>'  >>/tmp/wrtlog.html
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


cat <<EOF
Content-Type: text/html
Pragma: no-cache

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="en" xml:lang="en">
<xml version="1.0" encoding="@TR<<Encoding|ISO-8859-1>>>
<head>
	<link rel="stylesheet" type="text/css" href="/themes/active/webif.css">        
	<title></title>
	<style type="text/css">
		html, body { background-color: transparent; }
	</style>
</head>
EOF


echo '<body><center><table style="text-align: left; width: 80%;" border="0" cellpadding="0" cellspacing="0">'
echo '<tr><th colspan=1>EMail Test</th><td></td></tr>'
echo '<tr><th colspan=1>Sending file wrt_log.html to: '$FORM_AdminEmail' using SMTP Server: '$FORM_SMTP_Server'</th><td></td></tr>'
echo '<td><textarea readonly="readonly" cols="80" rows="24" name="DialListTraces">'
echo -e "+------------------------------------------------------------------------------+"

/usr/sbin/mini_sendmail -t -v -s$FORM_SMTP_Server $FORM_AdminEmail </tmp/wrtlog.html # > /tmp/ftp_test.txt

echo -e "+------------------------------------------------------------------------------+"

echo '</textarea></td><td></td></tr>'
echo '<tr><td align=center colspan=3><input class="flatbtn" type="submit" name="submit_action" value="@TR<<Close>>" onClick="window.close();" />'
echo '</center></table></body></html>'
?>

