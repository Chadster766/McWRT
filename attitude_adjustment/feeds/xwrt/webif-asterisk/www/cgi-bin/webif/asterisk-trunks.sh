#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
. /www/cgi-bin/webif/ast_functions.sh

uci_load mars

header "Asterisk" "Trunks" "<center><img src="/images/mars1.jpg"> @TR<<Asterisk Trunks>> <img src="/images/mars1.jpg"></center>" '' "$SCRIPT_NAME"


#--------------------------------------------------------------------------#


if [ "$FORM_action" = "" ]; then
  FORM_trunks_proto=${AST_TrunksProto:-$CONFIG_trunks_AST_TrunksProto}
  FORM_trunks_proto=${FORM_trunks_proto:-'0'}
  FORM_authentication=$CONFIG_trunks_AST_Authentication
  FORM_auth_domain=$CONFIG_trunks_AST_AuthDomain
  FORM_trunks_dtmfmode=$CONFIG_trunks_AST_TrunksDtmfMode
  FORM_caller_name=$CONFIG_trunks_AST_CallerName
  FORM_voip_trunks_codec_ulaw=$CONFIG_trunks_AST_VoIP_TrunksCodecULaw
  FORM_voip_trunks_codec_gsm=$CONFIG_trunks_AST_VoIP_TrunksCodecGSM
  FORM_pstn_trunks_codec_ulaw=$CONFIG_trunks_AST_PSTN_TrunksCodecULaw
  FORM_pstn_trunks_codec_gsm=$CONFIG_trunks_AST_PSTN_TrunksCodecGSM

  # IAX Registrations Form
  FORM_iax_reg_user1=$CONFIG_trunks_AST_IAX_RegUser1
  FORM_iax_reg_pwd1=$CONFIG_trunks_AST_IAX_RegPwd1
  FORM_iax_reg_host1=$CONFIG_trunks_AST_IAX_RegHost1
  FORM_iax_reg_port1=$CONFIG_trunks_AST_IAX_RegPort1
  FORM_iax_reg_context1=$CONFIG_trunks_AST_IAX_RegContext1

  FORM_iax_reg_user2=$CONFIG_trunks_AST_IAX_RegUser2
  FORM_iax_reg_pwd2=$CONFIG_trunks_AST_IAX_RegPwd2
  FORM_iax_reg_host2=$CONFIG_trunks_AST_IAX_RegHost2
  FORM_iax_reg_port2=$CONFIG_trunks_AST_IAX_RegPort2
  FORM_iax_reg_context2=$CONFIG_trunks_AST_IAX_RegContext2

  FORM_iax_reg_user3=$CONFIG_trunks_AST_IAX_RegUser3
  FORM_iax_reg_pwd3=$CONFIG_trunks_AST_IAX_RegPwd3
  FORM_iax_reg_host3=$CONFIG_trunks_AST_IAX_RegHost3
  FORM_iax_reg_port3=$CONFIG_trunks_AST_IAX_RegPort3
  FORM_iax_reg_context3=$CONFIG_trunks_AST_IAX_RegContext3

  FORM_iax_reg_user4=$CONFIG_trunks_AST_IAX_RegUser4
  FORM_iax_reg_pwd4=$CONFIG_trunks_AST_IAX_RegPwd4
  FORM_iax_reg_host4=$CONFIG_trunks_AST_IAX_RegHost4
  FORM_iax_reg_port4=$CONFIG_trunks_AST_IAX_RegPort4
  FORM_iax_reg_context4=$CONFIG_trunks_AST_IAX_RegContext4

  FORM_sip_reg_user1=$CONFIG_trunks_AST_SIP_RegUser1
  FORM_sip_reg_pwd1=$CONFIG_trunks_AST_SIP_RegPwd1
  FORM_sip_reg_host1=$CONFIG_trunks_AST_SIP_RegHost1
  FORM_sip_reg_port1=$CONFIG_trunks_AST_SIP_RegPort1
  FORM_sip_reg_context1=$CONFIG_trunks_AST_SIP_RegContext1

  FORM_sip_reg_user2=$CONFIG_trunks_AST_SIP_RegUser2
  FORM_sip_reg_pwd2=$CONFIG_trunks_AST_SIP_RegPwd2
  FORM_sip_reg_host2=$CONFIG_trunks_AST_SIP_RegHost2
  FORM_sip_reg_port2=$CONFIG_trunks_AST_SIP_RegPort2
  FORM_sip_reg_context2=$CONFIG_trunks_AST_SIP_RegContext2

  FORM_sip_reg_user3=$CONFIG_trunks_AST_SIP_RegUser3
  FORM_sip_reg_pwd3=$CONFIG_trunks_AST_SIP_RegPwd3
  FORM_sip_reg_host3=$CONFIG_trunks_AST_SIP_RegHost3
  FORM_sip_reg_port3=$CONFIG_trunks_AST_SIP_RegPort3
  FORM_sip_reg_context3=$CONFIG_trunks_AST_SIP_RegContext3

  FORM_sip_reg_user4=$CONFIG_trunks_AST_SIP_RegUser4
  FORM_sip_reg_pwd4=$CONFIG_trunks_AST_SIP_RegPwd4
  FORM_sip_reg_host4=$CONFIG_trunks_AST_SIP_RegHost4
  FORM_sip_reg_port4=$CONFIG_trunks_AST_SIP_RegPort4
  FORM_sip_reg_context4=$CONFIG_trunks_AST_SIP_RegContext4

  FORM_pstn_trunk=$CONFIG_trunks_AST_PSTN_TRUNK
  FORM_pstn_trunk_port=$CONFIG_trunks_AST_PSTN_TRUNK_PORT
  FORM_pstn_trunk_context=$CONFIG_trunks_AST_PSTN_TRUNK_Context
  FORM_pstn_caller_id=$CONFIG_trunks_AST_PSTN_CallerID

  FORM_pstn_trunk2=$CONFIG_trunks_AST_PSTN_TRUNK2
  FORM_pstn_trunk_port2=$CONFIG_trunks_AST_PSTN_TRUNK_PORT2  
  FORM_pstn_trunk_context2=$CONFIG_trunks_AST_PSTN_TRUNK_Context2
  FORM_pstn_caller_id2=$CONFIG_trunks_AST_PSTN_CallerID2

  FORM_pstn_trunk3=$CONFIG_trunks_AST_PSTN_TRUNK3
  FORM_pstn_trunk_port3=$CONFIG_trunks_AST_PSTN_TRUNK_PORT3
  FORM_pstn_trunk_context3=$CONFIG_trunks_AST_PSTN_TRUNK_Context3
  FORM_pstn_caller_id3=$CONFIG_trunks_AST_PSTN_CallerID3

  FORM_pstn_trunk4=$CONFIG_trunks_AST_PSTN_TRUNK4
  FORM_pstn_trunk_port4=$CONFIG_trunks_AST_PSTN_TRUNK_PORT4
  FORM_pstn_caller_id4=$CONFIG_trunks_AST_PSTN_CallerID4
  FORM_pstn_trunk_context4=$CONFIG_trunks_AST_PSTN_TRUNK_Context4
