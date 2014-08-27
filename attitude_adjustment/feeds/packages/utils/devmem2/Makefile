#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=devmem2
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/devmem2
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Simple program to read/write from memory.
endef

define Package/devmem2/description
  Simple program to read/write from/to any location in memory.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/devmem2/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/devmem2 $(1)/usr/sbin/devmem2
endef

$(eval $(call BuildPackage,devmem2))
