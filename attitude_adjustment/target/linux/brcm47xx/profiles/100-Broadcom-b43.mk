#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Broadcom-b43
  NAME:=Broadcom BCM43xx WiFi (b43, default)
  PACKAGES:=kmod-b43 kmod-b43legacy
endef

define Profile/Broadcom-b43/Description
	Package set compatible with hardware using Broadcom BCM43xx cards
	using the MAC80211 b43 and b43legacy drivers and b44 Ethernet driver.
endef

$(eval $(call Profile,Broadcom-b43))

