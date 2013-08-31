---
layout: default
title: How to install libraw1394
permalink: /install/
next_section: getting_started
---
# {{ page.title }}
<br>

## Goal  
<hr>  
* Install libraw1394-dev
* Set FireWire port permission
* Get source code for future reference
<br><br>


## Install 
<hr>  
### Install using `apt-get`
For a debian based system like Ubuntu, use `apt-get`
{% highlight sh %}
# clone from git repo
sudo apt-get install libraw1394-dev 
{% endhighlight %}  

### Compile from Source

#### 1. Downlowd source code

{% highlight sh %}
# clone from git repo
git clone git://git.kernel.org/pub/scm/libs/ieee1394/libraw1394.git 
# check out tag e.g. 2.1.0
git checkout v2.1.0
{% endhighlight %}

#### 2. Configure, build and install
{% highlight sh %}
# configure
./configure
# build
make
# install 
make install  # you may need sudo
{% endhighlight %}

<br><br>

## Set FireWire port permission
<hr>
By default, FireWire device permission is `600`, which means only `root` can
read and write this device and you need to `sudo ./program` run sample code. To
avoid this, we need to create a group, add yourself to the group and set the
device permission to `660`.



<br><br>

## Download libraw1394 source code
<hr>
Although, this tutorial can help you get started. It is a good idea to have both
libraw1394 and FireWire driver source code for future reference. 

{% highlight sh %}
# libraw1394 source code
git clone git://git.kernel.org/pub/scm/libs/ieee1394/libraw1394.git

# FireWire driver code
wget https://www.kernel.org/pub/linux/kernel/v3.x/linux-3.10.10.tar.xz 

{% endhighlight %}  

**Note:** FireWire driver code is under `drivers/firewire` foler.


<br><br>
## Download tutorial code
<hr>
Go to [download page](../../#Download)










