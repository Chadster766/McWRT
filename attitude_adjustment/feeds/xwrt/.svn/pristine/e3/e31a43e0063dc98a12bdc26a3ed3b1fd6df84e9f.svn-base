#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
###################################################################
# fon heartbeat page
#
# Description:
#
# Author(s) [in order of work date]:
#       m4rc0 <jansssenmaj@gmail.com>
#
# Major revisions:
#		Initial release 2008-11-20
#
# NVRAM variables referenced:
#       none
#
# Configuration files referenced:
#       /etc/fonheartbeat
#
# Required components:
# 

config_cb() {
	local cfg_type="$1"
	local cfg_name="$2"

	case "$cfg_type" in
		fonheartbeat)
			heartbeat_cfg="$cfg_name"
		;;
	esac
}

uci_load fonheartbeat


header "Network" "FON heartbeat" "@TR<<FON Heartbeat>>" '' "$SCRIPT_NAME"


cat <<EOF

<script type="text/javascript" language="JavaScript"><!--
 
var cX = 0; var cY = 0; var rX = 0; var rY = 0; ConfigChanged = false; ActiveProperty='';

function UpdateCursorPosition(e){ cX = e.pageX; cY = e.pageY;}
function UpdateCursorPositionDocAll(e){ cX = event.clientX; cY = event.clientY;}

if(document.all) { document.onmousemove = UpdateCursorPositionDocAll; }
else { document.onmousemove = UpdateCursorPosition; }

function AssignPosition(d) {
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

d.style.left = (cX-150) + "px";
d.style.top = (cY-210) + "px";

}

function OpenEditWindow(d,PROPERTYTEXT,PROPERTY,ShowCheckBox) {
if(d.length < 1) { return; }

document.getElementById('txtPropertyValue').value = document.getElementById(PROPERTY).value;
document.getElementById('PropertyText').innerHTML = PROPERTYTEXT;

ActiveProperty = PROPERTY;

if (ShowCheckBox == true) {
	
		document.getElementById('ShowTextBox').style.display = "none";
		document.getElementById('ShowCheckBox').style.display = "block";
		}
	else {
		document.getElementById('ShowTextBox').style.display = "block";
		document.getElementById('ShowCheckBox').style.display = "none";
		}


var dd = document.getElementById(d);
AssignPosition(dd);
dd.style.display = "block";

}

function SetConfigChanged() {
	
	ConfigChanged = true;
	
	}


function HideContent(d,doAction) {
if(d.length < 1) { return; }
document.getElementById(d).style.display = "none";

if (doAction == 'update' && ConfigChanged == true ){
	document.getElementById(ActiveProperty).value = document.getElementById('txtPropertyValue').value;
	document.getElementById('WarningConfigChange').style.display = "block";
	}
}

Number.prototype.numToHex = function(){
 return this.toString(16).toUpperCase();
}

String.prototype.hexToNum = function(){
 return parseInt(this, 16);
}

function SetUpMacAddress() {

	var MAC='$(/sbin/ifconfig eth0 2>/dev/null | grep HWaddr | cut -b39-|sed 's/[ \t]*$//')';

	arr_mac_octets = MAC.split(":");

	var last_octet_dec = arr_mac_octets[5].hexToNum();
	last_octet_dec = (last_octet_dec + 1).numToHex();

	var WLMAC=arr_mac_octets[0]+':'+arr_mac_octets[1]+':'+arr_mac_octets[2]+':'+arr_mac_octets[3]+':'+arr_mac_octets[4]+':'+last_octet_dec;

	document.getElementById('MAC_$heartbeat_cfg').value = MAC;
	document.getElementById('MACText').innerHTML = MAC;

	document.getElementById('WLMAC_$heartbeat_cfg').value = WLMAC;
	document.getElementById('WLMACText').innerHTML = WLMAC;
	
	ConfigChanged = true;
	document.getElementById('WarningConfigChange').style.display = "block";
	}

function SetEnabledStatus() {

	if (document.forms[0].HeartBeatEnable.checked == false) {
		document.getElementById('txtPropertyValue').value ='0';
		}
		
	else {
		document.getElementById('txtPropertyValue').value ='1';
		}

	ConfigChanged = true;
	}

	
//--></script>
EOF

