#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh

config_cb() {
	local cfg_type="$1"
	local cfg_name="$2"

	case "$cfg_type" in
		chilli)
			chilli_cfg="$cfg_name"
		;;
	esac
}

[ -n "$FORM_install_package" ] && unset FORM_submit

uci_load "hotspot"

if empty "$FORM_submit"; then
	eval "FORM_chilli_radiusserver1=\"\$CONFIG_${chilli_cfg}_radiusserver1\""
	eval "FORM_chilli_radiusserver2=\"\$CONFIG_${chilli_cfg}_radiusserver2\""
	eval "FORM_chilli_radiussecret=\"\$CONFIG_${chilli_cfg}_radiussecret\""
	eval "FORM_chilli_radiusauthport=\"\$CONFIG_${chilli_cfg}_radiusauthport\""
	eval "FORM_chilli_radiusacctport=\"\$CONFIG_${chilli_cfg}_radiusacctport\""
	eval "FORM_chilli_radiusnasid=\"\$CONFIG_${chilli_cfg}_radiusnasid\""
	eval "FORM_chilli_proxylisten=\"\$CONFIG_${chilli_cfg}_proxylisten\""
	eval "FORM_chilli_proxyport=\"\$CONFIG_${chilli_cfg}_proxyport\""
	eval "FORM_chilli_proxyclient=\"\$CONFIG_${chilli_cfg}_proxyclient\""
	eval "FORM_chilli_proxysecret=\"\$CONFIG_${chilli_cfg}_proxysecret\""
else
	SAVED=1
	validate <<EOF
string|FORM_chilli_radiusserver1|@TR<<hotspot_networking_RADIUS_Server_1#RADIUS Server 1||$FORM_chilli_radiusserver1
string|FORM_chilli_radiusserver2|@TR<<hotspot_networking_RADIUS_Server_2#RADIUS Server 2||$FORM_chilli_radiusserver2
string|FORM_chilli_radiussecret|@TR<<hotspot_networking_RADIUS_Secret#RADIUS Secret>>||$FORM_chilli_radiussecret
ports|FORM_chilli_radiusauthport|@TR<<hotspot_networking_RADIUS_Auth_Port#RADIUS Auth Port>>||$FORM_chilli_radiusauthport
ports|FORM_chilli_radiusacctport|@TR<<hotspot_networking_RADIUS Acct Port#RADIUS Acct Port>>||$FORM_chilli_radiusacctport
string|FORM_chilli_radiusnasid|@TR<<hotspot_networking_RADIUS_NAS_Id#RADIUS NAS Id>>||$FORM_chilli_radiusnasid
string|FORM_chilli_proxylisten|@TR<<hotspot_networking_Proxy_Listen#Proxy Listen>>||$FORM_chilli_proxylisten
string|FORM_chilli_proxyclient|@TR<<hotspot_networking_Proxy Client#Proxy Client>>||$FORM_chilli_proxyclient
string|FORM_chilli_proxyport|@TR<<hotspot_networking_Proxy_Port#Proxy Port>>||$FORM_chilli_proxyport
string|FORM_chilli_proxysecret|@TR<<hotspot_networking_Proxy_Secret#Proxy Secret>>||$FORM_chilli_proxysecret
EOF
	equal "$?" 0 && {
		[ "$chilli_cfg" = "" ] && {
			uci_add hotspot chilli
			uci_load "hotspot"
		}
		uci_set hotspot "$chilli_cfg" radiusserver1 "$FORM_chilli_radiusserver1"
		uci_set hotspot "$chilli_cfg" radiusserver2 "$FORM_chilli_radiusserver2"
		uci_set hotspot "$chilli_cfg" radiussecret "$FORM_chilli_radiussecret"
		uci_set hotspot "$chilli_cfg" radiusauthport "$FORM_chilli_radiusauthport"
		uci_set hotspot "$chilli_cfg" radiusacctport "$FORM_chilli_radiusacctport"
		uci_set hotspot "$chilli_cfg" radiusnasid "$FORM_chilli_radiusnasid"
		uci_set hotspot "$chilli_cfg" proxylisten "$FORM_chilli_proxylisten"
		uci_set hotspot "$chilli_cfg" proxyclient "$FORM_chilli_proxyclient"
		uci_set hotspot "$chilli_cfg" proxyport "$FORM_chilli_proxyport"
		uci_set hotspot "$chilli_cfg" proxysecret "$FORM_chilli_proxysecret"
	}
