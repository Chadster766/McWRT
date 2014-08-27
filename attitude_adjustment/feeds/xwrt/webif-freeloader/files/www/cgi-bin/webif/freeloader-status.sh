#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
###################################################################
# freeloader-status.sh
# (c)2007 X-Wrt project (http://www.x-wrt.org)
# (c)2007-02-22 m4rc0
#
#	version 1.16
#
# Description:
#	Show the status of the queues and the current download.
#
# Author(s) [in order of work date]:
#	m4rc0 <janssenmaj@gmail.com>
#
# Major revisions:
#
# NVRAM variables referenced:
#   none
#
# Configuration files referenced:
#   /etc/config/freeloader
#
#

header_inject_head=$(cat <<EOF
<meta http-equiv="refresh" content="60;url=$SCRIPT_NAME" />

<style type="text/css">
<!--
#viewarea table {
	width: 100%;
	text-align: left;
	font-size: 0.8em;
	border-style: none;
	border-spacing: 0;
}
#viewarea th {
	width: 5%;
}
#viewarea td {
	padding-left: 1px;
	padding-right: 1px;
}
#viewarea .torrentcol {
	width: 68%;
}
#viewarea .datecol {
	width: 17%;
}
#viewarea .logfonts {
	font-family: Courier,"Lucida Console",monospace;
	font-size: 0.8em;
	whitespace: pre;
}
#viewarea .leglog {
	margin-top: 0 auto 0 auto;
	padding: 0 4px 0 4px;
	width: 99%;
}
#viewarea .prelog {
	margin: 0.2em auto 1em auto;
	padding: 3px;
	width: 99%;
	height: 14.0em;
	overflow: auto;
	border: 1px solid;
}
#viewarea fieldset {
	border: 1px solid;
	padding: 3px;
}
// -->
</style>
EOF
)

header "Freeloader" "freeloader-status_subcategory#Status" "@TR<<freeloader-status_Freeloader_status#Freeloader status>>"

#Include settings
. /usr/lib/webif/freeloader-include.sh
freeloader_init_config

#Check the required packages
is_package_installed "curl"
pkg_curl=$?
is_package_installed "ctorrent"
pkg_ctorrent=$?
is_package_installed "nzbget"
pkg_nzbget=$?

if [ $pkg_nzbget -eq "0" ] || [ $pkg_ctorrent -eq "0" ] || [ $pkg_curl -eq "0" ]; then

#check if there is a PRIO download
if [ -f /tmp/prio.lock ]; then
	#Set the current working queue as the PRIO queue
	QUEUE_DIR=$QUEUE_PRIO
else
	#Set the current working queue as the normal queue
	QUEUE_DIR=$QUEUE_NORMAL
fi

if [ "$FORM_action" = "abort" ]; then
	EXTENSION=`sed -n 1p /tmp/currentdownloadextension 2>/dev/null`
	if [ $EXTENSION = 'torrent' ]; then
		killall -9 ctorrent  > /dev/null 2>&1
	elif [ $EXTENSION = 'link' ]; then
		killall -9 curl  > /dev/null 2>&1
	elif [ $EXTENSION = 'nzb' ]; then
		killall -9 nzbget  > /dev/null 2>&1		
	fi
cat << EOF
<script type="text/javascript">
<!--
window.location="freeloader-status.sh"
// -->
</script>
EOF
elif [ "$FORM_action" = "remove" ]; then
	if [ "$FORM_queue" = "normal" ]; then
		mv "$QUEUE_NORMAL/$FORM_torrent" "$QUEUE_ABORT/$FORM_torrent"  > /dev/null 2>&1
	elif [ "$FORM_queue" = "prio" ]; then
		mv "$QUEUE_PRIO/$FORM_torrent" "$QUEUE_ABORT/$FORM_torrent"  > /dev/null 2>&1
	fi
elif [ "$FORM_action" = "purge" ]; then
	if [ "$FORM_queue" = "done" ]; then
		rm "$QUEUE_DONE/$FORM_torrent"  > /dev/null 2>&1
	elif [ "$FORM_queue" = "abort" ]; then
		rm "$QUEUE_ABORT/$FORM_torrent"  > /dev/null 2>&1
	fi
