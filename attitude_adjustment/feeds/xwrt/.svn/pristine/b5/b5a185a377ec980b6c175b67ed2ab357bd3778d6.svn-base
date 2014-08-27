#!/usr/bin/webif-page "-U /tmp -u 4096"
<?
. "/usr/lib/webif/webif.sh"
. /www/cgi-bin/webif/ast_functions.sh

# header_inject_head=$(cat <<EOF
# meta http-equiv=\"refresh\" content=\"5;url=$SCRIPT_NAME?interval=5\" />"
# EOF
# 
# )

# mini_header "Asterisk" "Dialer" "@TR<<MARS Dialer Traces>><meta http-equiv=\"refresh\" content=\"4;url=$SCRIPT_NAME\">"

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


echo '<center><table style="text-align: left; width: 80%;" border="0" cellpadding="0" cellspacing="0">'
echo '<tr><th colspan=1>FTP Test</th><td></td></tr>'
echo '<tr><th colspan=1>Sending file ftp_test.txt to: '$FORM_Name':'$FORM_Pwd'@'$FORM_Host' in / directory</th><td></td></tr>'

echo '<td><textarea readonly="readonly" cols="80" rows="24" name="DialListTraces">'

# echo "Sending file ftp_test.txt to: "$FORM_Name":"$FORM_Pwd"@"$FORM_Host" in / directory"
echo -e "+------------------------------------------------------------------------------+\n"
cat /proc/cpuinfo > /tmp/ftp_test.txt
echo -e "+------------------------------------------------------------------------------+\n" >> /tmp/ftp_test.txt
free >> /tmp/ftp_test.txt
echo -e "+------------------------------------------------------------------------------+\n" >> /tmp/ftp_test.txt
df >> /tmp/ftp_test.txt
echo -e "+------------------------------------------------------------------------------+\n" >> /tmp/ftp_test.txt
mount >> /tmp/ftp_test.txt
echo -e "+------------------------------------------------------------------------------+\n" >> /tmp/ftp_test.txt
netstat -ln 2>&- | awk '$0 ~ /^Active UNIX/ {ignore = 1}; ignore != 1 { print $0 }' >> /tmp/ftp_test.txt
echo -e "+------------------------------------------------------------------------------+\n" >> /tmp/ftp_test.txt

wput -d /tmp/ftp_test.txt ftp://"$FORM_Name":"$FORM_Pwd"@"$FORM_Host"/ftp_test.txt

echo '</textarea></td><td></td></tr>'
echo '<tr><td align=center colspan=3><input class="flatbtn" type="submit" name="submit_action" value="@TR<<Close>>" onClick="window.close();" />'
echo '</center></table></body></html>'
?>

