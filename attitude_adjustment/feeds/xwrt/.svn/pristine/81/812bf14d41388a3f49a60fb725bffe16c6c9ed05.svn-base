#!/usr/bin/webif-page
<?
#################################
# Applications page
#
# Description:
#	List and install additional applications.
#
# Author(s) [in order of work date]:        
#	Dmytro Dykhman <dmytro@iroot.ca>
#
# Major revisions:
#
#
# Configuration files referenced:
#
#
# TODO:

. /www/cgi-bin/webif/applications-shell.sh
. /usr/lib/webif/functions.sh

var="#"
var1=""
var2=""
var3=""
var4=""
var5=0

txt1="This package is not installed.\n\nDo you want to install it now?\n"
txt2=$txt1"\n---------------------------------------------\n!!! Device is running out of Memory !!!\n\n ipkg is installing to external storage ...\n"
txt3="Package is installed, but path is currently down!\n\nPlease check your external storage."
txt4=""
txt5=$txt1"\n---------------------------------------------\n!!! Device is running out of Memory !!!\n\n ipkg is installing to external storage ...\n"

ct1=0
ct2=0
count=0
count1=1

LoadingJAVA="<table border='0'><tr><td><br/>Please Wait ...<br/><script type="text/javascript" src=\"/js/progress.js\"></script><script type='text/javascript'>
var bar1= createBar(350,15,'white',1,'black','blue',85,7,3,'');
</script></td></tr></table></div>
<table width='98%' border='0' cellspacing='10' ><tr class='appindex'>"

CONFIRM()
{
echo "<script type='text/javascript'>"
echo "function confirm$count() {"
if [ $var = "applications.sh?ipkg=" ] ; then echo "alert (\"Not enough memory to install!\")" ; fi
echo "if (window.confirm(\"$txt1\")){" 
echo "var ap='null';"
if [ $var = "applications.sh?ipkg=" ] ; then echo "ap= prompt('Enter path to external storage. Example /mnt/mmc', ' ');" ; fi
echo "loadwindow(0,'$SCRIPT_NAME',355,100,0,5);"
echo "window.location=\"$var&package=install&prompt=\" + ap"
echo "} }"
echo "</script>"
let "count+=1"
}

CHECK_FREE_MEM()
{

RAMFREE=$(free | uniq | grep -vE '^              total|Mem:|Swap:' | awk '{ print $4  }' )
let "RAMFREE+=600" #<-compensate for the ram we used to process this file
#echo "["$RAMFREE"]"

df | uniq | grep -vE '^Filesystem|tmpfs|cdrom' | awk '{ print $4 " " $1 }' | while read output;
do

usep=$(echo $output | awk '{ print $1}' ) # | cut -d'%' -f1  )
partition=$(echo $output | awk '{ print $2 }' )

if [ $partition = "/dev/mtdblock/4" ] || [ $partition = "/" ] || [ $partition = "/dev/root" ] ; then
if [ $usep -le $ct1 ] || [ $RAMFREE -lt $ct2 ] ; then
	if [ -s "/etc/config/app.ipkg" ] ; then
	if [ $RAMFREE -lt $ct2 ] ; then
		txt1="Low RAM !!!\n\nMinimum: $ct2 KB\nAvailable: $RAMFREE KB\n\nWe recommend you try to Swap extra memory."
		var="applications.sh?page=network"
		CONFIRM
	else
		var=$var"app"
		txt1=$txt2 
		CONFIRM
	fi

	else
		txt1=" !!! Device is running out of Memory !!!\n\nDo you want to install to remote location?\n"
		var="applications.sh?ipkg="
		CONFIRM
	fi
else
CONFIRM
fi
fi
done
let "count+=1"
}

TR_Remove_APP()
{
if [ $4 = 1 ] ; then txt4="<br/><font size=1 color=red>$txt3</font></b><br/><br/>" ; else txt4="" ; fi
echo "<tr><td width=90%><img src='/images/$1' width="22" height="22" align="middle" alt />&nbsp;$2$(echo $txt4 | sed -e s/'\\n\\n'/'<br\/>'/g )</td><td><form action='$3' method='post'><input type='hidden' name='remove' value='1' /><input type=submit class='flatbtn' name=rmvapp value='@TR<<Remove Application>>' $5 /></form></td></tr>"
}