if  [ "$FORM_submit" = "" ]; then
	config_get FORM_MAC $heartbeat_cfg mac
	config_get FORM_WLMAC $heartbeat_cfg wlmac
	config_get FORM_FONREV $heartbeat_cfg fonrev
	config_get FORM_FIRMWARE $heartbeat_cfg firmware
	config_get FORM_CHILLVER $heartbeat_cfg chillver
	config_get FORM_THCLVER $heartbeat_cfg thclver
	config_get FORM_DEVICE $heartbeat_cfg device
	config_get FORM_FONKEY $heartbeat_cfg fonkey
	config_get FORM_PORT $heartbeat_cfg port
	config_get FORM_SERVER $heartbeat_cfg server
	config_get FORM_USER $heartbeat_cfg user
	config_get FORM_ENABLED $heartbeat_cfg enabled
else
	eval FORM_MAC="\$FORM_MAC_${heartbeat_cfg}"
	eval FORM_WLMAC="\$FORM_WLMAC_${heartbeat_cfg}"
	eval FORM_FONREV="\$FORM_FONREV_${heartbeat_cfg}"
	eval FORM_FIRMWARE="\$FORM_FIRMWARE_${heartbeat_cfg}"
	eval FORM_CHILLVER="\$FORM_CHILLVER_${heartbeat_cfg}"
	eval FORM_THCLVER="\$FORM_THCLVER_${heartbeat_cfg}"
	eval FORM_DEVICE="\$FORM_DEVICE_${heartbeat_cfg}"
	eval FORM_FONKEY="\$FORM_FONKEY_${heartbeat_cfg}"
	eval FORM_PORT="\$FORM_PORT_${heartbeat_cfg}"
	eval FORM_SERVER="\$FORM_SERVER_${heartbeat_cfg}"
	eval FORM_USER="\$FORM_USER_${heartbeat_cfg}"
	eval FORM_ENABLED="\$FORM_ENABLED_${heartbeat_cfg}"

	if [ "$FORM_MAC" != "" ]; then
		uci_set "fonheartbeat" "$heartbeat_cfg" "mac" "$FORM_MAC"
		uci_set "fonheartbeat" "$heartbeat_cfg" "wlmac" "$FORM_WLMAC"
		uci_set "fonheartbeat" "$heartbeat_cfg" "fonrev" "$FORM_FONREV"
		uci_set "fonheartbeat" "$heartbeat_cfg" "firmware" "$FORM_FIRMWARE"
		uci_set "fonheartbeat" "$heartbeat_cfg" "chillver" "$FORM_CHILLVER"
		uci_set "fonheartbeat" "$heartbeat_cfg" "thclver" "$FORM_THCLVER"
		uci_set "fonheartbeat" "$heartbeat_cfg" "device" "$FORM_DEVICE"
		uci_set "fonheartbeat" "$heartbeat_cfg" "fonkey" "$FORM_FONKEY"
		uci_set "fonheartbeat" "$heartbeat_cfg" "port" "$FORM_PORT"
		uci_set "fonheartbeat" "$heartbeat_cfg" "server" "$FORM_SERVER"
		uci_set "fonheartbeat" "$heartbeat_cfg" "user" "$FORM_USER"
		uci_set "fonheartbeat" "$heartbeat_cfg" "enabled" "$FORM_ENABLED"
	else
		config_get FORM_MAC $heartbeat_cfg mac
		config_get FORM_WLMAC $heartbeat_cfg wlmac
		config_get FORM_FONREV $heartbeat_cfg fonrev
		config_get FORM_FIRMWARE $heartbeat_cfg firmware
		config_get FORM_CHILLVER $heartbeat_cfg chillver
		config_get FORM_THCLVER $heartbeat_cfg thclver
		config_get FORM_DEVICE $heartbeat_cfg device
		config_get FORM_FONKEY $heartbeat_cfg fonkey
		config_get FORM_PORT $heartbeat_cfg port
		config_get FORM_SERVER $heartbeat_cfg server
		config_get FORM_USER $heartbeat_cfg user
		config_get FORM_ENABLED $heartbeat_cfg enabled
	fi
fi

if [ "$FORM_ENABLED" = "1" ]; then
	StatusCheckBox="checked=\"checked\""
	StatusEnabled="Yes"
else
	StatusCheckBox=""
	StatusEnabled="No"
fi


