#!/usr/bin/webif-page
<%
. /usr/lib/webif/webif.sh
###################################################################
# Crontabs
#
# Description:
#
# Author(s) [in order of work date]:
#       m4rc0 <jansssenmaj@gmail.com>
#
# Major revisions:
#		Initial release 2008-11-28
#
# NVRAM variables referenced:
#       none
#
# Configuration files referenced:
#       various crontab files
#
# Required components:
# 

header "System" "Crontabs" "@TR<<Crontabs>>" '' "$SCRIPT_NAME"

#Check if config file exists
if [ ! -f /etc/config/crontabs ]; then
	touch /etc/config/crontabs
fi 


# Remove selected crontabentry
if  ! empty "$FORM_remove_crontabentry" ; then
	uci_remove "crontabs" "$FORM_remove_crontabentry"
fi

# Add new cron-entry
if ! empty "$FORM_MINUTES_newCron"; then
	uci_add "crontabs" "crontab" ""; crontab="$CONFIG_SECTION"
	uci_set "crontabs" "$crontab" "minutes" "$FORM_MINUTES_newCron"
	uci_set "crontabs" "$crontab" "hours" "$FORM_HOURS_newCron"
	uci_set "crontabs" "$crontab" "days" "$FORM_DAYS_newCron"
	uci_set "crontabs" "$crontab" "months" "$FORM_MONTHS_newCron"
	uci_set "crontabs" "$crontab" "weekdays" "$FORM_WEEKDAYS_newCron"
	#uci_set "crontabs" "$crontab" "user" "$FORM_USER_newCron"
	uci_set "crontabs" "$crontab" "command" "$FORM_COMMAND_newCron"
	uci_set "crontabs" "$crontab" "enabled" "$FORM_ENABLED_newCron"
	FORM_MINUTES_newCron=""
fi


config_cb() {
	local cfg_type="$1"
	local cfg_name="$2"

	case "$cfg_type" in
		crontab)
			append CRONTABS_cfg "$cfg_name"
		;;
		
	esac
}

# change rowcolors
get_tr() {
	if equal "$cur_color" "odd"; then
		cur_color="even"
		tr="<tr>"
	else
		cur_color="odd"
		tr="<tr class=\"odd\">"
	fi
}


cat <<EOF
<script type="text/javascript" language="JavaScript"><!--

 
var cX = 0; var cY = 0; var rX = 0; var rY = 0; var CronTab = ''; ConfigChanged = false;

function UpdateCursorPosition(e){ cX = e.pageX; cY = e.pageY;}
function UpdateCursorPositionDocAll(e){ cX = event.clientX; cY = event.clientY;}

if(document.all) { document.onmousemove = UpdateCursorPositionDocAll; }
else { document.onmousemove = UpdateCursorPosition; }

function AssignPosition(d,offsetX,offsetY) {
if(self.pageYOffset) {
	rX = self.pageXOffset;
	rY = self.pageYOffset;
	}
else if(document.documentElement && document.documentElement.scrollTop) {
	rX = document.documentElement.scrollLeft;
	rY = document.documentElement.scrollTop;
	}
else if(document.body) {
	rX = document.body.scrollLeft;
	rY = document.body.scrollTop;
	}
if(document.all) {
	cX += rX; 
	cY += rY;
	}

if (cY < 280) { cY = cY + 100; }
	
d.style.left = (cX-offsetX) + "px";
d.style.top = (cY-offsetY) + "px";

}