TR_APP()
{
if [ $var5 = 1 ] ; then var3="<font color=silver>"$var3"</font>" ; txt4="class='gradualshine' onMouseover='slowhigh(this)' onMouseout='slowlow(this)'" ; else txt4="" ; fi
echo "<td width=\"20%\"><center><a href=\"$var1\" rel=\"$var4\"><img src=\"/images/$var2\" border=\"0\" $txt4 alt /></a><br/>$var3</center></td>"
}

HighLight="class='gradualshine' onMouseover='slowhigh(this)' onMouseout='slowlow(this)'"

if [ "$FORM_page" = "index" ]; then
	echo "$HEADER"
	echo "$HTMLHEAD</head><body></body></html>"
	exit

elif [ "$FORM_page" = "web" ]; then

	echo "$HEADER"
	echo "$HTMLHEAD"
	#	   /- Space in KB
	#	  |
	#	  |	   	  	 /- RAM in KB
	#	  \/		  	\/
	let "ct1=4000" ; let "ct2=2000" ; var="applications-httpd.sh?ipkg=" ; CHECK_FREE_MEM #<- WebServer
	let "ct1=4500" ; let "ct2=4600" ; var="applications-proftpd.sh?ipkg=" ; CHECK_FREE_MEM #<-FTP Server
	let "ct1=100"	 ; let "ct2=10000000" ; var="applications-sql.sh?ipkg=" ; CHECK_FREE_MEM #<-SQL Server
	let "ct1=2000" ; let "ct2=800" ; var="applications-dlmanager.sh?ipkg=" ; CHECK_FREE_MEM #<-DL MANAGER

	echo "</head><body>"
	DIV_Windows_Header 0
	echo $LoadingJAVA

	#------------------------
	var3="Web Server"
	var2="app.4.jpg"
	var4="b1"
	if is_package_installed "apr" && is_package_installed "gdbm" && is_package_installed "expat" && is_package_installed "libdb" && is_package_installed "apr-util" && is_package_installed "apache" ; then
	var1="applications-httpd.sh" ; var5=0 ; TR_APP	
	else if  [ -s "/etc/config/app.httpd" ]  ; then 
	var1="javascript:alert('$txt3');" ; var5=0 ; TR_APP	
	else var1="javascript:confirm0()" ; var5=1 ; TR_APP	
	fi
	fi
	
	#------------------------
	var3="FTP Server"
	var2="app.6.jpg"
	var4="b2"
	if is_package_installed "openssl" && is_package_installed "proftpd" ; then
	var1="applications-proftpd.sh" ; var5=0 ; TR_APP
	else if  [ -s "/etc/config/app.proftpd" ]  ; then 
	var1="javascript:alert('$txt3');" ; var5=0 ; TR_APP
	else var1="javascript:confirm1()" ; var5=1 ; TR_APP
	fi
	fi

	#------------------------
	var3="SQL Server" ; var2="app.7.jpg" ; var4="b3"
	if is_package_installed "sql" && is_package_installed "sql2" ; then
	var1="applications-db.sh" ; var5=0 ; TR_APP
	else var1="javascript:confirm2()" ; var5=1 ; TR_APP
	fi
	#------------------------
	var3="Download Manager" ; var2="bkup.jpg" ; var4="b4"
	if is_package_installed "ctorrent" ; then
	var1="applications-dlmanager.sh" ; var5=0 ; TR_APP
	else if  [ -s "/etc/config/app.dlmanager" ]  ; then 
	var1="javascript:alert('$txt3');" ; var5=0 ; TR_APP
	else var1="javascript:confirm3()" ; var5=1 ; TR_APP
	fi
	fi

	cat <<EOF
	<td width="20%">&nbsp;</td><td width="20%">&nbsp;</td></tr>
	<tr><td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td>
	</tr></table>
EOF

TIP 0 "Lighttpd 1.4 - Powerfull webserver to serve web pages on World Wide Web."
TIP 0 "ProFTPD 1.3a - Powerfull FTP server for sharing files globally."
TIP 0 "SQL - Massive database server"
TIP 0 "Download Manager - Allows unattended downloads http,ftp,torrents"
echo "</body></html>"

exit

