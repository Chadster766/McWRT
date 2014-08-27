# parameters: 1
function config_load(package, var) {
	while (("/bin/ash -c '. /etc/functions.sh; unset NO_EXPORT; config_load \""package"\"; env | grep \"^CONFIG_\"'" | getline) == 1) {
		sub("^CONFIG_", "")
		if (match($0, "=") == 0) {
			if (var != "") CONFIG[var] = CONFIG[var] "\n" $0
			next
		}
		var=substr($0, 1, RSTART-1)
		CONFIG[var] = substr($0, RSTART+1, length($0) - RSTART)
	}
}

# parameters: 1
function config_load_state(package, var) {
	while (("/bin/ash -c '. /etc/functions.sh; unset NO_EXPORT; . \"/var/state/"package"\" 2>/dev/null; env | grep \"^CONFIG_\"'" | getline) == 1) {
		sub("^CONFIG_", "")
		if (match($0, "=") == 0) {
			if (var != "") CONFIG[var] = CONFIG[var] "\n" $0
			next
		}
		var=substr($0, 1, RSTART-1)
		CONFIG[var] = substr($0, RSTART+1, length($0) - RSTART)
	}
}

# parameters: 2
function config_get(package, option) {
	return CONFIG[package "_" option]
}

# parameters: 3
function config_get_bool(package, option, default, var) {
	var = config_get(package, option);
	if ((var == "1") || (var == "on") || (var == "true") || (var == "enabled")) return 1
	if ((var == "0") || (var == "off") || (var == "false") || (var == "disabled")) return 0
	return (default == "1" ? 1 : 0)
}

# parameters: 1
function uci_load(package, var) {
	while (("/bin/ash -c '. /etc/functions.sh; . /lib/config/uci.sh; unset NO_EXPORT; uci_load \""package"\"; env | grep \"^CONFIG_\"'" | getline) == 1) {
		sub("^CONFIG_", "")
		if (match($0, "=") == 0) {
			if (var != "") CONFIG[var] = CONFIG[var] "\n" $0
			next
		}
		var=substr($0, 1, RSTART-1)
		CONFIG[var] = substr($0, RSTART+1, length($0) - RSTART)
	}
}

# parameters: 4
function uci_set(package, config, option, value) {
	system("/bin/ash -c '. /etc/functions.sh; . /lib/config/uci.sh; uci_set \""package"\" \""config"\" \""option"\" \""value"\"'")
}

# WARNING: this function is a test and it may not be supported later!
# parameters: 2
# the second parameter is a two-dimensional array confoptval[config, option]
function uci_set_ar(package, confoptval, \
			cmd, var, nr, confopt) {
	cmd = "/bin/ash -c '\$0'"
	for (var in confoptval) {
		nr = split(var, confopt, SUBSEP)
		if (nr == 2)
			print ". /etc/functions.sh; . /lib/config/uci.sh; uci_set \""package"\" \""confopt[1]"\" \""confopt[2]"\" \""confoptval[var]"\"" | cmd
	}
	close(cmd)
}

# parameters: 3
function uci_add(package, type, config) {
	system("/bin/ash -c '. /etc/functions.sh; . /lib/config/uci.sh; uci_add \""package"\" \""type"\" \""config"\"'")
}

# parameters: 3
function uci_rename(package, config, value) {
	system("/bin/ash -c '. /etc/functions.sh; . /lib/config/uci.sh; uci_rename \""package"\" \""config"\" \""value"\"'")
}

# parameters: 3
function uci_remove(package, config, option) {
	system("/bin/ash -c '. /etc/functions.sh; . /lib/config/uci.sh; uci_remove \""package"\" \""config"\" \""option"\"'")
}

# parameters: 1
function uci_commit(package) {
	system("/bin/ash -c '. /etc/functions.sh; . /lib/config/uci.sh; uci_commit \""package"\"'")
}

# parameters: 1
function indent_level(level, i) {
	if (level > 0) {
		for (i = 1; i <= level; i++) printf "\t"
	}
}

function start_form(title, field_opts, field_opts2) {
	print "<div class=\"settings\"" field_opts ">"
	if (title != "") print "<h3><strong>" title "</strong></h3>"
	print "<div class=\"settings-content\"" field_opts2 ">"
	print "<table width=\"100%\" summary=\"Settings\">"
	field_open = 0
}

function end_form(form_help, form_help_link, field_open) {
	if (field_open == 1) print "</td></tr>"
	field_open = 0
	print "</table>"
	print "</div>"
	if (form_help != "" || form_help_link != "") {
		print "<blockquote class=\"settings-help\">"
		print "<h3><strong>@TR<<Short help>>:</strong></h3>"
		print form_help form_help_link
		print "</blockquote>"
	}
	print "<div class=\"clearfix\">&nbsp;</div></div>"
}

function textinput(name, value) {
	return "<input type=\"text\" name=\"" name "\" value=\"" value "\" />"
}

function textinput2(name, value, width) {
        return "<input type=\"text\" name=\"" name "\" value=\"" value "\" style=\"width:" width "em;\" />"
}
function textinput3(name, value) {
	print "<input type=\"text\" name=\"" name "\" value=\"" value "\" />"
}
function password(name, value, hidden) {
	print "<input id=\"" name "\" type=\"password\" name=\"" name "\" value=\"" value "\" />" hidden
}
function hidden(name, value) {
	return "<input type=\"hidden\" name=\"" name "\" value=\"" value "\" />"
}

function button(name, caption) {
	return "<input type=\"submit\" name=\"" name "\" value=\"@TR<<" caption ">>\" />"
}

function helpitem(name) {
	return "<h4>@TR<<" name ">>:</h4>"
}

function helptext(text) {
	return "<p>@TR<<" text ">></p>"
}

function sel_option(name, caption, default, sel) {
	if (default == name) sel = " selected=\"selected\""
	else sel = ""
	return "<option value=\"" name "\"" sel ">@TR<<" caption ">></option>"
}

function field(name, value, hidden) {
	if (field_open == 1) print "</td></tr>"
	if (value != "") field_opts=" id=\"" value "\""
	else field_opts=""
	if (hidden == "hidden") field_opts = field_opts " style=\"display: none\""
	print "<tr" field_opts ">"
	if (name != "") print "<td width=\"40%\">" name "</td><td width=\"60%\">"
	else print "<td colspan=\"2\">"

	field_open=1
}

function submit(name, value) {
	print "<input type=\"submit\" name=\"" name "\" value=\"" value "\" />"
}


function select(name, value) {
	opts = ""
	opts = opts " onchange=\"" onchange "(this)\""
	print "<select id=\"" name "\" name=\"" name "\"" opts ">"
	select_id = name
	select_open = 1
	select_default = value
}

function option(name, value) {
	if (name == select_default) option_selected=" selected=\"selected\""
	else option_selected=""
	if (value != "") option_title = value
	else option_title = name
	print "<option " option_selected " value=\"" name "\">" option_title "</option>"
}

