#!/usr/bin/webif-page
<?
#
# This page is synchronized between kamikaze and WR branches. Changes to it *must* 
# be followed by running the webif-sync.sh script.
#
. /usr/lib/webif/webif.sh

if ! empty "$FORM_umount"; then
	if ! empty "$FORM_mountdev"; then
		err_umount=$(umount $FORM_mountdev 2>&1)
		! equal "$?" "0" && {
			ERROR="@TR<<status_usb_umount_error_in#Error in>> $err_umount"
		}
	fi
fi

header "Status" "USB" "@TR<<status_usb_USB_Devices#USB Devices>>"

?>
<div class="settings">
<h3>@TR<<status_usb_All_connected_devices#All connected devices (excluding system hubs)>></h3>
<div>
<table style="width: 90%; margin-left: 2.5em; text-align: left; font-size: 0.9em;" border="0" cellpadding="3" cellspacing="2">
<?
[ -f /proc/bus/usb/devices ] && grep -e "^[TDPS]:" /proc/bus/usb/devices | sed 's/[[:space:]]*=[[:space:]]*/=/g' | sed 's/[[:space:]]\([^ |=]*\)=/|\1=/g' | sed 's/^/|/' | awk '
	BEGIN {
		i=0; RS="|"; FS="=";
		odd=1
		print "<tbody>"
		print "	<tr>"
		print "		<th>@TR<<status_usb_Bus#Bus>></th>"
		print "		<th>@TR<<status_usb_Device#Device>></th>"
		print "		<th>@TR<<status_usb_Product#Product>></th>"
		print "		<th>@TR<<status_usb_Manufacturer#Manufacturer>></th>"
		print "		<th>@TR<<status_usb_VPIDs#VendorID:ProdID>></th>"
		print "		<th>@TR<<status_usb_USB_version#USB version>></th>"
		print "		<th>@TR<<status_usb_Speed#Speed>></th>"
		print "	</tr>"
	}
	$1 ~ /^T: / { i++; }
	$1 ~ /^Bus/ { bus[i]=$2; }
	$1 ~ /^Dev#/ { device[i]=$2; }
	$1 ~ /^Ver/ { usbversion[i]=$2; }
	$1 ~ /^Vendor/ { vendorID[i]=$2; }
	$1 ~ /^ProdID/ { productID[i]=$2; }
	$1 ~ /^Manufacturer/ { manufacturer[i]=$2; }
	$1 ~ /^Product/ { product[i]=$2; }
	$1 ~ /^Spd/ { speed[i]=$2; gsub(/[[:space:]]*$/, "", speed[i]); }
	END {
		for ( j=1; j<=i; ++j ) {
			vpID=vendorID[j]":"productID[j];
			if ( length(product[j])<1 && vpID != "0000:0000" ) {
				"[ -n \"`which lsusb`\" ] && lsusb -d "vpID" | sed \"s/^.*"vpID" //\"" | getline product[j];
			}
			if ( length(manufacturer[j])<1 && productID[j]!="0000" ) {
				pid=vendorID[j];
				"[ -f /usr/share/usb.ids ] && grep -e \"^"pid"\" /usr/share/usb.ids | sed \"s/^"pid" *//\"" | getline manufacturer[j];
			}
			if ( vpID != "0000:0000" ) {
				if (odd == 1) {
					print "	<tr>"
					odd--
				} else {
					print "	<tr class=\"odd\">"
					odd++
				}
				print "		<td>" bus[j] "</td>"
				print "		<td>" device[j] "</td>"
				print "		<td>" product[j] "</td>"
				print "		<td>" manufacturer[j] "</td>"
				print "		<td>" vpID "</td>"
				print "		<td>" usbversion[j] "</td>"
				print "		<td>" speed[j] "&nbsp;@TR<<units_Mbps#Mbps>></td>"
				print "	</tr>"
			}
		}
		print "</tbody>"
	}
'
display_form <<EOF
end_form
EOF
usbpr=`ls //usb/lp* 2>//null`
if [ ! -z "$usbpr" ]; then
        inklevel_form=$(
        for p in $usbpr; do
                ink -d $p |  awk -v FS=":" '
                BEGIN
                {
                        line=0
                        print "string|<table style=\"width: 90%; margin-left: 2.
                }
                {
                        if (line==2)
                        {
                                print "string| @TR<<st
                        }
                        if (line >=4)
                        {
                                print "string| " $1 "
                                print "progressbar|inklevel| 200| " $2 "| "
                                print "string|"
                        }
                        line++
                }
                END
                {
                        print "string|"
                }'
        done;
        )
        display_form <<EOF
        start_form|@TR<>
        $inklevel_form
        end_form
