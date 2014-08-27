#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
. /www/cgi-bin/webif/ast_functions.sh
echo -e 'content-type: text/html\n'

DEBUG=1

print_cgi_data()
{
  echo "+----------------------------------------+"  >>testlog.txt
  date >>testlog.txt
  echo "SCRIPT_NAME="$SCRIPT_NAME >>testlog.txt
  echo "REQUEST_METHOD="$REQUEST_METHOD >>testlog.txt
  echo "PATH_INFO="$PATH_INFO >>testlog.txt
  echo "QUERY_STRING="$QUERY_STRING >>testlog.txt
  echo "REMOTE_ADDR= "$REMOTE_ADDR >>testlog.txt
  echo "REQUEST_URI="$REQUEST_URI >>testlog.txt
  echo "HTTP_REFERER="$HTTP_REFERER >>testlog.txt
  echo "REMOTE_USER="$REMOTE_USER >>testlog.txt
  echo ""
  echo 'FirstName: '$FORM_FirstName >>testlog.txt
  echo 'LastName: '$FORM_LastName >>testlog.txt
  echo 'Company: '$FORM_Company >>testlog.txt
  echo 'Email :' $FORM_Email >>testlog.txt
  echo 'TelNo: '$FORM_TelNo >>testlog.txt
  echo 'AccountNo: '$FORM_AccountNo >>testlog.txt
  echo 'Language: '$FORM_Language >>testlog.txt
  echo 'Source: '$FORM_Source >>testlog.txt
  echo 'step: '$FORM_step >>testlog.txt
  echo 'OnSuccess: '$FORM_OnSuccess >>testlog.txt
  echo 'OnDisabled: '$FORM_OnDisabled >>testlog.txt
  echo 'OnTelNoError: '$FORM_OnTelNoError >>testlog.txt
  echo 'OnBlackList: '$FORM_OnBlackList >>testlog.txt
  echo "+----------------------------------------+"  >>testlog.txt
}
#+------------------------------------------------------------------+

WC_Enabled=$(uci get mars.webcall.AST_WebCallEnabled)
WC_MaxRetries=$(uci get mars.webcall.AST_WebCallRetries)
if [ "$WC_MaxRetries" == "" ]; then
  WC_MaxRetries=3
fi

WC_Interval=$(uci get mars.webcall.AST_WebCallInterval)
if [ "$WC_MaxInterval" == "" ]; then
  WC_MaxInterval=120
fi

disabled=$(/usr/sbin/asterisk -r -x "database get MARS WebCall")

if [ "$DUBUG" = "1" ]
then
  echo "FORM_TelNo= "$FORM_TelNo
fi
 
if [ "$WC_Enabled" = "1" ] && [ "$disabled" != "Value: Disabled" ]
then
  print_cgi_data

  if  [ "$FORM_TelNo" != "" ]; then
    in_list=$(grep $FORM_TelNo /www/cgi-bin/webcall/blacklist.txt)
    if  [ "$REMOTE_ADDR" != "" ]; then
      if [ "$in_list" = "" ]; then
        in_list=$(grep $REMOTE_ADDR /www/cgi-bin/webcall/blacklist.txt)
      fi
    fi
  fi
  # echo 'in_list='$in_list


  if [ "$in_list" = "" ]; then
    LANGUAGE=$(uci get mars.system.AST_ServiceLanguage)
    TMP='/tmp'
    ASTCONF='/etc/asterisk/asterisk.conf'
    SPOOLOUT=$(sed -n $ASTCONF -e 's/astspooldir\( \)*=>\( \)*\([^ ]\)/\3/p')/outgoing

    CALL_TELNO=$(echo "$FORM_TelNo" | sed -e 's/[ -//]//g;s/[:-z]//g') ### KEEP only numbers

    if [ "$CALL_TELNO" = "" ]; then
      ERR=1
    else
      tn_length=$(expr length $CALL_TELNO)
      if [ "$tn_length" != "10" ]; then
        ERR=1 
      else
        localext=$(echo $FORM_AccountNo | sed -e 's/[+-]//g;s/[,;pP]/w/g')
        AST_ServerId=$(uci get mars.system.AST_ServerId)
        if  [ "$FORM_AccountNo" != "" ]; then
          ext_index=$(expr $FORM_AccountNo - $AST_ServerId)
          dial=$(uci get mars.extensions.AST_Ext"$ext_index"Dial)
        else
          dial=SIP/"$AST_ServerId"
        fi
      fi
    fi

    if [ "$ERR" != "1" ]; then
 
