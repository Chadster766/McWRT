#!/bin/sh

. /usr/lib/webif/webif.sh

#########################################################

write_hints()
{
  [ -d "/tmp/asterisk" ] || mkdir -p /tmp/asterisk
  TMP_FPATH="/tmp/asterisk/extensions.conf"
  DEST_FPATH="/etc/asterisk/extensions.conf"

  echo '<strong>Writing Hints</strong><br>'

  HINT_LINE=$(cat $DEST_FPATH | grep -n "hints" | cut -f 1 -d ':')
  if [ "$HINT_LINE" = "" ]; then
    cat $DEST_FPATH > $TMP_FPATH 
    echo -e '\n[hints]'  >> $TMP_FPATH 
  else
    cat $DEST_FPATH | head -n $HINT_LINE > $TMP_FPATH 
  fi

  echo 'exten => FXO1,hint,SIP/FXO1'  >> $TMP_FPATH 
  echo 'exten => FXO2,hint,SIP/FXO2'  >> $TMP_FPATH 
  echo 'exten => FXO3,hint,SIP/FXO3'  >> $TMP_FPATH 
  echo 'exten => FXO4,hint,SIP/FXO4'  >> $TMP_FPATH 

  COUNT=0
  EXT=$CONFIG_system_AST_ServerId

  while [  $COUNT -lt 8 ]; do
    eval DIAL=\$CONFIG_extensions_AST_Ext"$COUNT"Dial
    echo 'exten => '$EXT',hint,'$DIAL  >> $TMP_FPATH
  let COUNT=$COUNT+1
  let EXT=$EXT+1
done
    echo '' >> $TMP_FPATH

  rm $DEST_FPATH
  cp $TMP_FPATH $DEST_FPATH
}

#==================================================================#

