#!/bin/bash

TIMESTAMP=`date "+%Y%m%d-%H%M%S"`
LOGDIR=/home/rocketlogger/soil_battery
DEV=/dev/ttyACM0

# Stop logging
rocketlogger stop
sleep 1s
# Start logging
rocketlogger cont -r 1 -f ${LOGDIR}/soil_${TIMESTAMP}.csv -format csv -ch 0,1,2,3,4,5 -size 1g -d 0 -w 0

# Log teros sensor if connected
if test -e "$DEV"; then
	teroslogger -q -o ${LOGDIR}/teros_${TIMESTAMP}.csv $DEV 
fi
