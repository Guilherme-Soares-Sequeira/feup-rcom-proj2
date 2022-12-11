#!/bin/bash

# restart networking
systemctl restart networking

# config tux2 ip
ifconfig eth0 up
ifconfig eth0 172.16.11.1/24
ifconfig eth0

read -n 1 -p "Set up the ip address in tuxY4 and tuxY3 then press any key to continue."

# add route to tux3 through tux4
route add -net 172.16.10.0/24 gw 172.16.11.253

read -n 1 -p "Set up the ip address in the router then press any key to continue."

# make the router the default gateway
route add default gw 172.16.11.254

#maybe this is not supposed to stay here
echo 0 > /proc/sys/net/ipv4/conf/eth0/accept_redirects
echo 0 > /proc/sys/net/ipv4/conf/all/accept_redirects
