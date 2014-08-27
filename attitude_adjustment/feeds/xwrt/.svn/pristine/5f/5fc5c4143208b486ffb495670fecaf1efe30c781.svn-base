#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
###################################################################
# DHCP configuration
#
# Description:
#	DHCP configuration.
#
# Author(s) [in order of work date]:
#	Travis Kemen	<thepeople@users.berlios.de>
#	Adam Hill	<adam@voip-x.co.uk>
# Major revisions:
#	Allow DHCP options to be specified ( Adam H )
#
# UCI variables referenced:
#
# Configuration files referenced:
#   dhcp, network
#

header "Network" "DHCP" "@TR<<DHCP Configuration>>" 'onload="modechange()"' "$SCRIPT_NAME"

###################################################################
# Parse Settings, this function is called when doing a config_load
config_cb() {
	local cfg_type="$1"
	local cfg_name="$2"

	case "$cfg_type" in
		interface)
			if [ "$cfg_name" != "loopback" ]; then
				append networks "$cfg_name" "$N"
			fi
		;;
		dhcp)
			append dhcp_cfgs "$cfg_name" "$N"
		;;
		dnsmasq)
			append dnsmasq_cfgs "$cfg_name" "$N"
		;;
		host)
			append host_cfgs "$cfg_name" "$N"
		;;
	esac
}
uci_load network
uci_load dhcp
# create dnsmasq's section when missing
[ -z "$dnsmasq_cfgs" ] && {
	uci_add dhcp dnsmasq
	unset dhcp_cfgs dnsmasq_cfgs
	uci_load dhcp
}

vcfg_number=$(echo "$dhcp_cfgs" "$dnsmasq_cfgs" |wc -l)
let "vcfg_number+=1"

#add dhcp network
if [ "$FORM_add_dhcp" != "" ]; then
	uci_add "dhcp" "dhcp" ""
	uci_set "dhcp" "$CONFIG_SECTION" "interface" "$FORM_network_add"
	unset host_cfgs dnsmasq_cfgs dnsmasq_cfgs
	uci_load dhcp
	let "vcfg_number+=1"
fi

#remove static address
if [ "$FORM_remove_static" != "" ]; then
	uci_remove "dhcp" "$FORM_remove_static"
	unset host_cfgs dnsmasq_cfgs dnsmasq_cfgs
	uci_load dhcp
fi
#add static address
if [ -n "$FORM_static_name" -a -n "$FORM_static_mac_addr" -a -n "FORM_static_ip_addr" ]; then
	uci_add "dhcp" "host" ""
	uci_set "dhcp" "$CONFIG_SECTION" "name" "$FORM_static_name"
	uci_set "dhcp" "$CONFIG_SECTION" "mac" "$FORM_static_mac_addr"
	uci_set "dhcp" "$CONFIG_SECTION" "ip" "$FORM_static_ip_addr"
	unset host_cfgs dnsmasq_cfgs dnsmasq_cfgs FORM_static_name FORM_static_mac_addr FORM_static_ip_addr
	uci_load dhcp
fi

dnsmasq_cfgs=$(echo "$dnsmasq_cfgs" |uniq)
dhcp_cfgs=$(echo "$dhcp_cfgs" |uniq)

