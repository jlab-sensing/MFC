#!/bin/bash

HOSTS=("rocket1-jlab.ucsc.edu" "rocket3-jlab.ucsc.edu")

for host in "${HOSTS[@]}"; do
	echo $host
done
