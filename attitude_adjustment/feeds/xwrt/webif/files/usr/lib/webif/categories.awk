$1 == "-"		{ print "<li class=\"separator\">&nbsp;</li>"; next; }
$1 ~ "^" selected "$"	{ print "<li class=\"selected\"><a href=\"" $2 "?cat=" $1 "\">@TR<<" $1 ">></a></li>"; next; }
			{ print "<li><a href=\"" $2 "?cat=" $1 "\">@TR<<" $1 ">></a></li>"; }
