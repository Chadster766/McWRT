#!/usr/bin/haserl
content-type: text/html

<?
  print_cgi_data()
{
  echo "+----------------------------------------+"  >/tmp/testlog.txt
  echo "SCRIPT_NAME="$SCRIPT_NAME >>/tmp/testlog.txt
  echo "REQUEST_METHOD="$REQUEST_METHOD >>/tmp/testlog.txt
  echo "PATH_INFO="$PATH_INFO >>/tmp/testlog.txt
  echo "QUERY_STRING="$QUERY_STRING >>/tmp/testlog.txt
  echo "REMOTE_ADDR= "$REMOTE_ADDR >>/tmp/testlog.txt
  echo "REQUEST_URI="$REQUEST_URI >>/tmp/testlog.txt
  echo "HTTP_REFERER="$HTTP_REFERER >>/tmp/testlog.txt
  echo "REMOTE_USER="$REMOTE_USER >>/tmp/testlog.txt
  echo ""
  echo $FORM_BlockTelNo >>/tmp/testlog.txt
  echo $FORM_BlockIpAddress >>/tmp/testlog.txt
  echo "+----------------------------------------+"  >>/tmp/testlog.txt
}
?>

<?if [ $REQUEST_METHOD = "POST" ] ?>
<?
   print_cgi_data
  # Add TelNo & IP Address to blacklist.txt
  echo "$FORM_BlockTelNo : $FORM_BlockIpAddress" >> /www/cgi-bin/webcall/blacklist.txt 
?>
<?fi?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">
<html>
<head>
<title>Webcall</title>
</head>
<body>
<?if [ "$FORM_Language" = "E" ] ?>
<h1>Added to Black List</h1>
<p>You should not receive WebCall requests from this user anymore.
<p><input type="submit" value="Back" onclick="back(-1)">
<?el?>
<h1>Ajouté à la liste noire</h1>
<p>Vous ne receverez plus de demande d'appel de cet usager.
<p><input type="submit" value="Retour" onclick="back(-1)">
<?fi?>
</body>
</html>