fi

#--------------------------------------------------------------------------#

if [ "$FORM_action" = "Save Changes" ]; then
  uci_set mars trunks AST_TrunksProto "$FORM_trunks_proto"
  uci_set mars trunks AST_Authentication "$FORM_authentication"
  uci_set mars trunks AST_AuthDomain "$FORM_auth_domain"
  uci_set mars trunks AST_TrunksDtmfMode "$FORM_trunks_dtmfmode"
  uci_set mars trunks AST_VoIP_TrunksCodecULaw "$FORM_voip_trunks_codec_ulaw"
  uci_set mars trunks AST_VoIP_TrunksCodecGSM "$FORM_voip_trunks_codec_gsm"
  uci_set mars trunks AST_CallerName "$FORM_caller_name"
#  echo "Saving IAX Registration 1 ...<br>"

  uci_set mars trunks AST_IAX_RegUser1 "$FORM_iax_reg_user1"
  uci_set mars trunks AST_IAX_RegPwd1 "$FORM_iax_reg_pwd1"
  uci_set mars trunks AST_IAX_RegHost1 "$FORM_iax_reg_host1"
  uci_set mars trunks AST_IAX_RegPort1 "$FORM_iax_reg_port1"
  uci_set mars trunks AST_IAX_RegContext1 "$FORM_iax_reg_context1"
