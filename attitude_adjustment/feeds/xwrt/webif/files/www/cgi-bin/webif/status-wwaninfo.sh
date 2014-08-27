#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh

header_inject_head=$(cat <<EOF
<style type="text/css">
/*<![CDATA[*/
<!--

#wwanbars * { padding: 0; margin: 0; }

#wwanbars body {
	font-family: Verdana;
	font-size: 1em;
	line-height: 1em;
	padding: 1em;
}

#wwanbars .wwan_status { padding-top: 5em; }

#wwanbars ul { list-style-type: none; }

#wwanbars ul li { clear: both; height: 1.2em; }

#wwanbars .title { width: 10em; float: left; }

#wwanbars .progress {
	text-align: right;
	display: block;
	float: left;
	clear: right;
	font-size: 0.964em;
	padding: 0.1em;
	margin-bottom: 0.2em;
}

#wwanbars h4 { display: none; }

/* Legend */

#wwanbars .legend {
	position: absolute;
	margin-top: -5.3em;
	margin-left: 10em;
	clear: both;
	width: 30em;
	border-left: 1px solid Gray;
	border-right: 1px solid Gray;
}

#wwanbars dl {
	float: left;
	text-align: center;
	width: 30%;
	font-size: 0.8em;
	line-height: 1.5em;
}

#wwanbars dl.workable { width: 16%; }

#wwanbars dl.good { width: 16%; }

#wwanbars dl.excellent { width: 38%; }

#wwanbars dl+dl dt, dl+dl dd { border-left: 1px solid Gray; }

#wwanbars dd .title { display: none; }

#wwanbars dd.dbm { margin-top: 4em; }

/* Colors for status health */

#wwanbars dl.unreliable dt, span.progress.unreliable { background-color: #ff7474; }

#wwanbars dl.workable dt, span.progress.workable { background-color: #fffa74; }

#wwanbars dl.good dt, span.progress.good { background-color: #ace4ff; }

#wwanbars dl.excellent dt, span.progress.excellent { background-color: #6fff6c; }

-->
/*]]>*/
</style>

EOF
)

# the comgt package has changed the executable
COMGT=$(which comgt 2>/dev/null)
empty "$COMGT" && COMGT=$(which gcom 2>/dev/null)
# the webif^2's comgt query script
COMGTWEBIF="/usr/lib/webif/comgt.webif"

[ -x "$COMGT" ] && [ -s "$COMGTWEBIF" ] && {
	DEVICES="/dev/usb/tts/2 /dev/noz2"
	for DEV in $DEVICES
	do
		[ -c "$DEV" ] && [ -x "$COMGT" ] && {
			INFO=$($COMGT -d "$DEV" -s "$COMGTWEBIF" 2>/dev/null)
			STRENGTH=$(echo "$INFO" | grep "+CSQ:" | cut -d: -f2 | cut -d, -f1)
			CHARGING=$(echo "$INFO" | grep "+CBC:" | cut -d: -f2 | cut -d, -f1)
			CAPACITY=$(echo "$INFO" | grep "+CBC:" | cut -d: -f2 | cut -d, -f2)
		}
	done
}

header "Status" "UMTS" "@TR<<status_wwaninfo_UG_Status#UMTS/GPRS Status>>"

equal "$INFO" "" && {
	cat << EOF
<p>@TR<<status_wwaninfo_no_UG_device#UMTS / GPRS device not found.>></p>
EOF
	equal "$COMGT" "" || equal "$COMGTWEBIF" "" && {
		cat << EOF
<p>@TR<<status_wwaninfo_no_req_app#The required components are missing. Please install the latest <a href="system-ipkg.sh">comgt</a> package and <a href="info.sh">webif&sup2;</a>.>></p>
EOF
	}
	footer
	exit
}

cat << EOF
<div class="settings">
<h3><strong>@TR<<status_wwaninfo_device_info#Device Information>></strong></h3>
<table style="text-align: left;" border="0" cellpadding="3" cellspacing="3">
<tbody>
EOF

