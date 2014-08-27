#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
HOSTS_FILE=/etc/hosts
ETHERS_FILE=/tmp/dhcp.leases

config_cb() {
	local cfg_type="$1"
	local cfg_name="$2"

	case "$cfg_type" in
		host)
			append host_cfgs "$cfg_name" "$N"
		;;
	esac
}
uci_load dhcp
for cfgs in $host_cfgs; do
	config_get name "$cfgs" name
	config_get ip "$cfgs" ip
	config_get macaddr "$cfgs" mac
	cat $ETHERS_FILE |grep -q $macaddr
	[ "$?" != "0" ] && {
		append static_hosts "$name $ip $macaddr" "$N"
	}
done

header "Network" "WoL" "@TR<<Wake-On-LAN>>" ''

has_pkgs etherwake
has_pkgs wol
?>
<br />

<?
wokeup=""
empty "$FORM_wakecustom" || {
	validate <<EOF
mac|FORM_mac|@TR<<Hardware (MAC) address>>||$FORM_mac
EOF
	mac=$FORM_mac;
}
empty "$FORM_wake" || mac=$FORM_wake;
empty "$ERROR" && [ -n "$mac" ] && {
	if [ -n "$FORM_wolapp" ]; then
		i=${FORM_woliface##none}
		echo "<p>&nbsp;</p><p style=\"background:#ffffc0; color:#c00000; font-weight: bold;\">$FORM_wolapp${i:+ -i $i}: ";
		res=`$FORM_wolapp${i:+ -i $i} $mac 2>&1`;
		if [ -n "$res" ]; then echo "$res"; else echo "Waking up $mac..."; fi
		echo "</p><p>&nbsp;</p>";
	else
		echo "<p>&nbsp;</p><p  style=\"background:#ffffc0; color:#c00000; font-weight: bold;\">ERROR: No WOL application given! Please make sure you have installed either wol or etherwake, and you have selected one of them in the form below.</p><p>&nbsp;</p>";
	fi
}
empty $ERROR || { echo "<h3 class=Error>$ERROR</h3>"; }

?>

<form>
<table><tr><th>@TR<<WOL application>>:</th><td><select name="wolapp">
<?
	for i in ether-wake etherwake wol; do
		[ -n "`which $i`" ] && {
			echo -n "<option value=\"$i\"";
			[ "$i" = "$FORM_wolapp" ] && echo -n " SELECTED";
			echo ">$i</option>";
		}
	done
?>
</select></td></tr>
<tr><th>@TR<<WOL interface>>:</th><td><select name="woliface">
<?
	system_ifaces="$(ifconfig -a|grep Ethernet |cut -d" " -f1)"

	# set default to br-lan if existent
	if [ -z "$FORM_woliface" ]; then
		for i in $system_ifaces; do
			[ "$i" = "br-lan" ] && { FORM_woliface=$i; break; }
		done
	fi

	# list network interfaces as options
	echo -n "<option value=\"none\">default</option>";
	for i in $system_ifaces; do
		echo -n "<option value=\"$i\"";
		[ "$i" = "$FORM_woliface" ] && echo -n " SELECTED";
		echo ">$i</option>";
	done
?>
</select></td></tr></table>
<table border=1>
<tr><th>Machine</th><th>@TR<<IP Address>></th><th>@TR<<MAC Address>></th><th></th></tr>
<tr><td></td><td></td><td><input type="text" name="mac" value=<?
if [ -n "$FORM_mac" ]; then echo "\"$FORM_mac\""; else echo "\"00:00:00:00:00:00\""; fi ?>
></td><td><button name="wakecustom" type="submit" value="wakecustom">Wake up</button></td></tr>
<?

name_for_ip() {
	grep -e "^[\t ]*$1[\t ]+.*" $HOSTS_FILE | sed 's/^[\t ]*'$1'[\t ]*//'
}

if [ -e "$ETHERS_FILE" ]; then
	cat $ETHERS_FILE | awk '
	{
		if ($2 ~ /^[[:xdigit:]]{2,2}:[[:xdigit:]]{2,2}:[[:xdigit:]]{2,2}:[[:xdigit:]]{2,2}:[[:xdigit:]]{2,2}:[[:xdigit:]]{2,2}/) {
			print "<tr><td>" $4 "</td><td>" $3 "</td><td>" $2 "</td><td><button name=\"wake\" type=\"submit\" value=\"" $2 "\">Wake up</button></td></tr>"
		}
	}'
fi
if [ -n "$static_hosts" ]; then
	echo "$static_hosts" | awk '
	{
		if ($3 ~ /^[[:xdigit:]]{2,2}:[[:xdigit:]]{2,2}:[[:xdigit:]]{2,2}:[[:xdigit:]]{2,2}:[[:xdigit:]]{2,2}:[[:xdigit:]]{2,2}/) {
			print "<tr><td>" $1 "</td><td>" $2 "</td><td>" $3 "</td><td><button name=\"wake\" type=\"submit\" value=\"" $3 "\">Wake up</button></td></tr>"
		}
	}'
fi

?>
</table>
</form>
<br /><br />
<div class="tip">@TR<<wol_help#Here you can send a Wake-On-LAN packet to automatically boot up a computer that is turned off. The computer must support WOL, and the feature needs to be turned on in the BIOS for this to work. Unfortunately, there is no explicit response from that machine, so you do not know whether the waking was successful and the machine is really booting up.>></div>

<? footer ?>
<!--
##WEBIF:name:Network:699:WoL
-->

