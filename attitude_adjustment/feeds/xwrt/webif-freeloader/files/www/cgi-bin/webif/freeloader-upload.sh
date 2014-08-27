#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
###################################################################
# freeloader-upload.sh
# (c)2007 X-Wrt project (http://www.x-wrt.org)
# (c)2007-03-11 m4rc0
#
#	version 1.4
#
# Description:
#	Gives functionality to upload torrent/nzb and url info to freeloader.
#
# Author(s) [in order of work date]:
#	m4rc0 <janssenmaj@gmail.com>
#
# Major revisions:
#		1.3 Added username/password - m4rc0 25-3-2007
#		1.4 Added multiple url list - m4rc0 29-3-2007
#
#
# NVRAM variables referenced:
#   none
#
# Configuration files referenced:
#   /etc/config/freeloader
#
#

header_inject_head=$(cat <<EOF
<script type="text/javascript">
<!--
function webif_entityDecode(s) {
    var e = document.createElement("div");
    e.innerHTML = s;
    return e.firstChild.nodeValue;
}

function checkformURL(form) {
	if (form.uploadURL.value == "" && document.getElementById("divurl1").style.display == "block") {
		alert( webif_entityDecode( "@TR<<freeloader-upload_Enter_URL#Please enter a URL for uploading to the router.>>" ) );
		form.uploadURL.focus();
    		return false ;
		}
 
	if (form.uploadURLlist.value == "" && document.getElementById("divurl2").style.display == "block") {
		alert( webif_entityDecode( "@TR<<freeloader-upload_Enter_URL_list#Please enter a list of URLs for uploading to the router.>>" ) );
		form.uploadURLlist.focus();
    		return false ;
		}

	if (form.username.value != "" && form.password.value == "") {
  		alert ( webif_entityDecode( "@TR<<freeloader-upload_Enter_password#You forgot to enter a password, please enter one.>>" ) );
  		form.password.focus();
  		return false;
  		}

	if (form.username.value == "" && form.password.value != "") {
  		alert ( webif_entityDecode( "@TR<<freeloader-upload_Enter_username#You forgot to enter a username, please enter one.>>" ) );
  		form.username.focus();
  		return false;
  		}
 
  return true ;
}

function checkformTorrentNZB(form) {
  if (form.uploadfile.value == "") {
    alert( webif_entityDecode( "@TR<<freeloader-upload_Select_a_file#Please select a .torrent or .nzb file for uploading to the router.>>" ) );
    form.uploadfile.focus();
    return false ;
  }  
  return true ;
}

function togglediv(selecteddiv) {
	document.getElementById("divurl1").style.display = "none";
	document.getElementById("divurl2").style.display = "none";
	document.getElementById(selecteddiv).style.display = "block";
	
	document.getElementById("uploadURL").value = "";
	document.getElementById("uploadURLlist").value = "";
}

// -->
</script>

EOF
)

header "Freeloader" "freeloader-upload_subcategory#Upload" "@TR<<freeloader-upload_Freeloader_upload#Freeloader upload>>"

#Include settings
. /usr/lib/webif/freeloader-include.sh
freeloader_init_config

#check for installed packages and store the status
is_package_installed "curl"
pkg_curl=$?
is_package_installed "ctorrent"
pkg_ctorrent=$?
is_package_installed "nzbget"
pkg_nzbget=$?
is_package_installed "mini-sendmail"
pkg_minisendmail=$?

#if one of the packages is installed the page can be displayed
if [ $pkg_nzbget -eq "0" ] || [ $pkg_ctorrent -eq "0" ]; then
cat <<EOF
<div class="settings">
<h3>@TR<<freeloader-upload_Upload_c_n#Upload ctorrent and nzbget>></h3>
<div class="settings-content">
<form name="formuploadfile" action="freeloader-uploadcallback.sh" method="post" enctype="multipart/form-data" onsubmit="return checkformTorrentNZB(this);">
<table border="0" class="packages" width="100%">
<tr>
	<td width="40%"><b>@TR<<freeloader-upload_File#File>></b></td>
	<td>
		<input type="file" name="uploadfile" />
		<input type="submit" value="@TR<<freeloader-upload_GO#GO>>" />
	</td>
</tr>
<tr>
	<td width="40%"><b>@TR<<freeloader-upload_Priority#Priority>></b></td>
	<td>
		<input type="radio" name="queue" value="normal" checked="checked"/>@TR<<freeloader-upload_Priority_normal#normal>>
		<input type="radio" name="queue" value="prio" />@TR<<freeloader-upload_Priority_prio#prio>>
	</td>
</tr>
</table>
</form>
</div>
<blockquote class="settings-help">
<h3><strong>@TR<<Short help>>:</strong></h3>
<h4>@TR<<freeloader-upload_File#File>>:</h4><p>@TR<<freeloader-upload_File_helptext#Here you can upload .torrent and .nzb files for downloading by pressing the browse button. When the GO button is pressed the file is uploaded to the router.>></p>
<h4>@TR<<freeloader-upload_Priority#Priority>>:</h4><p>@TR<<freeloader-upload_Priority_helptext#With the priority switch you can select to which queue the file is uploaded.>></p>
</blockquote>
<div class="clearfix">&nbsp;</div></div>
EOF
fi