function OpenEditWindow(d,CRONTAB) {
if(d.length < 1) { return; }

document.getElementById('EditWindow').style.display = "none";

if (CRONTAB == 'newCron') {
	ResetForm();
}
else {
	SetValueSelect('sltMinutes','ddEveryXminute',document.getElementById('MINUTES_'+CRONTAB).value,1);
	SetValueSelect('sltHours','ddEveryXhour',document.getElementById('HOURS_'+CRONTAB).value,1);
	SetValueSelect('sltDays','ddEveryXday',document.getElementById('DAYS_'+CRONTAB).value,0);
	SetValueSelect('sltMonths','',document.getElementById('MONTHS_'+CRONTAB).value,0);
	SetValueSelect('sltDaysOfWeek','',document.getElementById('WEEKDAYS_'+CRONTAB).value,1);

	//document.getElementById('txtUsername').value = document.getElementById('USER_'+CRONTAB).value;
	document.getElementById('txtCommand').value = document.getElementById('COMMAND_'+CRONTAB).value;

	if (document.getElementById('ENABLED_'+CRONTAB).value == '1') {
			document.forms[0].chkCronEnabled.checked = true;
			document.getElementById('txthCronEnabled').value = '1';
	}
	else {
			document.forms[0].chkCronEnabled.checked = false;
			document.getElementById('txthCronEnabled').value = '0';
	}

	BuildInterval(false);
}

var dd = document.getElementById(d);
AssignPosition(dd,710,480);
dd.style.display = "block";

CronTab = CRONTAB;
}


function HideContent(d,doAction) {

	if(d.length < 1) { return; }
	document.getElementById(d).style.display = "none";

	//if (doAction == 'update' && document.getElementById('txtUsername').value != '' && document.getElementById('txtCommand').value != ''){
	if (doAction == 'update' && document.getElementById('txtCommand').value != ''){
		document.getElementById('MINUTES_'+CronTab).value = document.getElementById('txthMinutes').value;
		document.getElementById('HOURS_'+CronTab).value = document.getElementById('txthHours').value;
		document.getElementById('DAYS_'+CronTab).value = document.getElementById('txthDays').value;
		document.getElementById('MONTHS_'+CronTab).value = document.getElementById('txthMonths').value;
		document.getElementById('WEEKDAYS_'+CronTab).value = document.getElementById('txthDaysOfWeek').value;
		//document.getElementById('USER_'+CronTab).value = document.getElementById('txtUsername').value;
		document.getElementById('COMMAND_'+CronTab).value = document.getElementById('txtCommand').value;
		document.getElementById('ENABLED_'+CronTab).value = document.getElementById('txthCronEnabled').value;
	}

	if (ConfigChanged == true) {
			document.getElementById('WarningConfigChange').style.display = "block";
	}

}

function SetConfigChanged() {
	
	ConfigChanged = true;

}


function CheckValueSelect(SelectControl,IntervalControl) {
	
	var len = document.forms[0][SelectControl].length;
	var i = 0;
	var intervalSelected = '';
	var CurrentValue = -1;
	var sequenceCounter = 0;
	var chainStatus = false;
	var booleanSelected = false;
	var firstSelect = -1;
	var intervalCompleted = '';

	for (i = 0; i < len; i++) {

		booleanSelected = document.forms[0][SelectControl][i].selected;

		if ( booleanSelected ) {

			if ( document.forms[0][SelectControl][i].value != "" ) {
				var CurrentValue = parseInt(document.forms[0][SelectControl][i].value);
				
				sequenceCounter = sequenceCounter + 1;
				if ( sequenceCounter > 2 ) { chainStatus = true; }

				if ( i+1 == len ) { booleanSelected = false; }
				else { booleanSelected = document.forms[0][SelectControl][i+1].selected;}

				if ( intervalSelected == "" ) { 
					intervalSelected = CurrentValue + '';
					firstSelect = i;
				}
				
				
				if ( !booleanSelected ) {
					if ( chainStatus == true ) {
						intervalSelected = intervalSelected + '-' + CurrentValue;
					}
					else {
						if ( firstSelect != i ) {
							intervalSelected = intervalSelected + ',' + CurrentValue;
						}
					}
					
					chainStatus = false;
					sequenceCounter = 0;
					if ( intervalCompleted == '' ) {intervalCompleted = intervalCompleted + intervalSelected;}
					else { intervalCompleted = intervalCompleted + ',' + intervalSelected; }
					intervalSelected = '';
				}
			}
		}
	}

	if (intervalCompleted == "" ) {intervalCompleted = '*';}
	if ( IntervalControl != '' ) {
		var SelectedIntervalValue = document.forms[0][IntervalControl].value;
		if (SelectedIntervalValue != '' ) {
			intervalCompleted = intervalCompleted + '/' + SelectedIntervalValue;	
		}
	}

	
	return intervalCompleted;
}

