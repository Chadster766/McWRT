MARS: A simple Asterisk GUI for OpenWRT . 
By Noël Bouchard
Tele Data Inc. 2007-2009
Email: mars@teledata.qc.ca

+========================================================+

Installation

Backup 
Upload the compressed file to the root directory of the target system 
and decompress  using:

tar -czvf files.tar.gz 
tar -xzvf mars.tar.gz 

See the 'File content' section below for details.

- Asterisk Menu 

The file /www/cgi-bin/webif/.categories needs to be edited so that the 
Asterisk menu shows on the WebIf menu

##WEBIF:category:-
##WEBIF:category:Asterisk

- Asterisk auto start 

The file /etc/init.d/asterisk contains asterisk startup script.
To have asterisk start on reboot add a symbolic link to this file
to the /etc/rc.d directory:

cd /etc/rc.d
ln -s /etc/init.d/asterisk /etc/rc.d/S80asterisk

- External Storage

The scripts assume that the asterisk sounds and music on hold 
file as weel as the voicemail storage will be on an extenal directory.
These are assume to be in the external directory: /mnt/disc0_1/asterisk

the following symbolic link in /usr/lib/asterisk are used

ln -s /mnt/disc0_1/asterisk/moh-native /usr/lib/asterisk/moh-native
ln -s /mnt/disc0_1/asterisk/sounds /usr/lib/asterisk/sounds
ln -s /mnt/disc0_1/asterisk/voicemail /var/spool/asterisk/voicemail

You may need to modifiy the /etc/init.d/asterisk startup script and the symbolic links
if you use a different location.

+========================================================+

Startup

The configuration is read from the uci config file (/etc/config/mars)
to asterisk global variable. The config can be loaded from the command line 
after asterisk has started by issuing the following command.

asterisk -rx 'originate Local/#16 extension

This will execute the folowing dialplan code:

exten => #16,1,Set(NVRAM_READ=FALSE)
exten => #16,n,NoCDR();
exten => #16,n,Macro(get-config-file)
exten => #16,n,Hangup()

[macro-get-config-file]
; Call an agi to read and copy the config to global variable
exten => s,1,AGI(get_config_file.sh)
exten => s,n,SetGlobalVar(NVRAM_READ=TRUE)

+========================================================+

DialPlan

+========================================================+

WebCall

This feature allows to initiate a call from a given extension to an external
phone number using a HTML form on any Web Site
 

Modification to /etc/init.d/httpd 

WebCallEnabled=$(uci get mars.webcall.AST_WebCallEnabled)
if [ "$WebCallEnabled" = "1" ]; then
  httpd -p 32345 -h /www/webcall -r MARS
fi

+========================================================+

Warranty

+========================================================+

Asterisk modules needed 

app_cdr.so  
app_chanisavail.so 
app_controlplayback.so
app_db.so 
app_dial.so 
app_echo.so
app_exec.so 
app_hasnewvoicemail.so 
app_macro.so
app_mixmonitor.so 
app_playback.so 
app_queue.so
app_read.so 
app_record.so 
app_sayunixtime.so
app_setcallerid.so 
app_setcdruserfield.so 
app_stack.so
app_system.so 
app_transfer.so 
app_userevent.so
app_verbose.so 
app_voicemail.so 
cdr_csv.so
cdr_manager.so 
chan_iax2.so 
chan_local.so
chan_sip.so 
codec_a_mu.so 
codec_gsm.so
codec_ulaw.so 
format_g729.so 
format_gsm.so
format_pcm.so 
format_sln.so 
ormat_wav.so
format_wav_gsm.so 
func_callerid.so 
func_cdr.so
func_channel.so 
func_cut.so 
func_db.so
func_env.so 
func_global.so 
func_groupcount.so
func_language.so 
func_logic.so 
func_math.so
func_md5.so 
func_moh.so 
func_realtime.so
func_strings.so 
func_timeout.so 
pbx_config.so
pbx_spool.so 
res_adsi.so 
res_agi.so
res_clioriginate.so 
res_convert.so 
res_features.so
res_indications.so 
res_monitor.so 
res_musiconhold.so

Note: This list may include module that will not be used 
+========================================================+

File content:

readme.txt : This file

/etc 

These are asterisk configuration files that were modified
for the MARS system

/etc/asterisk/asterisk.conf
/etc/asterisk/cdr.conf
/etc/asterisk/cdr_manager.conf
/etc/asterisk/extensions.conf  : This is the dialplan
/etc/asterisk/features.conf
/etc/asterisk/http.conf  : Modified for the WebCall feature
/etc/asterisk/iax.conf
/etc/asterisk/logger.conf
/etc/asterisk/manager.conf
/etc/asterisk/modules.conf  
/etc/asterisk/musiconhold.conf
/etc/asterisk/queues.conf
/etc/asterisk/rtp.conf
/etc/asterisk/sip.conf
/etc/asterisk/users.conf
/etc/asterisk/voicemail.conf

