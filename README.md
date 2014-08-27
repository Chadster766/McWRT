McWRT
=====

A OpenWRT project for the Linksys WRT1900AC router

Original source of Marvell 88W8864 Linux Driver/Firmware
https://github.com/TheDgtl/mrvl_wlan_v7drv 

Base OpenWRT from fork of Belkin Mamba Tag v3.9
https://github.com/jimmychungbelkin/Mamba

You need to have installed gcc, binutils, patch, bzip2, flex,
make, gettext, pkg-config, unzip, libz-dev and libc headers.

For the Feeds to update (update.sh) from packages_12.09 you will need to have Subversion 1.7 installed
http://sagar.se/svn-1.7-on-wheezy.html

To build the image just run "make". If successful the image should be in the "bin" directory.
