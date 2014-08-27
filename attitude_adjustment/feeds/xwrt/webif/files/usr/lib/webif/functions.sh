#!/bin/sh
#
# Webif^2 utility functions - X-Wrt
#
. /etc/functions.sh

#
# Misc. functions
#

# workarounds for stupid busybox slowness on [ ]
empty() {
	case "$1" in
		"") return 0 ;;
		*) return 255 ;;
	esac
}

equal() {
	case "$1" in
		"$2") return 0 ;;
		*) return 255 ;;
	esac
}

neq() {
	case "$1" in
		"$2") return 255 ;;
		*) return 0 ;;
	esac
}

# very crazy, but also very fast :-)
exists() {
	( < $1 ) 2>&-
}

has_nvram_support() {
	exists "/usr/sbin/nvram"
}

fix_symlink_hack() {
	! touch "$1" >&- 2>&- && {
		local atmpfile
		atmpfile=$(mktemp "/tmp/webif-XXXXXX")
		cp -p "$1" "$atmpfile"
		equal "$?" "0" && {
			rm "$1"
			mv "$atmpfile" "$1"
		}
	}
}

remove_lines_from_file() {
	# $1=filename
	# $2=substring match indicating lines to remove (case sensitive)
	cat "$1" 2>/dev/null | grep -q "$2" && {
		fix_symlink_hack "$1"
		local _substr_sed
		_substr_sed=$(echo "$2" |  sed s/'\/'/'\\\/'/g)
		cat "$1" |  sed /"$_substr_sed"/d > "$1"
	}
}

# mktemp replacement that doesn't actually create the file (as busybox 1.3.1+ does)
mkuniqfilename() {
	local _lfile
	_lfile=$(mktemp $*)
	rm -f "$_lfile" 2>&- >&-
}

#
# Original config functions
#
#  These work with as a tuple based configuration system. apply.sh applies
#  the changes.
#
load_settings() {
	equal "$1" "nvram" || {
		exists /etc/config/$1 && . /etc/config/$1
	}
	exists /tmp/.webif/config-$1 && . /tmp/.webif/config-$1
}

save_setting() {
	# $1 = group
	# $2 = name
	# $3 = value
	exists /tmp/.webif/* || mkdir -p /tmp/.webif
	oldval=$(eval "echo \${$2}")
	oldval=${oldval:-$(nvram get "$2")}
	grep "^$2=" /tmp/.webif/config-$1 >&- 2>&- && {
		grep -v "^$2=" /tmp/.webif/config-$1 > /tmp/.webif/config-$1-new 2>&-
		mv /tmp/.webif/config-$1-new /tmp/.webif/config-$1 2>&- >&-
		oldval=""
	}
	equal "$oldval" "$3" || echo "$2=\"$3\"" >> /tmp/.webif/config-$1
}

#
# validate form variables using validate.awk
#
validate() {
	if empty "$1"; then
		eval "$(awk -f /usr/lib/webif/validate.awk)"
	else
		eval "$(echo "$1" | awk -f /usr/lib/webif/validate.awk)"
	fi
}

#
# Functions applicable to package management.
#

is_package_installed() {
	# $1 = package name
	# returns 0 if package is installed.
	for LOCATION in $(grep "^dest\>" /etc/opkg.conf 2>/dev/null | cut -d ' ' -f 3); do

		if [ "$LOCATION" = "/" ]; then
		        paths="$paths /usr/lib/opkg/status"
		else
		        paths="$paths $LOCATION/usr/lib/opkg/status"
		fi
	done
	[ -z "$paths" ] && return 1
	grep -q " $1\$" $paths >> /dev/null 2>&1
}

install_package() {
	# $1 = package name or URL
	# returns 0 if success.
	# if package is not found, and it isn't a URL, then it'll
	# try an 'opkg update' to see if it can locate it. Does
	# emit output to std devices.
	echo "@TR<<Installing package>>..."
	local retval
	opkg -force-overwrite install "$@" | sed 's/&/\&amp;/; s/"/\&#34;/; s/'\''/\&#39;/; s/\$/\&#36;/; s/</\&lt;/; s/>/\&gt;/; s/\\/\&#92;/; s/|/\&#124;/;'
	retval="$?"
	[ "$retval" != "0" ] && {
		! equal "$(echo "$1" | grep -q ":\/\/")" "0" && {
			# wasn't a URL, so update
			echo "@TR<<Refreshing outdated packages lists>>..."
			opkg update
			opkg -force-overwrite -force-defaults install "$@"
			retval="$?"
		}
	}
	[ "$retval" != "0" ] && echo "@TR<<Package install failed.>>"
	[ "$retval" -eq 0 ]
}

remove_package() {
	# global PKG_REMOVE_OPTIONS
	# $1 = package names
	# returns 0 if success.
	opkg $PKG_REMOVE_OPTIONS remove "$@" | sed 's/&/\&amp;/; s/"/\&#34;/; s/'\''/\&#39;/; s/\$/\&#36;/; s/</\&lt;/; s/>/\&gt;/; s/\\/\&#92;/; s/|/\&#124;/;'
}

update_package_list() {
	opkg update >> /dev/null
}

has_pkgs() {
	local pcnt=0
	local nothave=0
	local retval=0;
	for pkg in "$@"; do
		pcnt=$((pcnt + 1))
		empty $(opkg list_installed | grep "^$pkg ") && {
			echo -n "<p>@TR<<features_require_package#Features on this page require the package>>: \"<b>$pkg</b>\". &nbsp;<a href=\"/cgi-bin/webif/system-ipkg.sh?action=install&amp;pkg=$pkg&amp;prev=$SCRIPT_NAME\">@TR<<features_install#install now>></a>.</p>"
			retval=1;
			nothave=$((nothave + 1))
		}
	done	
	equal "$pcnt" "$nothave" && {
		_savebutton=""
	}
	return $retval;
}