for config in $dnsmasq_cfgs; do
	if [ "$FORM_submit" = "" ]; then
		config_get authoritative $config authoritative
		config_get domain $config domain
		config_get boguspriv $config boguspriv
		config_get filterwin2k $config filterwin2k
		config_get localise_queries $config localise_queries
		config_get expandhosts $config expandhosts
		config_get nonegcache $config nonegcache
		config_get readethers $config readethers
		config_get leasefile $config leasefile
	else
		eval authoritative="\$FORM_authoritative_$config"
		eval domain="\$FORM_domain_$config"
		eval boguspriv="\$FORM_boguspriv_$config"
		eval filterwin2k="\$FORM_filterwin2k_$config"
		eval localise_queries="\$FORM_localise_queries_$config"
		eval expandhosts="\$FORM_expandhosts_$config"
		eval nonegcache="\$FORM_nonegcache_$config"
		eval readethers="\$FORM_readethers_$config"
		eval leasefile="\$FORM_leasefile_$config"
	fi
	
	form_dnsmasq="start_form|DHCP Settings
		field|@TR<<Authoritative>>
		radio|authoritative_$config|$authoritative|1|@TR<<Enabled>>
		radio|authoritative_$config|$authoritative|0|@TR<<Disabled>>
		helpitem|Authoritative
		helptext|HelpText Authoritative#Should be set when dnsmasq is the only DHCP server on a network.
		field|@TR<<Domain>>
		text|domain_$config|$domain
		helpitem|Domain
		helptext|HelpText Domain#Specifies the domain for the DHCP server.
		field|@TR<<Bogus Private Reverse Lookups>>
		radio|boguspriv_$config|$boguspriv|1|@TR<<Enabled>>
		radio|boguspriv_$config|$boguspriv|0|@TR<<Disabled>>
		field|@TR<<filterwin2k>>
		radio|filterwin2k_$config|$filterwin2k|1|@TR<<Enabled>>
		radio|filterwin2k_$config|$filterwin2k|0|@TR<<Disabled>>
		field|@TR<<Localise Queries>>
		radio|localise_queries_$config|$localise_queries|1|@TR<<Enabled>>
		radio|localise_queries_$config|$localise_queries|0|@TR<<Disabled>>
		field|@TR<<Expand Hosts>>
		radio|expandhosts_$config|$expandhosts|1|@TR<<Enabled>>
		radio|expandhosts_$config|$expandhosts|0|@TR<<Disabled>>
		field|@TR<<Negative Caching>>
		radio|nonegcache_$config|$nonegcache|0|@TR<<Enabled>>
		radio|nonegcache_$config|$nonegcache|1|@TR<<Disabled>>
		field|@TR<<Read Ethers>>
		radio|readethers_$config|$readethers|1|@TR<<Enabled>>
		radio|readethers_$config|$readethers|0|@TR<<Disabled>>
		field|@TR<<Lease File>>
		text|leasefile_$config|$leasefile
		helpitem|Lease File
		helptext|HelpText Lease File#Use the specified file to store DHCP lease information. This should remain on /tmp unless you have a external hard drive because it writes out infomation for every lease.
		helpitem|More Information
		helplink|http://www.thekelleys.org.uk/dnsmasq/docs/dnsmasq-man.html
		end_form"
	append forms "$form_dnsmasq" "$N"
done

dhcp_option_select=$(awk -F: '{ print "option|" $1 "|" $2 }' /usr/lib/webif/dhcp_options.dat)

for config in $dhcp_cfgs; do
	count=1
	if [ "$FORM_submit" = "" ]; then
		config_get interface $config interface
		config_get start $config start
		config_get limit $config limit
		config_get options $config options
		config_get leasetime $config leasetime
		config_get_bool ignore $config ignore 0
		for opt in $(echo -n $options | awk -F"-O" '{ for(x=1; x<=NF; x++) print $x }')
		do
			eval $(echo -n "$opt" | awk -F, '($1 == "'$interface'") { print "option'"$count"'=\"" $2 "\"; value"'$count'"=\"" $3; for(x=4;x<=NF;x++) print "," $x; print "\"" }' | tr -d '\n')
			count=$(($count + 1))
		done
	else
		config_get interface $config interface
		eval start="\$FORM_start_$config"
		eval limit="\$FORM_limit_$config"
		eval leasetime="\$FORM_leasetime_$config"
		eval ignore="\$FORM_ignore_$config"
		eval "nextopt=\$FORM_option${count}_$config"
		lastused=0
		while [ "$nextopt" != "" ]
		do
			[ "$nextopt" != "none" ] && lastused=$count
			eval "option$count=\$nextopt; value$count=\$FORM_value${count}_$config"
			count=$(($count + 1))
			eval "nextopt=\$FORM_option${count}_$config"
		done
		count=$(($lastused + 1))
	fi
	eval "option$count=\"\"; value$count=\"\""
	
	#Save networks with a dhcp interface.
	append dhcp_networks "$interface" "$N"

	#convert leasetime to minutes
	lease_int=$(echo "$leasetime" | tr -d [a-z][A-Z])			
	time_units=$(echo "$leasetime" | tr -d [0-9])
	time_units=${time_units:-m}
	case "$time_units" in
		"h" | "H" ) let "leasetime=$lease_int*60";;
		"d" | "D" ) let "leasetime=$lease_int*24*60";;
		"s" | "S" ) let "leasetime=$lease_int/60";;
		"w" | "W" ) let "leasetime=$lease_int*7*24*60";;
		"m" | "M" ) leasetime="$lease_int";;  # minutes 			
		*) leasetime="$lease_int"; echo "<br />WARNING: Unknown suffix found on dhcp lease time: $leasetime";;
	esac

	form_dhcp="start_form|$interface DHCP
		field|@TR<<DHCP>>
		radio|ignore_$config|$ignore|0|@TR<<On>>
		radio|ignore_$config|$ignore|1|@TR<<Off>>
		field|@TR<<Start>>
		text|start_$config|$start
		field|@TR<<Limit>>
		text|limit_$config|$limit
		field|@TR<<Lease Time (in minutes)>>
		text|leasetime_$config|$leasetime
		"
	
	for loop in $(seq 1 $count)
	do
		eval "thisopt=\$option$loop"
		eval "thisval=\$value$loop"
		thisval=$(echo -n $thisval | tr -d ' ')

		append form_dhcp "field|@TR<<Option>>
		select|option${loop}_$config|$thisopt
		option|none|@TR<<none#None>>
		$dhcp_option_select
		text|value${loop}_$config|$thisval
		"
	done
	
	append form_dhcp "end_form" "$N"

	append forms "$form_dhcp" "$N"

	append validate_forms "int|start_$config|@TR<<DHCP Start>>||$start" "$N"
	append validate_forms "int|limit_$config|@TR<<DHCP Limit>>||$limit" "$N"
	append validate_forms "int|leasetime_$config|@TR<<DHCP Lease Time>>|min=1 max=2147483647|$leasetime" "$N"