function BuildInterval(ChangeConfig) {

	if (ConfigChanged == true) {
			document.getElementById('WarningConfigChange').style.display = "none";
	}
	
	document.getElementById('txthMinutes').value = CheckValueSelect('sltMinutes','ddEveryXminute');
	document.getElementById('txthHours').value = CheckValueSelect('sltHours','ddEveryXhour');
	document.getElementById('txthDays').value = CheckValueSelect('sltDays','ddEveryXday');
	document.getElementById('txthMonths').value = CheckValueSelect('sltMonths','');
	document.getElementById('txthDaysOfWeek').value = CheckValueSelect('sltDaysOfWeek','');
	

	var IntervalString = CheckValueSelect('sltMinutes','ddEveryXminute') + ' ';
	IntervalString = IntervalString + CheckValueSelect('sltHours','ddEveryXhour') + ' ';
	IntervalString = IntervalString + CheckValueSelect('sltDays','ddEveryXday') + ' ';
	IntervalString = IntervalString + CheckValueSelect('sltMonths','') + ' ';
	IntervalString = IntervalString + CheckValueSelect('sltDaysOfWeek','') + ' ';

//	if ( document.getElementById('txtUsername').value != '' ) {
//		IntervalString = IntervalString + document.getElementById('txtUsername').value + ' ';
//	}
//	else {
//		IntervalString = IntervalString + 'user' + ' ';
//		document.getElementById('txtUsername').value = 'user';
//	}

	if ( document.getElementById('txtCommand').value != '' ) {
		IntervalString = IntervalString + document.getElementById('txtCommand').value;
	}
	else {
		IntervalString = IntervalString + 'echo "X-WRT \-\- End user extensions for OpenWrt" | logger' + ' ';
		document.getElementById('txtCommand').value = 'echo "X-WRT \-\- End user extensions for OpenWrt" | logger';
	}
	
	document.getElementById('txtCrontabEntry').value = IntervalString;
	
	if (ChangeConfig == true) {
		SetConfigChanged();
	}
}

function ResetForm() {
	document.getElementById('sltMinutes').selectedIndex = 0;
	document.getElementById('sltHours').selectedIndex = 0;
	document.getElementById('sltDays').selectedIndex = 0;
	document.getElementById('sltMonths').selectedIndex = 0;
	document.getElementById('sltDaysOfWeek').selectedIndex = 0;
	
	document.getElementById('ddEveryXminute').selectedIndex = 0;
	document.getElementById('ddEveryXhour').selectedIndex = 0;
	document.getElementById('ddEveryXday').selectedIndex = 0;

	document.getElementById('chkCronEnabled').checked = false;
	
	//document.getElementById('txtUsername').value = '$selectedcrontab';
	document.getElementById('txtCommand').value = '';
	document.getElementById('txtCrontabEntry').value = '';
	
	ConfigChanged = false;

}

function SetValueSelect(SelectControl,IntervalControl,IntervalString,offset) {

	if (IntervalString.indexOf('/') > 0) {
		
		// Handle */5 and 20-40/5 variants
		var arrInterval = IntervalString.split('/');
		if (arrInterval[0] == '*') {
			SetControTopOption(SelectControl);
		}
		else {
			SetControlSelected(SelectControl,arrInterval[0],offset);
		}
		document.getElementById(IntervalControl).selectedIndex = arrInterval[1];
	}
	else {
		var arrIntervalString = IntervalString.split(',');
		if (arrIntervalString[0] == '*') {
			SetControTopOption(SelectControl);
		}
		else {	
			SetControlSelected(SelectControl,IntervalString,offset);
		}
		if (IntervalControl != '') {
				document.getElementById(IntervalControl).selectedIndex = 0;
		}
	}
}

