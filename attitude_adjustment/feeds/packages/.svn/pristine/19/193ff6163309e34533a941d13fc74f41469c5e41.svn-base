#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ttcp
PKG_VERSION:=3.8
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/ttcp

include $(INCLUDE_DIR)/package.mk

define Package/ttcp
  SECTION:=net
  CATEGORY:=Network
  TITLE:=A TCP connection performance tester
  URL:=https://dev.openwrt.org/browser/packages/net/ttcp/
endef

define Package/ttcp/description
	A small utility to test TCP connection performance.
endef

define Build/Compile		
	$(TARGET_CC) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/ttcp ttcp.c
endef

define Package/ttcp/install	
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ttcp $(1)/usr/bin/
endef

$(eval $(call BuildPackage,ttcp))