#if the curl package is installed the page can be displayed
if [ $pkg_curl -eq "0" ]; then
cat <<EOF
<div class="settings">
<h3>@TR<<freeloader-upload_Upload_curl#Upload curl>></h3>
<div class="settings-content">
<form name="formuploadurllist" action="freeloader-uploadcallback.sh" method="post" onsubmit="return checkformURL(this);">
<table border="0" class="packages" width="100%">
<tr>
	<td valign="top" width="40%"><b>@TR<<freeloader-upload_URL#URL>></b></td>
	<td>
		<div id="divurl1" style="display:block;"><input type="text" name="uploadURL" id="uploadURL" /><input type=submit value="@TR<<freeloader-upload_GO#GO>>" /></div>
		<div id="divurl2" style="display:none;"><textarea name="uploadURLlist" id="uploadURLlist" rows="6" cols="40"></textarea><input type=submit value="@TR<<freeloader-upload_GO#GO>>" /></div>
	</td>
</tr>
<tr>
	<td width="40%"></td>
	<td>
		<input type="radio" name="inputtype" value="single" onclick="togglediv('divurl1');" checked="checked"/>@TR<<freeloader-upload_single_url#single url>>
		<input type="radio" name="inputtype" value="multiple" onclick="togglediv('divurl2');" />@TR<<freeloader-upload_multiple_urls#multiple urls>>
	</td>
</tr>
<tr>
	<td width="40%"><b>@TR<<freeloader-upload_Priority#Priority>></b></td>
	<td>
		<input type="radio" name="queue" value="normal" checked="checked"/>@TR<<freeloader-upload_Priority_normal#normal>>
		<input type="radio" name="queue" value="prio" />@TR<<freeloader-upload_Priority_prio#prio>>
	</td>
</tr>
<tr>
	<td width="40%"><b>@TR<<freeloader-upload_Username#Username>></b></td>
	<td><input type="text" name="username" /></td>
</tr>
<tr>
	<td width="40%"><b>@TR<<freeloader-upload_Password#Password>></b></td>
	<td><input type="text" name="password" /></td>
</tr>
</table>
</form>
</div>
<blockquote class="settings-help">
<h3><strong>@TR<<Short help>>:</strong></h3>
<h4>@TR<<freeloader-upload_URL#URL>>:</h4><p>@TR<<freeloader-upload_URL_helptext#Give the URL of the file you want to download. In case you enter multiple urls, every url should be placed on a new line.>></p>
<h4>@TR<<freeloader-upload_Priority#Priority>>:</h4><p>@TR<<freeloader-upload_Priority_helptext#With the priority switch you can select to which queue the file is uploaded.>></p>
<h4>@TR<<freeloader-upload_Username#Username>>:</h4>
<h4>@TR<<freeloader-upload_Password#Password>>:</h4><p>@TR<<freeloader-upload_Credentials_helptext#The credentials needed to download the file from the server.>></p>
</blockquote>
<div class="clearfix">&nbsp;</div></div>
EOF
fi

#If the packages are not installed, give the user the possibilty to install the required packages.
if [ $pkg_ctorrent -eq "1" ]; then
	has_pkgs "ctorrent"
fi

if [ $pkg_nzbget -eq "1" ]; then
	has_pkgs "nzbget"
fi

if [ $pkg_curl -eq "1" ]; then
	has_pkgs "curl"
fi

if [ $pkg_minisendmail -eq "1" ]; then
	has_pkgs "mini-sendmail"
fi

crontab -l 2>/dev/null | grep -q "^[^#].*/getfreeloader.sh"
cron_getfreeloader=$?
crontab -l 2>/dev/null | grep -q "^[^#].*/killfreeloader.sh"
cron_killfreeloader=$?

if [ $cron_getfreeloader -eq "1" ] || [ $cron_killfreeloader -eq "1" ]; then
	cat << EOF
<div class="settings">
<h3>@TR<<freeloader-upload_Cron_setup#Cron setup>></h3>
<div class="settings-content">
EOF
	echo "<pre>"
	echo "@TR<<freeloader-upload_Add_crontab#Add the following lines to crontab>>:"
	echo ""
	echo "*/1 * * * * /usr/sbin/getfreeloader.sh"
	echo "*/1 * * * * /usr/sbin/killfreeloader.sh"
	echo "</pre>"
	echo "</div>"
	echo "<div class=\"clearfix\">&nbsp;</div></div>"
fi

#somehow firefox does not reset the radiobuttons after a reload of the page.
cat <<EOF
<script type="text/javascript">
document.formuploadurllist.inputtype[0].checked = true;
</script>


EOF
footer
?>
<!--
##WEBIF:name:Freeloader:10:freeloader-upload_subcategory#Upload
-->
