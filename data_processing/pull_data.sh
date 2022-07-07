#!/bin/bash

# Uncomment for debug printing
#set -x

USER="rocketlogger"
HOSTS=("rocket1-jlab.ucsc.edu" "rocket3-jlab.ucsc.edu")
FILES=("soil_20220629-214516_8.csv" "soil_20220629-211708_17.csv")
REMOTE_PATH="/home/rocketlogger/soil_battery"
LOCAL_PATH=`pwd`

for ((i=0; i < ${#HOSTS[@]}; i++)); do
	if [[ ! -d ${LOCAL_PATH}/${HOSTS[$i]} ]]; then
		mkdir ${LOCAL_PATH}/${HOSTS[$i]}
	fi

	scp -o "PubkeyAcceptedKeyTypes +ssh-rsa" -P 2322 ${USER}@${HOSTS[$i]}:${REMOTE_PATH}/${FILES[$i]} ${LOCAL_PATH}/${HOSTS[$i]}/

	# Change the directory just to be sure
	sed -i 's/fnames = glob(".*")/fnames = glob("soil_*.csv")/' twobat_plot.py

	cd ${HOSTS[$i]}
	python3 ../twobat_plot.py
	cd ..
done
