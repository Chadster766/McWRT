# $1 = type
# $2 = form variable name
# $3 = form variable value
# $4 = (radio button) value of button
# $5 = string to append
# $6 = additional attributes 

BEGIN {
	FS="|"
	select_open = 0
	optgroup_open = 0
}

# trim leading whitespaces 
{
	gsub(/^[ \t]+/,"",$1)
}

$1 ~ /^onchange/ {
	onchange = $2
}

$1 ~ /^onclick/ {
	onclick = $2
}

($1 != "") && ($1 !~ /^option/) && (optgroup_open == 1) {
	optgroup_open = 0
	print "</optgroup>"
}
($1 != "") && ($1 !~ /^(option|optgroup)/) && (select_open == 1) {
	select_open = 0
	print "</select>"
}
$1 ~ /^start_form/ {
	if ($3 != "") field_opts=" id=\"" $3 "\""
	else field_opts=""
	if ($4 == "hidden") field_opts = field_opts " style=\"display: none\""
	start_form($2, field_opts);
	form_help = ""
	form_help_link = ""
}
$1 ~ /^field/ {
	if (field_open == 1) print "</td></tr>"
	if ($3 != "") field_opts=" id=\"" $3 "\""
	else field_opts=""
	if ($4 == "hidden") field_opts = field_opts " style=\"display: none\""
	print "<tr" field_opts ">"
	if ($2 != "") print "<td width=\"40%\">" $2 "</td><td width=\"60%\">"
	else print "<td colspan=\"2\">"

	field_open=1
}
$1 ~ /^button/ {
	if (field_open == 1) print "</td></tr>"
	if ($3 != "") field_opts=" id=\"" $3 "\""
	else field_opts=""
	if ($4 == "hidden") field_opts = field_opts " style=\"display: none\""
	print "<tr" field_opts ">"
	if ($2 != "") print "<td width=\"40%\">" $2 "</td><td width=\"60%\">"
	else print "<td colspan=\"2\">"

	print "<input type=\"button\" name=\"" $2 "\" value=\"" $3 "\" onclick=\"" $4 "\"/>"
	print "</td></tr>"

	field_open=0
}
$1 ~ /^checkbox/ {
	if ($3==$4) opts="checked=\"checked\" "
	else opts=""
	if (onchange != "") opts = opts " onchange=\"" onchange "(this)\""
	if (onclick != "") opts = opts " onclick=\"" onclick "(this)\""
	print "<input id=\"" $2 "_" $4 "\" type=\"checkbox\" name=\"" $2 "\" value=\"" $4 "\" " opts " />"
}
$1 ~ /^radio/ {
	if ($3==$4) opts="checked=\"checked\" "
	else opts=""
	if (onchange != "") opts = opts " onchange=\"" onchange "(this)\""
	if (onclick != "") opts = opts " onclick=\"" onclick "(this)\""
	print "<input type=\"radio\" name=\"" $2 "\" value=\"" $4 "\" " opts " />"
}
$1 ~ /^select/ {
	opts = ""
	if (onchange != "") opts = opts " onchange=\"" onchange "(this)\""
	if (onclick != "") opts = opts " onclick=\"" onclick "(this)\""
	print "<select id=\"" $2 "\" name=\"" $2 "\"" opts ">"
	select_id = $2
	select_open = 1
	select_default = $3
}

($1 ~ /^optgroup/) && (select_open == 1) {
	print "<optgroup label=\"" $2 "\">"
	optgroup_open = 1
}

