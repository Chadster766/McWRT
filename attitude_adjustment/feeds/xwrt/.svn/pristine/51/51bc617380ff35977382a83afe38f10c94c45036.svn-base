#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh

config_cb() {
	local cfg_type="$1"
	local cfg_name="$2"

	case "$cfg_type" in
		syslogd)
			append config "$cfg_name"
		;;
	esac
}

if ! empty "$FORM_submit"; then
	uci_set "webif" "firewall" "log" "$FORM_enabled"
else
	uci_load webif
	config_get FORM_enabled firewall log
fi
uci_load syslog
#---------------------------------------------
# sets the type of log: file or circular
# defaults to circular, wich is the default install for openwrt
# use log-setup.sh to modify these parameters
config_get LOG_TYPE "$config" type
config_get LOG_FILE "$config" file
if equal $LOG_TYPE "file" ; then
	LOG_FILE=${LOG_FILE:-"/var/log/messages"}
	LOGREAD="cat "$LOG_FILE
else LOGREAD="logread"
fi

#--------------------------------------------
# sets all parameters for diplaying the settings of the current request being displayed
# and sets the filter itself
# this initialisation is a bit complicated - but like this we got a very simple condition to test in the awk parse loop
if ! empty "$FORM_submit" ; then
	#the  filter has been  selected via the form
	if equal $FORM_act "A" ; then
		FORM_value=""
	fi
	if equal $FORM_act "p" ; then
		FORM_value=$FORM_value":"
		FORM_act=":"
	fi
	FORM_filter=$FORM_act$FORM_value
elif ! empty "$FORM_filter" ; then
	# the filter has been selected via a link
	long=${#FORM_filter}
	idx=$(expr index "$FORM_filter" : )
	if equal $idx 1 ; then
		# PREFIX
		FORM_act="p"
		FORM_value=$(expr substr "$FORM_filter" 2 $(($long-1)))
		FORM_filter=$FORM_value
	else
		# the rest (SRC DST PROTO SPT DPT TYPE)
		idx=$(expr index "$FORM_filter" = )
		FORM_value=$(expr substr "$FORM_filter" $(($idx+1)) $(($long-$idx)))
		FORM_act=$(expr substr "$FORM_filter" 1 $idx)
		# the case of a blank field
		idx=$(expr index "$FORM_filter" - )
		if ! equal $idx 0 ; then
			FORM_filter=$FORM_act" "
		fi
	fi
else
	# no filter selected
	FORM_act="A"
	FORM_value=""
fi

if equal $FORM_act "SRC=" || equal $FORM_act "DST=" ; then
	DEC=$(nslookup $FORM_value | awk ' ( /^Name:/ ) {print $2}  ')
fi


header "Log" "Firewall Log View" "@TR<<Netfilter Log>>" '' "$SCRIPT_NAME"

has_pkgs iptables-mod-filter

# request for filtering -----------------------
display_form <<EOF
start_form|@TR<<Filter>>
field|@TR<<Log>>
radio|enabled|$FORM_enabled|1|@TR<<Enabled>>
radio|enabled|$FORM_enabled|0|@TR<<Disabled>>
field|@TR<<Filter>>
select|act|$FORM_act
option|A|@TR<<All>>
option|p|@TR<<Prefix>>
option|SRC=|@TR<<Source IP>>
option|DST=|@TR<<Destination IP>>
option|PROTO=|@TR<<Protocol>>
option|SPT=|@TR<<Source Port>>
option|DPT=|@TR<<Destination Port>>
option|TYPE=|@TR<<Type>>
text|value|$FORM_value
field|$DEC
end_form
EOF


# display --------------------------------
$LOGREAD | sort -r | awk -v filter=$FORM_filter -F ' ' '
BEGIN {
	print "<h3>@TR<<Listing>></h3>";
	print "<table width=\"90%\">";
	print "<th>@TR<<Date>></th><th>@TR<<Prefix>></th><th>@TR<<Source IP>></th><th>@TR<<Destination IP>></th><th>@TR<<Proto>></th><th>@TR<<Source Port>></th><th>@TR<<Dest. Port>></th><th>@TR<<Type>></th>";
	}

# is this line a netfilter record ?
(/IN=/ && /OUT=/ && /PROTO/) {
	#is this record requested ?
	if ( ( filter != "" ) && ( $0 !~ filter ) ) next;

	i=1; #field counter
	space=" "

	if ( i > NF) next;
	action=""
	idx=index($i,"IN=");
	prefix=""
	while ( ($i != "kernel:") ) i++;
	prefix=""
	i++
	while ( ($i !~ /IN=/) ) {
		if (prefix == "")
			prefix=(prefix, $i)
		else
			 prefix=(prefix, space, $i)
		i++
	}

	while ( ($i !~ /OUT=/) && (i <= NF) ) i++;
	if ( i > NF) next;
	if_out=substr( $i, 5, length($i) - 4);
	if (if_out == "") if_out="-";

	while ( ($i !~ /SRC=/)  && (i <= NF) ) i++;
	if ( i > NF) next;
	ip_src=substr( $i, 5, length($i) - 4);

	while (($i !~ /DST=/)  && (i <= NF) ) i++;
	if ( i > NF) next;
	ip_dst=substr( $i, 5, length($i) - 4);

	while ( ($i !~ /PROTO=/)  && (i <= NF) ) i++;
	if ( i > NF) next;
	proto=substr( $i, 7, length($i) - 6);

	spt="";
	dpt="";
	type="";
	if ( (proto == "TCP") || (proto == "UDP") )
		{
		while ( ($i !~ /SPT=/)  && (i <= NF) ) i++;
		if ( i > NF) next;
		spt=substr( $i, 5, length($i) - 4);

		while ( ($i !~ /DPT=/)  && (i <= NF) ) i++;
		if ( i > NF) next;
		dpt=substr( $i, 5, length($i) - 4);
		}
	if (proto == "ICMP")
		{
		while ( ($i !~ /TYPE=/)  && (i <= NF) ) i++;
		if ( i > NF) next;
		type=substr( $i, 6, length($i) - 5);
		}

	print "<tr><td>"$1" "$2" "$3"</td>"\
	"<td><a href=log-browse.sh?filter=:"prefix">"prefix"</a></td>"\
	"<td><a href=log-browse.sh?filter=SRC="ip_src">"ip_src"</a></td>"\
	"<td><a href=log-browse.sh?filter=DST="ip_dst">"ip_dst"</a></td>"\
	"<td><a href=log-browse.sh?filter=PROTO="proto">"proto"</a></td>"\
	"<td><a href=log-browse.sh?filter=SPT="spt">"spt"</a></td>"\
	"<td><a href=log-browse.sh?filter=DPT="dpt">"dpt"</a></td>"\
	"<td><a href=log-browse.sh?filter=TYPE="type">"type"</a></td>"\
	"<tr>\n";
	}

END { print "</table>"}
'


footer ?>

<!--
##WEBIF:name:Log:004:Firewall Log View
-->
