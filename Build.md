# Building and installing VMA #

## Prerequisites ##
  1. MLNX\_OFED (Download the correct version [here](https://code.google.com/p/libvma/wiki/Downloads)) - install with --vma
  1. Autoconf, Automake, libtool, unzip, patch, libnl-devel (standart RH6.4 release should come with everything required)

Please look at the file [install.sh](https://libvma.googlecode.com/git/install.sh) before building VMA.
You can either run `install.sh` or build manually as follow:

## Build ##
  1. ./autogen.sh
  1. ./configure --with-ofed=/usr --prefix=/usr --libdir=/usr/lib64 --includedir=/usr/include/mellanox --docdir=/usr/share/doc/libvma --sysconfdir=/etc
  1. make
  1. make sockperf (to build sockperf)

You can find libvma.so in _path\_to\_vma\_dir_/src/vma/.libs/libvma.so.

## Install ##
After build:
  1. sudo make install
  1. sudo make install-sockperf (to install sockperf)

## Run ##

Load libvma.so using LD\_PRELOAD=_path\_to\_libvma.so_ before your application, and run your application.


#### For example: ####
LD\_PRELOAD=libvma.so sockperf


#### Or: ####
export LD\_PRELOAD=libvma.so

sockperf

## NOTES ##
1. Download the source by pulling git.

The zip file googlecode offer for download does not contain the required file permissions.

If you still wish to use the zip file, run:

`find . -name \*.sh -exec chmod 755 {} \;`


`find ./ -exec touch {} \;`

2. DO NOT USE the scripts under ./build .

These are part of Mellanox internal automation system, and will not work for you.

We do not support manual building of RPM/DEB packages.

You can find the RPM & DEB packages for each version [here](https://code.google.com/p/libvma/wiki/Downloads).