$1 ~ /^txtfile/ {
	if (field_open == 0) print "<tr><td>"
	field_open=1
        rows=4
	cols=60
        file=$3
	print "<small><textarea style=\"white-space: nowrap; overflow: auto;\" wrap=\"off\" rows=\"" rows "\" cols=\"" cols "\" id=\"" $2 "\" name=\"" $2 "\"" opts ">"
        system("cat " file)
	print "</textarea></small>"
}
($1 ~ /^option/) && (select_open == 1) {
	if ($2 == select_default) option_selected=" selected=\"selected\""
	else option_selected=""
	if ($3 != "") option_title = $3
	else option_title = $2
	print "<option " option_selected " value=\"" $2 "\">" option_title "</option>"
}
($1 ~ /^listedit/) {
	if (field_open == 1) print "</td></tr>"
	n = split($4 " ", items, " ")
	for (i = 1; i <= n; i++) {
		if (items[i] != "") print "<tr><td width=\"50%\">" items[i] "</td><td><a href=\"" $3 $2 "remove=" items[i] "\">@TR<<Remove>></a></td></tr>"
	}
	print "<tr><td width=\"100%\" colspan=\"2\"><input type=\"text\" name=\"" $2 "add\" value=\"" $5 "\" /><input type=\"submit\" name=\"" $2 "submit\" value=\"@TR<<Add>>\" /></td></tr>"
	field_open=0
}
$1 ~ /^caption/ { print "<b>" $2 "</b>" }
$1 ~ /^string/ { print $2 }
$1 ~ /^tip/ { 
	print "<tr><td colspan=\"3\"><div class=\"tip\">" $2 "</div></td></tr>"
}	
$1 ~ /^textarea/ {
	rows = ""
	if ($4 != "") rows = " rows=\"" $4 "\""
	cols = ""
	if ($5 != "") cols = " cols=\"" $5 "\""
	print "<textarea id=\"" $2 "\" name=\"" $2 "\"" rows cols ">"
	print $3
	print "</textarea>"
}
#####################################################
# progressbar|id|title|width_pixels|percent_complete|filled_caption|unfilled_caption
# 
($1 ~ /^progressbar/) {
	tID = " "
	if ( $2 != "" ) { tID = "id=\""$2"\"" } else { tID = "" }
	if ( $5 !~ /%/ ) { tWIDTH = $5 } else {
		zz = split($5,t,"%")
		tWIDTH = t[1]
	}
	print "<div class=\"progressbar\" " tID " style=\"width:" $4 "px\">"
	print "	<span class=\"progress\" style=\"width:" tWIDTH "%\">" $6 "</span>"
	if ( $7 != "" ) print " <span class=\"progress\" style=\"width:" 100-tWIDTH "%\">" $7 "</span>"
	print "</div>"
	#show caption
	if ($3 != "" ) print "<em>" $3 "</em>"
}
$1 ~ /^text$/ { 	
	cols = ""
	if ($5 != "") {
		cols = "<tr><td colspan=\"" $5 "\""
		cols_end= "</td></tr>"	
	}
	print cols "<input id=\"" $2 "\" type=\"text\" name=\"" $2 "\" value=\"" $3 "\" />" $4 cols_end
}
$1 ~ /^password/ { print "<input id=\"" $2 "\" type=\"password\" name=\"" $2 "\" value=\"" $3 "\" />" $4 }
$1 ~ /^upload/ { print "<input id=\"" $2 "\" type=\"file\" name=\"" $2 "\"/>" }
$1 ~ /^formtag_begin/ { print "<form name=\"" $2 "\" action=\"" $3 "\" enctype=\"multipart/form-data\" method=\"post\">" }
$1 ~ /^formtag_end/ { print "</form>" }
$1 ~ /^submit/ { print "<input type=\"submit\" name=\"" $2 "\" value=\"" $3 "\" />" }
$1 ~ /^helpitem/ { form_help = form_help "<h4>@TR<<" $2 ">>:</h4>" }
$1 ~ /^helptext/ { form_help = form_help "<p>@TR<<" $2 ">></p>" }
$1 ~ /^helplink/ { form_help_link = "<a class=\"more-help\" href=\"" $2 "\">@TR<<more...>></a>" }

($1 ~ /^checkbox/) || ($1 ~ /^radio/) {
	print $5
}

$1 ~ /^end_form/ {
	field_open = field_open
	form_help = form_help
	end_form(form_help, form_help_link, field_open);
	field_open = 0
	form_help = ""
	form_help_link = ""
}