#  echo "Saving IAX Registration 2 ...<br>"

  uci_set mars trunks AST_IAX_RegUser2 "$FORM_iax_reg_user2"
  uci_set mars trunks AST_IAX_RegPwd2 "$FORM_iax_reg_pwd2"
  uci_set mars trunks AST_IAX_RegHost2 "$FORM_iax_reg_host2"
  uci_set mars trunks AST_IAX_RegPort2 "$FORM_iax_reg_port2"
  uci_set mars trunks AST_IAX_RegContext2 "$FORM_iax_reg_context2"

#  echo "Saving IAX Registration 3 ...<br>"

  uci_set mars trunks AST_IAX_RegUser3 "$FORM_iax_reg_user3"
  uci_set mars trunks AST_IAX_RegPwd3 "$FORM_iax_reg_pwd3"
  uci_set mars trunks AST_IAX_RegHost3 "$FORM_iax_reg_host3"
  uci_set mars trunks AST_IAX_RegPort3 "$FORM_iax_reg_port3"
  uci_set mars trunks AST_IAX_RegContext3 "$FORM_iax_reg_context3"
#  echo "Saving IAX Registration 4 ...<br>"

  uci_set mars trunks AST_IAX_RegUser4 "$FORM_iax_reg_user4"
  uci_set mars trunks AST_IAX_RegPwd4 "$FORM_iax_reg_pwd4"
  uci_set mars trunks AST_IAX_RegHost4 "$FORM_iax_reg_host4"
  uci_set mars trunks AST_IAX_RegPort4 "$FORM_iax_reg_port4"
  uci_set mars trunks AST_IAX_RegContext4 "$FORM_iax_reg_context4"

#  echo "Saving SIP Registration 1 ...<br>"
  uci_set mars trunks AST_SIP_RegUser1 "$FORM_sip_reg_user1"
  uci_set mars trunks AST_SIP_RegPwd1 "$FORM_sip_reg_pwd1"
  uci_set mars trunks AST_SIP_RegHost1 "$FORM_sip_reg_host1"
  uci_set mars trunks AST_SIP_RegPort1 "$FORM_sip_reg_port1"
  uci_set mars trunks AST_SIP_RegContext1 "$FORM_sip_reg_context1"
#  echo "Saving SIP Registration 2 ...<br>"
  uci_set mars trunks AST_SIP_RegUser2 "$FORM_sip_reg_user2"
  uci_set mars trunks AST_SIP_RegPwd2 "$FORM_sip_reg_pwd2"
  uci_set mars trunks AST_SIP_RegHost2 "$FORM_sip_reg_host2"
  uci_set mars trunks AST_SIP_RegPort2 "$FORM_sip_reg_port2"
  uci_set mars trunks AST_SIP_RegContext2 "$FORM_sip_reg_context2"
#  echo "Saving SIP Registration 3 ...<br>"
  uci_set mars trunks AST_SIP_RegUser3 "$FORM_sip_reg_user3"
  uci_set mars trunks AST_SIP_RegPwd3 "$FORM_sip_reg_pwd3"
  uci_set mars trunks AST_SIP_RegHost3 "$FORM_sip_reg_host3"
  uci_set mars trunks AST_SIP_RegPort3 "$FORM_sip_reg_port3"
  uci_set mars trunks AST_SIP_RegContext3 "$FORM_sip_reg_context3"
#  echo "Saving SIP Registration 4 ...<br>"
  uci_set mars trunks AST_SIP_RegUser4 "$FORM_sip_reg_user4"
  uci_set mars trunks AST_SIP_RegPwd4 "$FORM_sip_reg_pwd4"
  uci_set mars trunks AST_SIP_RegHost4 "$FORM_sip_reg_host4"
  uci_set mars trunks AST_SIP_RegPort4 "$FORM_sip_reg_port4"
  uci_set mars trunks AST_SIP_RegContext4 "$FORM_sip_reg_context4"

  uci_set mars trunks AST_PSTN_TrunksCodecULaw  "$FORM_pstn_trunks_codec_ulaw"
  uci_set mars trunks AST_PSTN_TrunksCodecGSM "$FORM_pstn_trunks_codec_gsm"
