#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=schedtool
PKG_VERSION:=1.3.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://freequaos.host.sk/schedtool/
PKG_MD5SUM:=0d968f05d3ad7675f1f33ef1f6d0a3fb

include $(INCLUDE_DIR)/package.mk

define Package/schedtool
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Scheduler control utility
  URL:=http://freequaos.host.sk/schedtool/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -c $(PKG_BUILD_DIR)/schedtool.c -o $(PKG_BUILD_DIR)/schedtool.o
	$(TARGET_CC) $(TARGET_CFLAGS) -c $(PKG_BUILD_DIR)/error.c -o $(PKG_BUILD_DIR)/error.o
	$(TARGET_CC) $(PKG_BUILD_DIR)/schedtool.o $(PKG_BUILD_DIR)/error.o -o $(PKG_BUILD_DIR)/schedtool     
endef

define Package/schedtool/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/schedtool $(1)/usr/bin/
endef

$(eval $(call BuildPackage,schedtool))
