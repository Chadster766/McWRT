#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
header "Status" "Asterisk" "@TR<<Asterisk Simple Managment>>"

if [ -e /var/run/asterisk.pid ]; then

ast_pid=$(cat /var/run/asterisk.pid)

ast_proc_info="$(cat /proc/$ast_pid/cmdline | tr '\0' ' ')"

asterisk_exec="$(echo $ast_proc_info | awk '{print $1}')"

if [ -z "$asterisk_exec" ]; then
	asterisk_exec="/usr/sbin/asterisk"
fi

ast_conf_file="$(echo $ast_proc_info | awk '{for (i=2; i<=NF; i++) if ($i == "-C") print $(i+1)}')"

if [ -e "$ast_conf_file" ]; then
	ast_conf_path="$(cat $ast_conf_file | grep '^astetcdir => ')"

	if [ -n "$ast_conf_path" ]; then
		ast_conf_path="$(echo $ast_conf_path | awk '{print $3}')"
	else
		ast_conf_path=/etc/asterisk
	fi
else
	ast_conf_path=/etc/asterisk
fi

echo '<center>'
echo '<a href="'$SCRIPT_NAME'">Version</a>'
echo '<a href="'$SCRIPT_NAME'?action=sip_peers">SIP/Peers</a>'
echo '<a href="'$SCRIPT_NAME'?action=sip_channels">SIP/Channels</a>'
echo '<a href="'$SCRIPT_NAME'?action=sip_registry">SIP/Registry</a>'
echo '<a href="'$SCRIPT_NAME'?action=iax_peers">IAX/Peers</a>'
echo '<a href="'$SCRIPT_NAME'?action=iax_channels">IAX/Channels</a>'
echo '<a href="'$SCRIPT_NAME'?action=iax_registry">IAX/Registry</a>'
echo '<a href="'$SCRIPT_NAME'?action=modules">Global/Modules</a>'
echo '<a href="'$SCRIPT_NAME'?action=cust_com">Custom/Command</a>'
echo '<a href="'$SCRIPT_NAME'?action=editor">.conf Editor</a>'
echo '<a href="'$SCRIPT_NAME'?action=reload">Reload</a>'
echo '<br /><br /><br />'
echo '<table><tr><td align=left border=0>'
echo '<pre>'
if [ "$FORM_action" = "reload" ]; then
	echo "<h3>Reloading...</h3>"
	$asterisk_exec -r -x 'reload'
elif [ "$FORM_action" = "sip_peers" ]; then
	echo "<h3>SIP/Peers</h3>"
	$asterisk_exec -r -x 'sip show peers'
elif [ "$FORM_action" = "sip_channels" ]; then
	echo "<h3>SIP/Channels</h3>"
	$asterisk_exec -r -x 'sip show channels'
elif [ "$FORM_action" = "sip_registry" ]; then
	echo "<h3>SIP/Registry</h3>"
	$asterisk_exec -r -x 'sip show registry'
elif [ "$FORM_action" = "iax_peers" ]; then
	echo "<h3>IAX/Peers</h3>"
	$asterisk_exec -r -x 'iax2 show peers'
elif [ "$FORM_action" = "iax_channels" ]; then
	echo "<h3>IAX/Channels</h3>"
	$asterisk_exec -r -x 'iax2 show channels'
elif [ "$FORM_action" = "iax_registry" ]; then
	echo "<h3>IAX/Registration</h3>"
	$asterisk_exec -r -x 'iax2 show registry'
elif [ "$FORM_action" = "modules" ]; then
	echo "<h3>Global/Modules</h3>"
	$asterisk_exec -r -x 'module show'
elif [ "$FORM_action" = "execute" ]; then
	echo "<h3>$FORM_exec_com</h3>"
	$asterisk_exec -r -x "$FORM_exec_com"
elif [ "$FORM_action" = "" ]; then
	$asterisk_exec -r -x 'core show version'
fi
echo '</pre>'
echo '</td></tr></table>'
echo '</center>'
if [ "$FORM_action" = "cust_com" ]; then
	echo '<form action="'$SCRIPT_NAME'" method=POST>'
	echo '<center>'
	echo '<INPUT type=text name="exec_com" value="'$FORM_exec_com'" size="25" maxlength="150">'
	echo '<br /><br /><INPUT type="submit" value="@TR<<Accept>>">'
	echo '<INPUT name="action" type="hidden" value="execute">'
	echo "<br /><br />Enter 'help' for commands details."
	echo '</center>'
	echo '</form>'
fi
if [ "$FORM_action" = "editor" ]; then
	echo '<center><form action="'$SCRIPT_NAME'" method=POST>'
	echo '<table style="width: 25%; text-align: left;" border="0" cellpadding="2" cellspacing="2" align="center">'
	ls $ast_conf_path/.  | awk -F' ' '
	{
		link=$1
		gsub(/\+/,"%2B",link)
		print "<tr><td><a href=\"'$SCRIPT_NAME'?action=edit&target=" link "\">@TR<<Edit>></td><td>" $1 "</td></tr></a>"
	}'
	echo '</table></form></center>'
fi
if [ "$FORM_conf" != "" ]; then
	echo "$FORM_conf" | tr -d '\r' > $ast_conf_path/$FORM_target
fi
if [ "$FORM_action" = "edit" ]; then
	conf_file="$( cat $ast_conf_path/$FORM_target )"
	echo '<form action="'$SCRIPT_NAME'" method=POST>'
	echo '<center>'
	echo '<TEXTAREA name="conf" rows="30" cols="100">'
	echo -n "$conf_file"
	echo '</TEXTAREA>'
	echo '<INPUT name="target" type="hidden" value="'$FORM_target'">'
	echo '<br /><INPUT type="submit" value="@TR<<Save Changes>>">'
	echo '</center>'
	echo '</form>'
fi
else
	if [ -x /usr/sbin/asterisk ]; then
		echo "@TR<<Asterisk is not running.>>"
	else
		echo "@TR<<Asterisk is not installed.>>"
		echo "@TR<<Install one of asterisk packages:>>"
		has_pkgs asterisk14 asterisk14-mini asterisk16
	fi
fi
footer ?>
<!--
##WEBIF:name:Status:901:Asterisk
-->
