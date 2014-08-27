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

value=$(nvram get $1)
# Send AGI Cmd to asterisk
echo "EXEC SetGlobalVar $1=${value}"

# Now read the response back from Asterisk,
read response
# echo $response >> /tmp/logfile.txt

exit 0
