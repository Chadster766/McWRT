#!/usr/bin/webif-page
<%
. /usr/lib/webif/webif.sh
###################################################################
# Services
#
# Description:
#	Services control page.
#       This page enables the user to enable/disabled/start 
#		and stop the services in the directory /etc/init.d
#
# Author(s) [in order of work date]:
#       m4rc0 <jansssenmaj@gmail.com>
#
# Major revisions:
#       2008-11-08 - Initial release
#
# Configuration files referenced:
#       none
#
# Required components:
# 

header "System" "Services" "@TR<<Services>>" '' "$SCRIPT_NAME"


# change rowcolors
get_tr() {
	if equal "$cur_color" "odd"; then
		cur_color="even"
		echo "<tr>"
	else
		cur_color="odd"
		echo "<tr class=\"odd\">"
	fi
}

#check if a service with an action is selected
if [ "$FORM_service" != "" ] && [ "$FORM_action" != "" ]; then
	/etc/init.d/$FORM_service $FORM_action > /dev/null 2>&1
fi

cat <<EOF
<h3><strong>@TR<<Available services>></strong></h3>
<table style="width: 90%; margin-left: 2.5em; text-align: left; font-size: 0.8em;" border="0" cellpadding="2" cellspacing="1" summary="@TR<<Services>>">
<tr>
<td>

<table style="margin-left: 2.5em; text-align: left;" border="0" cellpadding="2" cellspacing="1" summary="@TR<<Services>>">
EOF

# set the color-switch
cur_color="odd"

#for each service in init.d.....
for service in $(ls /etc/init.d | grep -v "rcS") ; do
	# select the right color
	get_tr

	#check if current $service is in the rc.d list
	if [ -x /etc/rc.d/S??${service} -o -x /etc/rc.d/K??${service} ] ; then
		echo "<td><img width=\"17\" src=\"/images/service_enabled.png\" alt=\"Service Enabled\" /></td>"
	else
		echo "<td><img width=\"17\" src=\"/images/service_disabled.png\" alt=\"Service Disabled\" /></td>"
	fi
	cat <<EOF
<td>&nbsp;</td>
<td>${service}</td>
<td><img height="1" width="100" src="/images/pixel.gif" alt="" /></td>
<td><a href="system-services.sh?service=${service}&amp;action=enable"><img width="13" src="/images/service_enable.png" alt="Enable Service" /></a></td>
<td valign="middle"><a href="system-services.sh?service=${service}&amp;action=enable">@TR<<system_services_service_enable#Enable>></a></td>
<td><img height="1" width="5" src="/images/pixel.gif" alt="" /></td>
<td><a href="system-services.sh?service=${service}&amp;action=disable"><img width="13" src="/images/service_disable.png" alt="Disable Service" /></a></td>
<td valign="middle"><a href="system-services.sh?service=${service}&amp;action=disable">@TR<<system_services_service_disable#Disable>></a></td>
<td><img height="1" width="60" src="/images/pixel.gif" alt="" /></td>
<td><a href="system-services.sh?service=${service}&amp;action=start"><img width="13" src="/images/service_start.png" alt="Start Service" /></a></td>
<td valign="middle"><a href="system-services.sh?service=${service}&amp;action=start">@TR<<system_services_sevice_start#Start>></a></td>
<td><img height="1" width="5" src="/images/pixel.gif" alt="" /></td>
<td><a href="system-services.sh?service=${service}&amp;action=restart"><img width="13" src="/images/service_restart.png" alt="Restart Service" /></a></td>
<td valign="middle"><a href="system-services.sh?service=${service}&amp;action=restart">@TR<<system_services_service_restart#Restart>></a></td>
<td><img height="1" width="5" src="/images/pixel.gif" alt="" /></td>
<td><a href="system-services.sh?service=${service}&amp;action=stop"><img width="13" src="/images/service_stop.png" alt="Stop Service" /></a></td>
<td valign="middle"><a href="system-services.sh?service=${service}&amp;action=stop">@TR<<system_services_service_stop#Stop>></a></td>
</tr>
EOF
done

	cat <<EOF
</table>
</td>

<td valign="top">
<table style="margin-left: 2.5em; text-align: left;" border="0" cellpadding="2" cellspacing="1" summary="@TR<<Services>>">
<tr>
<td><img width="17" src="/images/service_enabled.png" alt="Service Enabled" /></td>
<td>@TR<<system_services_service_enabled#Service Enabled>></td>
</tr>
<tr>
<td><img width="17" src="/images/service_disabled.png" alt="Service Disabled" /></td>
<td>@TR<<system_services_service_disabled#Service Disabled>></td>
</tr>

<tr><td colspan="2">&nbsp;</td></tr>
EOF

#if there is a service and an action selected... display status
if [ "$FORM_service" != "" ] && [ "$FORM_action" != "" ]; then
	
	case "$FORM_action" in
		enable)		status="enabled";;
		disable)	status="disabled";;
		start)		status="started";;
		restart)	status="restarted";;
		stop)		status="stopped";;
	esac

	cat <<EOF
<tr>
<td colspan="2">

<strong>Service ${FORM_service} was ${status}</strong>
</td>
</tr>
EOF
fi

cat <<EOF
</table>
</td>

</tr>
</table>
EOF

footer %>
<!--
##WEBIF:name:System:140:Services
-->
