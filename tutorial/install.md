---
title: Install libraw1394
permalink: /install/
next_section: getting_started
---

## Goal  
Install libraw1394

## Install 
For a debian based system like Ubuntu, use `apt-get`
{% highlight sh %}
# clone from git repo
git clone git://git.kernel.org/pub/scm/libs/ieee1394/libraw1394.git 
{% endhighlight %}


## Compile from Source

### Clone source code  

{% highlight sh %}
# clone from git repo
git clone git://git.kernel.org/pub/scm/libs/ieee1394/libraw1394.git 
# check out tag e.g. 2.1.0
git checkout v2.1.0
{% endhighlight %}

### Configure and build   
{% highlight sh %}
# configure
./configure
# build
make
# install 
make install  # you may need sudo
{% endhighlight %}

### Test  






