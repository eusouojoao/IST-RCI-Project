#!/bin/bash

net=064
server="193.136.138.142"
port=59000

for ((id = 0; id <= 99; id++)); do
	echo -n "UNREG $net $(printf "%02d" $id)" >/dev/udp/$server/$port
done