done

for network in $networks; do
	echo "$dhcp_networks" | grep -q "$network"
	if [ "$?" != "0" ]; then
		append network_options "option|$network" "$N"
	fi
done
if [ "$network_options" != "" ]; then
	field_dhcp_add="start_form
		select|network_add
		$network_options
		submit|add_dhcp|@TR<<Add DHCP>>
		end_form"
	append forms "$field_dhcp_add" "$N"
fi

if ! empty "$FORM_submit"; then
	SAVED=1
	validate <<EOF
$validate_forms
EOF
	equal "$?" 0 && {
		for config in $dnsmasq_cfgs; do
			eval authoritative="\$FORM_authoritative_$config"
			eval domain="\$FORM_domain_$config"
			eval boguspriv="\$FORM_boguspriv_$config"
			eval filterwin2k="\$FORM_filterwin2k_$config"
			eval localise_queries="\$FORM_localise_queries_$config"
			eval expandhosts="\$FORM_expandhosts_$config"
			eval nonegcache="\$FORM_nonegcache_$config"
			eval readethers="\$FORM_readethers_$config"
			eval leasefile="\$FORM_leasefile_$config"
			
			uci_set "dhcp" "$config" "authoritative" "$authoritative"
			uci_set "dhcp" "$config" "domain" "$domain"
			uci_set "dhcp" "$config" "local" "/$domain/"
			uci_set "dhcp" "$config" "boguspriv" "$boguspriv"
			uci_set "dhcp" "$config" "filterwin2k" "$filterwin2k"
			uci_set "dhcp" "$config" "localise_queries" "$localise_queries"
			uci_set "dhcp" "$config" "expandhosts" "$expandhosts"
			uci_set "dhcp" "$config" "nonegcache" "$nonegcache"
			uci_set "dhcp" "$config" "readethers" "$readethers"
			uci_set "dhcp" "$config" "leasefile" "$leasefile"
		done
			
		for config in $dhcp_cfgs; do
			eval start="\$FORM_start_$config"
			eval limit="\$FORM_limit_$config"
			eval leasetime="\$FORM_leasetime_$config"
			eval ignore="\$FORM_ignore_$config"
			
			if [ "$leasetime" != "" ]; then
				leasetime="${leasetime}m"
			fi
			
			config_get interface $config interface
			uci_set "dhcp" "$config" "start" "$start"
			uci_set "dhcp" "$config" "limit" "$limit"
			uci_set "dhcp" "$config" "leasetime" "$leasetime"
			uci_set "dhcp" "$config" "ignore" "$ignore"
			
			count=1
			optstring=""
			eval "nextopt=\$FORM_option${count}_$config"
			while [ "$nextopt" != "" ]
			do
				if [ "$nextopt" != "none" ]
				then
					eval "thisval=\$FORM_value${count}_$config"
					thisval=$(echo -n $thisval | tr -d ' ')
					if [ "$optstring" = "" ]
					then
						optstring="-O $interface,$nextopt,$thisval"
					else
						optstring="$optstring -O $interface,$nextopt,$thisval"
					fi
				fi
				count=$(($count + 1))
				eval "nextopt=\$FORM_option${count}_$config"
			done
			if [ "$optstring" != "" ]
			then
				uci_set "dhcp" "$config" "options" "$optstring"
			else
				uci_remove "dhcp" "$config" "options"
			fi
		done
	}