elif [ "$FORM_page" = "security" ]; then
	
	echo "$HEADER"
	echo "$HTMLHEAD"
	let "ct1=150" ; let "ct2=800" ; var="applications-hydra.sh?ipkg=" ; CHECK_FREE_MEM

	echo "</head><body>"
	DIV_Windows_Header 0
	echo $LoadingJAVA

	#------------------------
	var3="Hydra"
	var2="app.2.jpg"
	var4="b1"
	if  [ -s "/usr/sbin/hydra" ]  ; then 
	var1="applications-hydra.sh" ; var5=0 ; TR_APP
	else var1="javascript:confirm0()" ; var5=1 ; TR_APP
	fi
	#------------------------

	cat <<EOF
	<td width="20%">&nbsp;</td>
	<td width="20%">&nbsp;</td>
	<td width="20%">&nbsp;</td>
	<td width="20%">&nbsp;</td>
	</tr></table>
EOF
	TIP "300" "Hydra 4.5 - \"Password Brute Force\" - attacker for checking weak passwords. Great utility to check your (http,ftp,ssh) services."
	echo "</body></html>"
exit

elif [ "$FORM_page" = "network" ]; then

	echo "$HEADER"
	echo "$HTMLHEAD"
	var="applications-cifs.sh?ipkg=" ; CONFIRM
	let "ct1=2000" ; let "ct2=1000" ; var="applications-samba.sh?ipkg=" ; CHECK_FREE_MEM
	var="applications-swap.sh?ipkg=" ; CONFIRM
	echo "</head><body>"
	DIV_Windows_Header 0
	echo $LoadingJAVA

	#------------------------
	var3="Samba Client"
	var2="app.10.jpg"
	var4="b1"
	if is_package_installed "kmod-fs-cifs" && is_package_installed "cifsmount" ; then
	var1="applications-cifs.sh" ; var5=0 ; TR_APP
	else var1="javascript:confirm0()" ; var5=1 ; TR_APP
	fi
	#------------------------
	var3="Samba Server" ; var2="app.10.jpg" ; var4="b2"
	if is_package_installed "samba" ; then
	var1="applications-samba.sh" ; var5=0 ; TR_APP
	else var1="javascript:confirm1()" ; var5=1 ; TR_APP
	fi

	#------------------------
	var3="Memory Swap"
	var2="app.12.jpg"
	var4="b3"
	if is_package_installed "kmod-loop" && is_package_installed "losetup" && is_package_installed "swap-utils" ; then
	var1="applications-swap.sh" ; var5=0 ; TR_APP	
	else var1="javascript:confirm2()" ; var5=1 ; TR_APP
	fi
	
	cat <<EOF
	<td width="20%">&nbsp;</td>
	<td width="20%">&nbsp;</td>
	<td width="20%">&nbsp;</td>
	</tr>
EOF
	#------- ipkg settings -------
	if  [ -s "/etc/config/app.ipkg" ]  ; then echo "<tr class='appindex'><td width='20%'><center><a href='applications-ipkg.sh'><img src='/images/pkg.jpg' border='0' alt /></a><br/>ipkg</center></td>" ; else echo "<td width="20%">&nbsp;</td>" ; fi
cat <<EOF
	<td width="20%">&nbsp;</td><td width="20%">&nbsp;</td><td width="20%">&nbsp;</td><td width="20%">&nbsp;</td></tr></table>
EOF
	TIP 200 "Samba Client - Allows to map network drive from Windows based file system."
	TIP 250 "Samba Server - Allows to share directories over network."
	TIP 250 "Memory SWAP - Allows to set more RAM by using external storage.<br/><br/>Examples: Network Drive, MMC, USB"
	echo "</body></html>"
	exit

elif [ "$FORM_page" = "wireless" ]; then

	echo "$HEADER"
	echo "$HTMLHEAD"
	let "ct1=200" ; let "ct2=100" ; var="applications-aircrack.sh?ipkg=" ; CHECK_FREE_MEM
	let "ct1=500" ; let "ct2=100" ; var="applications-chillispot.sh?ipkg=" ; CHECK_FREE_MEM
	echo "</head><body>"
	DIV_Windows_Header 0
	echo $LoadingJAVA
	
	#------------------------
	var3="AirCrack" ; var2="app.1.jpg" ; var4="b1"
	if is_package_installed "aircrack-ng" ; then
	var1="applications-aircrack.sh" ; var5=0 ; TR_APP
	else var1="javascript:confirm0()" ; var5=1 ; TR_APP
	fi
	#------------------------
	var3="ChilliSpot" ; var2="app.14.gif" ; var4="b2"
	if is_package_installed "chillispot" ; then
	var1="applications-chillispot.sh" ; var5=0 ; TR_APP
	else var1="javascript:confirm1()" ; var5=1 ; TR_APP
	fi

	cat <<EOF
	<td width="20%">&nbsp;</td>
	<td width="20%">&nbsp;</td>
	<td width="20%">&nbsp;</td>
	<td width="20%">&nbsp;</td>
	<td width="20%">&nbsp;</td>
	</tr></table>
