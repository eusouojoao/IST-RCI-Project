#!/bin/bash

start_port=55000
end_port=55099
net=040

for ((port = start_port, id = 0; port <= end_port; port++, id++)); do
	cat <(echo "UNREG $net $(printf "%02d" $id)") | nc -u 193.136.138.142 59000
done
