#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh

#################################
#
# Wireless survey page
#
# Description:
#	Perform a wireless survey and display pretty results.
#
# Author(s) [in order of work date]:
#	Jeremy Collake <jeremy.collake@gmail.com>
#	Travis Kemen <thepeople@berlios.de>
#
# Completely rewritten for speed improvement into pure AWK by
#	Stefan Stapelberg <stefan@rent-a-guru.de>

MACLIST_URL="http://standards.ieee.org/cgi-bin/ouisearch?"

config_cb() {
	local cfg_type="$1"
	local cfg_name="$2"

	case "$cfg_type" in
	        wifi-device) append devices "$cfg_name" ;;
	        wifi-iface)  append vface "$cfg_name" "$N" ;;
	esac
}

uci_load wireless

for device in $devices; do
	config_get type $device type
	[ "$type" = "broadcom" ] && {
		config_get disabled $device disabled
		[ "$disabled" = "0" ] && scan_iface=1
	}
	[ "$type" = "atheros" ] && {
		config_get disabled $device disabled
		[ "$disabled" = "0" ] && atheros_devices="$atheros_devices $device"
	}
	[ "$type" = "mac80211" ] && {
		config_get disabled $device disabled
		[ "$disabled" = "1" ] && {
			FORM_clientswitch=1
		}
		scan_iface=1
	}
done

for ath_device in $atheros_devices; do
	for i in $vface; do
		config_get iface $i device
		if [ "$iface" = "$ath_device" ]; then
			config_get mode $i mode
			[ "$mode" = "sta" ] && scan_iface=1
		fi
	done
done

header "Status" "Site Survey" "@TR<<Site Survey>>"

equal "$scan_iface" "1" || {
	echo '<div class="settings">'
	echo "@TR<<No wireless networks were found>>."
	echo '</div>'
	footer
	exit
}

SCAN_BUTTON='@TR<<Start the scan now>>'
tempfile=$(mktemp /tmp/.survtemp.XXXXXX)

if [ "$FORM_clientswitch" != "" ]; then
	SCAN_BUTTON='@TR<<Start the scan again>>'
	echo "<p style=\"margin: 1em 0;\">@TR<<Please wait, the scan needs some time>> ...</p>"

	for device in $devices; do
		config_get type $device type
		[ "$type" = "broadcom" ] && {
			wifi down
			wlc stdin <<EOF
down
vif 0
enabled 0
vif 1
enabled 0
vif 2
enabled 0
vif 3
enabled 0

ap 1
mssid 1
apsta 0
infra 1

802.11d 0
802.11h 0
rxant 3
txant 3

radio 1
macfilter 0
maclist none
wds none

country IL0
channel 5
maxassoc 128
slottime -1

vif 0
closed 0
ap_isolate 0
up
vif 0
vlan_mode 0
ssid OpenWrt
enabled 1
EOF
			break
		}
		[ "$type" = "atheros" ] && {
			wifi down >/dev/null
			wlanconfig ath0 create wlandev wifi0 wlanmode sta >/dev/null
			ifconfig ath0 up
			sleep 3
		}
		[ "$type" = "mac80211" ] && {
			wifi down >/dev/null
			iw phy phy0 interface add wlan0 type managed
			ifconfig wlan0 up
			sleep 3
		}
	done
fi

got_result=0

for i in 1 2 3 4; do
	iwlist scan 2>/dev/null | tee "$tempfile" | grep -qi 'Cell [0-9][0-9] - Address:' && {
		got_result=1
		break;
	}
done

cat <<-EOF
<div class="settings">
<form enctype="multipart/form-data" method="post" action="$SCRIPT_NAME">
<h3><strong>@TR<<Wireless Survey>></strong></h3>
<p style="margin-bottom: 1em;">
@TR<<wlan_survey_helptext#Your wireless adaptor is not in client mode.<br />To do a scan it must be put into client mode for a few seconds.>>
<br /><br />
<span class="red">@TR<<wlan_survey_warning#<b>Note:</b> Your WLAN traffic will be interrupted during this brief period.>></span>
</p>
<input style="margin-left: 12em;" type="submit" name="clientswitch" value=" $SCAN_BUTTON " />
</form>
<div class="clearfix">&nbsp;</div>
</div>
<br />
EOF