echo "<h3><strong>@TR<<Heartbeat configuration>></strong></h3>"
echo "<div id=\"WarningConfigChange\" style=\"left:300px;top:0px;display:none;position:absolute;border-style: solid;background-color: white;padding: 5px;\"><img width=\"17\" src=\"/images/warning.png\" alt=\"Configuration changed\" /> Configuration changed. Press 'Save Changes'</div>"
echo "<table style=\"width: 90%; margin-left: 2.5em; text-align: left; font-size: 0.8em;\" border=\"0\" cellpadding=\"2\" cellspacing=\"1\" summary=\"@TR<<Mountpoints>>\">"
echo "<tr><td width=\"200\"><strong>MAC address</strong></td><td id='MACText'>$FORM_MAC</td><td align=\"center\" width=\"45\"><a href=\"javascript:SetUpMacAddress();\">setup</a></td><td width=\"35\" align=\"center\" valign=\"middle\"><a href=\"javascript:OpenEditWindow('EditWindow','MAC address','MAC_$heartbeat_cfg',false)\">@TR<<edit>></a></td><td><input id=\"MAC_$heartbeat_cfg\" type=\"hidden\" name=\"MAC_$heartbeat_cfg\" value=\"$FORM_MAC\" /></td></tr>"
echo "<tr class=\"odd\"><td width=\"200\"><strong>Wireless MAC address</strong></td><td id='WLMACText'>$FORM_WLMAC</td><td>&nbsp;</td><td width=\"35\" align=\"center\" valign=\"middle\"><a href=\"javascript:OpenEditWindow('EditWindow','Wireless MAC address','WLMAC_$heartbeat_cfg',false)\">@TR<<edit>></a></td><td><input id=\"WLMAC_$heartbeat_cfg\" type=\"hidden\" name=\"WLMAC_$heartbeat_cfg\" value=\"$FORM_WLMAC\" /></td></tr>"
echo "<tr><td width=\"200\"><strong>FON revision</strong></td><td>$FORM_FONREV</td><td>&nbsp;</td><td width=\"35\" align=\"center\" valign=\"middle\"><a href=\"javascript:OpenEditWindow('EditWindow','FON revision','FONREV_$heartbeat_cfg',false)\">@TR<<edit>></a></td><td><input id=\"FONREV_$heartbeat_cfg\" type=\"hidden\" name=\"FONREV_$heartbeat_cfg\" value=\"$FORM_FONREV\" /></td></tr>"
echo "<tr class=\"odd\"><td width=\"200\"><strong>Firmware version</strong></td><td>$FORM_FIRMWARE</td><td>&nbsp;</td><td width=\"35\" align=\"center\" valign=\"middle\"><a href=\"javascript:OpenEditWindow('EditWindow','Firmware version','FIRMWARE_$heartbeat_cfg',false)\">@TR<<edit>></a></td><td><input id=\"FIRMWARE_$heartbeat_cfg\" type=\"hidden\" name=\"FIRMWARE_$heartbeat_cfg\" value=\"$FORM_FIRMWARE\" /></td></tr>"
echo "<tr><td width=\"200\"><strong>CHILL version</strong></td><td>$FORM_CHILLVER</td><td>&nbsp;</td><td width=\"35\" align=\"center\" valign=\"middle\"><a href=\"javascript:OpenEditWindow('EditWindow','CHILL version','CHILLVER_$heartbeat_cfg',false)\">@TR<<edit>></a></td><td><input id=\"CHILLVER_$heartbeat_cfg\" type=\"hidden\" name=\"CHILLVER_$heartbeat_cfg\" value=\"$FORM_CHILLVER\" /></td></tr>"
echo "<tr class=\"odd\"><td width=\"200\"><strong>THCL version</strong></td><td>$FORM_THCLVER</td><td>&nbsp;</td><td width=\"35\" align=\"center\" valign=\"middle\"><a href=\"javascript:OpenEditWindow('EditWindow','THCL version','THCLVER_$heartbeat_cfg',false)\">@TR<<edit>></a></td><td><input id=\"THCLVER_$heartbeat_cfg\" type=\"hidden\" name=\"THCLVER_$heartbeat_cfg\" value=\"$FORM_THCLVER\" /></td></tr>"
echo "<tr><td width=\"200\"><strong>Device</strong></td><td>$FORM_DEVICE</td><td>&nbsp;</td><td width=\"35\" align=\"center\" valign=\"middle\"><a href=\"javascript:OpenEditWindow('EditWindow','Device','DEVICE_$heartbeat_cfg',false)\">@TR<<edit>></a></td><td><input id=\"DEVICE_$heartbeat_cfg\" type=\"hidden\" name=\"DEVICE_$heartbeat_cfg\" value=\"$FORM_DEVICE\" /></td></tr>"
echo "<tr class=\"odd\"><td width=\"200\"><strong>FON key</strong></td><td>$FORM_FONKEY</td><td>&nbsp;</td><td width=\"35\" align=\"center\" valign=\"middle\"><a href=\"javascript:OpenEditWindow('EditWindow','FON key','FONKEY_$heartbeat_cfg',false)\">@TR<<edit>></a></td><td><input id=\"FONKEY_$heartbeat_cfg\" type=\"hidden\" name=\"FONKEY_$heartbeat_cfg\" value=\"$FORM_FONKEY\" /></td></tr>"
echo "<tr><td width=\"200\"><strong>Port</strong></td><td>$FORM_PORT</td><td>&nbsp;</td><td width=\"35\" align=\"center\" valign=\"middle\"><a href=\"javascript:OpenEditWindow('EditWindow','Port','PORT_$heartbeat_cfg',false)\">@TR<<edit>></a></td><td><input id=\"PORT_$heartbeat_cfg\" type=\"hidden\" name=\"PORT_$heartbeat_cfg\" value=\"$FORM_PORT\" /></td></tr>"
echo "<tr class=\"odd\"><td width=\"200\"><strong>Server</strong></td><td>$FORM_SERVER</td><td>&nbsp;</td><td width=\"35\" align=\"center\" valign=\"middle\"><a href=\"javascript:OpenEditWindow('EditWindow','Server','SERVER_$heartbeat_cfg',false)\">@TR<<edit>></a></td><td><input id=\"SERVER_$heartbeat_cfg\" type=\"hidden\" name=\"SERVER_$heartbeat_cfg\" value=\"$FORM_SERVER\" /></td></tr>"
echo "<tr><td width=\"200\"><strong>User</strong></td><td>$FORM_USER</td><td>&nbsp;</td><td width=\"35\" align=\"center\" valign=\"middle\"><a href=\"javascript:OpenEditWindow('EditWindow','User','USER_$heartbeat_cfg',false)\">@TR<<edit>></a></td><td><input id=\"USER_$heartbeat_cfg\" type=\"hidden\" name=\"USER_$heartbeat_cfg\" value=\"$FORM_USER\" /></td></tr>"
echo "<tr class=\"odd\"><td width=\"200\"><strong>Enabled</strong></td><td>$StatusEnabled</td><td>&nbsp;</td><td width=\"35\" align=\"center\" valign=\"middle\"><a href=\"javascript:OpenEditWindow('EditWindow','Enabled','ENABLED_$heartbeat_cfg',true)\">@TR<<edit>></a></td><td><input id=\"ENABLED_$heartbeat_cfg\" type=\"hidden\" name=\"ENABLED_$heartbeat_cfg\" value=\"$FORM_ENABLED\" /></td></tr>"
echo "</table>"