#  echo "Saving PSTN FXO1 ...<br>"

  uci_set mars trunks AST_PSTN_TRUNK "$FORM_pstn_trunk"
  uci_set mars trunks AST_PSTN_TRUNK_PORT "$FORM_pstn_trunk_port"
  uci_set mars trunks AST_PSTN_TRUNK_Context "$FORM_pstn_trunk_context"
  uci_set mars trunks AST_PSTN_CallerID "$FORM_pstn_caller_id"

#  echo "Saving PSTN FXO2 ...<br>"
  uci_set mars trunks AST_PSTN_TRUNK2 "$FORM_pstn_trunk2"
  uci_set mars trunks AST_PSTN_TRUNK_PORT2 "$FORM_pstn_trunk_port2"
  uci_set mars trunks AST_PSTN_TRUNK_Context2 "$FORM_pstn_trunk_context2"
  uci_set mars trunks AST_PSTN_CallerID2 "$FORM_pstn_caller_id2"

#  echo "Saving PSTN FXO3 ...<br>"
  uci_set mars trunks AST_PSTN_TRUNK3 "$FORM_pstn_trunk3"
  uci_set mars trunks AST_PSTN_TRUNK_PORT3 "$FORM_pstn_trunk_port3"
  uci_set mars trunks AST_PSTN_TRUNK_Context3 "$FORM_pstn_trunk_context3"
  uci_set mars trunks AST_PSTN_CallerID3 "$FORM_pstn_caller_id3"

#  echo "Saving PSTN FXO4 ...<br>"
  uci_set mars trunks AST_PSTN_TRUNK4 "$FORM_pstn_trunk4"
  uci_set mars trunks AST_PSTN_TRUNK_PORT4 "$FORM_pstn_trunk_port4"
  uci_set mars trunks AST_PSTN_TRUNK_Context4 "$FORM_pstn_trunk_context4"
  uci_set mars trunks AST_PSTN_CallerID4 "$FORM_pstn_caller_id4"

#  echo "<strong>Applying changes to Config File</strong><br>"
###TMP###    write_sip_conf
###TMP###    write_iax_conf
####  echo '</td></tr></center></table>'
fi


###??? if [ "$FORM_action" = "Save Changes" ]; then

###???   uci_set mars trunks AST_TrunksProto "$FORM_trunks_proto"
###???   uci_set mars trunks AST_Authentication "$FORM_authentication"
###???   uci_set mars trunks AST_AuthDomain "$FORM_auth_domain"
###???   uci_set mars trunks AST_TrunksDtmfMode "$FORM_trunks_dtmfmode"
###???   uci_set mars trunks AST_VoIP_TrunksCodecULaw "$FORM_voip_trunks_codec_ulaw"
###???   uci_set mars trunks AST_VoIP_TrunksCodecGSM "$FORM_voip_trunks_codec_gsm"

###???   uci_set mars trunks AST_PSTN_TrunksCodecULaw  "$FORM_pstn_trunks_codec_ulaw"
###???   uci_set mars trunks AST_PSTN_TrunksCodecGSM "$FORM_pstn_trunks_codec_gsm"

###???   COUNT=1
###???   while [  "$COUNT" -lt 5 ]; do

