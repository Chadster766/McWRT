#!/bin/ash
#
#
# Handler for MARS config.
#
#
. /usr/lib/webif/webif.sh
. /www/cgi-bin/webif/ast_functions.sh

echo '<center><strong>Asterisk Configuration</strong><br>'

uci_load mars

FOUND=$(grep -s system /tmp/.uci/mars)
if [ "$FOUND" != "" ]; then
echo 'System modified'
fi

FOUND=$(grep -s "extensions" /tmp/.uci/mars)
if [ "$FOUND" != "" ]; then
WRITE_IAX=1
WRITE_SIP=1
WRITE_MBX=1
fi

FOUND=$(grep -s "trunk" /tmp/.uci/mars)
if [ "$FOUND" != "" ]; then
WRITE_IAX=1
WRITE_SIP=1
fi

FOUND=$(grep -s "queue" /tmp/.uci/mars)
if [ "$FOUND" != "" ]; then
WRITE_QUEUE=1
fi

FOUND=$(grep -s "servers" /tmp/.uci/mars)
if [ "$FOUND" != "" ]; then
WRITE_IAX=1
fi

if [ "$WRITE_SIP" != "" ]; then
 write_sip_conf
fi

if [ "$WRITE_IAX" != "" ]; then
  write_iax_conf
fi

if [ "$WRITE_MBX" != "" ]; then
  write_mailboxes
fi

if [ "$WRITE_QUEUE" != "" ]; then
write_queue
fi

if [ "$WRITE_SIP" != "" ] || [ "$WRITE_IAX" != "" ]; then
write_hints
fi

### Moved to apply.sh
### echo '<strong>Stopping Asterisk</strong><br><br>'
### /etc/init.d/asterisk stop
### sleep 2
### echo '<strong>Restarting Asterisk</strong><br><br>'
### /etc/init.d/asterisk start

#echo '<pre>'
# set | sort
#env | sort
#echo '</pre>'