cat >"$TMP/$CALL_TELNO" <<-EOF
Channel: $dial
MaxRetries: $WC_MaxRetries
RetryTime: $WC_Interval
WaitTime: 30
Context: webcall
extension: s
Set: OutNum=9$CALL_TELNO
Set: WC_Language=$FORM_Language
Set: FirstName=$FORM_FirstName
Set: LastName=$FORM_LastName
Set: Company=$FORM_Company
Set: Source=$FORM_Source
Priority: 1
EOF

 #### TMP NO CALL ###      
 mv "$TMP/$CALL_TELNO" "$SPOOLOUT/$CALL_TELNO"
      # next is only necessary to work around an old bug in OpenWRT's tmpfs
###      touch "$SPOOLOUT"

      echo >>/var/log/asterisk/webcall "$(date): From: $FORM_FirstName $FORM_LastName $FORM_Company $FORM_Email TelNo=$FORM_TelNo Ext=$FORM_AccountNo IP=$REMOTE_ADDR"
# Fill /www/calldata.html using call_template.html
      if [ "$LANGUAGE" = "F" ]; then
        cat /www/webcall/cgi-bin/call_template_fr.html | sed "s/<!--CallDateTime-->/$(date)/; \
s/<!--CallerFirstName-->/$FORM_FirstName/; s/<!--CallerName-->/$FORM_LastName/; \
s/<!--CallerCompany-->/$FORM_Company/; s/<!--CallerEmail-->/$FORM_Email/; \
s/<!--CallerTelNo-->/$FORM_TelNo/; s/<!--CallerIP-->/$REMOTE_ADDR/;  s/<!--Source-->/$FORM_Source/; \
s/<!--BlockTelNo-->/$FORM_TelNo/; s/<!--BlockIpAddress-->/$REMOTE_ADDR/;"   > /www/calldata.html
      else
        cat /www/webcall/cgi-bin/call_template.html | sed "s/<!--CallDateTime-->/$(date)/; \
s/<!--CallerFirstName-->/$FORM_FirstName/; s/<!--CallerName-->/$FORM_LastName/; \
s/<!--CallerCompany-->/$FORM_Company/; s/<!--CallerEmail-->/$FORM_Email/; \
s/<!--CallerTelNo-->/$FORM_TelNo/; s/<!--CallerIP-->/$REMOTE_ADDR/;  s/<!--Source-->/$FORM_Source/; \
s/<!--BlockTelNo-->/$FORM_TelNo/; s/<!--BlockIpAddress-->/$REMOTE_ADDR/;" > /www/calldata.html
      fi
    fi

    echo '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">'
    echo '<html><head><meta http-equiv="Content-Type" content="text/html;charset=ISO-8859-1">'
    echo '<title>WebCall</title>'

    URL_Success=$(uci get mars.webcall.AST_WC_OnSuccess)
    URL_TelNoError=$(uci get mars.webcall.AST_WC_OnTelNoError)

    if [ "$FORM_Language" = "E" ]
    then
      if [ "$FORM_TelNo" != "" ]
      then
        if [ "$FORM_OnSuccess" != "" ]; then
          echo "<meta http-equiv="refresh" content='0;"$FORM_OnSuccess"' />"
          echo '</head><body>'
        elif [ "$URL_Success" != "" ]; then      
          echo "<meta http-equiv="refresh" content='0;"$URL_Success"' />"
          echo '</head><body>'
        else
          echo '</head><body>'
          echo '<h1>Thank you !</h1>'
          echo '<p>You should receive a call from us soon at: '$FORM_TelNo
          echo '<p><input type="submit" value="Back" onclick="back(-1)">'
        fi
      else
        if [ "$FORM_OnTelNoError" != "" ]; then
          echo "<meta http-equiv="refresh" content='0;"$FORM_OnTelNoError"' />"
          echo '</head><body>'
        elif [ "$URL_TelNoError" != "" ]; then      
          echo "<meta http-equiv="refresh" content='0;"$URL_TelNoError"' />"
          echo '</head><body>'
        else
          echo '</head><body>'
          echo '<h1>Sorry !</h1>'
          echo '<p>Some error occured. We did get your phone number properly.'
          echo '<p><input type="submit" value="Back" onclick="back(-1)">'
        fi
      fi

    else ### Language = F

