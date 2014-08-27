#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
###################################################################
# Packages configuration page
#
# Description:
#	Allows installation and removal of packages.
#
# Author(s) [in order of work date]:
#   OpenWrt developers (??)
#   todo: person who added descriptions..
#   Dmytro
#   eJunky
#   emag
#   Jeremy Collake <jeremy.collake@gmail.com>
#
# Major revisions:
#
# NVRAM variables referenced:
#   none
#
# Configuration files referenced:
#   none
#
# Utilities/applets referenced:
#   ipkg
#
#

lists_path=$(cat /etc/opkg.conf 2>/dev/null | grep ^lists_dir | cut -d' ' -f3)
if [ -z "$lists_path" ]; then
	lists_path="/usr/lib/opkg/lists"
else
	lists_path="${lists_path%%/}"
fi

header "System" "Packages" "<img src=\"/images/pkg.jpg\" alt=\"\" />&nbsp;@TR<<system_ipkg_Packages#Packages>>" '' "$SCRIPT_NAME"

cat <<EOF
<script type="text/javascript">
function confirmT(action,pkg) {
if ( pkg == "uclibc" || pkg == "base-files" || pkg == "bridge" || pkg == "busybox" || pkg == "dnsmasq" || pkg == "dropbear" || pkg == "haserl" || pkg == "hotplug" || pkg == "iptables" || pkg == "kernel" || pkg == "mtd" || pkg == "wireless-tools" || pkg == "wlc") {
alert ("             <<< @TR<<big_warning|WARNING>> >>> \n\n@TR<<system_ipkg_Package|Package>> \"" + pkg + "\" @TR<<should not be removed>>!\n\n>>> @TR<<Removing may brick your router.>> <<<\n\n@TR<<System requires>> \"" + pkg + "\" @TR<<package to run.>>\n\n") ;
}
var actionStr = action=="install" ? "@TR<<system_ipkg_install|install>>" : 
	(action=="remove" ? "@TR<<system_ipkg_remove|remove>>" : action);
if (window.confirm("@TR<<Please Confirm>>!\n\n@TR<<Do you want to>> " + actionStr + " \"" + pkg + "\" @TR<<system_ipkg_package|package>>?")){
window.location="system-ipkg.sh?action=" + action + "&amp;pkg=" + pkg
} }
</script>
EOF

##################################################################
#
# Install from URL and Add Repository code - self-contained block.
#

repo_update_needed=0

! empty "$FORM_install_url" && {
	# just set up to pass-through to normal handler
	FORM_action="install"
	FORM_pkg="$FORM_pkgurl"
}

! empty "$FORM_install_repo" && {
validate << EOF
string|FORM_reponame|@TR<<system_ipkg_reponame#Repo. Name>>|min=4 max=40 required nospaces|$FORM_reponame
string|FORM_repourl|@TR<<system_ipkg_repourl#Repo. URL>>|min=4 max=4096 required|$FORM_repourl
EOF
	if equal "$?" "0"; then
		repo_update_needed=1
		# since firstboot doesn't make a copy of ipkg.conf, we must do it
		# todo: need a mutex or lock here
		tmpfile=$(mktemp "/tmp/.webif-ipkg-XXXXXX")
		cp -p "/etc/opkg.conf" "$tmpfile"
		echo "src $FORM_reponame $FORM_repourl" > "$tmpfile"
		cat "/etc/opkg.conf" >>"$tmpfile"
		rm "/etc/opkg.conf"
		mv "$tmpfile" "/etc/opkg.conf"
	else
		echo "<h3 class=\"warning\">$ERROR</h3>"
	fi
}

! empty "$FORM_remove_repo_name" && ! empty "$FORM_remove_repo_url" && {
	repo_update_needed=1
	repo_src_line="src[\/gz]* $FORM_remove_repo_name $FORM_remove_repo_url"
	remove_lines_from_file "$FORM_remove_repo_file" "$repo_src_line"
	# manually remove package lists since ipkg update won't..
	# todo: odd issue where 'rm -f /usr/lib/opkg/lists/* does not work - openwrt should investigate
	rm "${lists_path}/$FORM_remove_repo_name" >&- 2>&-
	echo "<br />Repository source was removed: $FORM_remove_repo_name<br />"
}

! empty "$FORM_update_package_lists" && repo_update_needed=1

equal "$repo_update_needed" "1" && {
	echo "<br />Repository sources updated. Performing update of package lists ...<br /><pre>"	
	mkdir "$lists_path" >&- 2>&-
	opkg update
	echo "</pre>"
}

if [ "$FORM_action" = "install" ]; then
	echo "<pre>@TR<<system_ipkg_pleasewait#Please wait>> ...<br />"
	install_package `echo "$FORM_pkg" | sed -e 's, ,+,g'`
	echo "</pre>"
elif [ "$FORM_action" = "remove" ]; then
	echo "<pre>@TR<<system_ipkg_pleasewait#Please wait>> ...<br />"
	opkg remove `echo "$FORM_pkg" | sed -e 's, ,+,g'`
	echo "</pre>"
fi