EOF
	TIP 0 "Aircrack-ng - Tools for wireless traffic monitoring and penetration/security testing."
	echo "</body></html>"
	exit

elif [ "$FORM_page" = "remove" ]; then
	echo "$HEADER"
	echo "$HTMLHEAD"
	echo "</head><table width='100%' border='0' cellspacing='5'>"
	#### Check list to remove
	pkgrmv="<input type='hidden' name='remove' value='1'><input type=submit class='flatbtn' name=rmvapp value='@TR<<Remove Application>>'>"
#------------------------
	echo "<tr><td colspan=2><br/><u>> Web Applications</u><br/><br/></td></tr>"
	
	if is_package_installed "apr" && is_package_installed "gdbm" && is_package_installed "expat" && is_package_installed "libdb" && is_package_installed "apr-util" && is_package_installed "apache" ; then
	TR_Remove_APP "app.4.jpg" "Web Server" "applications-httpd.sh" 0 "" 
	else if  [ -s "/etc/config/app.httpd" ]  ; then TR_Remove_APP	"app.4.jpg" "Web Server" "applications-httpd.sh" 1 "disabled" ; fi
	fi

	if is_package_installed "proftpd" ; then
	TR_Remove_APP "app.6.jpg" "FTP Server" "applications-proftpd.sh" 0 ""
	else if  [ -s "/etc/config/app.proftpd" ]  ; then TR_Remove_APP "app.6.jpg" "FTP Server" "applications-proftpd.sh" 1 "disabled" ; fi
	fi

	if is_package_installed "ctorrent" ; then
	TR_Remove_APP "bkup.jpg" "Download Manager" "applications-dlmanager.sh" 0 ""
	else if  [ -s "/etc/config/app.dlmanager" ]  ; then TR_Remove_APP "bkup.jpg" "Download Manager" "applications-dlmanager.sh" 1 "disabled" ; fi
	fi
#------------------------
	echo "<tr><td colspan=2><br/><u>> Security Applications</u><br/><br/></td></tr>"
	if  [ -s "/usr/sbin/hydra" ]  ; then TR_Remove_APP "app.2.jpg" "Hydra" "applications-hydra.sh" 0 "" ; fi
#------------------------
	echo "<tr><td colspan=2><br/><u>> Network Applications</u><br/><br/></td></tr>"
	if is_package_installed "kmod-fs-cifs"  &&  is_package_installed "cifsmount"  ; then TR_Remove_APP "app.10.jpg" "Samba Client" "applications-cifs.sh" 0 "" ; fi
	if is_package_installed "kmod-loop" && is_package_installed "swap-utils" &&  is_package_installed "losetup"  ; then TR_Remove_APP	 "app.12.jpg" "Memory Swap" "applications-swap.sh" 0 "" ; fi
#------------------------
	echo "<tr><td colspan=2><br/><u>> Wireless Applications</u><br/><br/></td></tr>"
	if is_package_installed "aircrack-ng" ; then TR_Remove_APP "app.1.jpg" "AirCrack" "applications-aircrack.sh" 0 ""
	else if  [ -s "/etc/config/app.aicrack" ]  ; then TR_Remove_APP "app.1.jpg" "AirCrack" "applications-aircrack.sh" 1 "disabled" ; fi
	fi
	if is_package_installed "chillispot" ; then TR_Remove_APP "app.14.gif" "ChilliSpot" "applications-chillispot.sh" 0 ""
	else if  [ -s "/etc/config/app.chillispot" ]  ; then TR_Remove_APP "app.14.gif" "ChilliSpot" "applications-chillispot.sh" 1 "disabled" ; fi
	fi

	echo "</table></html>"
	exit

