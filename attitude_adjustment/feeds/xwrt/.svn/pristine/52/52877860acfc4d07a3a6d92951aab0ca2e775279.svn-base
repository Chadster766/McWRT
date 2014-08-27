function value(name)
{
	var item = document.getElementById(name);
	return (item ? item.value : "");
}
function set_value(name, value)
{
	var item = document.getElementById(name);
	if (item) item.value = value;
}
function isset(name, val)
{
	return (value(name) == val);
}
function checked(name)
{
	var item = document.getElementById(name);
	return ((item) && item.checked);
}
function hide(name)
{
	var item = document.getElementById(name);
	if (item) 
		item.style.display = 'none';
}
function show(name)
{
	var item = document.getElementById(name);
	if (item)
		item.style.display = '';
}
function set_visible(name, value)
{
	if (value)
		show(name)
	else
		hide(name)
}
//+------------------------------------------------------------------------
var win = null;
function newWindow (mypage,myname,w,h,features)
{
var winl = (screen.width-w)/2;
var wint = (screen.height-h)/2;
if (winl < 0) winl = 0;
if (wint < 0) wint = 0;
var settings = 'height=' + h + ',';
settings += 'width=' + w + ',';
settings += 'top=' + wint + ',';
settings += 'left=' + winl + ',';
settings += features;
win = window.open(mypage,myname,settings);
win.window.focus();
}
function start_confirm (CallListNo)
{
  var res;
  res = confirm ("Start Calling. Please confirm");
  if (res)
  {
//    newWindow ('dialer_traces.sh?Rate=5&CallList=' + CallListNo, '','640','520','scrollbars, resizable');
    return (true);
  }
  else
    return (false);
//  return (confirm ("Start Calling. Please confirm"));
}
//+------------------------------------------------------------------------

function test_ftp (Name,Pwd,Host)
{
  newWindow ('ftp_test.sh?Name=' + Name + '&Pwd=' + Pwd + '&Host=' + Host,'', '800','520','scrollbars, resizable');
}

//+------------------------------------------------------------------------

function ConfirmFtpTest (Name,Pwd,Host)
{
//  return (confirm ('Testing FTP for: ' + Name + ':' + Pwd + '@' + Host + '\nPlease Confirm ...'));
  if (confirm ('Testing FTP for: ' + Name + ':' + Pwd + '@' + Host + '\nPlease Confirm ...') == true)
    location.href="/cgi-bin/webif/dictation-accounts.sh?action=FTP Test";
}


//+------------------------------------------------------------------------

function test_email (AdminEmail,SMTP_Server)
{
  newWindow ('test_email.sh?AdminEmail=' + AdminEmail + '&SMTP_Server=' + SMTP_Server ,'MARS EMail Test', '800','520','scrollbars, resizable');
}

//+------------------------------------------------------------------------

function ConfirmEMailTest (AdminEmail,SMTP_Server)
{
/*
  var res = false;

  if ((AdminEmail == '') || (SMTP_Server == ''))
  {
    confirm ('Please fill the Admin Email and SMTP Server fields first');
    return (true);
  }
  else
  {
    res = confirm ('Testing EMail for: ' + AdminEmail + ' on SMTP Server:' + SMTP_Server + '\nPlease Confirm ...');
    if (res)
      return (true);
    else
      return (false);
  }
//  return (res);
*/
  if (confirm ('Testing EMail for: ' + AdminEmail + ' on SMTP Server:' + SMTP_Server + '\nPlease Confirm ...') == true)
    location.href="/cgi-bin/webif/asterisk-system.sh?action=EMail Test";
}

//+------------------------------------------------------------------------

function test_upload (CallListFnameValue)
{
  if (CallListFnameValue == "")
  {  
    alert("Please select a file to upload ...")
      return false;
  }
  else
    return true;
}
//+------------------------------------------------------------------------
function confirm_delete (Del_Link)
{
  var res;
  res = confirm ("Delete File ? Please confirm");
  if (res)
  {
    top.location.href=Del_Link; 
  }
}
//+------------------------------------------------------------------------

function confirm_del ()
{
  return (confirm ("Delete File ? Please confirm"));
}

function GetLocalAreaCodes(AreaCode,Prefix)
{
  newWindow ('get_area_codes.sh?AreaCode=' + AreaCode + '&Prefix=' + Prefix ,'MARS Get Local Area Codes', '800','520','scrollbars, resizable');
}

function MngrLogin (URL)
{
  newWindow ('mngr_login.sh?URL=' + URL ,'Asterisk Manager', '800','520','scrollbars, resizable');
}