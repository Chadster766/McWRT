#!/bin/sh

##### TODO Change /tmp/vemail.msg for unique name.

result=0
FullFname=$1
Fname=$(basename $FullFname)

#TMP Traces
# echo 'Fname='$1'<br>'
# echo 'To='$2'<br>'
# echo 'CallerId(num)='$3'<br>'
# echo 'CallerId(name)='$4'<br>'
# echo 'CallerName='$5'<br>'
# echo 'LogFile='$6'<br>'

# echo 'FullFname='$FullFname'<br>'
# echo 'Fname='$Fname'<br>'

smtp_server=$(uci get mars.system.AST_SMTP_Server)
from_email=$(uci get mars.system.AST_ADMIN_Email)
server_name=$(uci get mars.system.AST_ThisServerName)
sender_name=$(uci get mars.pid.AST_PID_SenderName)

#TMP Traces
# echo 'Building msg body ...<br>'

echo 'From: '$from_email > /tmp/$Fname.msg
echo 'Subject: Phone-In Dictation Voice EMail' >> /tmp/$Fname.msg
echo 'MIME-Version: 1.0' >> /tmp/$Fname.msg
echo 'Content-Type: multipart/mixed; boundary="voicemail_020040301339450788"' >> /tmp/$Fname.msg
echo '' >> /tmp/$Fname.msg

echo '--voicemail_020040301339450788' >> /tmp/$Fname.msg
echo 'Content-Type: text/plain; charset=ISO-8859-1' >> /tmp/$Fname.msg
echo 'Content-Transfer-Encoding: 8bit' >> /tmp/$Fname.msg
echo '' >> /tmp/$Fname.msg

echo -e 'Phone-In Dictation Voice EMail\n' >> /tmp/$Fname.msg
echo 'New Recording received.' >> /tmp/$Fname.msg

echo 'On: '$(date -r $FullFname) >> /tmp/$Fname.msg
echo 'From: '$5  >> /tmp/$Fname.msg
echo -e 'CallerID: '$4' '$3'\n'  >> /tmp/$Fname.msg

echo 'Thank you for using our service !' >> /tmp/$Fname.msg
echo '' >> /tmp/$Fname.msg
if [ "$sender_name" = "" ]; then
  echo '	--TD Phone-in Dictation System' >> /tmp/$Fname.msg
else
  echo '	'$sender_name >> /tmp/$Fname.msg
fi
echo '' >> /tmp/$Fname.msg

echo '--voicemail_020040301339450788' >> /tmp/$Fname.msg
echo 'Content-Type: audio/x-wav; name='$Fname >> /tmp/$Fname.msg
echo 'Content-Transfer-Encoding: base64' >> /tmp/$Fname.msg
echo 'Content-Description: Voicemail sound attachment.' >> /tmp/$Fname.msg
echo 'Content-Disposition: attachment; filename='$Fname >> /tmp/$Fname.msg
echo '' >> /tmp/$Fname.msg
echo '' >> /tmp/$Fname.msg

#TMP Traces
# echo 'Encoding voice file to base64 ...<br>'

openssl enc -e -a -in $1 >> /tmp/$Fname.msg

echo '' >> /tmp/$Fname.msg
echo '--voicemail_020040301339450788--' >> /tmp/$Fname.msg

# Mailing Calls Log ...
#TMP Traces
# echo 'Sending $Fname ...<br>'

/usr/sbin/mini_sendmail -t -s$smtp_server $2 < /tmp/$Fname.msg
#Save res of mini_sendmail
result=$?

# Delete tmp file
rm /tmp/$Fname.msg

#TMP Traces
# echo 'Send result: '$result'<br>'

if [ "$6" != "" ]; then
  if [ "$result" != 0 ]; then
    echo -e 'RESULT: '$send_res 'from SendMail\n' >> $6
  fi
  echo -e $(date)'\n' >> $6
  echo -e 'Sent: '$1 >> $6 
  echo -e 'Dated: '$(date -r $FullFname) >> $6 
  echo -e 'To: '$2'\n' >> $6
  echo -e 'From: '$5 >> $6
  echo -e 'CallerID: '$4'  '$3'\n' >> $6
  echo -e '+-------------------------------------------------------------------------------+\n' >> $6
fi

#TMP Traces
# echo -e 'Done ...\n'

exit $result