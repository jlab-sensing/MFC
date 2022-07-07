#!/bin/bash

USER="rocketlogger"
HOSTS=("rocket1-jlab.ucsc.edu" "rocket3-jlab.ucsc.edu")
REMOTE_PATH="/home/rocketlogger/soil_battery/soil_latest.csv"
LOCAL_PATH=`pwd`

SCP_OPTIONS="-P 2322 -o 'PubkeyAcceptedKeyTypes +ssh-rsa'"

for host in "${HOSTS[@]}"; do
	scp $SCP_OPTIONS ${host}:${REMOTE_PATH} ${
	echo $host
	echo $REMOTE_PATH
	echo $LOCAL_PATH
	echo $SCP_OPTIONS
done