echo "<div id=\"EditWindow\" style=\"display:none;position:absolute;border-style: solid;background-color: white;padding: 5px;\">"
echo "<table style=\"text-align: left; font-size: 0.8em;\" border=\"0\" cellpadding=\"2\" cellspacing=\"1\" summary=\"@TR<<swap>>\">"
echo "<tr>"
echo "<td width=\"170\"><strong id='PropertyText'>Dummy</strong></td>"
echo "<td colspan=\"2\"><div id=\"ShowTextBox\"><input type=\"text\" id=\"txtPropertyValue\" name=\"txtPropertyValue\" value=\"Dummy\" onChange=\"SetConfigChanged()\" /></div></td>"
echo "<td colspan=\"2\"><div id=\"ShowCheckBox\"><input $StatusCheckBox type=\"checkbox\" id=\"HeartBeatEnable\" name=\"HeartBeatEnable\" onchange=\"SetEnabledStatus();\" /></div></td>"
echo "</tr>"
echo "<tr>"
echo "<td colspan=\"2\"><a href=\"javascript:HideContent('EditWindow','update')\">@TR<<Update>></a></td>"
echo "<td colspan=\"2\"><a href=\"javascript:HideContent('EditWindow','cancel')\">@TR<<Cancel>></a></td>"
echo "</tr>"
echo "</table>"
echo "</div>"



footer ?>
<!--
##WEBIF:name:Network:800:Fon heartbeat
-->
