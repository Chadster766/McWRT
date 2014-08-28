McWRT
=====

A OpenWRT project for the Linksys WRT1900AC router.

Original source of Marvell 88W8864 Linux Driver/Firmware

https://github.com/TheDgtl/mrvl_wlan_v7drv 

Base OpenWRT from fork of Belkin Mamba Tag v3.9

https://github.com/jimmychungbelkin/Mamba

You need to have installed gcc, binutils, patch, bzip2, flex, make, gettext, pkg-config, unzip, libz-dev, libc headers and Subversion 1.7.

For the Feeds to update (update.sh) from packages_12.09 you will need to have Subversion 1.7 installed.

http://sagar.se/svn-1.7-on-wheezy.html

To build the image just run "update.sh" and "make". If successful the image should be in the "bin" directory.


/**********************************/

To get the wireless driver to run upon router boot follow the instructions below:

1. Login in to the router's Web Interface and change the password
2. SSH (Linux SSH or Putty) into the router with username root and the password you entered

At the command prompt run the below commands:

1. echo ap8x > /etc/modules.d/10-ap8x
2. reboot


/**********************************/


