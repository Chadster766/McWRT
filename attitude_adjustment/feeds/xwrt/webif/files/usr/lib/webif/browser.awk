# WARNING! the input list must be sorted with directories first
BEGIN {
	gsub(/^\/\//, "/", path)
	print "<div class=\"settings\">"
	print "<h3><strong>@TR<<browser_Filesystem_Browser#Filesystem Browser>>: <a href=\"" url "?path=" path "\">" path "</a></strong></h3>"
	print "<div id=\"filebrowser\">"
	print "<table cellspacing=\"0\" summary=\"@TR<<browser_Filesystem_Browser#Filesystem Browser>>\">"
	print "<tbody>"
	print "<tr>"
	print "<td class=\"leftimage\"><a href=\"" url "?path=/\"><img src=\"/images/dir.gif\" alt=\"\" /></a></td>"
	print "<td><a href=\"" url "?path=/\">@TR<<browser_Root#Root>></a></td>"
	print "<td>&nbsp;</td><td>&nbsp;</td><td>&nbsp;</td>"
	print "</tr>"
	odd=0
	# do not let the user to make wrong things (hm, at least some of them)
	xdirectory="^(\/bin$|\/bin\/$|\/dev$|\/dev\/.*$|\/etc$|\/jffs$|\/lib$|\/lib\/modules$|\/lib\/modules\/.*$|\/mnt$|\/proc$|\/proc\/.*$|\/rom$|\/rom\/.*$|\/sbin$|\/sys$|\/sys\/.*$|\/usr$|\/usr\/bin$|\/usr\/lib$|\/usr\/lib\/webif$|\/usr\/sbin$|\/usr\/share$|\/www$|\/www\/.*$|\/www\/themes\/.*$|\/www\/cgi-bin\/webif$)"
	xdownload="^(\/dev$|\/dev\/.*$|\/proc$|\/proc\/.*$|\/sys$|\/sys\/.*$)"
	xedit="^(\/dev$|\/dev\/.*$|\/lib$|\/lib\/modules\/.*$|\/proc$|\/proc\/.*$|\/rom$|\/rom\/.*$|\/sys$|\/sys\/.*$)"
	xdelete="^(\/dev$|\/dev\/.*$|\/lib$|\/lib\/modules\/.*$|\/proc$|\/proc\/.*$|\/rom$|\/rom\/.*$|\/sys$|\/sys\/.*$)"
}

{
	type = substr($1, 1, 1);
	fname = $11
	for (i = 12; i <= NF; i++)
		fname = fname " " $i
	line = ""
	fullpath = path "/" fname
	gsub(/^\/\//, "/", fullpath)
}

(fname != ".") && (fname != "") {
	if (odd == 1) {
		print "<tr>"
		odd--
	} else {
		print "<tr class=\"odd\">"
		odd++
	}
	finfo = "@TR<<browser_Permissions#Permissions>>: " substr($1,2) "<br />@TR<<browser_Owner#Owner>>: " $3 "<br />@TR<<browser_Group#Group>>: " $4 "<br />@TR<<browser_Time#Time>>: " $6
	for (i=7; i<=10; i++) finfo = finfo " " $i
}

type == "d" {
	if (fname == "..") {
		fullpath = path "/.."
		gsub(/^\/\//, "/", fullpath)
		print "<td class=\"leftimage\"><a href=\"" url "?path=" fullpath "\"><img src=\"/images/dir.gif\" alt=\"@TR<<browser_Parent_Directory#Parent Directory>>\" /></a></td>"
		print "<td><a href=\"" url "?path=" fullpath "\">@TR<<browser_Parent_Directory#Parent Directory>></a></td>"
		print "<td>&nbsp;</td>"
		print "<td>&nbsp;</td>"
		print "<td>&nbsp;</td>"
	} else if (fname != ".") {
		print "<td class=\"leftimage\"><a class=\"tooltip\" href=\"" url "?path=" fullpath "\"><img src=\"/images/dir.gif\" alt=\"@TR<<browser_Directory#Directory>>\" /><span class=\"tooltip\">" finfo "</span></a></td>"
		print "<td><a href=\"" url "?path=" fullpath "\">" fname "</a></td>"
		print "<td>&nbsp;</td>"
		print "<td>&nbsp;</td>"
		if (fullpath ~ xdirectory) {
			print "<td class=\"rightimage\"><img src=\"/images/action_x_no.gif\" alt=\"@TR<<browser_Delete#Delete>>\" /></td>"
		} else {
			print "<td class=\"rightimage\"><a href=\"javascript:confirm_deldir('" path  "','" fullpath "')\"><img src=\"/images/action_x.gif\" alt=\"@TR<<browser_Delete#Delete>>\" /></a></td>"
		}
	}
}

type == "-" {
	print "<td class=\"leftimage\"><a class=\"tooltip\" href=\"/cgi-bin/webif/download.sh?script=" url "&amp;path=" path "&amp;savefile=" fname "\"><img src=\"/images/file.gif\" alt=\"@TR<<browser_File#File>>\" /><span class=\"tooltip\">" finfo "</span></a></td>"
	if (path ~ xdownload) {
		print "<td>" fname "</td>"
	} else {
		print "<td><a href=\"/cgi-bin/webif/download.sh?script=" url "&amp;path=" path "&amp;savefile=" fname "\">" fname "</a></td>"
	}
	printf "<td class=\"number\">"
	if ($5 >= 2 ** 30) {
		printf "%.1f GB", $5 / (2 ** 30)
	} else if ($5 >= 2 ** 20) {
		printf "%.1f MB", $5 / (2 ** 20)
	} else if ($5 >= 2 ** 15) {
		printf "%.1f kB", $5 / (2 ** 10)
	} else {
		printf "%d", $5
	}
	print "</td>"
	if ((path ~ xedit) || $5 >= 2 ** 16) {
		print "<td class=\"image\"><img src=\"/images/action_edit_no.gif\" alt=\"@TR<<browser_Edit#Edit>>\" /></td>"
	} else {
		print "<td class=\"image\"><a href=\"" url "?path=" path "&amp;edit=" fname "\"><img src=\"/images/action_edit.gif\" alt=\"@TR<<browser_Edit#Edit>>\" /></a></td>"
	}
	if (path ~ xdelete) {
		print "<td class=\"rightimage\"><img src=\"/images/action_x_no.gif\" alt=\"@TR<<browser_Delete#Delete>>\" /></td>"
	} else {
		print "<td class=\"rightimage\"><a href=\"javascript:confirm_delfile('" path  "','" fname "')\"><img src=\"/images/action_x.gif\" alt=\"@TR<<browser_Delete#Delete>>\" /></a></td>"
	}
}

(fname != ".") && (fname != "") {
	print "</tr>"
}

END {
	print "</tbody>"
	print "</table>"
	print "</div></div>"
}
