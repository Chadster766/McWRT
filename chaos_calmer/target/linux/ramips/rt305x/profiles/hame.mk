#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/MPRA1
	NAME:=HAME MPR-A1
	PACKAGES:=\
		kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-netdev
endef

define Profile/MPRA1/Description
	Package set for HAME MPR-A1 board
endef

$(eval $(call Profile,MPRA1))

define Profile/MPRA2
	NAME:=HAME MPR-A2
	PACKAGES:=\
		kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-netdev
endef

define Profile/MPRA2/Description
	Package set for HAME MPR-A2 board
endef

$(eval $(call Profile,MPRA2))

