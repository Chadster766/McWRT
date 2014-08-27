#!/usr/bin/webif-page
<? 
. /usr/lib/webif/webif.sh

if ! empty "$FORM_submit" ; then
	SAVED=1
	validate <<EOF
string|FORM_pw1|@TR<<Password>>|required min=5|$FORM_pw1
EOF
	equal "$FORM_pw1" "$FORM_pw2" || {
		[ -n "$ERROR" ] && ERROR="${ERROR}<br />"
		ERROR="${ERROR}@TR<<Passwords do not match>><br />"
	}
	if [ "$REMOTE_USER" = "root" -o "$REMOTE_USER" = "admin" ]; then
		empty "$ERROR" && {
			RES=$(
				(
					echo "$FORM_pw1"
					sleep 1
					echo "$FORM_pw2"
				) | passwd root 2>&1
			)
			equal "$?" 0 || ERROR="<pre>$RES</pre>"
		}
	else
		exists /tmp/.webif/file-httpd.conf && HTTPD_CONFIG_FILE=/tmp/.webif/file-httpd.conf || HTTPD_CONFIG_FILE=/etc/httpd.conf
		empty "$ERROR" && {
			cat $HTTPD_CONFIG_FILE | awk '
BEGIN {
	FS=":"
	system("/bin/rm /tmp/.webif/file-httpd.conf; mkdir /tmp/.webif/; touch /tmp/.webif/file-httpd.conf");
}
($1 != "") {
	if (($1 == "/cgi-bin/webif/") && (ENVIRON["REMOTE_USER"] != $2)) {
		print $1":"$2":"$3 >> "/tmp/.webif/file-httpd.conf"
	}
	if ($1 != "/cgi-bin/webif/") {
		print $1":"$2 >> "/tmp/.webif/file-httpd.conf"
	}
	if (ENVIRON["REMOTE_USER"] == $2) {
		("uhttpd -m " ENVIRON["FORM_pw1"]) | getline password
		print $1":"$2":"password >> "/tmp/.webif/file-httpd.conf"
	}
}'
	}
	fi
fi

header "System" "Password" "@TR<<Password>>" '' "$SCRIPT_NAME"

display_form <<EOF
start_form|@TR<<Password Change>>
field|@TR<<New Password>>:
password|pw1
field|@TR<<Confirm Password>>:
password|pw2
end_form
EOF

footer ?>

<!--
##WEBIF:name:System:250:Password
-->
