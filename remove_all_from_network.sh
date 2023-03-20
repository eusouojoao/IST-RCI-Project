#!/bin/bash

# net=099
server="193.136.138.142"
port=59000

for ((net = 0; net <= 999; net++)); do
	for ((id = 0; id <= 99; id++)); do
		echo -n "UNREG $(printf "%03d" $net) $(printf "%02d" $id)" >/dev/udp/$server/$port
	done
done
