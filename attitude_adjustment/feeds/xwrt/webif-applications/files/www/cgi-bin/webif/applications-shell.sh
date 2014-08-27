#!/bin/sh
#########################################
# Applications Common Functions
#
# Author(s) [in order of work date]:
#        Dmytro Dykhman <dmytro@iroot.ca>
#

tipcount=1
location=""

HEADER="HTTP/1.0 200 OK
Content-type: text/html; charset=UTF-8

"

HTMLHEAD="<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">

<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"en\" xml:lang=\"en\"><head><title></title>
<link rel='stylesheet' type='text/css' href='/themes/active/webif.css' />
<script type='text/javascript' src='/js/balloontip.js'></script>
<script type="text/javascript" src="/js/window.js"></script>
<script type='text/javascript' src='/js/imgdepth.js'></script>"

TIP(){ 
if [ $1 != 0 ]; then
style="style='width: $1px;'"
fi
echo "<div id=\"b$tipcount\" class=\"balloonstyle\" $style>$2</div>"
let "tipcount+=1" 
}

Load_remote_libs(){
	uci_load "app.ipkg"
	loc="$CONFIG_int_location"

	echo "#!/bin/sh" > /tmp/.D43S.tmp
	echo "START=98" > /tmp/.D43S.tmp
	echo "ls -l $loc/usr/lib/ | awk '{print \" if [ ! -f /usr/lib/\"\$9\" ] \\n then \\n ln -s $loc/usr/lib/\"\$9\" /usr/lib/\"\$9\" \\n fi\"}' > /tmp/.lib1.tmp" >> /tmp/.D43S.tmp
	echo "ls -l $loc/opt/lib/ | awk '{print \" if [ ! -f /usr/lib/\"\$9\" ] \\n then \\n ln -s $loc/opt/lib/\"\$9\" /usr/lib/\"\$9\" \\n fi\"}' > /tmp/.lib2.tmp" >> /tmp/.D43S.tmp
	echo "ls -l $loc/lib/ | awk '{print \" if [ ! -f /usr/lib/\"\$9\" ] \\n then \\n ln -s $loc/lib/\"\$9\" /usr/lib/\"\$9\" \\n fi\"}' > /tmp/.lib3.tmp" >> /tmp/.D43S.tmp
	echo "sh /tmp/.lib1.tmp ; rm /tmp/.lib1.tmp" >> /tmp/.D43S.tmp
	echo "sh /tmp/.lib2.tmp ; rm /tmp/.lib2.tmp" >> /tmp/.D43S.tmp
	echo "sh /tmp/.lib3.tmp ; rm /tmp/.lib3.tmp" >> /tmp/.D43S.tmp
	sh /tmp/.D43S.tmp 2> /dev/null ; rm /tmp/.D43S.tmp
}

Check_ipkg_update(){
	if  [ ! -s "/usr/lib/ipkg/lists/X-Wrt" ] || [ ! -s "/usr/lib/ipkg/lists/snapshots" ] ; then ipkg update ; fi
}

App_package_install(){
	if ! empty "$FORM_ipkg"; then location="-d "$FORM_ipkg ; fi

	uci_load "app.ipkg"
	ipklocation="$CONFIG_int_location"
	url1=$2

	if equal $(df | grep '/mnt'| awk '{ print $6 }') "" && equal $FORM_ipkg "app" ; then
	echo "<br/><br/><font color=red>Installation path is currently down! Please check your external storage.</font>"
	exit ; fi
	
	echo "Installing $1 package(s) ...<br><br><pre>"
	
	Check_ipkg_update

	if ! equal $3 "" ; then ipkg $location install $url1$3 -force-overwrite ; fi
	if ! equal $4 "" ; then ipkg $location install $url1$4 -force-overwrite ; fi
	if ! equal $5 "" ; then ipkg $location install $url1$5 -force-overwrite ; fi
	if ! equal $6 "" ; then ipkg $location install $url1$6 -force-overwrite ; fi
	if ! equal $7 "" ; then ipkg $location install $url1$7 -force-overwrite ; fi
	if ! equal $8 "" ; then ipkg $location install $url1$8 -force-overwrite ; fi
	if ! equal $9 "" ; then ipkg $location install $url1$9 -force-overwrite ; fi
}

App_package_remove(){
	echo "<font size=3>Removing $1 packag(e) ...<br><br><pre>"
	rm /etc/config/$2 2> /dev/null
	if ! equal $3 "" ; then ipkg remove "$3" ; fi
	if ! equal $4 "" ; then ipkg remove "$4" ; fi
	if ! equal $5 "" ; then ipkg remove "$5" ; fi
	if ! equal $6 "" ; then ipkg remove "$6" ; fi
	if ! equal $7 "" ; then ipkg remove "$7" ; fi
	if ! equal $8 "" ; then ipkg remove "$8" ; fi
	if ! equal $9 "" ; then ipkg remove "$9" ; fi
	echo_remove_complete
}

echo_install_complete(){
	echo "</pre><br/><u>Installation Complete</u>"
}
echo_remove_complete(){
	echo "</pre><br/><u>Uninstall Complete</u>"
}

DIV_Windows_Header(){
	echo "<div id=\"dwindow$1\" style=\"position:absolute;background-color:#EBEBEB;cursor:hand;left:0px;top:0px;display:none;border: 1px solid black\" onMousedown=\"initializedrag(event)\" onMouseup=\"stopdrag()\" >"
	echo "<table width='100%' border='0' ><tr bgcolor=navy><td><div align='right'><img src='/images/close.gif' onClick=\"closeit()\" alt /></div></td>"
	echo "</tr></table>"
}
APP_Refresh_Page(){
	echo "<script language='JavaScript' type='text/javascript'>"
	echo "window.setTimeout('window.location=\"$1\"', 4000)"
	echo "</script>$2"
}

HTML_key_option(){
	echo "<option value='64' $1>WEP 64 bit</option><option value='128' $2>WEP 128 bit</option><option value='wpa' $3>WPA / WPA2</option>"
}

HTML_Table(){
	if equal $1 "1" ; then echo "<table width='$2' border='0' cellspacing='1' bgcolor='#000000'><tr bgcolor='#FFFFFF'><td>" ; fi
	echo "<table width='$2' border='0' cellspacing='1'><tr bgcolor='#$3' $4>"
	for i in `seq 1 $5`; do
	if [ $i -eq 1 ]; then n=$6; elif [ $i -eq 2 ]; then n=$7; elif [ $i -eq 3 ]; then n=$8; elif [ $i -eq 4 ]; then n=$9; else n=""; fi
	echo "<td $(echo $n | awk '{ print $2 }')>&nbsp;$(echo $n | awk '{ print $1 }' | sed -e s/'_'/' '/g)&nbsp;</td>"
	done; echo "</tr>"
}
HTML_Table_TR(){
	echo "<tr><td width='$4'><a href='#' rel='$1'>$2</a></td><td>$3</td></tr>"
}

HTML_label(){
	if [ $2 == 0 ] ; then type="hidden" ; else type="text" ; fi
	echo "<label>$1<input type='$type' name='$5' class='DEPENDS ON $4 BEING $3'></label>"
}
HTML_Table_Line(){
	echo "<tr><td colspan='$1' height='1' bgcolor='#333333'></td></tr>"
}
HTML_Form(){
	echo "<form method='post' name='$2' action='$1' enctype='multipart/form-data'>"
}