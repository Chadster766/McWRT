#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
###################################################################
# Status DHCP Leases
#
#
# The page awaits validation in kamikaze.
#
# Description:
#	Shows DHCP leases, arp cache, /etc/ethers
#
# Author(s) [in order of work date]:
#	Jeremy Collake <jeremy.collake@gmail.com>
#	Markus Wigge <markus@freewrt.org>
#	Tim Yardley <tyardley@users.berlios.de>
#	Travis Kemen <kemen04@gmail.com>
#	Lubos Stanek  <lubek@users.berlios.de>
#
# Major revisions:
#
# Configuration files referenced:
#  /etc/config/network
#  /etc/config/dhcp
#  /etc/ethers
#
# Kernel proc interface:
#  /proc/net/arp
#

config_cb() {
	cfg_type="$1"
	cfg_name="$2"

	case "$cfg_type" in
		host)
			hosts_sections="${hosts_sections} ${cfg_name}"
		;;
	esac
}
option_cb() {
	local var_name="$1"; shift
	local var_value="$*"

	case "$cfg_type" in
		dnsmasq)
			case "$var_name" in
				leasefile) leasefile="$var_value" ;;
				readethers) includeethers="$var_value" ;;
			esac
		;;
	esac
}
uci_load dhcp

header "Status" "DHCP Clients" "@TR<<status_leases_dhcp_leases#DHCP Leases>>"
?>
<table style="width: 90%; text-align: left;" border="0" cellpadding="2" cellspacing="2" align="center">
<tbody>
	<tr>
		<th>@TR<<status_leases_MAC#MAC Address>></th>
		<th>@TR<<status_leases_IP#IP Address>></th>
		<th>@TR<<status_leases_Name#Name>></th>
		<th>@TR<<status_leases_Expires#Expires in>></th>
	</tr>
<?
exists "$leasefile" && awk -vdate="$(date +%s)" '
$1 > 0 {
	print "	<tr>"
	print "		<td>" $2 "</td>"
	print "		<td>" $3 "</td>"
	print "		<td>" $4 "</td>"
	print "		<td>"
	t = $1 - date
	h = int(t / 60 / 60)
	if (h > 0) printf h "@TR<<status_leases_h#h>> "
	m = int(t / 60 % 60)
	if (m > 0) printf m "@TR<<status_leases_min#min>> "
	s = int(t % 60)
	printf s "@TR<<status_leases_sec#sec>> "
	printf "	</td>"
	print "	</tr>"
}
' "$leasefile"
exists "$leasefile" && grep -q "." "$leasefile" > /dev/null
! equal "$?" "0" && {
	echo "	<tr>"
	echo "		<td>@TR<<status_leases_no_leases#There are no known DHCP leases.>></td>"
	echo "	</tr>"
}
?>
</tbody>
</table>

<br />

<table width="100%">
<tbody>
	<tr>
		<td><font size="-1"><strong>@TR<<status_leases_dhcp_leases#DHCP Leases>>:</strong>&nbsp; @TR<<status_leases_dhcp_leases_helptext#DHCP leases are assigned to network clients that request an IP address from the DHCP server of the router. Clients that requested their IP lease before this router was last rebooted may not be listed until they request a renewal of their lease.>></font></td>
	</tr>
</tbody>
</table>

<br />

<h2>@TR<<status_leases_additional#Additional information>></h2>

<h5><strong>@TR<<status_leases_arp_title#Address Resolution Protocol Cache (ARP)>></strong></h5>
<table style="width: 90%; margin-left: 2.5em; text-align: left; font-size: 0.8em;" border="0" cellpadding="3" cellspacing="2">
<tbody>
	<tr>
		<th>@TR<<status_leases_MAC#MAC Address>></th>
		<th>@TR<<status_leases_IP#IP Address>></th>
		<th>@TR<<status_leases_HW#HW Type>></th>
		<th>@TR<<status_leases_Flags#Flags>></th>
		<th>@TR<<status_leases_Mask#Mask>></th>
	</tr>
<?
uci_load "network"
excludeiface="$CONFIG_wan_ifname"
cat /proc/net/arp 2>/dev/null | awk -v exiface="$excludeiface" '
BEGIN {
	cntr=0
}
$1 ~ /^[[:digit:]]{1,3}\.[[:digit:]]{1,3}\.[[:digit:]]{1,3}\.[[:digit:]]{1,3}$/ && $4 ~ /^[[:xdigit:]]{2,2}:[[:xdigit:]]{2,2}:[[:xdigit:]]{2,2}:[[:xdigit:]]{2,2}:[[:xdigit:]]{2,2}:[[:xdigit:]]{2,2}$/ && $6 != exiface {
	print "		<tr>"
	print "		<td>" $4 "</td>"
	print "		<td>" $1 "</td>"
	if ($2 == "0x0") flags="NETROM"
	else if ($2 == "0x1") hwtype="ETHER"
	else if ($2 == "0x2") hwtype="EETHER"
	else if ($2 == "0x3") hwtype="AX25"
	else if ($2 == "0x4") hwtype="PRONET"
	else if ($2 == "0x5") hwtype="CHAOS"
	else if ($2 == "0x6") hwtype="IEEE802"
	else if ($2 == "0x7") hwtype="ARCNET"
	else if ($2 == "0x8") hwtype="APPLETLK"
	else if ($2 == "0xF") hwtype="DLCI"
	else if ($2 == "0x13") hwtype="ATM"
	else if ($2 == "0x17") hwtype="METRICOM"
	else if ($2 == "0x18") hwtype="IEEE1394"
	else if ($2 == "0x1B") hwtype="EUI64"
	else if ($2 == "0x20") hwtype="INFINIBAND"
	else hwtype=$2
	print "		<td>" hwtype "</td>"
	if ($3 == "0x2") flags="C (@TR<<status_leases_completed#completed>>)"
	else if ($3 == "0x4") flags="M (@TR<<status_leases_permanent#permanent>>)"
	else if ($3 == "0x8") flags="P (@TR<<status_leases_published#published>>)"
	else flags=$3
	print "		<td>" flags "</td>"
	print "		<td>" $5 "</td>"
	print "	</tr>"
	cntr++
}
END {
	if (cntr == 0) {
		print "	<tr>"
		print "		<td>@TR<<status_leases_no_arp_record#ARP Cache does not contain any correspondent record.>></td>"
		print "	</tr>"
	}
}'
?>
</tbody>
</table>