elif [ "$FORM_action" = "prio" ]; then
	if [ "$FORM_queue" = "normal" ]; then
		mv "$QUEUE_NORMAL/$FORM_torrent" "$QUEUE_PRIO/$FORM_torrent"  > /dev/null 2>&1
	elif [ "$FORM_queue" = "abort" ]; then
		mv "$QUEUE_ABORT/$FORM_torrent" "$QUEUE_PRIO/$FORM_torrent"  > /dev/null 2>&1
	fi
elif [ "$FORM_action" = "normal" ]; then
	if [ "$FORM_queue" = "prio" ]; then
		mv "$QUEUE_PRIO/$FORM_torrent" "$QUEUE_NORMAL/$FORM_torrent"  > /dev/null 2>&1
	elif [ "$FORM_queue" = "abort" ]; then
		mv "$QUEUE_ABORT/$FORM_torrent" "$QUEUE_NORMAL/$FORM_torrent"  > /dev/null 2>&1
	fi
elif [ "$FORM_action" = "suspend" ]; then
	touch $DOWNLOAD_DESTINATION/suspend.lock > /dev/null 2>&1
elif [ "$FORM_action" = "resume" ]; then
	rm $DOWNLOAD_DESTINATION/suspend.lock > /dev/null 2>&1
fi

cat <<EOF
<div id="viewarea">
<h3>@TR<<freeloader-status_Normal_queue#Normal queue>></h3>

<table>
<tr>
<th class="torrentcol">@TR<<freeloader-status_th_Torrent#Torrent>></th>
<th class="datecol">@TR<<freeloader-status_th_Date#Date>></th>
<th></th>
<th></th>
<th></th>
</tr>
EOF

if [ -f /tmp/currentdownloadfile ]; then
	CURRENT_DOWNLOADFILE=`sed -n 1p /tmp/currentdownloadfile 2>/dev/null`
	if [ "`ls -l $QUEUE_NORMAL 2>/dev/null | grep -v "$CURRENT_DOWNLOADFILE"`" != '' ]; then
		ls -l $QUEUE_NORMAL 2>/dev/null | grep -v "$CURRENT_DOWNLOADFILE" | awk 'NF == 9 {print "<tr><td>",$9,"</td><td>",$7,$6,$8,"</td><td>","&nbsp;","</td><td>","<a href=\"freeloader-status.sh?action=prio&amp;queue=normal&amp;torrent=" $9 "\">@TR<<freeloader-status_action_prio#prio>></a>","</td><td>","<a href=\"freeloader-status.sh?action=remove&amp;queue=normal&amp;torrent=" $9 "\">@TR<<freeloader-status_action_remove#remove>></a>","</td></tr>"};NF > 9 {filename=$9;for (i=10;i<= NF; i++){filename = filename " " $i};print "<tr><td>",filename,"</td><td>",$7,$6,$8,"</td><td>","&nbsp;","</td><td>","<a href=\"freeloader-status.sh?action=prio&amp;queue=normal&amp;torrent="filename"\">@TR<<freeloader-status_action_prio#prio>></a>","</td><td>","<a href=\"freeloader-status.sh?action=remove&amp;queue=normal&amp;torrent="filename"\">@TR<<freeloader-status_action_remove#remove>></a>","</td></tr>"}'
	else

	   echo "<tr><td colspan=\"5\">@TR<<freeloader-status_No_files_in_queue#There are currently no files in the queue.>></td></tr>"
	fi
else
	if [ "`ls -l $QUEUE_NORMAL 2>/dev/null `" != '' ]; then
		ls -l $QUEUE_NORMAL 2>/dev/null | awk 'NF == 9 {print "<tr><td>",$9,"</td><td>",$7,$6,$8,"</td><td>","&nbsp;","</td><td>","<a href=\"freeloader-status.sh?action=prio&amp;queue=normal&amp;torrent=" $9 "\">@TR<<freeloader-status_action_prio#prio>></a>","</td><td>","<a href=\"freeloader-status.sh?action=remove&amp;queue=normal&amp;torrent=" $9 "\">@TR<<freeloader-status_action_remove#remove>></a>","</td></tr>"};NF > 9 {filename=$9;for (i=10;i<= NF; i++){filename = filename " " $i};print "<tr><td>",filename,"</td><td>",$7,$6,$8,"</td><td>","&nbsp;","</td><td>","<a href=\"freeloader-status.sh?action=prio&amp;queue=normal&amp;torrent="filename"\">@TR<<freeloader-status_action_prio#prio>></a>","</td><td>","<a href=\"freeloader-status.sh?action=remove&amp;queue=normal&amp;torrent="filename"\">@TR<<freeloader-status_action_remove#remove>></a>","</td></tr>"}'
	else
	   echo "<tr><td colspan=\"5\">@TR<<freeloader-status_No_files_in_queue#There are currently no files in the queue.>></td></tr>"
	fi

