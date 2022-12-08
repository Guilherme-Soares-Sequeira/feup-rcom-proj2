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

//etherXX -> tux2
//etherWW -> tux3
//etherYY -> tux4 eth0
//etherZZ -> tux4 eth1

//etherAA -> Rc eth1

//remove ports from default bridges

/interface bridge port remove [find interface =etherXX]
/interface bridge port remove [find interface =etherWW]
/interface bridge port remove [find interface =etherYY]
/interface bridge port remove [find interface =etherZZ]

/interface bridge port remove [find interface =etherAA]

//add ports to bridges

/interface bridge port add bridge=bridgeY0 interface=etherXX
/interface bridge port add bridge=bridgeY1 interface=etherWW
/interface bridge port add bridge=bridgeY0 interface=etherYY
/interface bridge port add bridge=bridgeY1 interface=etherZZ

/interface bridge port add bridge=bridgeY1 interface=etherAA

```