#!/bin/bash

# restart networking
systemctl restart networking

# config tux4 ip
ifconfig eth0 up
ifconfig eth0 172.16.10.254/24
ifconfig eth0

ifconfig eth1 up
ifconfig eth1 172.16.11.253/24
ifconfig eth1

# enable IP forwarding
echo 1 > /proc/sys/net/ipv4/ip_forward

# disable ICMP echo-ignore-broadcast
echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts

read -n 1 -p "Set up the bridges and then click any key to continue."

# add Rc as a router
route add -net 172.16.1.0/24 gw 172.16.11.254

route add default gw 172.16.11.254