equal "$got_result" "1" || {
	SCAN_BUTTON="@TR<<Start survey>>"
	echo '<div class="settings" style="margin-top: 1em;">'
	echo '<p>@TR<<status_wlan_noresults#Sorry, there are no scan results. Please do a search by clicking on>>'
	echo "<i>${SCAN_BUTTON}</i> @TR<<above>>.</p>"
	echo '</div>'
	footer
	rm "$tempfile"
	exit
}

FORM_cells=$(awk -v "maclist_url=$MACLIST_URL" '
BEGIN { nc = 0; }
	$1 == "Cell" && $4 == "Address:" {
	nc = $2 + 0;	# strip leading zero
	macaddr[nc] = $5;
}
nc > 0 {
	if ($1 ~ /^ESSID:/) {
		sub(/^  *ESSID:"/, "", $0);
		sub(/"$/, "", $0);
		essid[nc] = $0;
		next;
	}
	if ($1 ~ /^Channel:/) {
		split($1, arr, ":");
		channel[nc] = arr[2];
		next;
	}
	if ($1 ~ /^Quality=/ && $2 ~ /^Signal$/) {
		split($1, arr, "=");
		quality[nc] = arr[2];
	
		split($3, arr, "=");
		siglevel[nc] = arr[2];
	
		split($6, arr, "=");
		noiselevel[nc] = arr[2];
		if (noiselevel == "") {
			noiselevel[nc] = -99;
		}
		next;
	}
	if ($1 ~ /^Encryption/) {
		split($2, arr, ":");
		enc[nc] = arr[2];
	}
}
END {	for (i=1; i <= nc; i++) {
		split(macaddr[i], arr, ":");
		abbr_macaddr = arr[1] "-" arr[2] "-" arr[3];

		noise_delta = -99 - noiselevel[i];
		integrity = siglevel[i] + noise_delta;
		psnr = 100 + integrity;

		img_url = "<img src=\"/images/";
		if (enc[i] == "on")
			img_url = img_url "lock.png";
		else	img_url = img_url "transmit.png";
		img_url = img_url "\" width=\"16\" height=\"16\" border=\"0\" alt=\"\" />";

		printf("string|<tr><td class=\"wlscan\" valign=\"top\" nowrap=\"nowrap\">%s<span class=\"essid\">%s</span><br />\n",
			img_url, essid[i]);
		printf("string|<span class=\"wlinfo\"><a title=\"@TR<<Cell>> %d\" href=\"%s?%s\" target=\"_blank\">%s</a>\n",
			i, maclist_url, abbr_macaddr, macaddr[i]);
		printf("string| @TR<<on>> @TR<<Channel>> %d</span>", channel[i]);
		if (quality[i] != "0/0")
			printf("<br />\nstring|<span class=\"wlinfo\">@TR<<Quality>>: %s</span>", quality[i]);
		printf("</td>\n");
		printf("string|<td valign=\"top\">@TR<<Signal>> %d @TR<<dbm>> / @TR<<Noise>> %d @TR<<dbm>><br />\n",
			siglevel[i], noiselevel[i]);
		printf("progressbar|SNR|@TR<<SNR>> %d @TR<<dbm>>|200|%d|\"%d%%\"\n", integrity, psnr, psnr);
		printf("string|</td></tr><tr><td colspan=\"2\">&nbsp;</td></tr>\n");
	}
}' "$tempfile")

rm -f "$tempfile"

# Restore radio to its original state
empty "$FORM_clientswitch" || wifi >/dev/null 2>&1

display_form <<-EOF
start_form|@TR<<Survey Results>>
$FORM_cells
end_form|
EOF

footer ?>
<!--
##WEBIF:name:Status:900:Site Survey
-->