if ! empty "$INFO"; then
	echo "$INFO" | awk -F: '
		BEGIN {
			print "	<tr>"
			print "		<th>@TR<<status_wwaninfo_dev_th_Information#Information>></th>"
			print "		<th>@TR<<status_wwaninfo_dev_th_Value#Value>></th>"
			print "	</tr>"
			odd=1
		}
		/^[#+ ]/ {next}
		{
			if (length($2) > 0) {
				if (odd == 1) {
					print "	<tr>"
					odd--
				} else {
					print "	<tr class=\"odd\">"
					odd++
				}
				col2=$2
				for (i=3; i<=NF; i++)
					col2 = col2 ":" $i
				print "		<td>" $1 "</td>"
				print "		<td>" col2 "</td>"
				print "	</tr>"
			}
		}'
else
	cat << EOF
	<tr>
		<td colspan="2">@TR<<status_wwaninfo_no_UG_device_info#No device information reported.>></td>
	</tr>
EOF
fi

cat << EOF
</tbody>
</table>
</div>
<br />
EOF

[ "$CHARGING" -ge 0 ] >/dev/null 2>&1 &&  [ "$CAPACITY" -ge 0 ] >/dev/null 2>&1 && {
	[  "$CHARGING" -eq 0 ] && charg_text="@TR<<status_wwaninfo_Notcharging#Not charging>>" || charg_text="@TR<<status_wwaninfo_Charging#Charging>>"
	display_form << EOF
start_form|@TR<<status_wwaninfo_Battery_Status#Battery Status>>
field|@TR<<status_wwaninfo_Status#Status>>
string|<div style="text-align: left">$charg_text</div>
field|@TR<<status_wwaninfo_Capacity#Capacity>>
progressbar|capacity||200|$CAPACITY|${CAPACITY}%||
end_form
EOF
}

! empty "$STRENGTH" && {
	cat << EOF
<div class="settings">
<h3><strong>@TR<<status_wwaninfo_Signal_Quality#Signal Quality>></strong></h3>
EOF

	# check if numeric
	[ "$STRENGTH" -ge 0 ] >/dev/null 2>&1 && {
		if [ "$STRENGTH" -gt 31 ]; then
			echo "<p>@TR<<status_wwaninfo_quality_unknown#Signal quality is invalid/unknown>>: ${STRENGTH}</p>"
		else
			progress_type="unreliable"
			[ "$STRENGTH" -gt 9 ] && progress_type="workable"
			[ "$STRENGTH" -gt 14 ] && progress_type="good"
			[ "$STRENGTH" -gt 19 ] && progress_type="excellent"
			cat << EOF
<div id="wwanbars"><div class="wwan_status">
	<ul>
		<li>
			<span class="title">@TR<<status_wwaninfo_Signal_Quality#Signal Quality>>:</span> <span class="progress ${progress_type}" style="width: ${STRENGTH}em;">${STRENGTH}</span>
		</li>
		<li>
			<span class="title">@TR<<status_wwaninfo_Power_Ratio#Power (dBm)>>:</span> <span class="progress ${progress_type}" style="width: ${STRENGTH}em;">$((-113 + $STRENGTH * 2))</span>
		</li>
	</ul>
	<h4>@TR<<status_wwaninfo_Legend#Legend>>:</h4>
	<div class="legend">
		<dl class="unreliable">
EOF
			if equal "$progress_type" "unreliable"; then
				echo "			<dt><strong>@TR<<status_wwaninfo_quality_Unreliable#Unreliable>></strong></dt>"
			else
				echo "			<dt>@TR<<status_wwaninfo_quality_Unreliable#Unreliable>></dt>"
			fi
			cat << EOF
			<dd><span class="title">@TR<<status_wwaninfo_Signal_Quality#Signal Quality>>:</span> 0..9</dd>
			<dd class="dbm"><span class="title">@TR<<status_wwaninfo_Power_Ratio#Power (dBm)>>:</span> -113..-95</dd>
		</dl>
		<dl class="workable">
EOF
			if equal "$progress_type" "workable"; then
				echo "			<dt><strong>@TR<<status_wwaninfo_quality_Workable#Workable>></strong></dt>"
			else
				echo "			<dt>@TR<<status_wwaninfo_quality_Workable#Workable>></dt>"
			fi
			cat << EOF
 			<dd><span class="title">@TR<<status_wwaninfo_Signal_Quality#Signal Quality>>:</span> 10..14</dd>
			<dd class="dbm"><span class="title">@TR<<status_wwaninfo_Power_Ratio#Power (dBm)>>:</span> -93..-85</dd>
		</dl>
		<dl class="good">
EOF
			if equal "$progress_type" "good"; then
				echo "			<dt><strong>@TR<<status_wwaninfo_quality_Good#Good>></strong></dt>"
			else
				echo "			<dt>@TR<<status_wwaninfo_quality_Good#Good>></dt>"
			fi
			cat << EOF
			<dd><span class="title">@TR<<status_wwaninfo_Signal_Quality#Signal Quality>>:</span> 15..19</dd>
			<dd class="dbm"><span class="title">@TR<<status_wwaninfo_Power_Ratio#Power (dBm)>>:</span> -83..-75</dd>
		</dl>
		<dl class="excellent">
EOF
			if equal "$progress_type" "excellent"; then
				echo "			<dt><strong>@TR<<status_wwaninfo_quality_Excellent#Excellent>></strong></dt>"
			else
				echo "			<dt>@TR<<status_wwaninfo_quality_Excellent#Excellent>></dt>"
			fi
			cat << EOF
			<dd><span class="title">@TR<<status_wwaninfo_Signal_Quality#Signal Quality>>:</span> 20..31</dd>
			<dd class="dbm"><span class="title">@TR<<status_wwaninfo_Power_Ratio#Power (dBm)>>:</span> -73..-51</dd>
		</dl>
	</div>
</div></div>
<br />
EOF
		fi
	} || {
		echo "<p>@TR<<status_wwaninfo_wrong_value#Wrong signal quality value>>: ${STRENGTH}</p>"
	}
}

cat << EOF
</div>
<br />
EOF

footer
?>
<!--
##WEBIF:name:Status:170:UMTS
-->
