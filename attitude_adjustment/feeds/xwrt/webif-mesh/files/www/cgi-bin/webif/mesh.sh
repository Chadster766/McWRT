#!/usr/bin/webif-page "-U /tmp -u 4096"
<? 

. /usr/lib/webif/webif.sh
uci_load "mesh"

header "Mesh" "Start" "@TR<<Mesh Start>>" ' onload="modechange()" ' "$SCRIPT_NAME"

echo "<h5><p>Mesh networks are a revolutionary networking architecture that allows direct connection between users.<br>Remember, once mesh mode is enabled, some networking options are forced.</p></h5>"

if equal "$mesh_installed" "1" ; then
	echo "TODO: write the basic options form"
else
	echo "<p><h5>Please choose one of the following mesh technologies.</p></h5><br>"

#####################################################################
# install OLSR daemon and Webif-olsr if asked

	if ! empty "$FORM_install_olsrd"; then
		tmpfile=$(mktemp "/tmp/.webif_olsrd-XXXXXX")
		echo "Installing OLSRD package ...<pre>"
		install_package "olsrd"
		install_package "olsrd-mod-dyn-gw"
		install_package "olsrd-mod-httpinfo"
		install_package "olsrd-mod-nameservice"
		install_package "olsrd-mod-secure"
		install_package "olsrd-mod-tas"
		echo "</pre>"
	fi

	if ! empty "$FORM_install_webifolsr"; then
		tmpfile=$(mktemp "/tmp/.webif_webifolsr-XXXXXX")
		echo "Installing webif-olsr package ...<pre>"
		install_package "webif-olsr"
		echo "</pre>"
	fi

#####################################################################
# check that OLSR daemon and Webif-olsr are installed or prepare install button

	install_olsrd_button=""
	! is_package_installed "olsrd" && install_olsrd_button="field|@TR<<OLSR daemon>>
		submit|install_olsrd| @TR<<Install>> |"

	install_webifolsr_button=""
	! is_package_installed "webif-olsr" && install_webifolsr_button="field|@TR<<X-Wrt OLSR page>>
		submit|install_webifolsr| @TR<<Install>> |"
	
	install_olsrd_help="helpitem|OLSR 
	helptext|HelpText install_mesh_help#OLSR is a pretty mature routing protocol mainly developed at Berlin Freifunk project and some other german wireless communities."

#####################################################################
# install BATMAN daemon and Webif-batman if asked

	if ! empty "$FORM_install_batman"; then
		tmpfile=$(mktemp "/tmp/.webif_batman-XXXXXX")
		echo "Installing BATMAN package ...<pre>"
		install_package "batman"
		echo "</pre>"
	fi

	if ! empty "$FORM_install_webifbatman"; then
		tmpfile=$(mktemp "/tmp/.webif_webifbatman-XXXXXX")
		echo "Installing webif-batman package ...<pre>"
		install_package "webif-batman"
		echo "</pre>"
	fi

#####################################################################
# check that BATMAN daemon and Webif-batman are installed or prepare install button

	install_batman_button=""
	! is_package_installed "batman" && install_batman_button="field|@TR<<BATMAN daemon>>
		submit|install_batman| @TR<<Install>> |"
	
	install_webifbatman_button=""
	! is_package_installed "webif-batman" && install_webifbatman_button="field|@TR<<X-Wrt BATMAN page>>
		submit|install_webifbatman| @TR<<Install>> |"

	install_batman_help="helpitem|BATMAN
	helptext|HelpText install_mesh_help#Batman is the young protocol born to supersede OLSR."

#####################################################################
# install Netsukuku daemon and Webif-netsukuku if asked

	if ! empty "$FORM_install_netsukuku"; then
		tmpfile=$(mktemp "/tmp/.webif_netsukuku-XXXXXX")
		echo "Installing Netsukuku package ...<pre>"
		install_package "netsukuku"
		echo "</pre>"
	fi

	if ! empty "$FORM_install_webifnetsukuku"; then
		tmpfile=$(mktemp "/tmp/.webif_webifnetsukuku-XXXXXX")
		echo "Installing webif-netsukuku package ...<pre>"
		install_package "webif-netsukuku"
		echo "</pre>"
	fi

#####################################################################
# check that Netsukuku daemon and Webif-netsukuku are installed or prepare install button

	install_netsukuku_button=""
	! is_package_installed "netsukuku" && install_netsukuku_button="field|@TR<<Netsukuku daemon>>
		submit|install_netsukuku| @TR<<Install>> |"

	install_webifnetsukuku_button=""
	! is_package_installed "webif-netsukuku" && install_webifnetsukuku_button="field|@TR<<X-Wrt Netsukuku page>>
		submit|install_webifnetsukuku| @TR<<Install>> |"
	
	install_netsukuku_help="helpitem|Netsukuku
	helptext|HelpText install_mesh_help#Netsukuku is a completely decentralized fractal protocol developed by the italian Freaknet Medialab."

#####################################################################
# install Webif-meganetwork if asked

	if ! empty "$FORM_install_webifmeganetwork"; then
		tmpfile=$(mktemp "/tmp/.webif_webifmeganetwork-XXXXXX")
		echo "Installing Webif-meganetwork package ...<pre>"
		install_package "webif-meganetwork"
		echo "</pre>"
	fi

#####################################################################
# check that Webif-meganetwork is installed or prepare install button

	install_webifmeganetwork_button=""
	! is_package_installed "webif-meganetwork" && install_webifmeganetwork_button="field|@TR<<webif-meganetwork>>
	submit|install_webifmeganetwork| @TR<<Install>> |"

	install_meganetwork_help="helpitem|Meganetwork 
	helptext|HelpText install_mesh_help#Meganetwork is an easy to use customized OLSR mesh platform."
fi

display_form <<EOF
onchange|modechange

start_form|@TR<<OLSR - Optimized Link-state Routing>>
$install_olsrd_button
$(empty "$install_olsrd_button" && echo 'string|@TR<<OLSR daemon installed>>')
string|<br>
$install_webifolsr_button
$(empty "$install_webifolsr_button" && echo 'string|</td></tr><tr><td>@TR<<X-Wrt OLSR page installed>></td></tr>')
$install_olsrd_help
end_form

start_form|@TR<<BATMAN - Better Approach To Mobile Ad-Hoc Networking>>
$install_batman_button
$(empty "$install_batman_button" && echo 'string|@TR<<BATMAN daemon installed>>')
string|<br>
$install_webifbatman_button
$(empty "$install_webifbatman_button" && echo 'string|</td></tr><tr><td>@TR<<X-Wrt BATMAN page installed>></td></tr>')
$install_batman_help
end_form

start_form|@TR<<Netsukuku>>
$install_netsukuku_button
$(empty "$install_netsukuku_button" && echo 'string|@TR<<Netsukuku daemon installed>>')
string|<br>
$install_webifnetsukuku_button
$(empty "$install_webifnetsukuku_button" && echo 'string|</td></tr><tr><td>@TR<<X-Wrt Netsukuku page installed>></td></tr>')
$install_netsukuku_help
end_form

start_form|@TR<<Meganetwork>>
$install_webifmeganetwork_button
$(empty "$install_webifmeganetwork_button" && echo 'string|@TR<<X-Wrt Meganetwork page installed>><br>')
$install_meganetwork_button
$install_meganetwork_help
end_form

EOF

footer ?>
<!--
##WEBIF:name:Mesh:100:Start
-->
