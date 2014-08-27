#
# Copyright (C) 2007,2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=input-utils
PKG_VERSION:=20081014-101501
PKG_RELEASE:=2

PKG_BUILD_DIR:=$(BUILD_DIR)/input/
PKG_SOURCE:=input-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://dl.bytesex.org/cvs-snapshots/
PKG_MD5SUM:=a6854dc5218301b67324b483d26f1bee

include $(INCLUDE_DIR)/package.mk

define Package/input-utils
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Utilities for the input layer of the Linux kernel
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		STRIP="true" \
		build
endef

define Package/input-utils/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/input-events $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/input-kbd $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lsinput $(1)/usr/bin/

endef

$(eval $(call BuildPackage,input-utils))