fi
cat << EOF
</table>

<br/>
<h3>@TR<<freeloader-status_Prio_queue#Prio queue>></h3>

<table>
<tr>
<th class="torrentcol">@TR<<freeloader-status_th_Torrent#Torrent>></th>
<th class="datecol">@TR<<freeloader-status_th_Date#Date>></th>
<th class="restcol"></th>
<th class="restcol"></th>
<th class="restcol"></th>
</tr>
EOF

if [ -f /tmp/currentdownloadfile ]; then
	CURRENT_DOWNLOADFILE=`sed -n 1p /tmp/currentdownloadfile 2>/dev/null`
	if [ "`ls -l $QUEUE_PRIO 2>/dev/null | grep -v "$CURRENT_DOWNLOADFILE"`" != '' ]; then
		ls -l $QUEUE_PRIO 2>/dev/null | grep -v "$CURRENT_DOWNLOADFILE" | awk 'NF == 9 {print "<tr><td>",$9,"</td><td>",$7,$6,$8,"</td><td>","<a href=\"freeloader-status.sh?action=normal&amp;queue=prio&amp;torrent=" $9 "\">@TR<<freeloader-status_action_normal#normal>></a>","</td><td>","&nbsp;","</td><td>","<a href=\"freeloader-status.sh?action=remove&amp;queue=prio&amp;torrent=" $9 "\">@TR<<freeloader-status_action_remove#remove>></a>","</td></tr>"};NF > 9 {filename=$9;for (i=10;i<= NF; i++){filename = filename " " $i};print "<tr><td>",filename,"</td><td>",$7,$6,$8,"</td><td>","<a href=\"freeloader-status.sh?action=normal&amp;queue=prio&amp;torrent="filename"\">@TR<<freeloader-status_action_normal#normal>></a>","</td><td>","&nbsp;","</td><td>","<a href=\"freeloader-status.sh?action=remove&amp;queue=prio&amp;torrent="filename"\">@TR<<freeloader-status_action_remove#remove>></a>","</td></tr>"}'
	else
	   echo "<tr><td colspan=\"5\">@TR<<freeloader-status_No_files_in_queue#There are currently no files in the queue.>></td></tr>"
	fi
else
	if [ "`ls -l $QUEUE_PRIO 2>/dev/null `" != '' ]; then
		ls -l $QUEUE_PRIO 2>/dev/null | awk 'NF == 9 {print "<tr><td>",$9,"</td><td>",$7,$6,$8,"</td><td>","<a href=\"freeloader-status.sh?action=normal&amp;queue=prio&amp;torrent=" $9 "\">@TR<<freeloader-status_action_normal#normal>></a>","</td><td>","&nbsp;","</td><td>","<a href=\"freeloader-status.sh?action=remove&amp;queue=prio&amp;torrent=" $9 "\">@TR<<freeloader-status_action_remove#remove>></a>","</td></tr>"};NF > 9 {filename=$9;for (i=10;i<= NF; i++){filename = filename " " $i};print "<tr><td>",filename,"</td><td>",$7,$6,$8,"</td><td>","<a href=\"freeloader-status.sh?action=normal&amp;queue=prio&amp;torrent="filename"\">@TR<<freeloader-status_action_normal#normal>></a>","</td><td>","&nbsp;","</td><td>","<a href=\"freeloader-status.sh?action=remove&amp;queue=prio&amp;torrent="filename"\">@TR<<freeloader-status_action_remove#remove>></a>","</td></tr>"}'
	else
	   echo "<tr><td colspan=\"5\">@TR<<freeloader-status_No_files_in_queue#There are currently no files in the queue.>></td></tr>"
	fi
fi
cat <<EOF
</table>

<br/><h3>@TR<<freeloader-status_Finished_torrents#Finished torrents>></h3>

<table>
<tr>
<th class="torrentcol">@TR<<freeloader-status_th_Torrent#Torrent>></th>
<th class="datecol">@TR<<freeloader-status_th_Date#Date>></th>
<th class="restcol"></th>
<th class="restcol"></th>
<th class="restcol"></th>
</tr>
EOF

