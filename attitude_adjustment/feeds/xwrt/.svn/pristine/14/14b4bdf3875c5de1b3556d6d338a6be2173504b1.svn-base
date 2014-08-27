#!/usr/bin/webif-page
<?
. /usr/lib/webif/webif.sh
. /www/cgi-bin/webif/ast_functions.sh

uci_load "mars"

header_inject_head=$(cat <<EOF
<script type="text/javascript">
<!--
function ShowAdvancedToggle()
{
  var tname, elem;
  for (i=0; i < 8; i++)
  {
    tname = 'adv_table' + i;
    if (document.getElementById("ShowAdvanced").checked)
       document.getElementById(tname).style.display = '';
    else
       document.getElementById(tname).style.display = 'none';
  }
}

-->
</script>

EOF
)


header "Asterisk" "Extensions" "<center><img src="/images/mars1.jpg"> @TR<<Asterisk Extensions>> <img src="/images/mars1.jpg"></center>" 'onload="ShowAdvancedToggle()"' "$SCRIPT_NAME"
DEBUG=0

if [ "$DEBUG" = "1" ]; then
  echo 'Form_action='$FORM_action'<br>'
  echo 'Form_submit='$FORM_submit'<br>'
fi

#--------------------------------------------------------------------------#

if [ "$FORM_action" = "" ]; then
  server_id=$CONFIG_system_AST_ServerId
  ext_id=0
  COUNT=0
  while [  $COUNT -lt 8 ]; do
    # EXT ServerId + 0 Form (200 / 300 / 400 / 500)
  
    eval VAL=\$CONFIG_extensions_AST_Ext"$COUNT"Password  
    VN=FORM_password"$COUNT"
    export $VN=$VAL
    
    eval VAL=\$CONFIG_extensions_AST_Ext"$COUNT"Type
    VN=FORM_type"$COUNT"
    export $VN=$VAL

    eval VAL=\$CONFIG_extensions_AST_Ext"$COUNT"Dial
    VN=FORM_dial"$COUNT"
    export $VN=$VAL

    eval VAL=\$CONFIG_extensions_AST_Ext"$COUNT"Port
    VN=FORM_port"$COUNT"
    let $VN=$VAL

    eval VAL=\$CONFIG_extensions_AST_Ext"$COUNT"DtmfMode
    VN=FORM_dtmfmode"$COUNT"
    export $VN=$VAL

    eval VAL=\$CONFIG_extensions_AST_Ext"$COUNT"CallerID
    VN=FORM_caller_id"$COUNT"
    export $VN="$VAL"

    eval VAL=\$CONFIG_extensions_AST_Ext"$COUNT"EMail
    VN=FORM_email"$COUNT"
    export $VN=$VAL

    eval VAL=\$CONFIG_extensions_AST_Ext"$COUNT"CareInvite
    VN=FORM_care_invite"$COUNT"
    export $VN=$VAL

    eval VAL=\$CONFIG_extensions_AST_Ext"$COUNT"NAT
    VN=FORM_nat"$COUNT"
    export $VN=$VAL

    eval VAL=\$CONFIG_extensions_AST_Ext"$COUNT"FromUser
    VN=FORM_from_user"$COUNT"
    export $VN=$VAL

    eval VAL=\$CONFIG_extensions_AST_Ext"$COUNT"FromDomain
    VN=FORM_from_domain"$COUNT"
    export $VN=$VAL

    let COUNT=$COUNT+1
  done

fi

if [ "$DEBUG" = "1" ]; then
  echo 'Form_password0='$Form_password0'<br>'
  echo 'Form_password1='$Form_password1'<br>'
fi


#--------------------------------------------------------------------------#

if [ "$FORM_action" = "Save Changes" ]; then
  COUNT=0
  ext_id=$CONFIG_system_AST_ServerId
  echo '<center><table>'
  echo "<tr><td><strong>Saving Extensions</strong></td></tr><br>"

  # Save the values to uci config file
  while [  $COUNT -lt 8 ]; 
  do
  # Save values to Config File
    if [ "$DEBUG" = "1" ]; then
      echo -e 'Saving Ext: '$COUNT'\n'
#      sleep 1
    fi
    eval VAR=\$FORM_password"$COUNT"
    uci_set mars extensions AST_Ext"$COUNT"Password $VAR
    eval VAR=\$FORM_type"$COUNT"
    uci_set mars extensions AST_Ext"$COUNT"Type $VAR
##    eval VAR=\$FORM_dial"$COUNT"
    uci_set mars extensions AST_Ext"$COUNT"Dial $VAR/$ext_id
    eval VAR=\$FORM_port"$COUNT"
    uci_set mars extensions AST_Ext"$COUNT"Port $VAR
    eval VAR=\$FORM_dtmfmode"$COUNT"
    uci_set mars extensions AST_Ext"$COUNT"DtmfMode $VAR
    eval VAR=\$FORM_caller_id"$COUNT"
    uci_set mars extensions AST_Ext"$COUNT"CallerID "$VAR"
    eval VAR=\$FORM_email"$COUNT"
    uci_set mars extensions AST_Ext"$COUNT"EMail $VAR

    eval VAR=\$FORM_care_invite"$COUNT"
    uci_set mars extensions AST_Ext"$COUNT"CareInvite $VAR
    eval VAR=\$FORM_nat"$COUNT"
    uci_set mars extensions AST_Ext"$COUNT"NAT $VAR
    eval VAR=\$FORM_from_user"$COUNT"
    uci_set mars extensions AST_Ext"$COUNT"FromUser $VAR
    eval VAR=\$FORM_from_domain"$COUNT"
    uci_set mars extensions AST_Ext"$COUNT"FromDomain $VAR



    let COUNT=$COUNT+1
    let ext_id=$ext_id+1
  done

  echo '</center></table>'
  SAVED=1
  FORM_action=""