fi

header "HotSpot" "hotspot_networking_Networking#Networking" "@TR<<hotspot_networking_Network_Settings#Network Settings>>" '' "$SCRIPT_NAME"

if ! empty "$FORM_install_package"; then
	echo "@TR<<hotspot_common_Installing_package#Installing chillispot package>>...<pre>"
	install_package "chillispot"
	echo "</pre>"
fi

display_form <<EOF
start_form|@TR<<hotspot_networking_Network_Settings#Network Settings>>
EOF

is_package_installed chillispot
equal "$?" "0" && chillispot_installed="1"

if [ "$chillispot_installed" = "1" ]; then
	display_form <<EOF
field|
string|<div class=warning>@TR<<hotspot_common_package_required#HotSpot will not work until you install ChilliSpot>>:</div>
submit|install_package| @TR<<hotspot_common_install_package#Install ChilliSpot Package>> |
helplink|http://www.chillispot.org/
end_form
EOF
else
	display_form <<EOF
field|@TR<<hotspot_networking_RADIUS_Server_1#RADIUS Server 1>>
text|chilli_radiusserver1|$FORM_chilli_radiusserver1
field|@TR<<hotspot_networking_RADIUS_Server_2#RADIUS Server 2>>
text|chilli_radiusserver2|$FORM_chilli_radiusserver2
helpitem|hotspot_networking_RADIUS_Server#RADIUS Server
helptext|hotspot_networking_RADIUS_Server_helptext#Primary and Secondary RADIUS Server.
field|@TR<<hotspot_networking_RADIUS_Secret#RADIUS Secret>>
text|chilli_radiussecret|$FORM_chilli_radiussecret
helpitem|hotspot_networking_RADIUS_Secret#RADIUS Secret
helptext|hotspot_networking_RADIUS_Secret_helptext#RADIUS Shared Secret.
field|@TR<<hotspot_networking_RADIUS_Auth_Port#RADIUS Auth Port>>
text|chilli_radiusauthport|$FORM_chilli_radiusauthport
field|@TR<<hotspot_networking_RADIUS Acct Port#RADIUS Acct Port>>
text|chilli_radiusacctport|$FORM_chilli_radiusacctport
field|@TR<<hotspot_networking_RADIUS_NAS_Id#RADIUS NAS Id>>
text|chilli_radiusnasid|$FORM_chilli_radiusnasid
helpitem|hotspot_networking_RADIUS_NAS_Id#RADIUS NAS Id
helptext|hotspot_networking_RADIUS_NAS_Id_helptext#RADIUS NAS Id.
field|@TR<<hotspot_networking_Proxy_Listen#Proxy Listen>>
text|chilli_proxylisten|$FORM_chilli_proxylisten
helpitem|hotspot_networking_Proxy_Listen#Proxy Listen
helptext|hotspot_networking_Proxy_Listen_helptext#IP Address to listen to (advanced uses only).
field|@TR<<hotspot_networking_Proxy Client#Proxy Client>>
text|chilli_proxyclient|$FORM_chilli_proxyclient
helpitem|hotspot_networking_Proxy Client#Proxy Client
helptext|hotspot_networking_Proxy Client_helptext#Clients from which we accept RADIUS Requests.
field|@TR<<hotspot_networking_Proxy_Port#Proxy Port>>
text|chilli_proxyport|$FORM_chilli_proxyport
helpitem|hotspot_networking_Proxy_Port#Proxy Port
helptext|hotspot_networking_Proxy_Port_heltext#UDP port to listen to.
field|@TR<<hotspot_networking_Proxy_Secret#Proxy Secret>>
text|chilli_proxysecret|$FORM_chilli_proxysecret
helpitem|hotspot_networking_Proxy_Secret#Proxy Secret
helptext|hotspot_networking_Proxy_Secret_helptext#RADIUS Shared Secret to accept for all clients.
helplink|http://www.chillispot.org/
end_form
EOF
fi
footer ?>
<!--
##WEBIF:name:HotSpot:2:hotspot_networking_Networking#Networking
-->