function SetControlSelected(SelectControl,IntervalString,offset) {

	var arrIntervalString = IntervalString.split(',');
	var ArrayLen = arrIntervalString.length;
	var tempIntervalString = '';
	var j = 0;
	var i = 0;
	var len = document.forms[0][SelectControl].length;
	var arrIntervalString = IntervalString.split(',');

	// translate 2-5 into 2,3,4,5
	for (i = 0; i < ArrayLen; i ++) {
		var PositionOfChar = arrIntervalString[i].indexOf('-');;
		if ( PositionOfChar > 0 ) {
			var arrInterval = arrIntervalString[i].split('-');
			tempIntervalString = arrInterval[0];
			var StartIndex = parseInt(arrInterval[0]);
			while ( StartIndex != arrInterval[1] ) {
				StartIndex = StartIndex + 1;
				tempIntervalString = tempIntervalString + ',' + StartIndex;
			}
		IntervalString = IntervalString.replace(arrIntervalString[i],tempIntervalString);
		}
	}

	// Set all the members in the select control
	var i = 0;
	var arrIntervalString = IntervalString.split(',');
	len = len - 1;

	document.forms[0][SelectControl].options[0].selected = false;

	for (i = 0; i < len; i++) {
		if ( i == parseInt(arrIntervalString[j]) ) {
			document.forms[0][SelectControl].options[i+offset].selected = true;
			j = j + 1;
		}
		else {
			document.forms[0][SelectControl].options[i+offset].selected = false;
		}
	}
}

function SetAllSelect(SelectControl) {
	document.forms[0][SelectControl].options[0].selected = false;
	var len = document.forms[0][SelectControl].length;
	var i = 1;
	for (i = 1; i < len; i++) {
		document.forms[0][SelectControl].options[i].selected = true;
		
	}

}

function SetControTopOption(SelectControl) {
	// Set the first member and reset all others
	document.forms[0][SelectControl].options[0].selected = true;
	var len = document.forms[0][SelectControl].length;
	var i = 1;
	for (i = 1; i < len; i++) {
		document.forms[0][SelectControl].options[i].selected = false;
	}

}

function SetEnabledStatusCron() {

	if (document.forms[0].chkCronEnabled.checked == false) {
		document.getElementById('txthCronEnabled').value ='0';
		}
		
	else {
		document.getElementById('txthCronEnabled').value ='1';
		}
		SetConfigChanged();
}

function RemoveCrontabEntry(crontabentry) {
	document.location.href='$SCRIPT_NAME?remove_crontabentry='+crontabentry;
}


//--></script>
EOF