write_sip_conf ()
{
  [ -d "/tmp/asterisk" ] || mkdir -p /tmp/asterisk
  TMP_FPATH="/tmp/asterisk/sip_custom.conf"
  DEST_FPATH="/etc/asterisk/sip_custom.conf"

  echo '<strong>Writing sip_custom.conf</strong><br>'

  # Start with a new file
  ## rm /etc/asterisk/sip_custom.conf

  echo -e '; Written by MARS\n' > $TMP_FPATH
  COUNT=1
  while [  $COUNT -lt 5 ]; 
  do
    eval sip_reg_user=\$CONFIG_trunks_AST_SIP_RegUser"$COUNT"
    if [ "$sip_reg_user" != "" ]; then
      eval sip_reg_pwd=\$CONFIG_trunks_AST_SIP_RegPwd"$COUNT"
      eval sip_reg_host=\$CONFIG_trunks_AST_SIP_RegHost"$COUNT"
      eval sip_reg_port=\$CONFIG_trunks_AST_SIP_RegPort"$COUNT"
      if [ "-$sip_reg_port" != "-" ]; then
        echo -e 'register => '$sip_reg_user':'$sip_reg_pwd'@'$sip_reg_host':'$sip_reg_port'\n' >> $TMP_FPATH
      else
        echo -e 'register => '$sip_reg_user':'$sip_reg_pwd'@'$sip_reg_host'/'$sip_reg_user'\n' >> $TMP_FPATH
      fi
    fi
    let COUNT=$COUNT+1
  done


#-------------------------------------------------------------------------------------------------------#

echo '<strong>Writing SIP extensions</strong><br>'

# Append the defined SIP Extensions as "friends" in sip_custom.conf
  echo '; Local SIP Extensions definitions.' >> $TMP_FPATH
  COUNT=0
  EXT=$CONFIG_system_AST_ServerId
  while [  $COUNT -lt 8 ]; do
    eval PWD=\$CONFIG_extensions_AST_Ext"$COUNT"Password
    eval TYPE=\$CONFIG_extensions_AST_Ext"$COUNT"Type

  if [ "-$PWD" != "-" ] && [ "$TYPE" = "SIP" ]; then
    eval DIAL=\$CONFIG_extensions_AST_Ext"$COUNT"Dial
    eval CALLER_ID=\$CONFIG_extensions_AST_Ext"$COUNT"CallerID
    eval DTMF_MODE=\$CONFIG_extensions_AST_Ext"$COUNT"DtmfMode
    eval PORT=\$CONFIG_extensions_AST_Ext"$COUNT"Port
    eval CARE_INVITE=\$CONFIG_extensions_AST_Ext"$COUNT"CareInvite
    eval NAT=\$CONFIG_extensions_AST_Ext"$COUNT"NAT
    eval FROM_USER=\$CONFIG_extensions_AST_Ext"$COUNT"FromUser
    eval FROM_DOMAIN=\$CONFIG_extensions_AST_Ext"$COUNT"FromDomain

   if [ "$DTMF_MODE" = "" ]; then
      DTMF_MODE="rfc2833"
    fi
    echo '['$EXT']' >> $TMP_FPATH
    echo 'type=friend' >> $TMP_FPATH
    echo 'secret='$PWD >> $TMP_FPATH
    echo 'port='$PORT >> $TMP_FPATH
    echo 'nat='$NAT >> $TMP_FPATH
    echo 'canreinvite='$CARE_INVITE  >> $TMP_FPATH
    echo 'call-limit=100'  >> $TMP_FPATH
    echo 'mailbox='$EXT'@default' >> $TMP_FPATH
    echo 'host=dynamic' >> $TMP_FPATH
    echo 'dtmfmode='$DTMF_MODE >> $TMP_FPATH
    echo 'dial=SIP/'$EXT >> $TMP_FPATH
    if [ "$FROM_USER" != "" ]; then
      echo 'from_user='$FROM_USER >> $TMP_FPATH
    fi
    if [ "$FROM_DOMAIN" != "" ]; then
      echo 'from_domain='$FROM_DOMAIN >> $TMP_FPATH
    fi
    echo 'allow=ulaw' >> $TMP_FPATH
    echo 'allow=gsm' >> $TMP_FPATH
    echo -e 'context=default\n' >> $TMP_FPATH
#    echo -e 'callerid='$CALLER_ID'\n' >> $TMP_FPATH
  fi
  let COUNT=$COUNT+1
  let EXT=$EXT+1
done

#-------------------------------------------------------------------------------------------------------#

  echo '<strong>Writing FXO Trunks</strong><br>'

  # Write the defined SIP FXO in sip_custom.conf
  sip_pstn_codec_ulaw=$CONFIG_trunks_AST_PSTN_TrunksCodecULaw
  sip_pstn_codec_gsm=$CONFIG_trunks_AST_PSTN_TrunksCodecGSM

  sip_pstn_trunk=$CONFIG_trunks_AST_PSTN_TRUNK
  sip_pstn_trunk_port=$CONFIG_trunks_AST_PSTN_TRUNK_PORT
  sip_pstn_trunk_context=$CONFIG_trunks_AST_PSTN_TRUNK_Context
  sip_pstn_caller_id=$CONFIG_trunks_AST_PSTN_CallerID

# Append the defined PSTN FXO in sip_custom.conf
  if [ "-$sip_pstn_trunk" != "-" ]; then
    echo '[FXO1]  ; FXO1' >> $TMP_FPATH
    echo 'username=FXO1' >> $TMP_FPATH
    echo 'type=friend' >> $TMP_FPATH
    echo 'secret=FXO1' >> $TMP_FPATH
    echo ';qualify=yes' >> $TMP_FPATH
    echo 'call-limit=100'  >> $TMP_FPATH
#    echo 'port=5061' >> $TMP_FPATH
    echo 'port='$sip_pstn_trunk_port >> $TMP_FPATH
    echo 'host=dynamic' >> $TMP_FPATH
    echo 'dial='$sip_pstn_trunk >> $TMP_FPATH
####    echo 'dtmfmode=info' >> $TMP_FPATH
    echo 'dtmfmode='$DTMF_MODE >> $TMP_FPATH
    echo 'careinvite=yes' >> $TMP_FPATH
    if [ "-$sip_pstn_codec_ulaw" != "-" ]; then
      echo 'allow=ulaw' >> $TMP_FPATH
    fi
    if [ "-$sip_pstn_codec_gsm" != "-" ]; then
      echo 'allow=gsm' >> $TMP_FPATH
    fi
    echo -e 'context='$sip_pstn_trunk_context'\n' >> $TMP_FPATH
#    echo -e 'callerid='$sip_pstn_caller_id'\n' >> $TMP_FPATH
  fi

  COUNT=2
  while [  $COUNT -lt 5 ]; do

    eval sip_pstn_trunk=\$CONFIG_trunks_AST_PSTN_TRUNK"$COUNT"
    if [ "-$sip_pstn_trunk" != "-" ]; then
      eval sip_pstn_trunk_port=\$CONFIG_trunks_AST_PSTN_TRUNK_PORT"$COUNT"
      eval sip_pstn_trunk_context=\$CONFIG_trunks_AST_PSTN_TRUNK_Context"$COUNT"
      eval sip_pstn_caller_id=\$CONFIG_trunks_AST_PSTN_CallerID"$COUNT"

  # Write the defined SIP FXOs in sip_custom.conf
      echo '[FXO'$COUNT']  ; FXO'$COUNT >> $TMP_FPATH
      echo 'username=FXO'$COUNT >> $TMP_FPATH
      echo 'type=friend' >> $TMP_FPATH
      echo 'secret=FXO'$COUNT >> $TMP_FPATH
      echo ';qualify=yes' >> $TMP_FPATH
      echo 'call-limit=100'  >> $TMP_FPATH
#      echo 'port=5061' >> $TMP_FPATH
      echo 'port='$sip_pstn_trunk_port >> $TMP_FPATH
      echo 'host=dynamic' >> $TMP_FPATH
      echo 'dial='$sip_pstn_trunk >> $TMP_FPATH
###      echo 'dtmfmode=info' >> $TMP_FPATH
      echo 'dtmfmode='$DTMF_MODE >> $TMP_FPATH
      echo 'careinvite=yes' >> $TMP_FPATH
      if [ "-$sip_pstn_codec_ulaw" != "-" ]; then
        echo 'allow=ulaw' >> $TMP_FPATH
      fi
      if [ "-$sip_pstn_codec_gsm" != "-" ]; then
        echo 'allow=gsm' >> $TMP_FPATH
      fi
      echo -e 'context='$sip_pstn_trunk_context'\n' >> $TMP_FPATH
    fi

    let COUNT=$COUNT+1
  done

#-------------------------------------------------------------------------------------------------------#

  echo '<strong>Writing SIP Trunks</strong><br>'

  eval auth=\$CONFIG_trunks_AST_Authentication
  eval domain=\$CONFIG_trunks_AST_AuthDomain
  eval dtmfmode=\$CONFIG_trunks_AST_TrunksDtmfMode
  eval sip_voip_codec_ulaw=\$CONFIG_trunks_AST_VoIP_TrunksCodecULaw
  eval sip_voip_codec_gsm=\$CONFIG_trunks_AST_VoIP_TrunksCodecGSM

  COUNT=1
  while [  $COUNT -lt 5 ]; do
    eval sip_reg_user=\$CONFIG_trunks_AST_SIP_RegUser"$COUNT"
      if [ "-$sip_reg_user" != "-" ]; then

        eval sip_reg_pwd=\$CONFIG_trunks_AST_SIP_RegPwd"$COUNT"
        eval sip_reg_host=\$CONFIG_trunks_AST_SIP_RegHost"$COUNT"
        eval sip_reg_port=\$CONFIG_trunks_AST_SIP_RegPort"$COUNT"
        eval sip_reg_context=\$CONFIG_trunks_AST_SIP_RegContext"$COUNT"

        echo '; SIP VoIP Account'$COUNT >> $TMP_FPATH
        echo '['$sip_reg_user']' >> $TMP_FPATH
        echo 'type=friend' >> $TMP_FPATH
        echo 'username='$sip_reg_user  >> $TMP_FPATH

        if [ "$auth" != "plain-text" ]; then
          echo 'auth='$auth  >> $TMP_FPATH
        fi
        if [ "$auth" = "md5" ]; then
          echo 'fromuser='$sip_reg_user  >> $TMP_FPATH
          echo 'fromdomain='$domain  >> $TMP_FPATH
        fi
        #Broadvoice exception
###    if [ "$domain" = "sip.broadvoice.com" ]; then
            echo 'user=phone' >> $TMP_FPATH
            echo 'insecure=very'  >> $TMP_FPATH
            echo 'authname='$sip_reg_user  >> $TMP_FPATH
#      echo 'dtmfmode=inband'  >> $TMP_FPATH
###    fi
        echo 'dtmfmode='$dtmfmode  >> $TMP_FPATH
        echo 'secret='$sip_reg_pwd  >> $TMP_FPATH
        echo 'host='$sip_reg_host  >> $TMP_FPATH
        if [ "-$sip_reg_port" != "-" ]; then
          echo 'port='$sip_reg_port  >> $TMP_FPATH
        else
          echo 'port=5060'  >> $TMP_FPATH
        fi
        echo 'call-limit=100'  >> $TMP_FPATH
        echo 'canreinvite=no' >> $TMP_FPATH
        echo 'qualify=no' >> $TMP_FPATH
        echo 'disallow=all' >> $TMP_FPATH
        if [ "-$sip_voip_codec_ulaw" != "-" ]; then
          echo 'allow=ulaw' >> $TMP_FPATH
        fi
        if [ "-$sip_voip_codec_gsm" != "-" ]; then
          echo 'allow=gsm' >> $TMP_FPATH
        fi
        echo -e 'context='$sip_reg_context'\n' >> $TMP_FPATH
      fi
    let COUNT=$COUNT+1
  done

  rm $DEST_FPATH
  cp $TMP_FPATH $DEST_FPATH
}