EOF
fi
?>
<div class="settings">
<h3>@TR<<status_usb_Mounted_USB_SCSI#Mounted USB / SCSI devices>></h3>
<div>
<table style="width: 90%; margin-left: 2.5em; text-align: left; font-size: 0.9em;" border="0" cellpadding="3" cellspacing="2">
<?
mounted_devices="$(cat /proc/mounts | grep -e "^/dev/sd[a-p]\{0,2\}" -e "^/dev/scsi/.*")"
! equal "$mounted_devices" "" && {
	echo "$mounted_devices" | awk -v url="$SCRIPT_NAME" '
	BEGIN {
		odd=1
		print "<tbody>"
		print "	<tr>"
		print "		<td colspan=\"5\"><h3>@TR<<status_usb_filesystems#File systems>></h3></td>"
		print "	</tr>"
		print "	<tr>"
		print "		<th>@TR<<status_usb_Device_Path#Device Path>></th>"
		print "		<th>@TR<<status_usb_Mount_Point#Mount Point>></th>"
		print "		<th>@TR<<status_usb_File_System#File System>></th>"
		print "		<th>@TR<<status_usb_Read_Write#Read/Write>></th>"
		print "		<th>@TR<<status_usb_Action#Action>></th>"
		print "	</tr>"
	}
	{
		if (odd == 1) {
			print "	<tr>"
			odd--
		} else {
			print "	<tr class=\"odd\">"
			odd++
		}
		print "		<td>" $1 "</td>"
		print "		<td>" $2 "</td>"
		print "		<td>" $3 "</td>"
		$4 = "," $4 ","
		if ($4 ~ /,ro,/)
			print "		<td>@TR<<status_usb_ro#Read only>></td>"
		else if ($4 ~ /,rw,/)
			print "		<td>@TR<<status_usb_rw#Read/Write>></td>"
		else
			print "		<td>&nbsp;</td>"
		print "		<td><form method=\"post\" action=\"" url "\"><input type=\"submit\" value=\" @TR<<status_usb_umount#umount>> \" name=\"umount\" /><input type=\"hidden\" value=\"" $1 "\" name=\"mountdev\" /></form></td>"
		print "	</tr>"
	}
	END {
		print "</tbody>"
	}'

	mnts="$(echo "$mounted_devices" | awk '
	{
	        sub(/[[:digit:]]{0,2}$/, "", $1)
    	    print $1
    	    print $2
	}' | sort -u | awk '
		BEGIN {
	        OFS = ""
    	    ORS = ""
    	    print "("
	}
	{
        if (FNR > 1) print "|"
        gsub(/\//, "\\/")
        print "^" $1
	}
	END {
        print ")"
	}')"
}
! empty "$mnts" && {
	swap_devices="$(cat "/proc/swaps" 2>/dev/null | egrep "$mnts")"
} || {
	swap_devices="$(cat "/proc/swaps" 2>/dev/null | grep "^/dev/sd[a-p]\{0,2\}[[:space:]]")"
}
! empty "$swap_devices" && {
	echo "$swap_devices" | awk '
	BEGIN {
		odd=1
		print "</table>"
		print "<br />"
		print "<table style=\"width: 90%; margin-left: 2.5em; text-align: left; font-size: 0.9em;\" border=\"0\" cellpadding=\"3\" cellspacing=\"2\">"
		print "<tbody>"
		print "	<tr>"
		print "		<td colspan=\"5\"><h3>@TR<<status_usb_swaps#Swaps>></h3></td>"
		print "	</tr>"
		print "	<tr>"
		print "		<th>@TR<<status_usb_swap_PartitionFilename#Partition/Filename>></th>"
		print "		<th>@TR<<status_usb_swap_Type#Type>></th>"
		print "		<th>@TR<<status_usb_swap_Size#Size>></th>"
		print "		<th>@TR<<status_usb_swap_Used#Used>></th>"
		print "		<th>@TR<<status_usb_swap_Priority#Priority>></th>"
	}
	{
		if (odd == 1) {
			print "	<tr>"
			odd--
		} else {
			print "	<tr class=\"odd\">"
			odd++
		}
		print "		<td>" $1 "</td>"
		if ($2 == "partition")
			print "		<td>@TR<<status_usb_swap_partition#partition>></td>"
		else if ($2 == "file")
			print "		<td>@TR<<status_usb_swap_file#file>></td>"
		else
			print "		<td>" $2 "</td>"
		print "		<td align=\"right\">" $3 "</td>"
		print "		<td align=\"right\">" $4 "</td>"
		print "		<td align=\"right\">" $5 "</td>"
		print "	</tr>"
	}
	END {
		print "</tbody>"
	}'
}
display_form <<EOF
end_form
EOF

if [ -f /proc/bus/usb/drivers ]; then
	cat <<EOF
<div class="settings">
<h3>@TR<<status_usb_Loaded_USB_drivers#Loaded USB drivers>></h3>
<div>
<table style="width: 90%; margin-left: 2.5em; text-align: left; font-size: 0.9em;" border="0" cellpadding="3" cellspacing="2">
<tbody>
EOF
	sed -e 's/.*://; s/ //g;' /proc/bus/usb/drivers | sort | awk '
	{
		print "	<tr><td>" $1 "</td></tr>"
	}
	END {
		if (NR<1) print "	<tr><td>&nbsp;</td></tr>"
	}'
	display_form <<EOF
</tbody>
end_form
EOF
fi

footer ?>
<!--
##WEBIF:name:Status:454:USB
-->
