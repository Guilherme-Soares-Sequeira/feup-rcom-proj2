# reset system configuration
```
/system reset-configuration
```

# bridge handling 
```
// print bridges
/interface bridge print

//add bridge 1 and 2

/interface bridge add name=10
/interface bridge add name=11

//remove default bridge

/interface bridge remove bridge

//etherXX -> tux2 XX = 06
//etherWW -> tux3 WW = 14
//etherYY -> tux4 eth0 YY = 20 
//etherZZ -> tux4 eth1 ZZ = 24

//etherAA -> Rc eth2 AA = 18

//remove ports from default bridges

/interface bridge port remove [find interface =etherXX]
/interface bridge port remove [find interface =etherWW]
/interface bridge port remove [find interface =etherYY]
/interface bridge port remove [find interface =etherZZ]

/interface bridge port remove [find interface =etherAA]

//add ports to bridges

/interface bridge port add bridge=bridge10 interface=etherXX
/interface bridge port add bridge=bridge11 interface=etherWW
/interface bridge port add bridge=bridge10 interface=etherYY
/interface bridge port add bridge=bridge11 interface=etherZZ

/interface bridge port add bridge=bridge11 interface=etherAA

```