repo_list=$(awk '/^[[:space:]]*src[\/gz]*[[:space:]]/ { print "<tr class=\"repositories\"><td><a href=\"./system-ipkg.sh?remove_repo_file=" FILENAME "&amp;remove_repo_name=" $2 "&amp;remove_repo_url=" $3 "\">@TR<<system_ipkg_removerepo#remove>></a>&nbsp;&nbsp;" $2 "</td><td colspan=\"2\">" $3 "</td></tr>"}' /etc/opkg.conf /etc/opkg/*)

display_form <<EOF
start_form|@TR<<system_ipkg_addrepo#Add Repository>>
field|@TR<<system_ipkg_reponame#Repo. Name>>
text|reponame|$FORM_reponame|
field|@TR<<system_ipkg_repourl#Repo. URL>>
text|repourl|$FORM_repourl|
field|&nbsp;
submit|install_repo|@TR<<system_ipkg_addrepo#Add Repository>>|
EOF
?>
</td></tr><tr><td colspan="2" class="repositories"><h4>@TR<<system_ipkg_currentrepos#Current Repositories>>:</h4></td></tr>
<?
echo "${repo_list}"
display_form <<EOF
helpitem|Add Repository
helptext|HelpText Add Repository#A repository is a server that contains a list of packages that can be installed on your OpenWrt device. Adding a new one allows you to list packages here that are not shown by default.
helpitem|Backports Tip
helptext|HelpText Backports Tip#For a much larger assortment of packages, see if there is a backports repository available for your firmware.
end_form
start_form|@TR<<system_ipkg_installfromurl#Install Package From URL>>
field|@TR<<system_ipkg_packageurl#URL of Package>>
text|pkgurl|$FORM_pkgurl
field|
submit|install_url|@TR<<system_ipkg_installfromurl#Install Package From URL>>|
helpitem|Install Package
helptext|HelpText Install Package#Normally one installs a package by clicking on the install link in the list of packages below. However, you can install a package not listed in the known repositories here.
end_form
EOF

# Block ends
##################################################################

display_form <<EOF
start_form|@TR<<system_ipkg_packagesavailable#Packages Available>>|||nohelp
field|&nbsp;
submit|update_package_lists|@TR<<system_ipkg_updatelists#Update package lists>>|
end_form
EOF
?>

<div class="settings">
<h3>@TR<<system_ipkg_installedpackages#Installed Packages>></h3>
<div class="packages">
<table>
<tr>
	<th width="150">@TR<<system_ipkg_th_action#Action>></th>
	<th width="200">@TR<<system_ipkg_th_package#Package>></th>
	<th width="150">@TR<<system_ipkg_th_version#Version>></th>
	<th>@TR<<system_ipkg_th_desc#Description>></th>
</tr>
<?
opkg list_installed |grep -e "Collected errors:" -e "has no architecture specified" -v | awk -F ' ' '
($2 !~ /terminated/) && ($1 !~ /Done./) {
	link=$1
	gsub(/\+/,"%2B",link)
	gsub(/^ */,"",link)
	gsub(/ *$/,"",link)
	version=$3
	desc=$5
	for (i=6; i <= NF; i++)
			desc = desc " " $i
	gsub(/&/, "\\&amp;", desc)
	gsub(/</, "\\&lt;", desc)
	gsub(/>/, "\\&gt;", desc)
	print "<tr class=\"packages\"><td><a href=\"javascript:confirmT('\''remove'\'','\''" link "'\'')\">@TR<<system_ipkg_Uninstall#Uninstall>></a></td><td>" $1 "</td><td>" version "</td><td>" desc "</td></tr>"
}
'
display_form <<EOF
end_form
EOF
?>
<div class="settings">
<h3>@TR<<system_ipkg_availablepackages#Available packages>></h3>
<div class="packages">
<table>
<tr>
	<th width="150">@TR<<system_ipkg_th_action#Action>></th>
	<th width="250">@TR<<system_ipkg_th_package#Package>></th>
	<th width="150">@TR<<system_ipkg_th_version#Version>></th>
	<th>@TR<<system_ipkg_th_desc#Description>></th>
</tr>
<?
repo_list=$(awk -v lists="$lists_path" '/^[[:space:]]*src[\/gz]*[[:space:]]/ { printf " " lists "/" $2 }' /etc/opkg.conf)
status_list=$(awk '/^[[:space:]]*dest[[:space:]]/ { if ($3 == "/") printf " /usr/lib/opkg/status"; else printf " "$3"/usr/lib/opkg/status" }' /etc/opkg.conf)
[ -z "$status_list" ] && status_list="/usr/lib/opkg/status"
egrep 'Package:|Description:|Version:' $status_list $repo_list 2>&- | sed -e 's, ,,' -e "s,^[^:]*${lists_path}/,," | awk -F: '
$1 ~ /status/ {
	installed[$3]++;
}
($1 !~ /terminated/) && ($1 !~ /\/status/) && (!installed[$3]) && ($2 !~ /Description/) && ($2 !~ /Version/) {
	if (current != $1) print "<tr><th>" $1 "</th></tr>"
	link=$3
	gsub(/\+/,"%2B",link)
	gsub(/^ */,"",link)
	gsub(/ *$/,"",link)
	getline verline
	split(verline,ver,":")
	getline descline
	split(descline,desc,":")
	gsub(/&/, "\\&amp;", desc[3])
	gsub(/</, "\\&lt;", desc[3])
	gsub(/>/, "\\&gt;", desc[3])
	print "<tr class=\"packages\"><td><a href=\"javascript:confirmT('\''install'\'','\''" link "'\'')\">@TR<<system_ipkg_Install#Install>></a></td><td>" $3 "</td><td>" ver[3] "</td><td>" desc[3] "</td></tr>"
	current=$1
}
'
display_form <<EOF
end_form
EOF

footer ?>
<!--
##WEBIF:name:System:300:Packages
-->
