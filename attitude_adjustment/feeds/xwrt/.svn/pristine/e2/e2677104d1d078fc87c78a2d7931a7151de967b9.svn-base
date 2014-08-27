BEGIN {
	print "<form method=\"post\" action=\"" url "\" enctype=\"multipart/form-data\">"
	print "<div class=\"settings\">"
	print "<h3><strong>@TR<<editor_File#File>>: " path "/" file "</strong></h3>"
	print "<table cellspacing=\"0\" summary=\"@TR<<browser_Filesystem_Browser#Filesystem Browser>>\">"
	print "<tbody>"
	print "<tr>"
	print "<td>"
	print hidden("path", path)
	print hidden("edit", file)
	printf "<textarea name=\"filecontent\" cols=\"80\" rows=\"20\">"
}

{
	gsub("&", "\\&amp;", $0)
	gsub("<", "\\&lt;", $0)
	gsub(">", "\\&gt;", $0)
	print $0
}

END {
	print "</textarea><br />"
	print button("save", "&nbsp;Save Changes&nbsp;") "&nbsp;" button("cancel", "&nbsp;Back&nbsp;")
	print "<br /><div class=\"tip\">@TR<<editor_click_apply#After you are done changing files and making other configuration adjustments, you must click the Apply link to make the changes permanent.>></div>"
	print "</td>"
	print "</tr>"
	print "</tbody>"
	print "</table>"
	print "</div>"
	print "</form>"
}