elif [ "$FORM_page" = "list" ]; then
	echo "$HEADER"
	echo "$HTMLHEAD"

	cat <<EOF
	</head><body><table width="98%" border="0" align="right" >
	<tr>
	<td><a href="applications.sh?page=web" target="AppIndex"><img src="/images/app.8.jpg" border="0" alt /></a></td>
	<td><strong>Web Applications</strong></td></tr>
	<tr><td colspan="2">&nbsp;</td></tr>
	<tr>
	<td><a href="applications.sh?page=security" target="AppIndex"><img src="/images/app.5.jpg" border="0" alt /></a></td>
	<td><strong>Security Applications</strong></td>
	</tr>
	<tr><td colspan="2">&nbsp;</td></tr>
	<tr>
	<td><a href="applications.sh?page=network" target="AppIndex"><img src="/images/app.9.jpg" border="0" alt /></a></td>
	<td><strong>Network Applications</strong></td>
	</tr>
	<tr><td colspan="2">&nbsp;</td></tr>
	<tr>
	<td><a href="applications.sh?page=wireless" target="AppIndex"><img src="/images/wscan.jpg" border="0" alt /></a></td>
	<td><strong>Wireless Applications</strong></td>
	</tr>
	<tr><td colspan="2">&nbsp;</td></tr>
	<tr><td colspan=2 height=1 bgcolor="#CCCCCC"></td></tr>
	<tr>
	<td><a href="applications.sh?page=remove" target="AppIndex"><img src="/images/app.11.jpg" border="0" alt /></a></td>
	<td><strong>Remove Applications</strong></td>
	</tr>
	<tr><td colspan="2">&nbsp;</td></tr></table></body></html>
EOF

elif [ "$FORM_page" = "" ] && [ "$FORM_package" = "" ]; then

. /usr/lib/webif/webif.sh
	header "Applications" "" ""


	cat <<EOF
	<br/><font color="#FF0000">This page is currently in development process. Some features may not function. </font><br/>
	<table width="100%" border="0" cellspacing="1">
	<tr><td width="35%">&nbsp;</td><td width="65%">&nbsp;</td></tr>
	<tr><td><IFRAME SRC="applications.sh?page=list" STYLE="width:100%; height:450px; border:1px dotted #888888;" FRAMEBORDER="0" SCROLLING="no" name="AppList"></IFRAME></td>
	<td><IFRAME SRC="applications.sh?page=index" STYLE="width:98%; height:450px; border:1px dotted #888888;" FRAMEBORDER="0" SCROLLING="yes" name="AppIndex"></IFRAME></td>
	</tr></table></div></div></body></html>
EOF

fi

if [ "$FORM_package" = "install" ]; then

	echo "<html><head><META http-equiv="refresh" content=\"10;URL=applications.sh?page=index\"></head>"
	if [ ! -d $FORM_prompt ] || [ "$FORM_prompt" = "" ]  || [ "$FORM_prompt" = " " ]  ; then
		echo "Directory <b>'$FORM_prompt'</b> missing - I will try to mount it (hint: Samba Client)"
		exit
	fi
	echo "<body><br/>Applying changes to ipkg ...<br/<br/><br/>To disable or change location goto <b>\"Network Applications\" -> \"ipkg\"</b></body></html>"
	echo "config ipkg int
	option location	'$FORM_prompt'" > /etc/config/app.ipkg
	if equal $(grep "$FORM_prompt" < /etc/ipkg.conf) "" ; then #<-Check make sure we do't have same entry
	
	cp /etc/ipkg.conf /etc/ipkg.conf.bak
	echo "
dest app $FORM_prompt" >> /etc/ipkg.conf

	### --- Read file
	tempfile=$(mktemp /tmp/.ipkg.XXXXXX)
	chmod 644 $tempfile
	exec 3<&0
	exec 0</etc/profile
	while read line
	do
	
	if equal $(echo $line | cut -c8-11) "PATH" ; then
	if equal $(grep "$FORM_prompt" < /etc/profile) "" ; then #<- Just in case to double check
	echo "export PATH=/bin:/sbin:/usr/bin:/usr/sbin:$FORM_prompt/bin:$FORM_prompt/sbin:$FORM_prompt/usr/bin:$FORM_prompt/usr/sbin:$FORM_prompt/opt/bin:$FORM_prompt/opt/sbin" >> $tempfile
	else
	echo "$line" >> $tempfile
	fi
	else

	# TODO Make this less compilated! becase echo cuts \ and saves us: export PS1='u@h:w$ '
	#
	if equal $(echo $line | cut -c1-10 | sed -e s/' '//g) "exportPS1" ; then echo "export PS1='\u@\h:\w\\\$ '" >> $tempfile ; else echo "$line" >> $tempfile ; fi
	
	fi
	done

	mv /etc/profile /etc/profile.bak
	mv $tempfile /etc/profile
	exec 0<&3
	exit 0
	### ---

	fi

	exit
fi
?>
<!--
##WEBIF:name:Applications:1:
-->
