#!/bin/sh
###################################################################
# killfreeloader.sh
# (c)2007 X-Wrt project (http://www.x-wrt.org)
# (c)2007-03-13 m4rc0
#
#	version 1.3
#
# Description:
#	Checks for any downloads and kills them if needed.
#
# Author(s) [in order of work date]:
#	m4rc0 <janssenmaj@gmail.com>
#
# Major revisions:
#	
#
#
# NVRAM variables referenced:
#   none
#
# Configuration files referenced:
#   /etc/config/freeloader
#
#

#Include functions
. /usr/lib/webif/webif.sh
#Include settings
. /usr/lib/webif/freeloader-include.sh

#exit immediately, no root
empty "$DOWNLOAD_ROOT" && exit

#this LS command is needed because otherwise the abort.lock and suspend.lock will not be found, some kind of caching problem on CIFS shares
ls "$DOWNLOAD_DESTINATION"  > /dev/null 2>&1

if ! [ -f "$DOWNLOAD_DESTINATION/suspend.lock" ]; then

	#if the abort.lock file exists, read the PID from the file, kill the process and remove the abort.lock
	if [ -f "$DOWNLOAD_DESTINATION/abort.lock" ] ; then

		#kill all ctorrent/curl/nzbget processes
		EXTENSION=`sed -n 1p /tmp/currentdownloadextension`

		if [ $EXTENSION = 'torrent' ]; then
			killall -9 ctorrent
		elif [ $EXTENSION = 'link' ]; then
			killall -9 curl
		elif [ $EXTENSION = 'nzb' ]; then
			killall -9 nzbget
		fi

		rm "$DOWNLOAD_DESTINATION/abort.lock"
			
		CURRENT_DOWNLOADFILE=`sed -n 1p /tmp/currentdownloadfile`
		#log status
		logstatus "The download $CURRENT_DOWNLOADFILE was aborted."
	fi	

	#check if there are .torrent or .link files in the PRIO queue and check if the PRIO queue has not been started yet
	if [ "`ls $QUEUE_PRIO/*.link $QUEUE_PRIO/*.torrent $QUEUE_PRIO/*.nzb`" != '' ]; then
		if [ -f /tmp/freeloader.lock ]; then
			if ! [ -f /tmp/prio.lock ]; then
				#create a prio.lock file
				touch /tmp/prio.lock

				#kill the getfreeloader process (so the .torrent/.link/.nzb files are NOT moved when ctorrent/curl/nzbget is killed)
				killall -15 getfreeloader.sh
	
				#kill all ctorrent/curl/nzbget processes
				EXTENSION=`sed -n 1p /tmp/currentdownloadextension`
	
				if [ $EXTENSION = 'torrent' ]; then
					killall -9 ctorrent
				elif [ $EXTENSION = 'link' ]; then
					killall -9 curl
				elif [ $EXTENSION = 'nzb' ]; then
					killall -9 nzbget
				fi

				#Clean up the control file for downloading
				rm /tmp/currentlogfile
				rm /tmp/currentdownloadfile
				rm /tmp/currentdownloadextension
				rm /tmp/freeloader.lock
				
				#log status
				logstatus "All downloads are aborted, prio download started. Normal download will be resumed later."
	
				#mail this status
				mailstatus "All downloads are aborted, prio download started. Normal download will be resumed later."
			fi
		fi
	fi
else
	#check if ctorrent/curl/nzbget is running
	if [ `pidof ctorrent curl nzbget` ]; then
		
		#kill the getfreeloader process (so the .torrent/.link/.nzb files are NOT moved when ctorrent/curl/nzbget is killed)
		killall getfreeloader.sh
		
		#kill all ctorrent/curl/nzbget processes
		EXTENSION=`sed -n 1p /tmp/currentdownloadextension`
		if [ $EXTENSION = 'torrent' ]; then
			killall -9 ctorrent
		elif [ $EXTENSION = 'link' ]; then
			killall -9 curl
		elif [ $EXTENSION = 'nzb' ]; then
			killall -9 nzbget
		fi

		#Clean up the control file for downloading
		rm /tmp/currentlogfile
		rm /tmp/currentdownloadfile
		rm /tmp/currentdownloadextension
		rm /tmp/freeloader.lock
		if [ -f /tmp/prio.lock ]; then
			rm /tmp/prio.lock
		fi

		#log status
		logstatus "All normal downloads were aborted. All processes were suspended."

		#mail this status
		mailstatus "All downloads are aborted and the processes are suspended."	
	fi
fi