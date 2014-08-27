#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=calvaria
PKG_RELEASE:=1

PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/calvaria
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Maemo CAL partition access tool
  MAINTAINER:=Michael Buesch <mb@bu3sch.de>
endef

define Package/calvaria/description
  Calvaria - Maemo CAL partition variable access tool
  The CAL partition (/dev/mtdblock1) is used to store configuration
  and calibration data.
endef

define Build/Prepare
	$(CP) ./files/src/* $(PKG_BUILD_DIR)/
endef

define Package/calvaria/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/calvaria $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/cal2p54 $(1)/usr/bin/
endef

$(eval $(call BuildPackage,calvaria))