echo "<div id=\"EditWindow\" style=\"display:none;position:absolute;border-style: solid;background-color: white;padding: 5px;\">"
echo "<table style=\"text-align: left; font-size: 0.8em;\" border=\"0\" cellpadding=\"2\" cellspacing=\"1\" summary=\"@TR<<Crontab>>\">"
echo "<tr>"
echo "<td><strong>Minutes</strong></td>"
echo "<td><strong>Hours</strong></td>"
echo "<td><strong>Days</strong></td>"
echo "<td><strong>Months</strong></td>"
echo "<td><strong>Days of the week</strong></td>"
echo "</tr>"
echo "<tr>"
echo "<td>"
echo "<select id=\"sltMinutes\" size=\"8\" name=\"sltMinutes\" class=\"input\" style=\"width:160px;\" multiple=\"multiple\">"
echo "<option selected=\"selected\" value=\"\">every minute</option>"
i=0; while [ $i -le 59 ]; do echo "<option value=\"$i\">$i</option>"; i=$(($i+1)); done
echo "</select>"
echo "</td>"
echo "<td>"
echo "<select id=\"sltHours\" size=\"8\" name=\"sltHours\" class=\"input\" style=\"width:160px;\" multiple=\"multiple\">"
echo "<option selected=\"selected\" value=\"\">every hour</option>"
i=0; while [ $i -le 23 ]; do echo "<option value=\"$i\">$i</option>"; i=$(($i+1)); done
echo "</select>"
echo "</td>"
echo "<td>"
echo "<select id=\"sltDays\" size=\"8\" name=\"sltDays\" class=\"input\" style=\"width:160px;\" multiple=\"multiple\">"
echo "<option selected=\"selected\" value=\"\">every day</option>"
i=1; while [ $i -le 31 ]; do echo "<option value=\"$i\">$i</option>"; i=$(($i+1)); done
echo "</select>"
echo "</td>"
echo "<td>"
echo "<select id=\"sltMonths\" size=\"8\" name=\"sltMonths\" class=\"input\" style=\"width:160px;\" multiple=\"multiple\">"
echo "<option selected=\"selected\" value=\"\">every month</option>"
echo "<option value=\"1\">January</option>"
echo "<option value=\"2\">February</option>"
echo "<option value=\"3\">March</option>"
echo "<option value=\"4\">April</option>"
echo "<option value=\"5\">May</option>"
echo "<option value=\"6\">June</option>"
echo "<option value=\"7\">July</option>"
echo "<option value=\"8\">August</option>"
echo "<option value=\"9\">September</option>"
echo "<option value=\"10\">October</option>"
echo "<option value=\"11\">November</option>"
echo "<option value=\"12\">December</option>"
echo "</select>"
echo "</td>"
echo "<td>"
echo "<select id=\"sltDaysOfWeek\" size=\"8\" name=\"sltDaysOfWeek\" class=\"input\" style=\"width:160px;\" multiple=\"multiple\">"
echo "<option selected=\"selected\" value=\"\">every day of the week</option>"
echo "<option value=\"0\">Sunday</option>"
echo "<option value=\"1\">Monday</option>"
echo "<option value=\"2\">Tuesday</option>"
echo "<option value=\"3\">Wednesday</option>"
echo "<option value=\"4\">Thursday</option>"
echo "<option value=\"5\">Friday</option>"
echo "<option value=\"6\">Saturday</option>"
echo "</select>"
echo "</td>"
echo "</tr>"
echo "<tr>"
echo "<td align=\"right\"><input type=\"hidden\" id=\"txthMinutes\" name=\"txthMinutes\" value=\"\" /><a href=\"javascript:SetAllSelect('sltMinutes');\">set all</a>&nbsp;&nbsp;<a href=\"javascript:SetControTopOption('sltMinutes');\">clear</a></td>"
echo "<td align=\"right\"><input type=\"hidden\" id=\"txthHours\" name=\"txthHours\" value=\"\" /><a href=\"javascript:SetAllSelect('sltHours');\">set all</a>&nbsp;&nbsp;<a href=\"javascript:SetControTopOption('sltHours');\">clear</a></td>"
echo "<td align=\"right\"><input type=\"hidden\" id=\"txthDays\" name=\"txthDays\" value=\"\" /><a href=\"javascript:SetAllSelect('sltDays');\">set all</a>&nbsp;&nbsp;<a href=\"javascript:SetControTopOption('sltDays');\">clear</a></td>"
echo "<td align=\"right\"><input type=\"hidden\" id=\"txthMonths\" name=\"txthMonths\" value=\"\" /><a href=\"javascript:SetAllSelect('sltMonths');\">set all</a>&nbsp;&nbsp;<a href=\"javascript:SetControTopOption('sltMonths');\">clear</a></td>"
echo "<td align=\"right\"><input type=\"hidden\" id=\"txthDaysOfWeek\" name=\"txthDaysOfWeek\" value=\"\" /><a href=\"javascript:SetAllSelect('sltDaysOfWeek');\">set all</a>&nbsp;&nbsp;<a href=\"javascript:SetControTopOption('sltDaysOfWeek');\">clear</a></td>"
echo "</tr>"
echo "<tr>"
echo "<td>Every"
echo "<select id=\"ddEveryXminute\" name=\"ddEveryXminute\">"
echo "<option selected=\"selected\" value=\"\">none</option>"
i=1; while [ $i -le 59 ]; do echo "<option value=\"$i\">$i</option>"; i=$(($i+1)); done
echo "</select>"
echo "th"
echo "</td>"
echo "<td>Every"
echo "<select id=\"ddEveryXhour\" name=\"ddEveryXhour\">"
echo "<option selected=\"selected\" value=\"\">none</option>"
i=0; while [ $i -le 23 ]; do echo "<option value=\"$i\">$i</option>"; i=$(($i+1)); done
echo "</select>"
echo "th"
echo "</td>"
echo "<td>Every"
echo "<select id=\"ddEveryXday\" name=\"ddEveryXday\">"
echo "<option selected=\"selected\" value=\"\">none</option>"
i=1; while [ $i -le 31 ]; do echo "<option value=\"$i\">$i</option>"; i=$(($i+1)); done
echo "</select>"
echo "th"
echo "</td>"
echo "<td>&nbsp;</td>"
echo "<td>&nbsp;</td>"
echo "</tr>"
echo "<tr><td colspan=\"5\">&nbsp;</td></tr>"
#echo "<tr><td align=\"right\"><strong>User</strong></td><td colspan=\"3\"><input style=\"width:400px;\" id=\"txtUsername\" type=\"text\" name=\"txtUsername\" value=\"\" /></td><td><a href=\"javascript:BuildInterval(true);\">generate crontab entry</a></td></tr>"
echo "<tr><td align=\"right\"><strong>Command</strong></td><td colspan=\"3\"><input style=\"width:400px;\" id=\"txtCommand\" type=\"text\" name=\"txtCommand\" value=\"\" /></td><td><a href=\"javascript:BuildInterval(true);\">generate crontab entry</a></td></tr>"
echo "<tr><td align=\"right\"><strong>Enabled</strong></td><td colspan=\"3\"><input type=\"checkbox\" id=\"chkCronEnabled\" name=\"chkCronEnabled\" onchange=\"SetEnabledStatusCron();\"/><input type=\"hidden\" id=\"txthCronEnabled\" name=\"txthCronEnabled\" value=\"0\" /></td><td><a href=\"javascript:ResetForm();\">reset form</a></td></tr>"
echo "<tr><td colspan=\"5\">&nbsp;</td></tr>"
echo "<tr><td align=\"right\"><strong>Crontab entry</strong></td><td colspan=\"4\"><input readonly=\"readonly\" style=\"width:400px;\" id=\"txtCrontabEntry\" type=\"text\" name=\"txtCrontabEntry\" value=\"\" /></td></tr>"
echo "<tr><td colspan=\"5\"></td></tr>"
echo "<tr><td colspan=\"3\">&nbsp;</td><td colspan=\"2\"><a href=\"javascript:HideContent('EditWindow','update');\">update crontab entry</a>&nbsp;&nbsp;&nbsp;<a href=\"javascript:HideContent('EditWindow','cancel')\">cancel</a></td></tr>"
echo "</table>"
echo "</div>"