if [ "`ls -l $QUEUE_DONE 2>/dev/null `" != '' ]; then
	ls -l $QUEUE_DONE 2>/dev/null | awk 'NF == 9 {print "<tr><td>",$9,"</td><td>",$7,$6,$8,"</td><td>","&nbsp;","</td><td>","&nbsp;","</td><td>","<a href=\"freeloader-status.sh?action=purge&amp;queue=done&amp;torrent=" $9 "\">@TR<<freeloader-status_action_purge#purge>></a>","</td></tr>"};NF > 9 {filename=$9;for (i=10;i<= NF; i++){filename = filename " " $i};print "<tr><td>",filename,"</td><td>",$7,$6,$8,"</td><td>","&nbsp;","</td><td>","&nbsp;","</td><td>","<a href=\"freeloader-status.sh?action=purge&amp;queue=done&amp;torrent="filename"\">@TR<<freeloader-status_action_purge#purge>></a>","</td></tr>"}'
else
	echo "<tr><td colspan=\"5\">@TR<<freeloader-status_No_finished_torrents#There are no finished downloads at the moment.>></td></tr>"
fi

cat <<EOF
</table>

<br/><h3>@TR<<freeloader-status_Aborted_torrents#Aborted torrents>></h3>

<table>
<tr>
<th class="torrentcol">@TR<<freeloader-status_th_Torrent#Torrent>></th>
<th class="datecol">@TR<<freeloader-status_th_Date#Date>></th>
<th class="restcol"></th>
<th class="restcol"></th>
<th class="restcol"></th>
</tr>
EOF

if [ "`ls -l $QUEUE_ABORT 2>/dev/null `" != '' ]; then
	ls -l $QUEUE_ABORT 2>/dev/null | awk 'NF == 9 {print "<tr><td>",$9,"</td><td>",$7,$6,$8,"</td><td>","<a href=\"freeloader-status.sh?action=normal&amp;queue=abort&amp;torrent=" $9 "\">@TR<<freeloader-status_action_normal#normal>></a>","</td><td>","<a href=\"freeloader-status.sh?action=prio&amp;queue=abort&amp;torrent=" $9 "\">@TR<<freeloader-status_action_prio#prio>></a>","</td><td>","<a href=\"freeloader-status.sh?action=purge&amp;queue=abort&amp;torrent=" $9 "\">@TR<<freeloader-status_action_purge#purge>></a>","</td></tr>"};NF > 9 {filename=$9;for (i=10;i<= NF; i++){filename = filename " " $i};print "<tr><td>",filename,"</td><td>",$7,$6,$8,"</td><td>","<a href=\"freeloader-status.sh?action=normal&amp;queue=abort&amp;torrent="filename"\">@TR<<freeloader-status_action_normal#normal>></a>","</td><td>","<a href=\"freeloader-status.sh?action=prio&amp;queue=abort&amp;torrent="filename"\">@TR<<freeloader-status_action_prio#prio>></a>","</td><td>","<a href=\"freeloader-status.sh?action=purge&amp;queue=abort&amp;torrent="filename"\">@TR<<freeloader-status_action_purge#purge>></a>","</td></tr>"}'
else
	echo "<tr><td colspan=\"5\">@TR<<freeloader-status_No_aborted_downloads#There are no aborted downloads at the moment.>></td></tr>"
fi

cat <<EOF
</table>

<br/><h3>@TR<<freeloader-status_Currently_downloading#Currently downloading>></h3>

<table>
<tr>
<th class="torrentcol">@TR<<freeloader-status_th_Torrent#Torrent>></th>
<th class="datecol">@TR<<freeloader-status_th_Date#Date>></th>
<th class="restcol"></th>
<th class="restcol"></th>
<th class="restcol"></th>
</tr>
EOF