#==================================================================#
#==================================================================#


# $1 UserName
# $2 Pwd
# $3 DEST_PATH

write_remote_server()
{
  echo '['$1']' >> $3
  echo 'type=friend' >> $3
  echo 'user='$1 >> $3
  echo 'secret='$2 >> $3
  echo 'host=dynamic' >> $3
  echo -e 'context=default\n' >> $3
#  echo -e 'trunk=yes\n' >> $3
}

#==================================================================#

write_iax_conf ()
{
  TMP_FPATH="/tmp/asterisk/iax_custom.conf"
  DEST_FPATH="/etc/asterisk/iax_custom.conf"

  server_id=$CONFIG_system_AST_ServerId

  echo '<strong>Writing iax_custom.conf</strong><br>'
###  rm /etc/asterisk/iax_custom.conf
  echo -e '; Written by MARS\n' > $TMP_FPATH
  echo '; IAX2 and Remote MARS servers Registrations.' >> $TMP_FPATH
  iax_reg_user1=$CONFIG_trunks_AST_IAX_RegUser1
  if [ "-$iax_reg_user1" != "-" ]; then
    iax_reg_pwd1=$CONFIG_trunks_AST_IAX_RegPwd1
    iax_reg_host1=$CONFIG_trunks_AST_IAX_RegHost1
    echo 'register => '$iax_reg_user1':'$iax_reg_pwd1'@'$iax_reg_host1 >> $TMP_FPATH
  fi
  iax_reg_user2=$CONFIG_trunks_AST_IAX_RegUser2
  if [ "-$iax_reg_user2" != "-" ]; then
    iax_reg_pwd2=$CONFIG_trunks_AST_IAX_RegPwd2
    iax_reg_host2=$CONFIG_trunks_AST_IAX_RegHost2
    echo 'register => '$iax_reg_user2':'$iax_reg_pwd2'@'$iax_reg_host2 >> $TMP_FPATH
  fi
  iax_reg_user3=$CONFIG_trunks_AST_IAX_RegUser3
  if [ "-$iax_reg_user3" != "-" ]; then
    iax_reg_pwd3=$CONFIG_trunks_AST_IAX_RegPwd3
    iax_reg_host3=$CONFIG_trunks_AST_IAX_RegHost3
    echo 'register => '$iax_reg_user3':'$iax_reg_pwd3'@'$iax_reg_host3 >> $TMP_FPATH
  fi
  iax_reg_user4=$CONFIG_trunks_AST_IAX_RegUser4
  if [ "-$iax_reg_user4" != "-" ]; then
    iax_reg_pwd4=$CONFIG_trunks_AST_IAX_RegPwd4
    iax_reg_host4=$CONFIG_trunks_AST_IAX_RegHost4
    echo 'register => '$iax_reg_user4':'$iax_reg_pwd4'@'$iax_reg_host4 >> $TMP_FPATH
  fi

#-------------------------------------------------------------------------#
  # Get local server username and pwd
  if [ "$server_id" = "200" ]; then
    this_server_name=$CONFIG_servers_AST_Server1UserName
    this_server_pwd=$CONFIG_servers_AST_Server1Pwd
  elif [ "$server_id" = "300" ]; then
    this_server_name=$CONFIG_servers_AST_Server2UserName
    this_server_pwd=$CONFIG_servers_AST_Server2Pwd
  elif [ "$server_id" = "400" ]; then
    this_server_name=$CONFIG_servers_AST_Server3UserName
    this_server_pwd=$CONFIG_servers_AST_Server3Pwd
  elif [ "$server_id" = "500" ]; then
    this_server_name=$CONFIG_servers_AST_Server4UserName
    this_server_pwd=$CONFIG_servers_AST_Server4Pwd
  fi

  uci set mars.system.AST_ThisServerName=$this_server_name
  uci set mars.system.AST_ThisServerPwd=$this_server_pwd

#-------------------------------------------------------------------------#
  # MARS Server1 registration
  if [ "$server_id" != "200" ]; then
    host=$CONFIG_servers_AST_Server1Host
    if [ "-$host" != "-" ]; then
      echo 'register => '$this_server_name':'$this_server_pwd'@'$host  >> $TMP_FPATH
    fi
  fi

  # MARS Server2 registration
  if [ "$server_id" != "300" ]; then
    host=$CONFIG_servers_AST_Server2Host
    if [ "-$host" != "-" ]; then
      echo 'register => '$this_server_name':'$this_server_pwd'@'$host  >> $TMP_FPATH
    fi
  fi
  # MARS Server3 registration
  if [ "$server_id" != "400" ]; then
    host=$CONFIG_servers_AST_Server3Host
    if [ "-$host" != "-" ]; then
      echo 'register => '$this_server_name':'$this_server_pwd'@'$host  >> $TMP_FPATH
    fi
  fi
  # MARS Server4 registration
  if [ "$server_id" != "500" ]; then
    host=$CONFIG_servers_AST_Server4Host
    if [ "-$host" != "-" ]; then
      echo 'register => '$this_server_name':'$this_server_pwd'@'$host  >> $TMP_FPATH
    fi
  fi
  echo -e '\n'  >> $TMP_FPATH

#-------------------------------------------------------------------------#

  # This server definition
  echo '; Remote MARS Server definitions.' >> $TMP_FPATH

  # Remote MARS Server1 definition
  if [ "$server_id" != "200" ]; then
    host=$CONFIG_servers_AST_Server1Host
    if [ "$host" != "" ]; then
      user_name=$CONFIG_servers_AST_Server1UserName
      pwd=$CONFIG_servers_AST_Server1Pwd
      write_remote_server $user_name $pwd $TMP_FPATH
    fi
  fi

  # Remote MARS Server2 definition
  if [ "$server_id" != "300" ]; then
    host=$CONFIG_servers_AST_Server2Host
    if [ "$host" != "" ]; then
      user_name=$CONFIG_servers_AST_Server2UserName
      pwd=$CONFIG_servers_AST_Server2Pwd
      write_remote_server $user_name $pwd $TMP_FPATH
    fi
  fi

  # Remote MARS Server3 definition
  if [ "$server_id" != "400" ]; then
    host=$CONFIG_servers_AST_Server3Host
    if [ "$host" != "" ]; then
      user_name=$CONFIG_servers_AST_Server3UserName
      pwd=$CONFIG_servers_AST_Server3Pwd
      write_remote_server $user_name $pwd $TMP_FPATH
    fi
  fi

  # Remote MARS Server4 definition
  if [ "$server_id" != "500" ]; then
    host=$CONFIG_servers_AST_Server4Host
    if [ "$host" != "" ]; then
      user_name=$CONFIG_servers_AST_Server4UserName
      pwd=$CONFIG_servers_AST_Server4Pwd
      write_remote_server $user_name $pwd $TMP_FPATH
    fi
  fi

#-------------------------------------------------------------------------#

# Local extensions definitions
echo '; Local Extensions definitions.' >> $TMP_FPATH
echo '<strong>Writing Extensions</strong><br>'
COUNT=0
EXT=$CONFIG_system_AST_ServerId
while [  $COUNT -lt 8 ]; do
  eval PWD=\$CONFIG_extensions_AST_Ext"$COUNT"Password
  eval TYPE=\$CONFIG_extensions_AST_Ext"$COUNT"Type
  eval DIAL=\$CONFIG_extensions_AST_Ext"$COUNT"Dial
  eval CALLER_ID=\$CONFIG_extensions_AST_Ext"$COUNT"CallerID
  eval PORT=\$CONFIG_extensions_AST_Ext"$COUNT"Port

  if [ "-$PWD" != "-" ] && [ "$TYPE" = "IAX2" ]; then
    echo -e '['$EXT']' >> $TMP_FPATH
    echo 'type=friend' >> $TMP_FPATH
    echo 'secret='$PWD >> $TMP_FPATH
    echo 'port='$PORT >> $TMP_FPATH
#    echo 'nat=yes' >> $TMP_FPATH
    echo 'mailbox='$EXT'@default' >> $TMP_FPATH
    echo 'host=dynamic' >> $TMP_FPATH
    echo 'dial=IAX2/'$EXT >> $TMP_FPATH
    echo 'call-limit=100'  >> $TMP_FPATH
    echo 'allow=ulaw' >> $TMP_FPATH
    echo 'allow=gsm' >> $TMP_FPATH
    echo -e 'context=default\n' >> $TMP_FPATH
#    echo -e 'callerid='$CALLER_ID'\n' >> $TMP_FPATH
  fi
  let COUNT=$COUNT+1
  let EXT=$EXT+1
done

#-------------------------------------------------------------------------#

  sip_voip_codec_ulaw=$CONFIG_trunks_AST_VOIP_TrunksCodecULaw
  sip_voip_codec_gsm=$CONFIG_trunks_AST_VOIP_TrunksCodecGSM

  COUNT=1
  while [  $COUNT -lt 5 ]; do

    eval iax_reg_user=\$CONFIG_trunks_AST_IAX_RegUser"$COUNT"
    if [ "-$iax_reg_user" != "-" ]; then
      eval iax_reg_pwd=\$CONFIG_trunks_AST_IAX_RegPwd"$COUNT"
      eval iax_reg_host=\$CONFIG_trunks_AST_IAX_RegHost"$COUNT"
      eval iax_reg_context=\$CONFIG_trunks_AST_IAX_RegContext"$COUNT"

      echo '; IAX2 VoIP Account'$COUNT' (Inbound)' >> $TMP_FPATH
      echo '['$iax_reg_user']' >> $TMP_FPATH
      echo 'type=friend' >> $TMP_FPATH
      echo 'secret='$iax_reg_pwd  >> $TMP_FPATH
      echo 'host='$iax_reg_host  >> $TMP_FPATH
      if [ "-$sip_voip_codec_ulaw" != "-" ]; then
        echo 'allow=ulaw' >> $TMP_FPATH
      fi
      if [ "-$sip_voip_codec_gsm" != "-" ]; then
        echo 'allow=gsm' >> $TMP_FPATH
      fi
      if [ "$auth" != "plain-text" ]; then
        echo 'auth='$auth  >> $TMP_FPATH
      fi
      echo -e 'context='$iax_reg_context'\n' >> $TMP_FPATH
    fi
    let COUNT=$COUNT+1
  done

  rm $DEST_FPATH
  cp $TMP_FPATH $DEST_FPATH
}

