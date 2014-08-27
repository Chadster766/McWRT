#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
###################################################################
# download.sh
#
# Description:
#  Display processes status. It enables the user to send
#  a signal to the particular process.
#
# Author(s) [in order of work date]:
#  Lubos Stanek <lubek@users.berlios.de>
#
# Major revisions:
#

[ -n "$FORM_interval" ] || FORM_interval=20

! empty "$FORM_kill" && ! empty "$FORM_signal" && ! empty "$FORM_pid" && {
	err_kill=$(kill -$FORM_signal $FORM_pid 2>&1)
	! equal "$?" "0" && {
		ERROR="@TR<<status_processes_kill_error#Error in>> $err_kill"
	}
	FORM_interval=0
}

! empty "$FORM_refreshstop" && {
	FORM_interval=0
}

[ "$FORM_interval" -gt 0 ] >/dev/null 2>&1 && {
	meta_refresh="<meta http-equiv=\"refresh\" content=\"$FORM_interval;url=$SCRIPT_NAME?interval=$FORM_interval\" />"
}

header_inject_head=$(cat <<EOF
$meta_refresh

<script type="text/javascript">
<!--
function targetwindow(url) {
	var wasOpen  = false;
	var win = window.open(url);    
	return (typeof(win)=='object')?true:false;
}
-->
</script>

<style type="text/css">
<!--
#proctable table {
	width: 98%;
	margin-left: auto;
	margin-right: auto;
	text-align: left;
	font-size: 0.9em;
	border-style: none;
	border-spacing: 0;
}
#proctable td, th {
	padding-left: 0.2em;
	padding-right: 0.2em;
}
#proctable .number, .buttons {
	text-align: right;
}
#content .procwarn {
	position: relative;
	margin-left: 40%;
	text-align: right;
}
-->
</style>

EOF

)

header "Status" "Processes" "@TR<<status_processes_Running_Processes#Running Processes>>"

echo "<div class=\"settings\">"
echo "<form name=\"refresh\" method=\"post\" action=\"$SCRIPT_NAME\">"

[ "$FORM_interval" -gt 0 ] >/dev/null 2>&1 && {
	cat <<EOF
<input type="submit" value=" @TR<<status_processes_Stop_Refreshing#Stop Refreshing>> " name="refreshstop" />
@TR<<status_processes_Interval#Interval>>: $FORM_interval (@TR<<status_processes_in_seconds#in seconds>>)
EOF
} || {
	cat <<EOF
<input type="submit" value=" @TR<<status_processes_Auto_Refresh#Auto Refresh>> " name="refreshstart" />
@TR<<status_processes_Interval#Interval>>:

<select name="interval">
EOF
	for sec in $(seq 3 59); do
		[ "$sec" -eq 20 ] && {
			echo "<option value=\"$sec\" selected=\"selected\">$sec</option>"
		} || {
			echo "<option value=\"$sec\">$sec</option>"
		}
	done
	cat <<EOF
</select>
@TR<<status_processes_in_seconds#in seconds>>
EOF
}
echo "</form>"

[ "$FORM_interval" -gt 0 ] >/dev/null 2>&1 && {
	echo "<blockquote class=\"procwarn\">"
	echo "<p>@TR<<status_processes_moreinfo#For more information about fields <a href=\"#pslegend\">see the legend</a>...>></p>"
	echo "</blockquote>"
} || {
	echo "<blockquote class=\"procwarn\">"
	echo "<p><strong>@TR<<big_warning#WARNING>></strong>: @TR<<status_processes_warniinfo#Sending a signal to the application may result in the system malfunction! You should be pretty sure what you are doing before firing the button. <a href=\"#signallegend\">See the most used signal descriptions</a>...>></p>"
	echo "</blockquote>"
	signal_list=$(echo "1)SIGHUP|2)SIGINT|3)SIGQUIT|4)SIGILL|5)SIGTRAP|6)SIGABRT|7)SIGBUS|8)SIGFPE|9)SIGKILL|10)SIGUSR1|11)SIGSEGV|12)SIGUSR2|13)SIGPIPE|14)SIGALRM|15)SIGTERM|17)SIGCHLD|18)SIGCONT|19)SIGSTOP|20)SIGTSTP|21)SIGTTIN|22)SIGTTOU|23)SIGURG|24)SIGXCPU|25)SIGXFSZ|26)SIGVTALRM|27)SIGPROF|28)SIGWINCH|29)SIGIO|30)SIGPWR|31)SIGSYS" |
		 awk 'BEGIN{ RS="|"; FS=")" } { print "<option value=\"" $2 "\">" $2 " (" $1 ")</option>" }')
	signal_list="<select name=\"signal\">$signal_list</select>"
}
?>
<br />