###???     VNAME=FORM_iax_reg_user"$COUNT"
###???     eval VAR=\$$VNAME
###??? ##    eval VAR=\$FORM_iax_reg_user"$COUNT"
###???     uci_set mars trunk AST_IAX_RegUser"$COUNT" "$VAR"
###???     eval VAR=\$FORM_iax_reg_pwd"$COUNT"
###???     uci_set mars trunk AST_IAX_RegPwd"$COUNT" $VAR
###???     eval VAR=\$FORM_iax_reg_host"$COUNT"
###???     uci_set mars trunk AST_IAX_RegHost"$COUNT" $VAR
###???     eval VAR=\$FORM_iax_reg_pwd"$COUNT"
###???     uci_set mars trunk AST_IAX_RegPwd"$COUNT" $VAR
###???     eval VAR=\$FORM_iax_reg_pwd"$COUNT"
###???     uci_set mars trunk AST_IAX_RegPort"$COUNT" $VAR
###???     eval VAR=\$FORM_iax_reg_context"$COUNT"
###???     uci_set mars trunk AST_IAX_RegContext"$COUNT" $VAR

###???     eval VAR=\$FORM_sip_reg_user"$COUNT"
###???     uci_set mars trunk AST_SIP_RegUser$COUNT $VAR
###???     eval VAR=\$FORM_sip_reg_pwd"$COUNT"
###???     uci_set mars trunk AST_SIP_RegPwd"$COUNT" $VAR
###???     eval VAR=\$FORM_sip_reg_host"$COUNT"
###???     uci_set mars trunk AST_SIP_RegHost"$COUNT" $VAR
###???     eval VAR=\$FORM_sip_reg_pwd"$COUNT"
###???     uci_set mars trunk AST_SIP_RegPwd"$COUNT" $VAR
###???     eval VAR=\$FORM_sip_reg_pwd"$COUNT"
###???     uci_set mars trunk AST_SIP_RegPort"$COUNT" $VAR
###???     eval VAR=\$FORM_sip_reg_context"$COUNT"
###???     uci_set mars trunk AST_SIP_RegContext"$COUNT" $VAR

###???     if [ "$COUNT" = "1" ]; then
###???       uci_set mars trunks AST_PSTN_TRUNK "$FORM_pstn_trunk"
###???       uci_set mars trunks AST_PSTN_TRUNK_PORT "$FORM_pstn_trunk_port"
###???       uci_set mars trunks AST_PSTN_TRUNK_Context "$FORM_pstn_trunk_context"
###???       uci_set mars trunks AST_PSTN_CallerID "$FORM_pstn_caller_id"
###???     else
###???       eval VAR=\$FORM_pstn_trunk"$COUNT"
###???       uci_set mars trunks AST_PSTN_TRUNK"$COUNT" "$VAR"
###???       eval VAR=\$FORM_pstn_trunk_port"$COUNT"
###???       uci_set mars trunks AST_PSTN_TRUNK_PORT"$COUNT" "$VAR"
###???       eval VAR=\$FORM_pstn_trunk_context"$COUNT"
###???       uci_set mars trunks AST_PSTN_TRUNK_Context"$COUNT" "$VAR"
###???       eval VAR=\$FORM_pstn_caller_id"$COUNT"
###???       uci_set mars trunks AST_PSTN_CallerID"$COUNT" "$VAR"
###???     fi
###???     let COUNT=$COUNT+1
###???   done
###??? fi

#####################################################

echo '<center><table><tr><td colspan=3>'
echo '<h3>VoIP Trunks Protocol</h3></td>'
echo '<td colspan=2; align=center><a href="/MngrManual_en.html#Trunks" target="_blank"><h3>Help</h3></a></td></tr>'

echo '<tr><th>Authentication </th>'
echo '<td>'
select_2 authentication "$FORM_authentication"
option_2 "plain-text" "plain text"
option_2 md5 "md5"
option_2 rsa "rsa"
echo '</select></td><th>Auth. Domain</th>'
echo '<td><INPUT type=text name="auth_domain" value="'$FORM_auth_domain'" size="24" maxlength="64"></td></tr>'

echo '<tr><th>DtmfMode</th>'
echo '<td>'
select_2 trunks_dtmfmode "$FORM_trunks_dtmfmode"
option_2 rfc2833 "rfc2833"
option_2 info "info"
option_2 inband "inband"
echo '</select></td></tr>'

echo '<tr><th>Codecs: </th><td>'
checkbox voip_trunks_codec_ulaw "$FORM_voip_trunks_codec_ulaw" 1 uLaw
echo '</td><td colspan=2>'
checkbox voip_trunks_codec_gsm "$FORM_voip_trunks_codec_gsm" 1 GSM
echo '</td></tr>'

