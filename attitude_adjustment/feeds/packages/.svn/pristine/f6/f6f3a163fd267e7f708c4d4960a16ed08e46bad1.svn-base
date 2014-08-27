# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=testusb
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)
PKG_BUILD_DEPENDS := libpthread

include $(INCLUDE_DIR)/package.mk

define Package/testusb
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@DEVEL +libpthread
  TITLE:=USB testing utility
  URL:=http://www.linux-usb.org/usbtest/testusb.c
endef

define Package/testusb/description
	This tool can be used to perform various tests with use of the usbtest driver. 
endef

define Build/Prepare
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -Os -lpthread $(PKG_BUILD_DIR)/testusb.c -o $(PKG_BUILD_DIR)/$(PKG_NAME)
endef

define Package/testusb/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/testusb $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,testusb))
