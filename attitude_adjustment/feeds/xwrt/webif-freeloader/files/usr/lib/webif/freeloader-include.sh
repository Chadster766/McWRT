# freeloader-include.sh created by m4rc0 02-03-2007
# version 1.3
###################################################################
# freeloader-include.sh
# (c)2007 X-Wrt project (http://www.x-wrt.org)
# (c)2007-03-02 m4rc0
#
#	version 1.3
#
# Description:
#	Holds the major setting for freeloader.
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

uci_add_section_if_not_exists() {
	local _val
	eval "_val=\$CONFIG_${2}_TYPE" 2>/dev/null
	! equal "$_val" "$1" && {
		uci_add "$1" "$1" "$2"
	}
}
uci_add_option_if_not_exists() {
	local _val
	equal "$(set | grep "CONFIG_${2}_${3}=")" "" && {
		uci_add_section_if_not_exists "$1" "$2"
		uci_set "$1" "$2" "$3" "$4"
		freeloader_commit=1
	}
}

freeloader_init_config() {
	freeloader_commit=0
	[ -e /etc/config/freeloader ] || touch /etc/config/freeloader
	uci_add_option_if_not_exists "freeloader" "download" "enable" "1"
	uci_add_option_if_not_exists "freeloader" "download" "root" "/mnt/usbdrive/freeloader"
	uci_add_option_if_not_exists "freeloader" "ctorrent" "downloadrate" ""
	uci_add_option_if_not_exists "freeloader" "ctorrent" "uploadrate" "12"
	uci_add_option_if_not_exists "freeloader" "email" "enable" "0"
	uci_add_option_if_not_exists "freeloader" "email" "emailfrom" "root@localhost"
	uci_add_option_if_not_exists "freeloader" "email" "emailto" "root@localhost"
	uci_add_option_if_not_exists "freeloader" "email" "smtpserver" "127.0.0.1"
	uci_add_option_if_not_exists "freeloader" "curl" "ftplogin" "anonymous"
	uci_add_option_if_not_exists "freeloader" "curl" "ftppasswd" "freeloader@"
	[ "$freeloader_commit" -gt "0" ] && {
		uci_commit "freeloader"
		uci_load "freeloader"
	}
	unset freeloader_commit
}

uci_load "freeloader"

# Set the working directories
DOWNLOAD_ROOT="$CONFIG_download_root"
QUEUE_NORMAL="$DOWNLOAD_ROOT/downloadnormal"
QUEUE_PRIO="$DOWNLOAD_ROOT/downloadprio"
QUEUE_DONE="$DOWNLOAD_ROOT/downloaddone"
QUEUE_ABORT="$DOWNLOAD_ROOT/downloadabort"
LOG_DIRECTORY="$DOWNLOAD_ROOT/downloadlog"
DOWNLOAD_TEMP="$DOWNLOAD_ROOT/downloadtemp"
DOWNLOAD_DESTINATION="$DOWNLOAD_ROOT"
CTORRENT_UPLOAD_RATE="$CONFIG_ctorrent_uploadrate"

# Initialize the directory structure
[ -d "$DOWNLOAD_ROOT" ] && {
	[ ! -d "$QUEUE_NORMAL" ] && mkdir -p "$QUEUE_NORMAL" > /dev/null 2>&1
	[ ! -d "$QUEUE_PRIO" ] && mkdir -p "$QUEUE_PRIO" > /dev/null 2>&1
	[ ! -d "$QUEUE_DONE" ] && mkdir -p "$QUEUE_DONE" > /dev/null 2>&1
	[ ! -d "$QUEUE_ABORT" ] && mkdir -p "$QUEUE_ABORT" > /dev/null 2>&1
	[ ! -d "$LOG_DIRECTORY" ] && mkdir -p "$LOG_DIRECTORY" > /dev/null 2>&1
	[ ! -d "$DOWNLOAD_TEMP" ] && mkdir -p "$DOWNLOAD_TEMP" > /dev/null 2>&1
}

# Set email settings
EMAIL_FROM="$CONFIG_email_emailfrom"
EMAIL_TO="$CONFIG_email_emailto"
EMAIL_SMTP="$CONFIG_email_smtpserver"

# functions
mailstatus()
{
	[ "$CONFIG_email_enable" -gt 0 ] >/dev/null 2>&1 && {
		echo $1 | mini_sendmail -f$EMAIL_FROM -s$EMAIL_SMTP $EMAIL_TO
	}
}

logstatus()
{
	[ -d "$LOG_DIRECTORY" ] && {
		echo "`date` -- $1" >> "$LOG_DIRECTORY/freeloader.log"
	}
}

