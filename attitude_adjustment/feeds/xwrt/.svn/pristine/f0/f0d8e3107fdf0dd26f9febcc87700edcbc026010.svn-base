#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh

timeout=60
if empty "$FORM_reboot"; then
	reboot_msg="<form method=\"post\" action=\"$SCRIPT_NAME\"><input type=\"submit\" value=\" @TR<<Yes, really reboot now>> \" name=\"reboot\" /></form>"
else
	uci_load "network"
	router_ip="$CONFIG_lan_ipaddr"
	[ -n "$SERVER_PORT" ] && [ "$SERVER_PORT" != "80" ] && router_ip="$router_ip:$SERVER_PORT"
	header_inject_head="<meta http-equiv=\"refresh\" content=\"$timeout;http://$router_ip\" />"
	reboot_msg="@TR<<Rebooting now>>...
<br/><br/>
@TR<<reboot_wait#Please wait about>> $timeout @TR<<reboot_seconds#seconds.>> @TR<<reboot_reload#The webif&sup2; should automatically reload.>>
<br/><br/>
<center>
<script type=\"text/javascript\">
<!--
var bar1=createBar(350,15,'white',1,'black','blue',85,7,3,'');
-->
</script>
</center>"
fi

header "System" "Reboot" ""
?>
<br/><br/><br/>
<table width="90%" border="0" cellpadding="2" cellspacing="2" align="center">
<tr>
	<td><script type="text/javascript" src="/js/progress.js"></script><? echo -n "$reboot_msg" ?><br/><br/><br/></td>
</tr>
</table>
<? footer ?>
<?
! empty "$FORM_reboot" && {
	reboot &
	exit
}
?>
<!--
##WEBIF:name:System:910:Reboot
-->
