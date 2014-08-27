#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
# todo:
#  add /enable/disable for mx and wildcard / connection type.

config_cb() {
        local cfg_type="$1"
        local cfg_name="$2"
        case "$cfg_type" in
                updatedd)
                        cfg="$cfg_name"
                ;;
        esac
}

config_load "updatedd"
if [ "$cfg" = "" ]; then
	touch /etc/config/updatedd
	uci_add updatedd "updatedd"
	cfg="$CONFIG_SECTION"
fi
if empty "$FORM_submit"; then
	config_get FORM_service "$cfg" service 
	config_get FORM_username "$cfg" username
	config_get FORM_password "$cfg" password
	config_get FORM_host "$cfg" host
	config_get_bool FORM_update "$cfg" update 0
else
	SAVED=1
	validate <<EOF
string|FORM_service|@TR<<Service Type>>|required|$FORM_service
string|FORM_username|@TR<<User Name>>|required|$FORM_username
string|FORM_password|@TR<<Password>>|required|$FORM_password
string|FORM_host|@TR<<Host Name>>|required|$FORM_host
EOF
	equal "$?" 0 && {
		uci_set updatedd "$cfg" update "$FORM_update"
		uci_set updatedd "$cfg" service "$FORM_service"
		uci_set updatedd "$cfg" username "$FORM_username"
		uci_set updatedd "$cfg" password "$FORM_password"
		uci_set updatedd "$cfg" host "$FORM_host"
	}
fi

header "Network" "DynDNS" "@TR<<DynDNS Settings>>" '' "$SCRIPT_NAME"

#define supported services
services="changeip dyndns eurodyndns ovh noip ods hn regfish tzo zoneedit"

#generate fields for supported services
for service in $services; do
	service_option="$service_option
option|$service"
	
	opkg list_installed | grep -q "$service"
	! equal "$?" 0 && {
		package_checker="$package_checker
field|@TR<<Dynamic DNS Package>>|install_$service|hidden
string|<div class=\"warning\">$service will not work until you install the $service package. </div>
submit|install_$service|@TR<<Install>> $service @TR<<Package>>|"

		js="$js
v = isset('service','$service');
set_visible('install_$service', v);"

		eval FORM_installer="\$FORM_install_$service"

		if ! empty "$FORM_installer"; then
			echo "Installing $service package ...<pre>"
			install_package "updatedd-mod-$service"
			echo "</pre>"
		fi
	}
done

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
start_form|@TR<<DynDNS>>
field|@TR<<Dynamic DNS Update>>
radio|update|$FORM_update|1|@TR<<Enable>>
radio|update|$FORM_update|0|@TR<<Disable>>
field|@TR<<Service Type>>
select|service|$FORM_service
$service_option
$package_checker
end_form

start_form|@TR<<Account>>
field|@TR<<User Name>>
text|username|$FORM_username
field|@TR<<Password>>
password|password|$FORM_password
end_form

start_form|@TR<<Host>>
field|@TR<<Host Name>>
text|host|$FORM_host
end_form
EOF

footer ?>
<!--
##WEBIF:name:Network:651:DynDNS
-->
