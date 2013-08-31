---
layout: default
title: Broadcast packets
permalink: /broadcast/
prev_section: async_client
next_section: 
---

## Brief 
This tutorial explains how to broadcast packet all nodes.  FireWire
protocal supports asynchronous and isochronous transfer. The isochronous
transfer uses channel number to address listener nodes and essentially is a
multicast transfer. This tutorial shows how to broadcast data to all the nodes
on the same bus using asynchronous write. 

## Broadcast packet 
The asynchronous broadcast request packet is same as normal asynchronous write
packet (quadlet/block), except that the destination id (node id) is set to
`0xffff` to indicate that it's a broadcast.   

Put a table here 

## Sample Code
Sample code can be found at www.path.to.broadcast.example.com  

Code snippet
```cpp
#include <libraw1394/raw1394.h>

int rc;   # return code
raw1394_start_write(handle, addr, bla, bla)

```

The `raw1394_start_write`


## Caveat
**Speed**: In FireWire driver __core-cdev.c__
**Address**: 

## Reference 
For more detail check libraw1394 source code and kernel driver/firewire



