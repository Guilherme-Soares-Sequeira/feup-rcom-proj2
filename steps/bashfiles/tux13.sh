#!/bin/bash

# restart networking
systemctl restart networking

# config tux3 ip
ifconfig eth0 up
ifconfig eth0 172.16.10.1/24
ifconfig eth0

# add route to tux2 through tux4
route add -net 172.16.11.0/24 gw 172.16.10.254

read -n 1 -p "Set up the bridges and then click any key to continue."

route add default gw 172.16.10.254
