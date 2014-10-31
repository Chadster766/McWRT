#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/GW6X00
  NAME:=Tecom GW6X00
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-ohci kmod-usb-storage \
	kmod-fs-ext4 kmod-nls-cp437 kmod-nls-iso8859-1 e2fsprogs \
	kmod-ipt-nathelper-extra wlc
endef

define Profile/GW6X00/Description
	Package set compatible with the Tecom GW6000 and GW6200 based
	on the BCM96348GW reference design.
endef
$(eval $(call Profile,GW6X00))

