#!/bin/sh

# Set a variable called stdin to help us
# get the variables from Asterisk
stdin="0"

# Read in the variables from Asterisk
while [ "$stdin" != "" ]
do
read stdin
done

URL=$(grep "\[URL_$1\]" /usr/lib/asterisk/agi-bin/web_audio.txt)
# echo $URL >> /tmp/logfile.txt
echo "EXEC SetGlobalVar URL=${URL}"
read response
# echo $response >> /tmp/logfile.txt

exit 0
