BEGIN {
	print "<form method=\"post\" action=\"" url "\" enctype=\"multipart/form-data\">"
	start_form("@TR<<notes_header#Notes>>")
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
	print button("save", "&nbsp;Save Changes&nbsp;") "&nbsp;" button("cancel", "&nbsp;Revert&nbsp;")
	print "<br /><div class=\"tip\">@TR<<notes_save_changes#The changes are permanent after you click the Save Changes button.>></div>"
	end_form("&nbsp;")
	print "</form>"
}
