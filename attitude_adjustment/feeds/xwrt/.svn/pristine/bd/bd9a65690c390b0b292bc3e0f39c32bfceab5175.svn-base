#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh

crondir="/etc/crontabs/"

header "Status" "Crontabs" "@TR<<Cron Tables>>"


crontab_help="helpitem|crontabs
helptext|crontabs_helptext#The Cron Tables is a list of jobs the cron daemon (crond) should execute at specified intervals or times.
"

for crontab in $(ls $crondir/); do
	for tab in $(cat $crondir$crontab | sed '/^#/d; /^[[:space:]]*$/d; s/ /@/g'); do
		tab_lines="${tab_lines}field|
string|<pre>$tab</pre>
"
	done
	[ -n "$tab_lines" ] && {
		[ -z "$cron_text" ] && cronhelp="$crontab_help"
		cron_text="${cron_text}start_form|@TR<<Cron Jobs>> : $crontab
$(echo "$tab_lines" | sed 's/@/ /g')
$cronhelp
end_form
"
	tab_lines=""
	cronhelp=""
	}
done

display_form <<EOF
start_form|@TR<<Cron Tables Directory>>
field|@TR<<Cron Tables Directory>>
string|$crondir
end_form
$cron_text
EOF

footer ?>
<!--
##WEBIF:name:Status:175:Crontabs
-->