echo "<h3><strong>@TR<<Crontab configuration root>></strong></h3>"
echo "<div id=\"WarningConfigChange\" style=\"left:300px;top:0px;display:none;position:absolute;border-style: solid;background-color: white;padding: 5px;\"><img width=\"17\" src=\"/images/warning.png\" alt=\"Configuration changed\" /> Configuration changed. Press 'Save Changes'</div>"
echo "<table style=\"width: 90%; margin-left: 2.5em; text-align: left; font-size: 0.8em;\" border=\"0\" cellpadding=\"2\" cellspacing=\"1\" summary=\"@TR<<Crontabs>>\">"

uci_load crontabs
cur_color="odd"

for crontab in $CRONTABS_cfg; do

	if  [ "$FORM_submit" = "" ]; then
		config_get FORM_MINUTES $crontab minutes
		config_get FORM_HOURS $crontab hours
		config_get FORM_DAYS $crontab days
		config_get FORM_MONTHS $crontab months
		config_get FORM_WEEKDAYS $crontab weekdays
		#config_get FORM_USER $crontab user
		config_get FORM_COMMAND $crontab command
		config_get FORM_ENABLED $crontab enabled
	else
		
		
		eval FORM_MINUTES="\$FORM_MINUTES_${crontab}"
		eval FORM_HOURS="\$FORM_HOURS_${crontab}"
		eval FORM_DAYS="\$FORM_DAYS_${crontab}"
		eval FORM_MONTHS="\$FORM_MONTHS_${crontab}"
		eval FORM_WEEKDAYS="\$FORM_WEEKDAYS_${crontab}"
		#eval FORM_USER="\$FORM_USER_${crontab}"
		eval FORM_COMMAND="\$FORM_COMMAND_${crontab}"
		eval FORM_ENABLED="\$FORM_ENABLED_${crontab}"

		if [ "$FORM_MINUTES" != "" ]; then
		
			uci_set "crontabs" "$crontab" "minutes" "$FORM_MINUTES"
			uci_set "crontabs" "$crontab" "hours" "$FORM_HOURS"
			uci_set "crontabs" "$crontab" "days" "$FORM_DAYS"
			uci_set "crontabs" "$crontab" "months" "$FORM_MONTHS"
			uci_set "crontabs" "$crontab" "weekdays" "$FORM_WEEKDAYS"
			#uci_set "crontabs" "$crontab" "user" "$FORM_USER"
			uci_set "crontabs" "$crontab" "command" "$FORM_COMMAND"
			uci_set "crontabs" "$crontab" "enabled" "$FORM_ENABLED"
		else
			config_get FORM_MINUTES $crontab minutes
			config_get FORM_HOURS $crontab hours
			config_get FORM_DAYS $crontab days
			config_get FORM_MONTHS $crontab months
			config_get FORM_WEEKDAYS $crontab weekdays
			#config_get FORM_USER $crontab user
			config_get FORM_COMMAND $crontab command
			config_get FORM_ENABLED $crontab enabled
		fi
	fi

	#check if crontab is enabled
	if [ "$FORM_ENABLED" = "1" ]; then
		ENABLEDIMAGE="<img width=\"17\" src=\"/images/service_enabled.png\" alt=\"Cron entry Enabled\" />"
		CRON_ENABLED="Yes"
	else
		ENABLEDIMAGE="<img width=\"17\" src=\"/images/service_disabled.png\" alt=\"Cron entry Disabled\" />"
		CRON_ENABLED="No"
	fi

	FORM_escCOMMAND=`echo "$FORM_COMMAND" | sed -e 's/\&/\&amp;/g' -e 's/"/\&quot;/g' -e 's/</\&lt;/g' -e 's/>/\&gt;/g'` 	
	
	get_tr
	echo $tr"<td width=\"35\" align=\"center\" valign=\"middle\" rowspan=\"7\">$ENABLEDIMAGE</td><td width=\"100\"><strong>Minutes</strong></td><td>$FORM_MINUTES</td><td width=\"35\" align=\"center\" valign=\"middle\" rowspan=\"7\"><a href=\"javascript:OpenEditWindow('EditWindow','$crontab')\">@TR<<edit>></a></td><td width=\"35\" align=\"center\" valign=\"middle\" rowspan=\"7\"><a href=\"javascript:RemoveCrontabEntry('$crontab')\">@TR<<remove>></a></td><td><input id=\"MINUTES_$crontab\" type=\"hidden\" name=\"MINUTES_$crontab\" value=\"$FORM_MINUTES\" /></td></tr>"
	echo $tr"<td width=\"100\"><strong>Hours</strong></td><td>$FORM_HOURS</td><td><input id=\"HOURS_$crontab\" type=\"hidden\" name=\"HOURS_$crontab\" value=\"$FORM_HOURS\" /></td></tr>"
	echo $tr"<td width=\"100\"><strong>Days</strong></td><td>$FORM_DAYS</td><td><input id=\"DAYS_$crontab\" type=\"hidden\" name=\"DAYS_$crontab\" value=\"$FORM_DAYS\" /></td></tr>"
	echo $tr"<td width=\"100\"><strong>Months</strong></td><td>$FORM_MONTHS</td><td><input id=\"MONTHS_$crontab\" type=\"hidden\" name=\"MONTHS_$crontab\" value=\"$FORM_MONTHS\" /></td></tr>"
	echo $tr"<td width=\"100\"><strong>Weekdays</strong></td><td>$FORM_WEEKDAYS</td><td><input id=\"WEEKDAYS_$crontab\" type=\"hidden\" name=\"WEEKDAYS_$crontab\" value=\"$FORM_WEEKDAYS\" /></td></tr>"
	#echo $tr"<td width=\"100\"><strong>User</strong></td><td>$FORM_USER</td><td><input id=\"USER_$crontab\" type=\"hidden\" name=\"USER_$crontab\" value=\"$FORM_USER\" /></td></tr>"
	echo $tr"<td width=\"100\"><strong>Command</strong></td><td>$FORM_escCOMMAND</td><td><input id=\"COMMAND_$crontab\" type=\"hidden\" name=\"COMMAND_$crontab\" value=\"$FORM_escCOMMAND\" /></td></tr>"
	echo $tr"<td width=\"100\"><strong>Enabled</strong></td><td>$CRON_ENABLED</td><td><input id=\"ENABLED_$crontab\" type=\"hidden\" name=\"ENABLED_$crontab\" value=\"$FORM_ENABLED\" /></td></tr>"
	echo "<tr><td colspan=\"6\"><img alt=\"\" height=\"5\" width=\"1\" src=\"/images/pixel.gif\" /></td></tr>"
	
