# McWRT

### An OpenWRT project for the Linksys WRT1900AC V1 router.

**Original source of Marvell 88W8864 Linux Driver/Firmware**

https://github.com/TheDgtl/mrvl_wlan_v7drv

**Base OpenWRT from fork of Belkin Mamba Tag v3.9**

https://github.com/jimmychungbelkin/Mamba





-

## Warning!

**Flashing the firmware should always be done over a wired (Ethernet) connection. Not doing so could render your device unusable.**

-

### Install instructions:

**Flashing McWRT from the stock firmware**

1. Navigate to your WRT1900AC Router page. (The stock adress is 192.168.1.1).
2. Login with your Linksys Smart Wifi account, or with your local router password.
2. Navigate to the Connectivity tab on the left side.
3. Select choose file in the 'Manual:' tab.
4. Choose the image to load, probably located in your download folder. The default filename for McWRT is: 'openwrt-armadaxp--jffs2-128k.img)'.
5. Click start.

**Notes**

1. After the firmware has been installed, the unit will reboot. You can verify if the firmware has been correctly installed by checking if the eSATA light is ON. 
2. The default router login page is on 192.168.200.1
3. The default SSID's are: 'MCWRT-WPA2-2.4' (2,4 GHz.) and 'MCWRT-WPA2-5.0' (5,0 GHz.)
4. The default password for both the SSID's is 'mcwrt123'.
5. The default username for the router login page is 'root', with no default password.
6. The unit will ask you to set a password after the first login.

-

### Install instructions:

**Reverting to the stock Linksys firmware**

1. Navigate to your McWRT router login page. (The stock McWRT adress is 192.168.200.1).
2. Select the 'System' tab (Can be found in the top bar), and click on 'Backup / Flash Firmware'.
3. In the 'Flash new firmware image' section, uncheck 'Keep settings'.
4. Load the image by clicking on browse. The default filename is: 'FW_WRT1900AC_1.1.10.167514_prod.img'.
4. Click 'flash image'.
5. The router will reboot, and you are now able to configure your router.
6. The default router login page is 192.168.1.1.

**Notes**

1. The default image file can be downloaded on: 'http://cache-www.belkin.com/support/dl/FW_WRT1900AC_1.1.10.167514_prod.img' (Direct link)
2. Do not unplug your router while flashing the firmware. The update is in progress if the power button is blinking.
3. In the unhappy case of a power shortage while flashing new firmware, the guide located at 'http://wiki.openwrt.org/toh/linksys/wrt1900ac' can assist you. (Instructions can be found at the: 'Corrupt Bootloader Recovery' Tab.) (Bottom of the page)

## Compiling instructions

**Compiling McWRT**

1. Enter the command 'make V=s'.
2. When prompted, press enter.
3. If the build has been succesfully compiled, the McWRT files would be located in the 'bin' directory.

**Notes**

1. Libtool 2.4 version mismatch error:
2. Change line 20 of file attitude_adjustment/package/feeds/packages/cyrus-sasl/Makefile to 'PKG_FIXUP:=autoreconf libtool'.

### Requirements

**Packages that needs to be installed**

1. gcc.
2. binutils.
3. patch.
4. bzip2.
5. flex.
6. make.
7. gettext.
8. pkg-config.
9. unzip.
10. libz-dev.
11. libc headers.
12. Subversion 1.7.
