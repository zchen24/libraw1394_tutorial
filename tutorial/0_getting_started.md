---
layout: tutorial
title: Getting Started
permalink: /getting_started/
next_section: bus_reset
---

# {{ page.title }}
<br>

## Goal
<hr>
#### This tutorial shows:  
* how to check libraw1394 version  
* how to create a `raw1394handle_t`  
* how to get port and node information  

See source code: [HERE](https://github.com/zchen24/libraw1394_tutorial/blob/master/src/0_getting_started.cpp)

<br>

## Version number
<hr>

#### Code
{% highlight cpp linenos %}
// ----- check libraw1394 verison ----
const char* libraw1394_version = raw1394_get_libversion();
std::cout << "lib version = " << libraw1394_version << std::endl;
{% endhighlight %}  
<br>

#### Code explained
The `raw1394_get_libversion()` returns current library version, I use it to check if I am linking against compiled library or the version from `apt-get install`.

<br>

## FireWire handle
<hr>
<br>

## Port and node info
<hr>
<br>


## Reference
* [http://www.dennedy.org/libraw1394/](http://www.dennedy.org/libraw1394/)  


