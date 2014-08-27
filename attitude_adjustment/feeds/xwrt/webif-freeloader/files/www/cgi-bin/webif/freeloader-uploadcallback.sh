#!/usr/bin/haserl -u
Content-Type: text/html; charset=UTF-8
Pragma: no-cache

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="en" xml:lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<meta http-equiv="refresh" content="0;url=freeloader-upload.sh" />
<title></title>
<script type="text/javascript">
<!--
window.location="freeloader-upload.sh"
// -->
</script>
</head>
<body>
<?
###################################################################
# freeloader-uploadcallback.sh
# (c)2007 X-Wrt project (http://www.x-wrt.org)
# (c)2007-03-02 m4rc0
#
#	version 1.7
#
# Description:
#	When the file is uploaded this page makes sure the file placed in the right directory.
#
# Author(s) [in order of work date]:
#	m4rc0 <janssenmaj@gmail.com>
#
# Major revisions:
#		1.5 Added Username/password - m4rc0 25-3-2007
#		1.7 Added multiple url list - m4rc0 29-3-2007
#
#
# NVRAM variables referenced:
#   none
#
# Configuration files referenced:
#   /etc/config/freeloader
#
#

#Include functions
. /usr/lib/webif/webif.sh
#Include settings
. /usr/lib/webif/freeloader-include.sh

if [ -n "$FORM_uploadfile" ]; then
	#Get only the filename from the path
	#This fix is for IE-browsers,which will send the complete path. firefox will only send the filename.
	FORM_uploadfile_name=`echo $FORM_uploadfile_name|awk '{n=split($0,fn,"\\\"); print fn[n]}'`

	if [ "$FORM_queue" = "normal" ]; then
		mv $FORM_uploadfile "$QUEUE_NORMAL/$FORM_uploadfile_name"
	else
		mv  $FORM_uploadfile "$QUEUE_PRIO/$FORM_uploadfile_name"
	fi
	
	logstatus "$FORM_uploadfile_name has been uploaded to the $FORM_queue queue."
fi

if [ -n "$FORM_uploadURL" ]; then
	URL_FILENAME=`echo $FORM_uploadURL | awk '{n=split($0,fn,"/"); print fn[n]}'`
	if [ "$FORM_queue" = "normal" ]; then
		if [ -n "$FORM_username" ] && [ -n "$FORM_password" ]; then
			echo "username=$FORM_username" > "$QUEUE_NORMAL/$URL_FILENAME.link.fci"
			echo "password=$FORM_password" >> "$QUEUE_NORMAL/$URL_FILENAME.link.fci"
		fi
		echo "$FORM_uploadURL" > "$QUEUE_NORMAL/$URL_FILENAME.link"
	else
		if [ -n "$FORM_username" ] && [ -n "$FORM_password" ]; then
			echo "username=$FORM_username" > "$QUEUE_PRIO/$URL_FILENAME.link.fci"
			echo "password=$FORM_password" >> "$QUEUE_PRIO/$URL_FILENAME.link.fci"
		fi
		echo "$FORM_uploadURL" > "$QUEUE_PRIO/$URL_FILENAME.link"
	fi
	
	logstatus "$URL_FILENAME has been uploaded to the $FORM_queue queue."
fi

if [ -n "$FORM_uploadURLlist" ]; then
	
	#generate a random number to make the filename unique when a user uploads a multiple url list
	RANDOMNUMBER=`dd if=/dev/urandom bs=2 count=1 2>/dev/null | hexdump -d | awk 'int($1) == 0 { print (($2 % 254) + 1) }'`
	URL_FILENAME="freeloader_url_list${RANDOMNUMBER}"

	if [ "$FORM_queue" = "normal" ]; then
		if [ -n "$FORM_username" ] && [ -n "$FORM_password" ]; then		
			echo "username=$FORM_username" > "$QUEUE_NORMAL/$URL_FILENAME.link.fci"
			echo "password=$FORM_password" >> "$QUEUE_NORMAL/$URL_FILENAME.link.fci"
		fi
		echo "$FORM_uploadURLlist" > "$QUEUE_NORMAL/$URL_FILENAME.link"
	else
		if [ -n "$FORM_username" ] && [ -n "$FORM_password" ]; then		
			echo "username=$FORM_username" > "$QUEUE_PRIO/$URL_FILENAME.link.fci"
			echo "password=$FORM_password" >> "$QUEUE_PRIO/$URL_FILENAME.link.fci"
		fi
		echo "$FORM_uploadURList" > "$QUEUE_PRIO/$URL_FILENAME.link"
	fi

	logstatus "$URL_FILENAME has been uploaded to the $FORM_queue queue."
fi
?>
</body>
</html>
