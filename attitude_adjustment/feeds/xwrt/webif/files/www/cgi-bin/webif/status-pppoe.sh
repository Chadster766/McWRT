#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
header "Status" "PPPoE" "@TR<<PPPoE Status>>"
?>
<?
if [ "$FORM_action" = "reconnect" ]; then
	ifdown wan && ifup wan
elif [ "$FORM_action" = "disconnect" ]; then
	ifdown wan
elif [ "$FORM_action" = "connect" ]; then
	ifup wan
fi
?>
<table style="width: 90%; text-align: left;" border="0" cellpadding="2" cellspacing="2" align="center">
<tbody>
	<tr>
		<th><a href="status-pppoe.sh?action=reconnect" style="color: red">Reconnect</a></th>
	</tr>
	</tr>
		<tr><td><br /></td></tr>
	<tr>
	<tr>
		<th><b>Manual Control:</b></th>
	</tr>
	<tr>
		<th><a href="status-pppoe.sh?action=disconnect" style="color: green">Disconnect</a></th>
	</tr>
	<tr>
		<th><a href="status-pppoe.sh?action=connect" style="color: green">Connect</a></th>
	</tr>
		<tr><td><br /><br /></td></tr>
	<tr>
		<th><b>Ip Addr:</b></th>
	</tr>
	<tr>
		<td><pre><? ifconfig |grep ppp0 >>/dev/null
			if [ $? = 0 ]; then
			/sbin/ifconfig ppp0 | grep inet | awk '{print $2}'| awk -F : '{print $2}'
			fi  ?></pre></td>
	</tr>
		<tr><td><br /><br /></td></tr>
	<tr>
		<th><b>Ifconfig ppp0</b></th>
	</tr>
	<tr>
		<td><pre><? ifconfig |grep ppp0 >>/dev/null
			if [ $? = 0 ]; then
			ifconfig ppp0 
			fi ?></pre></td>
	</tr>
		<tr><td><br /><br /></td></tr>
	<tr>
		<th><b>Syslog: pppd (Last 500 lines)</b></th>
	</tr>
	<tr>
		<td><pre><? logread | grep pppd |tail -n 500 -|sort -r | sed ' s/\&/\&amp;/g; s/</\&lt;/g; s/>/\&gt;/g' ?></pre></td>
	</tr>
</tbody>
</table>
<? footer ?>
<!--
##WEBIF:name:Status:500:PPPoE
-->
