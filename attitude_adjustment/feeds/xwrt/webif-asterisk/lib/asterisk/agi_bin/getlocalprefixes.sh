#!/bin/sh

if [ -z "$1" ] || [ -z "$2" ]; then
  echo 'Usage: getlocalprefix <NPA> <NXX>'
  echo 'NPA = Area Code (3 digits)'
  echo 'NXX = Telno prefix (3 digits)'
  echo 'Ex: For 81355551212 : NPA=813 NXX=555'
else
###  echo '<pre>'
echo 'Fetching the XML page from Local Calling Guide ...'
wget -O /tmp/loc_pref.txt "http://www.localcallingguide.com/xmllocalprefix.php?npa=$1&nxx=$2"
echo ''
echo 'Extracting the local prefixes ...'
grep "<n..>" </tmp/loc_pref.txt >/tmp/loc_pref2.txt
sed -e 's/<npa>//; s/<\/npa>//; s/<nxx>//; s/<\/nxx>//' </tmp/loc_pref2.txt >/tmp/loc_pref.txt
awk '{printf("%s",$0);getline;print}'  </tmp/loc_pref.txt >/tmp/local.txt
rm /tmp/loc_pref.txt
rm /tmp/loc_pref2.txt
echo ''
echo 'Moving the local prefix list to: /usr/lib/asterisk/agi-bin'
mv /tmp/local.txt /usr/lib/asterisk/agi-bin
cat /usr/lib/asterisk/agi-bin/local.txt | sort
### echo '</pre>'
fi
