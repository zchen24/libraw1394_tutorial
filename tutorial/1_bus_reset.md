---
layout: tutorial
title: Bus reset
permalink: /bus_reset/
prev_section: getting_started
next_section: arm_server
---

## Brief 
This is bus reset tutorial

## Broadcast packet 
The asynchronous broadcast request packet is same as normal asynchronous write
packet (quadlet/block), except that the destination id (node id) is set to
`0xffff` to indicate that it's a broadcast.   

## Sample Code
Sample code can be found at www.path.to.broadcast.example.com  


