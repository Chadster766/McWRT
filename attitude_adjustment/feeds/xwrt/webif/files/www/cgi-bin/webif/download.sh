#!/usr/bin/haserl
<?
. /usr/lib/webif/webif.sh
###################################################################
# download.sh
#
# Description:
#  The primary goal is to be able to download files off the
#  httpd root. The second goal is to limit the time of the 
#  existence of the temporary file and remove it when the
#  timeout expires.
#  The page accepts both POST and GET requests.
#
# Author(s) [in order of work date]:
#  Lubos Stanek <lubek@users.berlios.de>
#
# Major revisions:
#
# Parameters: both POST and GET form
#  timetodelete - time in seconds, the file will be deleted after the timeout
#  script - the url, the page will redirect to in case of error
#  path - file path (no end /)
#  savefile - local file name
#  realname - target file name
#
# Example:
#  http://192.168.1.1/cgi-bin/webif/download.sh?script=/cgi-bin/webif/system-editor.sh&timetodelete=60&path=/tmp&savefile=config.tgz-Fxdc4H&realname=config.tgz
#  script=/cgi-bin/webif/system-editor.sh
#  timetodelete=60
#  path=/tmp
#  savefile=config.tgz-Fxdc4H
#  realname=config.tgz

script="${FORM_script:-/cgi-bin/webif/info.sh}"
redir="${script}?path=$FORM_path"

! empty "$FORM_path" && ! empty "$FORM_savefile" && {
	[ -e "$FORM_path/$FORM_savefile" ] && {
		fsize=$(ls -al "$FORM_path/$FORM_savefile" 2>/dev/null | awk '{ print $5 }')
	}
	[ "$fsize" -gt 0 ] >/dev/null 2>&1 && {
		lockpath="/tmp/$FORM_path"
		lockfile="$lockpath/$FORM_savefile"
		[ -e "$lockfile" ] && {
			locked="0"
		} || {
			mkdir -p "$lockpath" 2>/dev/null
			trap "rm -f $lockfile 2>/dev/null; rmdir -p $lockpath 2>/dev/null; exit" INT TERM EXIT
			touch "$lockfile" 2>/dev/null
			locked="1"
		}
		[ "$fsize" -le $((1048576*6)) ] && {
			md5field=$(md5sum "$FORM_path/$FORM_savefile" 2>/dev/null | awk '{ print $1 }')
		}
		echo "Content-Type: application/octet-stream"
		echo "Content-Disposition: inline;filename=\"${FORM_realname:-$FORM_savefile}\""
		echo "Content-Description: \"${FORM_realname:-$FORM_savefile}\""
		echo "Content-Length: $fsize"
		echo "Pragma: no-cache"
		! empty "$md5field" && echo "Content-MD5: $md5field"
		echo ""
		[ "$FORM_timetodelete" -gt 0 ] >/dev/null 2>&1 && {
			dd if="$FORM_path/$FORM_savefile" bs=$((2**15)) 2>/dev/null
			sleep "$FORM_timetodelete" 2>/dev/null
			equal "$locked" "1" && {
				rm -f "$FORM_path/$FORM_savefile" 2>/dev/null
				rm -f "$lockfile" 2>/dev/null
				rmdir -p "$lockpath" 2>/dev/null
			}
		} || {
			dd if="$FORM_path/$FORM_savefile" bs=$((2**15)) 2>/dev/null
			sleep 1
			equal "$locked" "1" && {
				rm -f "$lockfile" 2>/dev/null
				rmdir -p "$lockpath" 2>/dev/null
			}
		}
		equal "$locked" "1" && {
			set - INT TERM EXIT
		} || exit
	}
}
echo "Content-Type: text/html; charset=UTF-8"
echo "Content-Disposition: inline"
echo "Pragma: no-cache"
echo ""
echo "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
echo "<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"en\" xml:lang=\"en\">"
echo "<head>"
echo "	<meta http-equiv=\"refresh\" content=\"0;url=$redir\" />"
echo "	<title>Webif&sup2; Administration Console</title>"
echo "	<style type=\"text/css\">"
echo "		*{color:Red;background:White;font-family:Verdana,Helvetica,sans-serif}"
echo "		p{position:absolute;width:99%;top:50%;margin-top:-3em;line-height:3em;text-align:center}"
echo "	</style>"
echo "</head>"
echo "<body>"
echo "<p><b>Error</b> downloading a file<br />"
echo "Redirecting to <a href=\"$redir\">previous page</a></p>"
echo "</body>"
echo "</html>"
?>