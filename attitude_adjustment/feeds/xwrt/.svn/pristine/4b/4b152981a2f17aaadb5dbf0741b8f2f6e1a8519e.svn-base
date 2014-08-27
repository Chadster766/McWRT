#!/bin/sh

# Set a variable called stdin to help us
# get the variables from Asterisk
stdin="0"

# Read in the variables from Asterisk,
# and write them to a log file
while [ "$stdin" != "" ]
do
read stdin
# if [ "$stdin" != EOF ]; then
# echo $stdin >> /tmp/agi_input_log.txt
# fi
done

# Test if given NPA-NXX in local.txt file
# NOTE MUST use absolute path

SUB_S=$(expr substr "$1" 1 3)

N="$(grep $1 /usr/lib/asterisk/agi-bin/local.txt)"

if [ -z "$N" ]; then
  if [ "$SUB_S" = "800" ]; then
    N=$SUB_S
  elif [ "$SUB_S" = "866" ]; then
    N=$SUB_S
  elif [ "$SUB_S" = "877" ]; then
    N=$SUB_S
  elif [ "$SUB_S" = "888" ]; then
    N=$SUB_S
  fi
fi

if [ -z "$N" ]; then
echo "SET VARIABLE IS_LOCAL FALSE"
RES=0
else
echo "SET VARIABLE IS_LOCAL TRUE"
RES=1
fi


# echo "SET VARIABLE IS_LOCAL $N" # $(grep $1 /usr/lib/asterisk/agi-bin/local.txt)"
read response
# echo $response >> /tmp/agi_input_log.txt

exit $RES
