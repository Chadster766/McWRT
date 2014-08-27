#
# Copyright (C) 2009-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=armadaxp
BOARDNAME:=Marvell Armada XP
FEATURES:=targz usb jffs2 squashfs

LINUX_VERSION:=3.2.36

CFLAGS:=-Os -pipe -march=armv7-a -mfpu=vfpv3 -mfloat-abi=soft

include $(INCLUDE_DIR)/target.mk

KERNELNAME:="uImage"

DEFAULT_PACKAGES +=

$(eval $(call BuildTarget))