###      if [ "$FORM_TelNo" != "" ]
      if [ "$tn_length" = "10" ]
      then
        if [ "$FORM_OnSuccess" != "" ]; then
          echo "<meta http-equiv="refresh" content='0;"$FORM_OnSuccess"' />"
          echo '</head><body>'
        elif [ "$URL_Success" != "" ]; then      
          echo "<meta http-equiv="refresh" content='0;"$URL_Success"' />"
          echo '</head><body>'
        else
          echo '</head><body>'
          echo '<h1>Merci !</h1>'
          echo '<p>Vous devriez recevoir un appel très bientôt au: '$FORM_TelNo
          echo '<p><input type="submit" value="Retour" onclick="back(-1)">'
        fi
     else
        if [ "$FORM_OnTelNoError" != "" ]; then
          echo "<meta http-equiv="refresh" content='0;"$FORM_OnTelNoError"' />"
          echo '</head><body>'
        elif [ "$URL_TelNoError" != "" ]; then      
          echo "<meta http-equiv="refresh" content='0;"$URL_TelNoError"' />"
          echo '</head><body>'
        else
          echo '</head><body>'
          echo '<h1>Désolé ...</h1>'
          echo '<p>Votre numéro de téléphone est invalide'
          echo '<p><input type="submit" value="Retour" onclick="back(-1)">'
        fi
      fi
    fi
    echo '</body></html>'

  else # [in_list]
    URL_BlackList=$(uci get mars.webcall.AST_WC_OnBlackList)
    echo '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">'
    echo '<html><head><meta http-equiv="Content-Type" content="text/html;charset=ISO-8859-1">'
    echo '<title>WebCall</title>'

    if [ "$FORM_Language" = "E" ]
    then
      if [ "$FORM_OnBlackList" != "" ]; then
        echo "<meta http-equiv="refresh" content='0;"$FORM_OnBlackList"' />"
        echo '</head><body>'
      elif [ "$URL_BlackList" != "" ]; then      
        echo "<meta http-equiv="refresh" content='0;"$URL_BlackList"' />"
        echo '</head><body>'
      else
        echo '</head><body>'
        echo '<h1>Black List</h1>'
        echo '<p>Sorry, you are not allowed to use this service anymore ...'
        echo '<p><input type="submit" value="Back" onclick="back(-1)">'
      fi
    else
      if [ "$FORM_OnBlackList" != "" ]; then
        echo "<meta http-equiv="refresh" content='0;"$FORM_OnBlackList"' />"
        echo '</head><body>'
      elif [ "$URL_BlackList" != "" ]; then      
        echo "<meta http-equiv="refresh" content='0;"$URL_BlackList"' />"
        echo '</head><body>'
      else
        echo '</head><body>'
        echo '<h1>Liste Noire</h1>'
        echo "<p>Désolé. Vous n\'êtes plus autorisé à utiliser ce service..."
        echo '<p><input type="submit" value="Retour" onclick="back(-1)">'
      fi
    fi
    echo '</body></html>'

  fi

else ### [WC_Disabled = 1]

  URL_Disabled=$(uci get mars.webcall.AST_WC_OnDisabled)
  echo '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">'
  echo '<html><head><meta http-equiv="Content-Type" content="text/html;charset=ISO-8859-1">'
  echo '<title>WebCall</title>'
  if [ "$FORM_Language" = "E" ]
  then
    if [ "$FORM_OnDisabled" != "" ]; then
      echo "<meta http-equiv="refresh" content='0;"$FORM_OnDisabled"' />"
      echo '</head><body>'
    elif [ "$URL_Disabled" != "" ]; then      
      echo "<meta http-equiv="refresh" content='0;"$URL_Disabled"' />"
      echo '</head><body>'
    else
      echo '</head><body>'
      echo '<h1>WebCall Not Available</h1>'
      echo '<p>Sorry, the WebCall service is not available at this time ...'
      echo '<p><input type="submit" value="Back" onclick="back(-1)">'
    fi
  else
    if [ "$FORM_OnDisabled" != "" ]; then
      echo "<meta http-equiv="refresh" content='0;"$FORM_OnDisabled"' />"
      echo '</head><body>'
    elif [ "$URL_Disabled" != "" ]; then      
      echo "<meta http-equiv="refresh" content='0;"$URL_Disabled"' />"
      echo '</head><body>'
    else
      echo '</head><body>'
      echo '<h1>WebCall Non Disponible</h1>'
      echo "Désolé. Le service WebCall n\'est pas disponible pour le moment ..."
      echo '<input type="submit" value="Retour" onclick="back(-1)">'
    fi
  fi
  echo '</body></html>'
fi

?>