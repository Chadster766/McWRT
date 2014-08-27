# 
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=event_test
PKG_VERSION:=0.01
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/event_test

include $(INCLUDE_DIR)/package.mk

define Package/event_test
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Event device test program
endef

define Package/event_test/description
 This package contains an utility for testing event devices.
endef

define Build/Prepare
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/event_test/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/event_test $(1)/sbin/
endef

$(eval $(call BuildPackage,event_test))