if [ -f /tmp/currentdownloadfile ]; then
	CURRENT_DOWNLOADFILE=`sed -n 1p /tmp/currentdownloadfile 2>/dev/null`
	if [ -f $DOWNLOAD_DESTINATION/suspend.lock ]; then
		echo "<tr><td colspan=\"5\"><font color=\"red\">@TR<<freeloader-status_Suspending_process#The process is being suspend at the moment, please wait...>><font></td></tr>"
		echo "<tr><td colspan=\"5\">&nbsp;<td></tr>"
		ls -l $QUEUE_DIR 2>/dev/null | grep "$CURRENT_DOWNLOADFILE\$" | awk 'NF == 9 {print "<tr><td>",$9,"</td><td>",$7,$6,$8,"</td><td>","&nbsp;","</td><td>","&nbsp;","</td><td>","&nbsp;","</td></tr>"};NF > 9 {filename=$9;for (i=10;i<= NF; i++){filename = filename " " $i};print "<tr><td>",filename,"</td><td>",$7,$6,$8,"</td><td>","&nbsp;","</td><td>","&nbsp;","</td><td>","&nbsp;","</td></tr>"}'
	else
		ls -l $QUEUE_DIR 2>/dev/null | grep "$CURRENT_DOWNLOADFILE\$" | awk 'NF == 9 {print "<tr><td>",$9,"</td><td>",$7,$6,$8,"</td><td>","<a href=\"freeloader-status.sh?action=abort&amp;queue=current&amp;torrent=" $9 "\">@TR<<freeloader-status_action_abort#abort>></a>","</td><td>","&nbsp;","</td><td>","<a href=\"freeloader-status.sh?action=suspend\">@TR<<freeloader-status_action_suspend#suspend>></a>","</td></tr>"};NF > 9 {filename=$9;for (i=10;i<= NF; i++){filename = filename " " $i};print "<tr><td>",filename,"</td><td>",$7,$6,$8,"</td><td>","<a href=\"freeloader-status.sh?action=abort&amp;queue=current&amp;torrent="filename"\">@TR<<freeloader-status_action_abort#abort>></a>","</td><td>","&nbsp;","</td><td>","<a href=\"freeloader-status.sh?action=suspend\">@TR<<freeloader-status_action_suspend#suspend>></a>","</td></tr>"}'
	fi
else
	if [ -f $DOWNLOAD_DESTINATION/suspend.lock ]; then
		echo "<tr><td colspan=\"4\">@TR<<freeloader-status_Download_suspended#Download queue is suspended.>></td><td><a href=\"freeloader-status.sh?action=resume\">@TR<<freeloader-status_action_resume#resume>></a></td></tr>"
	else
		echo "<tr><td colspan=\"5\">@TR<<freeloader-status_No_downloaded_files#There are no files being downloaded at the moment.>></td></tr>"
	fi
fi

cat <<EOF
</table>
<br/>
EOF

if [ -f /tmp/currentlogfile ]; then
	CURRENT_LOGFILE=`sed -n 1p /tmp/currentlogfile 2>/dev/null`
	EXTENSION=`sed -n 1p /tmp/currentdownloadextension 2>/dev/null`

	if [ $EXTENSION = 'torrent' ]; then
		echo "<fieldset><legend>@TR<<freeloader-status_Torrent_log#Torrent log>></legend>"
		echo -n "<pre class=\"logfonts prelog\" title=\"@TR<<freeloader-status_Start_log#Start of the log>>\">"
		head -c 2000 "$LOG_DIRECTORY/$CURRENT_LOGFILE" 2>/dev/null | tr '\r' '\n'| sed '/Check exist:/d; /^warn,.*bit field refer file/d; /^This is normal/d; /^Press /d; s/&/\&amp;/; s/</\&lt;/; s/>/\&gt;/;' | sed 24q
		echo
		echo "</pre>"
		echo -n "<pre class=\"prelog\" title=\"@TR<<freeloader-status_End_log#End of the reversed log>>\">"
		tail -c 5000 "$LOG_DIRECTORY/$CURRENT_LOGFILE" 2>/dev/null | tr '\r' '\n' | sed '/Check exist:/d; /^warn,.*bit field refer file/d; /^This is normal/d; /^Press /d; s/&/\&amp;/; s/</\&lt;/; s/>/\&gt;/; 1!G;h;$!d;' | sed 70q
		echo
		echo "</pre>"