fi

if [ "$FORM_action" = "" ]; then
  server_id=$CONFIG_system_AST_ServerId
  ext_id=0
  COUNT=0

  echo '<center><table border="0" cellpadding="4" cellspacing="4"><tr>'
  # List 8 Extensions on two rows in a table
  while [  $COUNT -lt 8 ]; do
    # EXT ServerId + 0 Form (200 / 300 / 400 / 500)
    # Get EXT 200 Vars
    echo '<td align=left border=0><table><tr><td align=left border=0 colspan=2>'
    echo '<h3>Extension: '${server_id}'</h3></td></tr>'
    echo '<INPUT name="action" type="hidden" value="SaveExt">'
    echo '<INPUT name="ext" type="hidden" value='"${ext_id}"'>'
    echo '<tr><th>Password: </th>'
    eval VAL=\$FORM_password"$COUNT"
    echo '<td><INPUT type=text name="password'$COUNT'" value="'$VAL'" size="8" maxlength="8"></td></tr>'
    echo '<tr><th>SIP/IAX2: </th>'
    eval VAL=\$FORM_type"$COUNT"
###    echo '<td><INPUT type=text name="type'$COUNT'" value="'$VAL'" size="8" maxlength="8"></td></tr>'
    echo '<td>'
    select_2 "type$COUNT" "$VAL"
    option_2 SIP "SIP"
    option_2 IAX2 "IAX2"
    echo '</select></td></tr>'

###    echo '<tr><th>Dial: </th>'
###    eval VAL=\$FORM_dial"$COUNT"
###    echo '<td><INPUT type=text name="dial'$COUNT'" value="'$VAL'" size="8" maxlength="64" disabled="disabled"></td></tr>'
    echo '<tr><th>Port: </th>'
    eval VAL=\$FORM_port"$COUNT"
    echo '<td><INPUT type=text name="port'$COUNT'" value="'$VAL'" size="5" maxlength="5"></td></tr>'
    echo '<tr><th>DtmfMode:&nbsp;&nbsp;&nbsp;&nbsp;</th>'
    echo '<td>'

    eval TYPE_VAL=\$FORM_type"$COUNT"
    eval VAL=\$FORM_dtmfmode"$COUNT"
    if [ "$TYPE_VAL" != "IAX2" ]; then
      select_2 "dtmfmode$COUNT" "$VAL"
    else
      select_2 "dtmfmode$COUNT" "$VAL" disabled
    fi
    option_2 rfc2833 "rfc2833"
    option_2 info "INFO"
    echo '</select></td></tr>'
###    echo '<tr><th>CallerID: </th>'
###    eval VAL=\$FORM_caller_id"$COUNT"
###    echo '<td><INPUT type=text name="caller_id'$COUNT'" value="'$VAL'" size="16" maxlength="64" disabled="disabled"></td></tr>'
    echo '<tr><th>EMail: </th><td>'
    eval VAL=\$FORM_email"$COUNT"
    echo '<INPUT type=text name="email'$COUNT'" value="'$VAL'" size="16" maxlength="64"></td></tr>'


    echo '<tr><td colspan="2"><table border="0" cellpadding="0" cellspacing="0" id="adv_table'$COUNT'"><tr>'
    echo '<tr id="care_invide_line"><th>Care Invite: </th><td>'
    eval VAL=\$FORM_care_invite"$COUNT"
    select_2 "care_invite$COUNT" "$VAL"
    option_2 no "No"
    option_2 yes "Yes"
    echo '</select></td></tr>'

    echo '<tr><th>NAT: </th><td>'
    eval VAL=\$FORM_nat"$COUNT" "id=nat$COUNT"
    select_2 "nat$COUNT" "$VAL"
    option_2 no "No"
    option_2 yes "Yes"
    echo '</select></td></tr>'


    echo '<tr><th>From User: </th><td>'
    eval VAL=\$FORM_from_user"$COUNT"
    echo '<INPUT type=text name="from_user'$COUNT'" id="from_user'$COUNT'" value="'$VAL'" size="16" maxlength="64"></td></tr>'
    echo '<tr><th>From Domain: </th><td>'
    eval VAL=\$FORM_from_domain"$COUNT"
    echo '<INPUT type=text name="from_domain'$COUNT'" id="from_domain'$COUNT'" value="'$VAL'" size="16" maxlength="64"></td></tr>'
    echo '</table></td></tr>'

  
    echo '<tr><td colspan=2><br><h3></h3></td></tr>'
    echo '</td></tr></table>'

    echo '</td>'

    let "server_id+=1"
    let "ext_id+=1"
    let COUNT=$COUNT+1
    if [ $COUNT -eq 4 ]; then
      echo '</tr><tr>'
    fi
  done
  if [ "$FORM_ShowAdvanced" ]; then
    echo '<tr><td></td><td><input type="checkbox" checked="checked" value="1" name="ShowAdvanced" id="ShowAdvanced"  onclick="ShowAdvancedToggle()">&nbsp;Show Advanced Options</td>'
  else
    echo '<tr><td></td><td><input type="checkbox" value="1" name="ShowAdvanced" id="ShowAdvanced" onclick="ShowAdvancedToggle()">&nbsp;Show Advanced Options</td>'
  fi

  echo '<td align=center><a href="/MngrManual_en.html#Extensions" target="_blank">Help</a></td><td></td></tr>'
  echo '</table></center></tr>'
fi

if [ "$DEBUG" = "1" ]; then
echo '<pre>'
set
echo '</pre>'
fi

footer ?>

<!--
##WEBIF:name:Asterisk:130:Extensions
-->

