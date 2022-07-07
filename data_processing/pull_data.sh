#!/bin/bash

set -x

USER="rocketlogger"
HOSTS=("rocket1-jlab.ucsc.edu" "rocket3-jlab.ucsc.edu")
REMOTE_PATH="/home/rocketlogger/soil_battery/soil_latest.csv"
LOCAL_PATH=`pwd`

for host in "${HOSTS[@]}"; do
	if [[ ! -d ${LOCAL_PATH}/${host} ]]; then
		mkdir ${LOCAL_PATH}/${host}
	fi

	scp -o "PubkeyAcceptedKeyTypes +ssh-rsa" -P 2322 ${USER}@${host}:${REMOTE_PATH} ${LOCAL_PATH}/${host}/soil_latest.csv

	sed -i 's/fnames = glob(".*")/fnames = glob("soil_*.csv")/' twobat_plot.py

	cd ${host}
	python3 ../twobat_plot.py
	cd ..
done