echo '<tr><th>Caller Name: </th>'
echo '<td colspan ="3"><INPUT type=text name="caller_name" value="'$FORM_caller_name'" size="24" maxlength="64"></td></tr>'

echo '</td></tr>'


#-----------------------------------------------------------------------------------------------------------------------+
# SIP Registrations Form

echo '<tr><td colspan=5><br><h3>SIP Registrations</h3></td></tr>'
echo '<tr><th></th><th>UserName (/DID)</th><th>Password</th><th>Host / [Port] (default 5060)</th><th>Context</td></tr>'
echo '</td></tr><tr><td colspan=4>'
echo '<tr><th>SIP1: </th>'
echo '<td><INPUT type=text name="sip_reg_user1" value="'$FORM_sip_reg_user1'" size="24" maxlength="128"></td>'
echo '<td><INPUT type=text name="sip_reg_pwd1" value="'$FORM_sip_reg_pwd1'" size="24" maxlength="128"></td>'
echo '<td><INPUT type=text name="sip_reg_host1" value="'$FORM_sip_reg_host1'" size="24" maxlength="128">'
echo '<INPUT type=text name="sip_reg_port1" value="'$FORM_sip_reg_port1'" size="6" maxlength="6"></td>'
echo '<td>'
select_2 sip_reg_context1 "$FORM_sip_reg_context1"
option_2 "incoming" "incoming"
option_2 "incoming-direct" "incoming-direct"
option_2 "dial-in_dictation" dial-in_dictation
echo '</select></td></tr>'

echo '<tr><th>SIP2: </th>'
echo '<td><INPUT type=text name="sip_reg_user2" value="'$FORM_sip_reg_user2'" size="24" maxlength="128"></td>'
echo '<td><INPUT type=text name="sip_reg_pwd2" value="'$FORM_sip_reg_pwd2'" size="24" maxlength="128"></td>'
echo '<td><INPUT type=text name="sip_reg_host2" value="'$FORM_sip_reg_host2'" size="24" maxlength="128">'
echo '<INPUT type=text name="sip_reg_port2" value="'$FORM_sip_reg_port2'" size="6" maxlength="6"></td>'
echo '<td>'
select_2 sip_reg_context2 "$FORM_sip_reg_context2"
option_2 "incoming" "incoming"
option_2 "incoming-direct" "incoming-direct"
option_2 "dial-in_dictation" dial-in_dictation
echo '</select></td></tr>'

echo '<tr><th>SIP3: </th>'
echo '<td><INPUT type=text name="sip_reg_user3" value="'$FORM_sip_reg_user3'" size="24" maxlength="128"></td>'
echo '<td><INPUT type=text name="sip_reg_pwd3" value="'$FORM_sip_reg_pwd3'" size="24" maxlength="128"></td>'
echo '<td><INPUT type=text name="sip_reg_host3" value="'$FORM_sip_reg_host3'" size="24" maxlength="128">'
echo '<INPUT type=text name="sip_reg_port3" value="'$FORM_sip_reg_port3'" size="6" maxlength="6"></td>'
echo '<td>'
select_2 sip_reg_context3 "$FORM_sip_reg_context3"
option_2 "incoming" "incoming"
option_2 "incoming-direct" "incoming-direct"
option_2 "dial-in_dictation" dial-in_dictation
echo '</select></td></tr>'
echo '<tr><th>SIP4: </th>'
echo '<td><INPUT type=text name="sip_reg_user4" value="'$FORM_sip_reg_user4'" size="24" maxlength="128"></td>'
echo '<td><INPUT type=text name="sip_reg_pwd4" value="'$FORM_sip_reg_pwd4'" size="24" maxlength="128"></td>'
echo '<td><INPUT type=text name="sip_reg_host4" value="'$FORM_sip_reg_host4'" size="24" maxlength="128">'
echo '<INPUT type=text name="sip_reg_port4" value="'$FORM_sip_reg_port4'" size="6" maxlength="6"></td>'
echo '<td>'
select_2 sip_reg_context4 "$FORM_sip_reg_context4"
option_2 "incoming" "incoming"
option_2 "incoming-direct" "incoming-direct"
option_2 "dial-in_dictation" dial-in_dictation
echo '</select></td></tr>'


