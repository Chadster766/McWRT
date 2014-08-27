#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
header "Status" "Netstat" "@TR<<Netstat>>"
###################################################################
# TCP/IP status page
#
# Description:
#	Shows connections to the router, netstat stuff, routing table..
#
# Author(s) [in order of work date]:
#	Original webif developers
#	Jeremy Collake <jeremy.collake@gmail.com>
#
# Major revisions:
#
# NVRAM variables referenced:
#	todo
#
# Configuration files referenced:
#		none
#
?>
<table style="width: 90%; text-align: left;" border="0" cellpadding="2" cellspacing="2" align="center">
<tbody>

	<tr>
		<th><b>@TR<<Physical Connections|Ethernet/Wireless Physical Connections>></b></th>
	</tr>
	<tr>
		<td><pre><? cat /proc/net/arp ?></pre></td>
	</tr>

	<tr><td><br /><br /></td></tr>

	<tr>
		<th><b>@TR<<Routing Table|Routing Table>></b></th>
	</tr>
	<tr>
		<td><pre><? netstat -rn ?></pre></td>
	</tr>

	<tr><td><br /><br /></td></tr>

	<tr>
		<th><b>@TR<<Router Listening Ports|Router Listening Ports>></b></th>
	</tr>
	<tr>
		<td><pre><? netstat -ln 2>&- | awk '$0 ~ /^Active UNIX/ {ignore = 1}; ignore != 1 { print $0 }' ?></pre></td>
	</tr>

	<tr><td><br /><br /></td></tr>

	<tr>
		<th><b>@TR<<Router Connections|Connections to the Router>></b></th>
	</tr>
	<tr>
		<td><pre><? netstat -n 2>&- | awk '$0 ~ /^Active UNIX/ {ignore = 1}; ignore != 1 { print $0 }' ?></pre></td>
	</tr>
</tbody>
</table>

<br />
<?

footer ?>
<!--
##WEBIF:name:Status:400:Netstat
-->
