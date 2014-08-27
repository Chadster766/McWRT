#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh

update_changes

case "$CHANGES" in
	""|0)FORM_mode=nochange
esac
case "$FORM_mode" in
	nochange) header "$FORM_cat" "" "@TR<<No config change.|No configuration changes were made.>>";;
	clear)
		rm -r /tmp/.webif/* >&- 2>&-
		rm /tmp/.uci/* >&- 2>&-
		header "$FORM_cat" "" "@TR<<Config discarded.|Your configuration changes have been discarded.>>"
		CHANGES=""
		echo "${FORM_prev:+<meta http-equiv=\"refresh\" content=\"2; URL=$FORM_prev\" />}"
		;;
	review)
		header "$FORM_cat" "" "@TR<<Config changes:|Current configuration changes:>>"
		for configname in $(ls /tmp/.webif/config-* 2>&-); do
			grep = $configname >&- 2>&- && {
				echo -n "<h3>${configname#/tmp/.webif/config-}</h3><br /><pre>"
				cat $configname
				echo '</pre><br />'
			}
		done
		CONFIGFILES=""
		for configname in $(ls /tmp/.webif/file-* 2>&-); do
			exists "$configname" && {
				configname=$(echo $configname | sed s/'\/tmp\/.webif\/'//g)
				CONFIGFILES="$CONFIGFILES ${configname#/tmp/.webif/file-}"
			}
		done
		for configname in $(ls /tmp/.uci/* 2>&- | grep -v "\\.lock\$" 2>&-); do
				echo -n "<h3>${configname#/tmp/.uci/}</h3><br /><pre>"
				cat $configname
				echo '</pre><br />'
		done
		CONFIGFILES="${CONFIGFILES:+<h3 style="display:inline">Config files: </h3>$CONFIGFILES<br />}"
		echo "$CONFIGFILES"

		EDITED_FILES=""
		for edited_file in $(find "/tmp/.webif/edited-files/" -type f 2>&-); do
			edited_file=$(echo "$edited_file" | sed s/'\/tmp\/.webif\/edited-files'//g)
			EDITED_FILES="$EDITED_FILES $edited_file"
		done

		EDITED_FILES="${EDITED_FILES:+<h3 style="display:inline">@TR<<Edited files:>> </h3>$EDITED_FILES<br />}"
		echo "$EDITED_FILES"
		;;
	save)
		header "$FORM_cat" "" "@TR<<Updating config...|Updating your configuration...>>"
		CHANGES=""
		echo "<pre>"
		sh /usr/lib/webif/apply.sh 2>&1
		echo "</pre>${FORM_prev:+<meta http-equiv=\"refresh\" content=\"4; URL=$FORM_prev\" />}"
		;;
esac

SCRIPT_NAME=${FORM_prev:-$SCRIPT_NAME}

footer

?>