#==================================================================#
#==================================================================#


write_mailboxes()
{
  SMTP_Server=$CONFIG_system_AST_SMTP_Server
  language=$CONFIG_system_AST_ServiceLanguage
  server_id=$CONFIG_system_AST_ServerId
  echo '<strong>Writing Mailboxes</strong><br>'
#  rm /etc/asterisk/voicemail_custom.conf
  echo '; Voicemail Configuration' > /etc/asterisk/voicemail_custom.conf
  echo '[general]' >> /etc/asterisk/voicemail_custom.conf
  echo 'format=wav49' >> /etc/asterisk/voicemail_custom.conf
  echo 'mailcmd=/usr/sbin/mini_sendmail -t -v -s'$SMTP_Server >> /etc/asterisk/voicemail_custom.conf
  echo 'emaildateformat=%A, %d %B %Y at %H:%M:%S' >> /etc/asterisk/voicemail_custom.conf
  echo 'minmessage=5'  >> /etc/asterisk/voicemail_custom.conf
  echo 'maxsilence=5'  >> /etc/asterisk/voicemail_custom.conf
  echo 'maxmessage=300'  >> /etc/asterisk/voicemail_custom.conf
  if [ $language = "F" ]; then
    echo 'emailsubject=MARS: Nouveau message (${VM_MSGNUM}) Boîte Vocale: ${VM_MAILBOX}' >>/etc/asterisk/voicemail_custom.conf
    echo 'emailbody=Vous venez de recevoir un message !\nDurée: ${VM_DUR} (No: ${VM_MSGNUM})\nBoîte Vocale: ${VM_MAILBOX}\nDe: ${VM_CALLERID}\nLe: ${VM_DATE}.\nMerci !\n\n\t--MARS\n' >>/etc/asterisk/voicemail_custom.conf
  else
    echo 'emailsubject=MARS: New message (${VM_MSGNUM})' >>/etc/asterisk/voicemail_custom.conf
    echo 'emailbody=You just received a ${VM_DUR} long message (number ${VM_MSGNUM})\nMailbox: ${VM_MAILBOX}\nFrom: ${VM_CALLERID}\nOn: ${VM_DATE}.\nThanks!\n\n\t--MARS\n' >>/etc/asterisk/voicemail_custom.conf
  fi
  echo '' >> /etc/asterisk/voicemail_custom.conf

  echo '[default]' >> /etc/asterisk/voicemail_custom.conf

  COUNT=0
  EXT=$server_id
  while [  $COUNT -lt 8 ]; do
    eval PWD=\$CONFIG_extensions_AST_Ext"$COUNT"Password
    eval EMAIL=\$CONFIG_extensions_AST_Ext"$COUNT"EMail
    if [ "-$PWD" != "-" ];  then
#      echo $EXT' => '$PWD',,'$EMAIL',,attach=yes|saycid=no|envelope=no|delete=no' >> '/etc/asterisk/voicemail_custom.conf'
      echo $EXT' => '$EXT',,'$EMAIL',,attach=yes|saycid=no|envelope=no|delete=no' >> '/etc/asterisk/voicemail_custom.conf'
    fi
    let COUNT=$COUNT+1
    let EXT=$EXT+1
  done
  echo ''  >> '/etc/asterisk/voicemail_custom.conf'
  echo '#include voicemail_additional.conf' >> '/etc/asterisk/voicemail_custom.conf'

#  lc=$(wc -l /etc/asterisk/voicemail.conf | awk '{print $1}')
#  head -n $(($lc-4)) /etc/asterisk/voicemail.conf > /tmp/vmtemp.conf
#  cat /etc/asterisk/voicemail_custom.conf >> /tmp/vmtemp.conf
  rm /etc/asterisk/voicemail.conf
#  mv /tmp/vmtemp.conf /etc/asterisk/voicemail.conf
  mv /etc/asterisk/voicemail_custom.conf /etc/asterisk/voicemail.conf
}

