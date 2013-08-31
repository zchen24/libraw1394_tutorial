---
layout: default
title: ARM Server
permalink: /arm_server/
prev_section: bus_reset
next_section: async_client
---

## Brief 
Arm_server tutorial is here

## Broadcast packet 
The asynchronous broadcast request packet is same as normal asynchronous write
packet (quadlet/block), except that the destination id (node id) is set to
`0xffff` to indicate that it's a broadcast.   

Put a table here 

## Sample Code
Sample code can be found at www.path.to.broadcast.example.com  

## Reference 
For more detail check libraw1394 source code and kernel driver/firewire