done

echo "<tr><td colspan=\"3\">&nbsp;</td><td>&nbsp;</td><td align=\"center\"><a href=\"javascript:OpenEditWindow('EditWindow','newCron')\">@TR<<add>></a></td><td></td></tr>"

echo "</table>"


echo "<input id=\"MINUTES_newCron\" type=\"hidden\" name=\"MINUTES_newCron\" value=\"\" />"
echo "<input id=\"HOURS_newCron\" type=\"hidden\" name=\"HOURS_newCron\" value=\"\" />"
echo "<input id=\"DAYS_newCron\" type=\"hidden\" name=\"DAYS_newCron\" value=\"\" />"
echo "<input id=\"MONTHS_newCron\" type=\"hidden\" name=\"MONTHS_newCron\" value=\"\" />"
echo "<input id=\"WEEKDAYS_newCron\" type=\"hidden\" name=\"WEEKDAYS_newCron\" value=\"\" />"
#echo "<input id=\"USER_newCron\" type=\"hidden\" name=\"USER_newCron\" value=\"\" />"
echo "<input id=\"COMMAND_newCron\" type=\"hidden\" name=\"COMMAND_newCron\" value=\"\" />"
echo "<input id=\"ENABLED_newCron\" type=\"hidden\" name=\"ENABLED_newCron\" value=\"\" />"


footer %>
<!--
##WEBIF:name:System:130:Crontabs
-->