#==================================================================#

write_queue()
{
###  uci_load "mars"

  strategy=$CONFIG_queue_AST_QueueStrategy
  if [ "$strategy" = "0" ]; then
    strategy="rrmemory"
#    strategy="roundrobin"
  else
    strategy="ringall"
  fi
  echo '<strong>Writing Queue</strong><br>'
  echo '; Queue Configuration' > /etc/asterisk/queues_custom.conf

  echo '[Queue1]' >> /etc/asterisk/queues_custom.conf
  echo 'music=default' >> /etc/asterisk/queues_custom.conf
  echo 'strategy='$strategy >> /etc/asterisk/queues_custom.conf
  echo 'timeout=20' >> /etc/asterisk/queues_custom.conf
  echo 'retry=5' >> /etc/asterisk/queues_custom.conf
  echo 'maxlen=0' >> /etc/asterisk/queues_custom.conf
  echo 'periodic-announce-frequency=30' >> /etc/asterisk/queues_custom.conf
  echo 'announce-frequency=60' >> /etc/asterisk/queues_custom.conf
  echo 'announce-holdtime=yes' >> /etc/asterisk/queues_custom.conf
  echo 'monitor-join=no' >> /etc/asterisk/queues_custom.conf

  COUNT=0
  while [  $COUNT -lt 8 ]; do
    eval is_member=\$CONFIG_queue_AST_Queue"$COUNT"
    if [ "-$is_member" != "-" ];  then
      eval DIAL=\$CONFIG_extensions_AST_Ext"$COUNT"Dial
      ### IMPORTANT Test var because writing empty 'member=>' cause asterisk to CRASH with seg fault
      if [ "$DIAL" != "" ]; then
        echo 'member=>'$DIAL >> '/etc/asterisk/queues_custom.conf'
      fi
    fi
    let COUNT=$COUNT+1
  done

### TEST ###
###  rm /tmp/apply-mars.sh
}

