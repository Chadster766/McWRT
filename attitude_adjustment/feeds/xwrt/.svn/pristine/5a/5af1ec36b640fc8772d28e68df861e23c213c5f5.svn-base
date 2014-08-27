#!/bin/sh

# Set a variable called stdin to help us
# get the variables from Asterisk
stdin="0"

# Read in the variables from Asterisk
while [ "$stdin" != "" ]
do
read stdin
done

## admin_email=$(nvram get "AST_ADMIN_Email")
## smtp_server=$(nvram get "AST_SMTP_Server")
admin_email=$(uci get mars.system.AST_ADMIN_Email)
smtp_server=$(uci get mars.system.AST_SMTP_Server)

echo 'From: '$admin_email >/tmp/wa_dir.html
echo 'Subject: Web Audio Directory' >>/tmp/wa_dir.html
echo 'MIME-Version: 1.0' >>/tmp/wa_dir.html
echo -e 'Content-Type: text/html\n' >>/tmp/wa_dir.html

echo '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">' >>/tmp/wa_dir.html
echo '<html xmlns="http://www.w3.org/1999/xhtml" lang="en" xml:lang="en">' >>/tmp/wa_dir.html
echo '<xml version="1.0" encoding="ISO-8859-1"' >>/tmp/wa_dir.html

echo '<body>' >>/tmp/wa_dir.html
echo '<b>Web Audio Directory</b><p>' >>/tmp/wa_dir.html
echo '<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\"><tr><td><pre><strong><big>' >>/tmp/wa_dir.html

grep -v "\[URL" /usr/lib/asterisk/agi-bin/web_audio.txt > /tmp/wa_dir.txt
cat /tmp/wa_dir.txt >>/tmp/wa_dir.html

echo '</big></strong></pre></td></tr></table></body></html>' >>/tmp/wa_dir.html
#echo "Mailing Calls Log ..."
/usr/sbin/mini_sendmail -t -s$smtp_server $admin_email </tmp/wa_dir.html

exit 0
