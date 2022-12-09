<!--
eth1 -> PY.1
eth2 -> Bridge 11
S0 port to RS232 on the patch panel and from RS232 to the Router MTIK
username: admin
password: (blank)
-->
# Commercial Router configuration

## Reset Configuration

```
/system reset-configuration <!-- y -->
```

## Set up IPs

```
/ip address add address=172.16.1.19/24 interface=ether1
/ip address add address=172.16.11.254/24 interface=ether2
```

## Set up routes

```
/ip route add dst-address=0.0.0.0/0 gateway=172.16.1.254
/ip route add dst-address=172.16.10.0/24 gateway=172.16.11.253
```

## Disable NAT
<!-- DO NOT USE IN DEMONSTRATION -->
```
/ip firewall nat disable 0
/ip firewall nat add chain=srcnat action=masquerade out-interface=ether1
```