#==================================================================#

write_mail_cmd()
{
  SMTP_Server=$CONFIG_system_AST_SMTP_Server

  echo '<strong>Writing Mail Cmd file.</strong><br>'

  echo 'mailcmd=/usr/sbin/mini_sendmail -t -v' $SMTP_Server > /etc/asterisk/voicemail_email.conf
}

#==================================================================#

radio_button()
{
  if [ "$2" = "$3" ]; then
    opts="checked=\"checked\" "
  else
    opts=""
  fi
  echo "<input type=\"radio\" name=\""$1"\" value=\""$3"\" " $opts " > $4  "
}

checkbox()
{
  if [ "$2" = "$3" ]; then
    opts="checked=\"checked\" "
  else
    opts=""
  fi
  echo "<input type=\"checkbox\" name=\""$1"\" value=\""$3"\" "$opts" > $4  "
}

select_2()
{
  if [ "$3" = "" ]; then
    opts=""
  else
    opts=" disabled=\"disabled\""
  fi
  echo "<select id=\""$1"\" name=\""$1"\" "$opts" >"

  select_id=$1
  select_default=$2
}

option_2()
{
  if [ "$1" = "$select_default" ]; then 
    option_selected=" selected=\"selected\""
  else
    option_selected=""
  fi
  if  [ "$2" != "" ]; then
    option_title=$2
  else 
    option_title=$1
  fi
  echo  "<option "$option_selected" value=\""$1"\"> "$option_title" </option>"
}
# +=========================================================================+