<h3><strong>@TR<<status_processes_Processes_Status#Processes Status>></strong></h3>
<div id="proctable" class="proctable">
<table>
<tbody>
<?

proclist=$(echo -n "";ps); echo "$proclist" | grep -v "[p]s " |
	awk -v interval="$FORM_interval" -v siglist="$signal_list" -v url="$SCRIPT_NAME" '
BEGIN {
	odd=1
	tr_ind = ""
	td_ind = "\t"
}
function readcmdline(pid) {
	if (("/bin/cat /proc/" pid "/cmdline 2>/dev/null | tr \"\\0\" \" \"" | getline) > 0) return $0
	else return ""
}
{
	for (i=1; i<=NF; i++) {
		gsub(/^ */, "", $i)
		gsub(/ *$/, "", $i)
		gsub(/&/, "\\&amp;", $i)
		gsub(/</, "\\&lt;", $i)
		gsub(/>/, "\\&gt;", $i)
	}
	if ($1 == "PID") {
		print tr_ind "<tr>"
		print td_ind "<th>@TR<<status_processes_" $1 "#" $1 ">></th>"
		print td_ind "<th>@TR<<status_processes_" $2 "#" $2 ">></th>"
		print td_ind "<th>@TR<<status_processes_" $3 "#" $3 ">></th>"
		print td_ind "<th>@TR<<status_processes_" $4 "#" $4 ">></th>"
		print td_ind "<th>@TR<<status_processes_" $5 "#" $5 ">></th>"
		if (interval < 1)
			print td_ind "<th>@TR<<status_processes_Signal#Signal>></th>"
		print tr_ind "</tr>"
	} else {
		if (odd == 1) {
			print tr_ind "<tr>"
			odd--
		} else {
			print tr_ind "<tr class=\"odd\">"
			odd++
		}
		for (i=1; i<=4; i++) {
			printf td_ind
			if ((i==1) || (i==3))
				printf "<td class=\"number\">"
			else
				printf "<td>"
			print $i "</td>"
		}
		pid = $1
		lcol = $5
		for (i=6; i<=NF; i++) lcol = lcol " " $i
		if (length(lcol) >= 50) {
			fulcmd = readcmdline($1)
			if (fulcmd) lcol = fulcmd
		}
		print td_ind "<td>" lcol "</td>"
		if (interval < 1) {
			if ((lcol ~ /^\[/) || lcol ~ /init/)
				print td_ind "<td>&nbsp;</td>"
			else {
				print td_ind "<td class=\"buttons\"><form method=\"post\" action=\""url"\">" siglist "<input type=\"hidden\" value=\"" pid "\" name=\"pid\" />&nbsp;<input type=\"submit\" value=\" @TR<<status_processes_Send_signal#Send>> \" name=\"kill\" /></form></td>"
			}
		}
		print tr_ind "</tr>"
	}
}
'

display_form <<EOF
string|</tbody>
end_form
EOF

# helps
[ "$FORM_interval" -gt 0 ] >/dev/null 2>&1 && {
	cat <<EOF
<a name="pslegend"></a>
<h4>@TR<<status_processes_Legend#Legend>>:</h4>
<p>@TR<<status_processes_Legend_Text#Memory sizes are in kB units.<br />Stat shortcuts meaning: A=Active, I=Idle (waiting for startup), O=Nonexistent, R=Running, S=Sleeping, T=Stopped, W=Swapped, Z=Canceled.<br />Commands enclosed in &quot;[...]&quot; are kernel threads.<br />For more information see the <a href="http://www.opengroup.org/onlinepubs/009695399/utilities/ps.html" onclick="return !targetwindow(this.href);">ps command description</a>.>></p>
EOF
} || {
	cat <<EOF
<a name="signallegend"></a>
<h4>@TR<<status_processes_Signals_Legend#Most used signals>>:</h4>
<p>@TR<<status_processes_Signals_Legend_Text#1) SIGHUP - Hangup<br />9) SIGKILL - Kill (can't be caught or ignored)<br />15) SIGTERM - Termination<br />10) SIGUSR1/12) SIGUSR2 - User-defined signals.<br />For more information see the <a href="http://www.opengroup.org/onlinepubs/009695399/utilities/kill.html" onclick="return !targetwindow(this.href);">kill command description</a>.>></p>

EOF
}

footer ?>
<!--
##WEBIF:name:Status:110:Processes
-->