/etc/config/mars : The mars uci config file

/etc/init.d/asterisk : Modified asterisk startup script
/etc/init.d/httpd : modified httpd startup script

/usr

These are agi scripts and example WebAudio list file

/usr/lib/asterisk/agi-bin/get_config_file.sh : get config from UCI file to asterisk Global Vars
/usr/lib/asterisk/agi-bin/getlocalprefixes.sh
/usr/lib/asterisk/agi-bin/getnvram.sh
/usr/lib/asterisk/agi-bin/getnvramvars.sh : get config from nvram to asterisk Global Vars
/usr/lib/asterisk/agi-bin/islocalprefix.sh : test if a dialed phone no is in local prefix list
/usr/lib/asterisk/agi-bin/send_wa_dir.sh : email WebAudio directory to admin email
/usr/lib/asterisk/agi-bin/web_audio.sh : get URL for given program number.
/usr/lib/asterisk/agi-bin/web_audio.txt : WebAudio program list

These files are for the status display module
/usr/lib/asterisk/static-http/asterisk-status.html
/usr/lib/asterisk/static-http/asterisk-status.js
/usr/lib/asterisk/static-http/astman.css
/usr/lib/asterisk/static-http/astman.js
/usr/lib/asterisk/static-http/floating_window_with_tabs.css
/usr/lib/asterisk/static-http/floating_window_with_tabs.js
/usr/lib/asterisk/static-http/floating_window_with_tabs-skin2.css
/usr/lib/asterisk/static-http/images
/usr/lib/asterisk/static-http/prototype.js
/usr/lib/asterisk/static-http/setup.css
/usr/lib/asterisk/static-http/images/bg_pgns.gif
/usr/lib/asterisk/static-http/images/digiumlogo.gif
/usr/lib/asterisk/static-http/images/floating_window_resize.gif
/usr/lib/asterisk/static-http/images/mod_temp1_02.gif
/usr/lib/asterisk/static-http/images/mod_temp1_03.gif
/usr/lib/asterisk/static-http/images/prototype.js
/usr/lib/asterisk/static-http/images/select_arrow.gif
/usr/lib/asterisk/static-http/images/select_arrow_down.gif
/usr/lib/asterisk/static-http/images/select_arrow_over.gif
/usr/lib/asterisk/static-http/images/setup.css
/usr/lib/asterisk/static-http/images/skin2_tab_left_active.gif
/usr/lib/asterisk/static-http/images/skin2_tab_left_inactive.gif
/usr/lib/asterisk/static-http/images/skin2_tab_right_active.gif
/usr/lib/asterisk/static-http/images/skin2_tab_right_inactive.gif
/usr/lib/asterisk/static-http/images/tab_center_active.gif
/usr/lib/asterisk/static-http/images/tab_left_active.gif
/usr/lib/asterisk/static-http/images/tab_left_inactive.gif
/usr/lib/asterisk/static-http/images/tab_right_active.gif
/usr/lib/asterisk/static-http/images/tab_right_inactive.gif
/usr/lib/asterisk/static-http/images/TD_LOGO2.gif

Modified Webif File
/usr/lib/webif/apply.sh : modified to include mars apply
/usr/lib/webif/apply-mars.sh : new apply addon 
/usr/lib/webif/form.awk : One modif in header 
Modified to process MARS data on apply
/usr/lib/webif/webif.sh

/www

/www/mars.js : JavaScript used by MARS 

/www/cgi-bin/webcall/webcall_black_list.sh : Black list script for WebCall

These files implement the asterisk  GUI as webif pages.
/www/cgi-bin/webif/.categories : modified to add asterisk sub-menu
/www/cgi-bin/webif/ast_functions.sh
/www/cgi-bin/webif/asterisk-calllog.sh
/www/cgi-bin/webif/asterisk-editor.sh
/www/cgi-bin/webif/asterisk-extensions.sh
/www/cgi-bin/webif/asterisk-management.sh
/www/cgi-bin/webif/asterisk-servers.sh
/www/cgi-bin/webif/asterisk-status.sh
/www/cgi-bin/webif/asterisk-system.sh
/www/cgi-bin/webif/asterisk-trunks.sh
/www/cgi-bin/webif/asterisk-webcall.sh
/www/cgi-bin/webif/get_area_codes.sh
/www/cgi-bin/webif/parse_asterisk_calls.awk
/www/cgi-bin/webif/send_call_log.sh
/www/cgi-bin/webif/send_wrt_log.sh

Files used by the WebCall feature
/www/webcall/td_icon.ico
/www/webcall/cgi-bin/call_template.html
/www/webcall/cgi-bin/call_template_fr.html
/www/webcall/cgi-bin/webcall.sh
/www/webcall/images/talk.jpg

+========================================================+

Contact:

TELE DATA Inc.
Noel Bouchard
mars@teledata.qc.ca
or
514 723 3987 
(9-5 EST mon-fri)

+========================================================+