#-----------------------------------------------------------------------------------------------------------------------+

echo '<tr><td colspan=5><br><h3>IAX Registrations</h3></td></tr>'

echo '<tr><th></th><th>UserName (/DID)</th><th>Password</th><th>Host</th><th>Context</th></tr>'
echo '<tr><th>IAX1: </th>'
echo '<td><INPUT type=text name="iax_reg_user1" value="'$FORM_iax_reg_user1'" size="24" maxlength="64"></td>'
echo '<td><INPUT type=text name="iax_reg_pwd1" value="'$FORM_iax_reg_pwd1'" size="24" maxlength="64"></td>'
echo '<td><INPUT type=text name="iax_reg_host1" value="'$FORM_iax_reg_host1'" size="24" maxlength="64"></td>'
echo '<td>'
select_2 iax_reg_context1 "$FORM_iax_reg_context1"
option_2 "incoming" "incoming"
option_2 "incoming-direct" "incoming-direct"
option_2 "dial-in_dictation" dial-in_dictation
echo '</select></td></tr>'
echo '<tr><th>IAX2: </th>'
echo '<td><INPUT type=text name="iax_reg_user2" value="'$FORM_iax_reg_user2'" size="24" maxlength="64"></td>'
echo '<td><INPUT type=text name="iax_reg_pwd2" value="'$FORM_iax_reg_pwd2'" size="24" maxlength="64"></td>'
echo '<td><INPUT type=text name="iax_reg_host2" value="'$FORM_iax_reg_host2'" size="24" maxlength="64"></td>'
echo '<td>'
select_2 iax_reg_context2 "$FORM_iax_reg_context2"
option_2 "incoming" "incoming"
option_2 "incoming-direct" "incoming-direct"
option_2 "dial-in_dictation" dial-in_dictation
echo '</select></td></tr>'
echo '<tr><th>IAX3: </th>'
echo '<td><INPUT type=text name="iax_reg_user3" value="'$FORM_iax_reg_user3'" size="24" maxlength="64"></td>'
echo '<td><INPUT type=text name="iax_reg_pwd3" value="'$FORM_iax_reg_pwd3'" size="24" maxlength="64"></td>'
echo '<td><INPUT type=text name="iax_reg_host3" value="'$FORM_iax_reg_host3'" size="24" maxlength="64"></td>'
echo '<td>'
select_2 iax_reg_context3 "$FORM_iax_reg_context3"
option_2 "incoming" "incoming"
option_2 "incoming-direct" "incoming-direct"
option_2 "dial-in_dictation" dial-in_dictation
echo '</select></td></tr>'
echo '<tr><th>IAX4: </th>'
echo '<td><INPUT type=text name="iax_reg_user4" value="'$FORM_iax_reg_user4'" size="24" maxlength="64"></td>'
echo '<td><INPUT type=text name="iax_reg_pwd4" value="'$FORM_iax_reg_pwd4'" size="24" maxlength="64"></td>'
echo '<td><INPUT type=text name="iax_reg_host4" value="'$FORM_iax_reg_host4'" size="24" maxlength="64"></td>'
echo '<td>'
select_2 iax_reg_context4 "$FORM_iax_reg_context4"
option_2 "incoming" "incoming"
option_2 "incoming-direct" "incoming-direct"
option_2 "dial-in_dictation" dial-in_dictation
echo '</select></td></tr>'

#-----------------------------------------------------------------------------------------------------------------------+

# PSTN FXO1
echo '<tr><td colspan=5><h3>PSTN Trunks</h3></td></tr>'
echo '<tr><td>Codecs: </td><td>'
checkbox pstn_trunks_codec_ulaw "$FORM_pstn_trunks_codec_ulaw" 1 uLaw
echo '</td><td colspan=2>'
checkbox pstn_trunks_codec_gsm "$FORM_pstn_trunks_codec_gsm" 1 GSM
echo '</td></tr>'