<br />

<?
equal "$includeethers" "1" && {
cat <<EOF
<h5><strong>@TR<<status_leases_ethers_title#Ethernet Address to IP Number Database (/etc/ethers)>></strong></h5>
<table style="width: 90%; margin-left: 2.5em; text-align: left; font-size: 0.8em;" border="0" cellpadding="3" cellspacing="2">
<tbody>
	<tr>
		<th>@TR<<status_leases_MAC#MAC Address>></th>
		<th>@TR<<status_leases_IP#IP Address>></th>
	</tr>
EOF
exists /etc/ethers && awk '
BEGIN {
	cntr=0
}
(($1 ~ /^[[:xdigit:]]{2,2}:[[:xdigit:]]{2,2}:[[:xdigit:]]{2,2}:[[:xdigit:]]{2,2}:[[:xdigit:]]{2,2}:[[:xdigit:]]{2,2}$/) && ($2 ~ /^[[:digit:]]{1,3}\.[[:digit:]]{1,3}\.[[:digit:]]{1,3}\.[[:digit:]]{1,3}$/)) {
	print "	<tr>"
	print "		<td>" $1 "</td>"
	print "		<td>" $2 "</td>"
	print "	</tr>"
	cntr++
}
END {
	if (cntr == 0) {
		print "	<tr>"
		print "		<td>@TR<<status_leases_ethers_empty#File /etc/ethers does not contain any Ethernet address/IP address pair.>></td>"
		print "	</tr>"
	}
}' /etc/ethers
! exists /etc/ethers && {
	echo "	<tr>"
	echo "		<td>@TR<<status_leases_no_ethers#File /etc/ethers does not exist.>></td>"
	echo "	</tr>"
}
cat <<EOF
</tbody>
</table>

EOF
} # equal "$includeethers" "1"
?>

<br />

<h5><strong>@TR<<status_leases_hosts_title#Hosts IP to Hostname File Map (/etc/hosts)>></strong></h5>
<table style="width: 90%; margin-left: 2.5em; text-align: left; font-size: 0.8em;" border="0" cellpadding="3" cellspacing="2">
<tbody>
	<tr>
		<th>@TR<<status_leases_IP#IP Address>></th>
		<th>@TR<<status_leases_Hostname#Hostname>></th>
	</tr>
<?
exists /etc/hosts && cat /etc/hosts 2>/dev/null | awk '
BEGIN {
	cntr=0
}
($1 ~ /^[[:digit:]]{1,3}\.[[:digit:]]{1,3}\.[[:digit:]]{1,3}\.[[:digit:]]{1,3}$/) {
	print "	<tr>"
	print "		<td>" $1 "</td>"
	print "		<td>" $2 "</td>"
	print "	</tr>"
	cntr++
}
END {
	if (cntr == 0) {
		print "	<tr>"
		print "		<td>@TR<<status_leases_hosts_empty#File /etc/hostss does not contain any IP address/Hostname pair.>></td>"
		print "	</tr>"
	}
}'
! exists /etc/hosts && {
	cat <<EOF
	<tr>
		<td>@TR<<status_leases_no_hosts#File /etc/hosts does not exist.>></td>
	</tr>
EOF
}
cat <<EOF
</tbody>
</table>

<br />

EOF

! empty "${hosts_sections}" && {
	cat <<EOF
<h5><strong>@TR<<status_leases_dhcphosts_title#DHCP Static IP Addresses Map>></strong></h5>
<table style="width: 90%; margin-left: 2.5em; text-align: left; font-size: 0.8em;" border="0" cellpadding="3" cellspacing="2">
<tbody>
	<tr>
		<th>@TR<<status_leases_MAC#MAC Address>></th>
		<th>@TR<<status_leases_IP#IP Address>></th>
		<th>@TR<<status_leases_Hostname#Hostname>></th>
	</tr>
EOF
	for hsec in ${hosts_sections}; do
		echo "	<tr>"
		for nm in mac ip name; do
			eval "value=\"\${CONFIG_${hsec}_${nm}}\""
			echo "	<td>${value:-&nbsp;}</td>"
		done
		echo "	</tr>"
	done
	cat <<EOF
</tbody>
</table>

EOF
}

footer ?>
<!--
##WEBIF:name:Status:200:DHCP Clients
-->
