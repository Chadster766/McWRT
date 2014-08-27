#!/usr/bin/webif-page
<?
. "/usr/lib/webif/webif.sh"

header_inject_head=$(cat <<EOF
	<meta http-equiv="refresh" content="20" />
EOF
)

header "Status" "System" "@TR<<Device Status>>"

MEMINFO=$(busybox free | grep "Mem:")
nI="0"
for CUR_VAR in $MEMINFO; do
	case "$nI" in
		1)	TOTAL_MEM=$CUR_VAR;;
		3)	FREE_MEM=$CUR_VAR
			break;;
	esac
	let "nI+=1"
done

USED_MEM=$(expr $TOTAL_MEM - $FREE_MEM)
MEM_PERCENT_FREE=$(expr $FREE_MEM "*" 100 / $TOTAL_MEM)
MEM_PERCENT_USED=$(expr 100 - $MEM_PERCENT_FREE)

SWAPINFO=$(busybox free | grep "Swap:")
nI="0"
for CUR_VAR in $SWAPINFO; do
	case "$nI" in
		1)	TOTAL_SWAP=$CUR_VAR;;
		3)	FREE_SWAP=$CUR_VAR
			break;;
	esac
	let "nI+=1"
done

[ "$TOTAL_SWAP" -gt 0 ] 2>/dev/null && {
	USED_SWAP=$(expr $TOTAL_SWAP - $FREE_SWAP)
	SWAP_PERCENT_FREE=$(expr $FREE_SWAP "*" 100 / $TOTAL_SWAP)
	SWAP_PERCENT_USED=$(expr 100 - $SWAP_PERCENT_FREE)
	swap_usage="
string|<tr id=\"swap\"><td>@TR<<Swap>>: $TOTAL_SWAP @TR<<KiB>></td><td>
progressbar|swapuse|@TR<<Used>>: $USED_SWAP @TR<<KiB>> ($SWAP_PERCENT_USED%)|200|$SWAP_PERCENT_USED|$SWAP_PERCENT_USED%||"
	swap_usage_help="
helpitem|Swap
helptext|Helptext Swap#When a program requires more memory than is physically available in the computer, currently unused information can be written to a temporary buffer on the hard disk, called swap, thereby freeing memory."
}

#todo: if we're not going to use 'free' vars, remove from calculatin
ACTIVE_CONNECTIONS=$(cat "/proc/net/ip_conntrack" | wc -l)
MAX_CONNECTIONS=$(cat "/proc/sys/net/ipv4/netfilter/ip_conntrack_max")
FREE_CONNECTIONS=$(expr $MAX_CONNECTIONS - $ACTIVE_CONNECTIONS)
FREE_CONNECTIONS_PERCENT=$(expr $FREE_CONNECTIONS "*" 100 / $MAX_CONNECTIONS)
USED_CONNECTIONS_PERCENT=$(expr 100 - $FREE_CONNECTIONS_PERCENT)

# _loadavg should be set by the header code..
# empty "$_loadavg" && {
#	_loadavg="${_uptime#*load average: }"
#	_uptime="${_uptime#*up }"
#}


tab=0;
var="";
mounts_form=$(
df | uniq | awk 'BEGIN { mcount=0 };
	/\// {
		if( tab == 1 )
		{
			filled_caption=$4;
			print "string|<tr><td><strong>"$5"</strong><br /><em>"var"</em></td><td>"
			print "progressbar|mount_" mcount "|" $2 "@TR<<KiB>> @TR<<mount_of#of>> " $1 "@TR<<KiB>>|200|" $4 "|" filled_caption "|"; mcount+=1
			print "string|</td></tr>"
		}
		else if( $5 != "" )
		{
			filled_caption=$5;
			print "string|<tr><td><strong>"$6"</strong><br /><em>"$1"</em></td><td>"
			print "progressbar|mount_" mcount "|" $3 "@TR<<KiB>> @TR<<mount_of#of>> " $2 "@TR<<KiB>>|200|" $5 "|" filled_caption "|"; mcount+=1
			print "string|</td></tr>"
		}
		else
		{
			tab=1;
			var=$1;
		}
	}'
)
swap_form=$(cat /proc/swaps | awk 'BEGIN { mcount=0 };
	/\// {
		filled_caption = $4 / ($3 / 100);
		if (filled_caption - int(filled_caption) > 0)
			filled_caption = int(filled_caption + 1)
		else
			filled_caption = int(filled_caption)
		if ($2 == "partition")
			swap_type="@TR<<status_basic_swap_partition#swap partition>>"
		else
			swap_type="@TR<<status_basic_swap_file#swap file>>"
		print "string|<tr><td><strong>"swap_type" "$5"</strong><br /><em>"$1"</em></td><td>"
		print "progressbar|swap_" mcount "|" $4 "@TR<<KiB>> @TR<<mount_of#of>> " $3 "@TR<<KiB>>|200|" $4 "|" filled_caption "%|"; mcount+=1
		print "string|</td></tr>"
	}'
)

#start_form|@TR<<Load Average>>
#string|<tr><td style="font-size:1.2em;color:red;">$_loadavg</<tr><td>
#helpitem|Load Average
#helptext|Helptext Load Average#The load average represents the average number of active processes during the past 1, 5, and 15 minutes
#end_form|

display_form <<EOF
start_form|@TR<<RAM Usage>>
string|<tr id="ram"><td>@TR<<Total>>: $TOTAL_MEM @TR<<KiB>></td><td>
progressbar|ramuse|@TR<<Used>>: $USED_MEM @TR<<KiB>> ($MEM_PERCENT_USED%)|200|$MEM_PERCENT_USED|$MEM_PERCENT_USED%||
string|</td></tr>
$swap_usage
helpitem|RAM Usage
helptext|Helptext RAM Usage#This is the current RAM usage. The amount free represents how much applications have available.
$swap_usage_help
end_form|

start_form|@TR<<Tracked Connections>>
string|<tr id="con"><td>@TR<<Maximum>>: $MAX_CONNECTIONS</td><td>
progressbar|conntrackuse|@TR<<Used>>: $ACTIVE_CONNECTIONS ($USED_CONNECTIONS_PERCENT%)|200|$USED_CONNECTIONS_PERCENT|$USED_CONNECTIONS_PERCENT%||
string|</td></tr>
helpitem|Tracked Connections
helptext|Helptext Tracked Connections#This is the number of connections in your router's conntrack table. <a href="status-conntrackread.sh">View Conntrack Table</a>.
end_form|

start_form|@TR<<Mount Usage>>
$mounts_form
$swap_form
helpitem|Mount Usage
helptext|Helptext Mount Usage#This is the amount of space total and used on the filesystems mounted to your router.
end_form|
EOF

footer ?>
<!--
##WEBIF:name:Status:100:System
-->