echo '<tr><td colspan=5><h3></h3></td></tr>'
echo '<tr><th></th><th>DIAL</th><th>Port</th><th>CallerID</th><th>Context</td></tr>'

echo '<tr><th>Line 1 (FXO1)</th>'
echo '<td><INPUT type=text name="pstn_trunk" value="'$FORM_pstn_trunk'" size="24" maxlength="128"></td>'
echo '<td><INPUT type=text name="pstn_trunk_port" value="'$FORM_pstn_trunk_port'" size="8" maxlength="8"></td>'
echo '<td><INPUT type=text name="pstn_caller_id" value="'$FORM_pstn_caller_id'" size="16" maxlength="32"></td><td>'
select_2 pstn_trunk_context $FORM_pstn_trunk_context
option_2 "incoming" "incoming"
option_2 "incoming-direct" "incoming-direct"
option_2 "dial-in_dictation" dial-in_dictation
echo '</select></td></tr>'

#  PSTN FXO2
echo '<tr><th>Line 2 (FXO2)</th>'
echo '<td><INPUT type=text name="pstn_trunk2" value="'$FORM_pstn_trunk2'" size="24" maxlength="128"></td>'
echo '<td><INPUT type=text name="pstn_trunk_port2" value="'$FORM_pstn_trunk_port2'" size="8" maxlength="8"></td>'
echo '<td><INPUT type=text name="pstn_caller_id2" value="'$FORM_pstn_caller_id2'" size="16" maxlength="32"></td><td>'
select_2 pstn_trunk_context2 "$FORM_pstn_trunk_context2"
option_2 "incoming" "incoming"
option_2 "incoming-direct" "incoming-direct"
option_2 "dial-in_dictation" dial-in_dictation
echo '</select></td></tr>'

#  PSTN FXO3
echo '<tr><th>Line 3 (FXO3)</th>'
echo '<td><INPUT type=text name="pstn_trunk3" value="'$FORM_pstn_trunk3'" size="24" maxlength="128"></td>'
echo '<td><INPUT type=text name="pstn_trunk_port3" value="'$FORM_pstn_trunk_port3'" size="8" maxlength="8"></td>'
echo '<td><INPUT type=text name="pstn_caller_id3" value="'$FORM_pstn_caller_id3'" size="16" maxlength="32"></td><td>'
select_2 pstn_trunk_context3 "$FORM_pstn_trunk_context3"
option_2 "incoming" "incoming"
option_2 "incoming-direct" "incoming-direct"
option_2 "dial-in_dictation" dial-in_dictation
echo '</select></td></tr>'

#  PSTN FXO4
echo '<tr><th>Line 4 (FXO4)</th>'
echo '<td><INPUT type=text name="pstn_trunk4" value="'$FORM_pstn_trunk4'" size="24" maxlength="128"></td>'
echo '<td><INPUT type=text name="pstn_trunk_port4" value="'$FORM_pstn_trunk_port4'" size="8" maxlength="8"></td>'
echo '<td><INPUT type=text name="pstn_caller_id4" value="'$FORM_pstn_caller_id4'" size="16" maxlength="32"></td><td>'
select_2 pstn_trunk_context4 "$FORM_pstn_trunk_context4"
option_2 "incoming" "incoming"
option_2 "incoming-direct" "incoming-direct"
option_2 "dial-in_dictation" dial-in_dictation
echo '</select></td></tr>'
echo '</table>'

#    echo "<a href=${SCRIPT_NAME}>Back</a>"
#    echo "<script language=\"JavaScript\">location.href=\"${SCRIPT_NAME}\"></script>"
### cat <<EOF
### &nbsp;&nbsp;&nbsp; Going back ... <a href="${SCRIPT_NAME}">${SCRIPT_NAME}</a><br /><br />
### <script language="JavaScript" type="text/javascript">
### setTimeout('top.location.href=\"$SCRIPT_NAME\"',"300")
### </script>
### EOF

footer ?>

<!--
##WEBIF:name:Asterisk:140:Trunks
-->

