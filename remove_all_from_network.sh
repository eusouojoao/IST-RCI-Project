#!/bin/bash

net=040
server="193.136.138.142"
port=59000

for ((id = 0; id <= 99; id++)); do
	echo -n "UNREG $(printf "%03d" $net) $(printf "%02d" $id)" >/dev/udp/$server/$port
done
