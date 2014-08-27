#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=fortune-mod
PKG_VERSION:=1.2.1
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://freeware.sgi.com/source/fortune-mod
PKG_MD5SUM:=383b1de06b35450235732c7ae7b5f0e3

include $(INCLUDE_DIR)/package.mk

define Package/fortune-mod
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=display fortune cookies on demand.
  URL:=http://packages.debian.org/fortune-mod
endef

define Package/fortune-mod/description
Fortune mod displays fortune cookies on demand
endef

define Build/Compile
	$(HOSTCC) -Wall -o $(PKG_BUILD_DIR)/util/strfile-host $(PKG_BUILD_DIR)/util/strfile.c -DVERSION=\"$(PKG_VERSION)\"
	$(PKG_BUILD_DIR)/util/strfile-host $(PKG_BUILD_DIR)/datfiles/fortunes $(PKG_BUILD_DIR)/datfiles/fortunes.dat
	$(MAKE) -C $(PKG_BUILD_DIR)/fortune
endef

define Package/fortune-mod/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/fortune/fortune $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/share/games/fortunes
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/datfiles/fortunes $(1)/usr/share/games/fortunes
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/datfiles/fortunes.dat $(1)/usr/share/games/fortunes
endef

$(eval $(call BuildPackage,fortune-mod))