fi
		
		

#####################################################################
# modechange script
#
cat <<EOF
<script type="text/javascript" src="/webif.js"></script>
<script type="text/javascript">
<!--
function modechange()
{
	var v;
	$js

	hide('save');
	show('save');
}
-->
</script>

EOF

display_form <<EOF
onchange|modechange
$validate_error
$forms
EOF

display_form <<EOF
start_form|@TR<<network_hosts_DHCP_Static_IPs#Static IP addresses (for DHCP)>>
field|@TR<<Name>>
text|static_name|$FORM_static_name
field|@TR<<MAC Address>>
text|static_mac_addr|$FORM_static_mac_addr
field|@TR<<IP Address>>
text|static_ip_addr|$FORM_static_ip_addr
helpitem|network_hosts_Static_IPs#Static IP addresses
helptext|network_hosts_Static_IPs_helptext#The file /etc/ethers contains database information regarding known 48-bit ethernet addresses of hosts on an Internetwork. The DHCP server uses the matching IP address instead of allocating a new one from the pool for any MAC address listed in this file.
end_form
EOF

cat <<EOF
<hr class="separator" />
<h5><strong>@TR<<network_dhcp_static_addresses#Static Addresses>></strong></h5>
<table style="width: 90%; margin-left: 2.5em; text-align: left; font-size: 0.8em;" border="0" cellpadding="3" cellspacing="2" summary="Settings">
<tr>
	<th>@TR<<network_hosts_MAC#MAC Address>></th>
	<th>@TR<<network_hosts_IP#IP Address>></th>
	<th>@TR<<network_hosts_Name#Name>></th>
	<th></th>
</tr>
EOF

odd=0
for config in $host_cfgs; do
	config_get mac $config mac
	config_get ip $config ip
	config_get name $config name
	if [ "$odd" = 0 ]; then
		echo "<tr>"
		let odd+=1
	else
		echo "<tr class=\"odd\">"
		let odd-=1
	fi
	echo "<td>${mac}</td>"
	echo "<td>${ip}</td>"
	echo "<td>${name}</td>"
	echo "<td><a href=\"$SCRIPT_NAME?remove_static=${config}\">@TR<<Remove>> ${name}</a></td></tr>"
done
echo "</table><br />"

cat <<EOF
<hr class="separator" />
<h5><strong>@TR<<network_hosts_Active_Leases#Active DHCP Leases>></strong></h5>
<table style="width: 90%; margin-left: 2.5em; text-align: left; font-size: 0.8em;" border="0" cellpadding="3" cellspacing="2" summary="Settings">
<tr>
	<th>@TR<<network_hosts_MAC#MAC Address>></th>
	<th>@TR<<network_hosts_IP#IP Address>></th>
	<th>@TR<<network_hosts_Name#Name>></th>
	<th>@TR<<network_hosts_Expires#Expires in>></th>
</tr>
EOF

cat /tmp/dhcp.leases 2>/dev/null | awk -v date="$(date +%s)" '
BEGIN {
	odd=1
	counter = 0
}
$1 > 0 {
	counter++
	if (odd == 1)
	{
		print "	<tr>"
		odd--
	} else {
		print "	<tr class=\"odd\">"
		odd++
	}
	print "		<td>" $2 "</td>"
	print "		<td>" $3 "</td>"
	print "		<td>" $4 "</td>"
	print "		<td>"
	t = $1 - date
	h = int(t / 60 / 60)
	if (h > 0) printf h "@TR<<network_hosts_h#h>> "
	m = int(t / 60 % 60)
	if (m > 0) printf m "@TR<<network_hosts_min#min>> "
	s = int(t % 60)
	printf s "@TR<<network_hosts_sec#sec>> "
	print "		</td>"
	print "	</tr>"
}
END {
	if (counter == 0) {
		print "	<tr>"
		print "		<td colspan=\"4\">@TR<<network_hosts_No_leases#There are no known DHCP leases.>></td>"
		print "	</tr>"
	}
	print "</table>"
}'

footer ?>
<!--
##WEBIF:name:Network:425:DHCP
-->