load_html()
{
  Fname=$1

  if [ "$Fname" = "" ]; then
    echo 'ERROR No Fname given'
  else
    result=0

  FIELDS=$(grep "<!-- Fields" $Fname | sed -e 's/<!-- Fields //; s/-->//;')
  #TMP Traces
  if [ "$DEBUG" = "1" ]; then
    echo "SCRIPT_NAME="$SCRIPT_NAME"<br>"
    echo "FORM_action="$FORM_action"<br>"
    echo "FORM_submit="$FORM_submit"<br>"
    echo "Fname="$Fname"<br>"
    echo "FIELDS="$FIELDS"<br>"

  fi

  PFNAME=FORM_$(echo $FIELDS | awk '{print $1}')
  eval FNAME=\$$PFNAME
# echo $PFNAME"="$FNAME"<br>"

  SED_STR=""

  for P_NO in $(seq 2 32); do
    FIELD_NAME=$(echo $FIELDS | awk '{print $'$P_NO'}')
    if [ "$FIELD_NAME" != "" ]; then
      FFNAME=FORM_$FIELD_NAME
      eval FVALUE=\$$FFNAME

      if [ "$DEBUG" = "1" ]; then
        echo "FIELD_NAME"$P_NO"= "$FIELD_NAME" FVALUE= "$FVALUE"<br>"
      fi
      # Test for TextArea field
      if [ "$(echo $FIELD_NAME | grep Area)" != "" ]; then
        SED_STR=$SED_STR"s/name=\"$FIELD_NAME\">/name=\"$FIELD_NAME\"> $FVALUE/;"
      # Test for Text field
      elif [ "$(echo $FIELD_NAME | grep TextF)" != "" ]; then
        SED_STR=$SED_STR"s/name=\"$FIELD_NAME\" value=\"\"/name=\"$FIELD_NAME\" value=\"$FVALUE\"/;"
      elif [ "$(echo $FIELD_NAME | grep Check)" != "" ]; then
        if [ "$FVALUE" = "1" ]; then
          SED_STR=$SED_STR"s/name=\"$FIELD_NAME\"/name=\"$FIELD_NAME\" checked=\"checked\"/;"
        fi
      elif [ "$(echo $FIELD_NAME | grep Radio)" != "" ]; then
        SED_STR=$SED_STR"s/name=\"$FIELD_NAME\" value=\"$FVALUE\" /name=\"$FIELD_NAME\" value=\"$FVALUE\" checked=\"checked\"/;"
      elif [ "$(echo $FIELD_NAME | grep Select)" != "" ]; then
        SED_STR=$SED_STR"s/option value=\"$FVALUE\"/option value=\"$FVALUE\" selected=\"selected\" /;"
      fi
    fi
  done

  cat $Fname | sed "s|<!--SCRIPT_NAME-->|$SCRIPT_NAME|; s|<!--Fname-->|$FORM_Fname|; \
  $SED_STR "
fi
}

