#!/bin/bash

DATE_WITH_TIME=`date "+%Y%m%d-%H%M%S"`
NUMBER=`cat /home/rocketlogger/num.txt`
FNAME=soil_${DATE_WITH_TIME}_${NUMBER}.csv
DEV=/dev/ttyACM0

echo $FNAME

# Stop logging
rocketlogger stop
sleep 1s
# Start logging
rocketlogger cont -r 1 -f /home/rocketlogger/soil_battery/$FNAME -format csv -ch 0,1,2,3,4,5 -size 1g -d 0 -w 0
echo $(( $NUMBER+1 )) > /home/rocketlogger/num.txt

# Log teros sensor if connected
if test -e "$DEV"; then
	teroslogger -q -o /home/rocketlogger/soil_battery/tmp_$DATE_WITH_TIME $DEV 
fi
