#
# Copyright (C) 2011-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=leds-monitor
PKG_RELEASE:=2

include $(INCLUDE_DIR)/package.mk

define Package/leds-monitor
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Mamba board utilities
  MAINTAINER:=Belkin Inc.
  DEPENDS:=+libpthread
endef

define Package/leds-monitor/description
 This package contains some small utility apps which use to monitor the
 leds events, status...
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(INSTALL_DATA) ./src/leds_monitor.c $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -Wall \
		-o $(PKG_BUILD_DIR)/leds_monitor $(PKG_BUILD_DIR)/leds_monitor.c -lpthread -v
endef

define Package/leds-monitor/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/leds_monitor $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,leds-monitor))
