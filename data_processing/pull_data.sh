#!/bin/bash

USER="rocketlogger"
HOSTS=("rocket1-jlab.ucsc.edu" "rocket3-jlab.ucsc.edu")
REMOTE_PATH="/home/rocketlogger/soil_battery/soil_latest.csv"
LOCAL_PATH=`pwd`

for host in "${HOSTS[@]}"; do
	scp -o "PubkeyAcceptedKeyTypes +ssh-rsa" -P 2322 ${USER}@${host}:${REMOTE_PATH} ${LOCAL_PATH}
done