### notice: this part will be probably changed with more advanced parser
		echo "<p class=\"logfonts leglog\"><u>@TR<<freeloader-status_Torrent_Legend#Legend>></u>:<br />"
		echo "/ 0/3/50 [672/672/672] 0MB,1130MB | 0,20K/s | 0,0K E:0,31 P:4/10<br />"
		echo "- - - -- &nbsp;--- --- --- &nbsp;--- ------ &nbsp;&nbsp;- -- &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- - &nbsp;&nbsp;&nbsp;- -- &nbsp;&nbsp;----<br />"
		echo "A B C &nbsp;D &nbsp;&nbsp;E &nbsp;&nbsp;F &nbsp;&nbsp;G &nbsp;&nbsp;&nbsp;H &nbsp;&nbsp;&nbsp;&nbsp;I &nbsp;&nbsp;&nbsp;&nbsp;J &nbsp;K &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;L M &nbsp;&nbsp;&nbsp;N &nbsp;O &nbsp;&nbsp;&nbsp;&nbsp;P<br />"
		echo "@TR<<freeloader-status_torrent_leg_A#A: Ticker; this character changes to indicate that the client is running.>><br />"
		echo "@TR<<freeloader-status_torrent_leg_B#B: Number of seeders (complete peers) to which you are connected.>><br />"
		echo "@TR<<freeloader-status_torrent_leg_C#C: Number of leechers (incomplete peers) to which you are connected.>><br />"
		echo "@TR<<freeloader-status_torrent_leg_D#D: Total number of peers in the swarm, as last reported by the tracker.>><br />"
		echo "@TR<<freeloader-status_torrent_leg_E#E: Number of pieces of the torrent that you have completed.>><br />"
		echo "@TR<<freeloader-status_torrent_leg_F#F: Total number of pieces in the torrent.>><br />"
		echo "@TR<<freeloader-status_torrent_leg_G#G: Number of pieces currently available from you and your connected peers.>><br />"
		echo "@TR<<freeloader-status_torrent_leg_H#H: Total amount of data you have downloaded.>><br />"
		echo "@TR<<freeloader-status_torrent_leg_I#I: Total amount of data you have uploaded.>><br />"
		echo "@TR<<freeloader-status_torrent_leg_J#J: Your current total download rate.>><br />"
		echo "@TR<<freeloader-status_torrent_leg_K#K: Your current total upload rate.>><br />"
		echo "@TR<<freeloader-status_torrent_leg_L#L: Amount of data downloaded since the last status line update.>><br />"
		echo "@TR<<freeloader-status_torrent_leg_M#M: Amount of data uploaded since the last status line update.>><br />"
		echo "@TR<<freeloader-status_torrent_leg_N#N: Number of tracker connection errors.>><br />"
		echo "@TR<<freeloader-status_torrent_leg_O#O: Number of successful tracker connections.>><br />"
		echo "@TR<<freeloader-status_torrent_leg_P#P: Completion ratio of current file (when -n is used).>><br />"
		echo "@TR<<freeloader-status_torrent_leg_Additional#Additional information such as tracker connection status may be displayed at the end of the status line when appropriate.>></p>"
### end of notice
		echo "</fieldset>"
	elif [ $EXTENSION = 'link' ]; then
		echo "<fieldset><legend>@TR<<freeloader-status_Link_log#Link log>></legend>"
		echo -n "<pre class=\"prelog\" title=\"@TR<<freeloader-status_Start_log#Start of the log>>\">"
		head -n 2 "$LOG_DIRECTORY/$CURRENT_LOGFILE" 2>/dev/null | sed 's/&/\&amp;/; s/</\&lt;/; s/>/\&gt;/;'
		echo
		echo "</pre>"
		echo -n "<pre class=\"prelog\" title=\"@TR<<freeloader-status_End_log#End of the reversed log>>\">"
		tail -c 1558 "$LOG_DIRECTORY/$CURRENT_LOGFILE" 2>/dev/null | tr '\r' '\n' | sed 's/&/\&amp;/; s/</\&lt;/; s/>/\&gt;/; 1!G;h;$!d;' | sed 70q
		echo
		echo "</pre></fieldset>"
	elif [ $EXTENSION = 'nzb' ]; then
		echo "<fieldset><legend>@TR<<freeloader-status_Nzb_log#Nzb log>></legend>"
		echo -n "<pre class=\"prelog\" title=\"@TR<<freeloader-status_End_log#End of the reversed log>>\">"
		tail -c 1558 "$LOG_DIRECTORY/$CURRENT_LOGFILE" 2>/dev/null | tr '\r' '\n' | sed 's/&/\&amp;/; s/</\&lt;/; s/>/\&gt;/; 1!G;h;$!d;' | sed 70q
		echo
		echo "</pre></fieldset>"
	fi
fi
cat <<EOF
</div>
EOF
else
	echo "<p>@TR<<freeloader-common_None_required_installed#None of the required packages are installed, check the <a href=\"freeloader-upload.sh\">upload-page</a> to install the packages.>></p>"
fi

footer ?>
<!--
##WEBIF:name:Freeloader:05:freeloader-status_subcategory#Status
-->
