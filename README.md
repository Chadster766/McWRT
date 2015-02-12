McWRT
=====
Unless you have a USB to TTL cable Attitude Adjustment Release AAv1.0.8 should be used.
===

###An OpenWRT project initially for the Linksys WRT1900AC router. With the release of the Marvell supported wireless driver and the successful stimulation of OpenWRT development for the WRT1900AC. McWRT is now looking to expand to include other routers for it's use and continued development.

**Original source of Marvell 88W8864 Linux Driver/Firmware**

https://github.com/TheDgtl/mrvl_wlan_v7drv

**Base OpenWRT from fork of Belkin Mamba Tag v3.9**

https://github.com/jimmychungbelkin/Mamba

-

You need to have installed gcc, binutils, patch, bzip2, flex, make, gettext, pkg-config, unzip, libz-dev, libc headers and Subversion 1.7.


-

**To build the image run "make V=s" and then when prompted press enter a lot of times :-) If successful the image should be in the "bin" directory.**

**Flashing firmware should be done connect to the router with an Ethernet cable.**

-

**How to update from Belkin/Linksys UI**

1. Login to WRT1900AC local UI
2. Navigate to the Connectivity tab
3. Select Manual firmware update
4. Select image to load (e.g., openwrt-armadaxp--jffs2-128k.img)
5. Select Update firmware

After the firmware is updated, the unit will reboot, and the default ip address will be 192.168.200.1. 
The default SSID's will be MAMBA_2G4, and MAMBA_5G2.
The default username for OpwnWrt is 'root', and there is no default password set.
The unit will ask you to set a default password after you login to the UI.

-

**How to update from OpenWrt to Belkin/Linksys**

1. Login to the WRT1900AC web UI
2. Select the 'System' Tab, and then 'Backup / Flash Firmware' tab
3. In the 'Flash new firmware image' section click the 'choose file' button and select your firmware
4. Click 'flash image'

-

Compile Notes
-
**Libtool 2.4 Version Version mismatch error:**

1. Change line 20 of file attitude_adjustment/package/feeds/packages/cyrus-sasl/Makefile
2. PKG_FIXUP:=autoreconf libtool
