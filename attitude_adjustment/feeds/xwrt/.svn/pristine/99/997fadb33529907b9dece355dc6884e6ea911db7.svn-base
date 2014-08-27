#!/bin/sh

# Set a variable called stdin to help us
# get the variables from Asterisk
stdin="0"

# Read in the variables from Asterisk,
# and write them to a log file
while [ "$stdin" != "" ]
do
read stdin
# if [ "$stdin" != EOF ]
# then
# echo $stdin >> /tmp/logfile.txt
# fi
done

/bin/uci show "mars" | grep AST_* | sort | while read LINE; do

### VAR=$(echo "$LINE" | sed 's/mars\./mars /; s/\./ /' | awk '{print $3}')
### VAR=$(echo $LINE | cut -f 3 -d '.')
POS=$(expr index "$LINE" A)
VAR=$(expr substr "$LINE" $POS 100)

# Send AGI Cmd to asterisk
echo "EXEC SetGlobalVar \"$VAR\""
#done

# Now read the response back from Asterisk,
### Reading resp cause problems with first read. just ignore
# read response
# echo $response >> /tmp/logfile.txt
done

exit 0
