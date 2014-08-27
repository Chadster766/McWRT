#!/usr/bin/webif-page
<?
###################################################################
# GetData
#
# Description:
#		Gets data for the dynamic header
#
# Author(s) [in order of work date]:
#       m4rc0 <jansssenmaj@gmail.com>
#
# Major revisions:
#		Initial release 2008-11-29
#
# NVRAM variables referenced:
#       none
#
# Configuration files referenced:
#       none
#
# Required components:
# 

_uptime="$(uptime)"
_loadavg="${_uptime#*load average: }"
_uptime="${_uptime#*up }"
_uptime="${_uptime%%, load *}"
echo "`date +%T`|`date +%F`|$_uptime|$_loadavg"
?>
