#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dash
PKG_VERSION:=0.5.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://gondor.apana.org.au/~herbert/dash/files
PKG_MD5SUM:=f6cedb10ae7258adb5ab17a10ae80d51

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/dash
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=Debian Almquist shell
  URL:=http://gondor.apana.org.au/~herbert/dash/
endef

define Package/dash/description
 A small footprint, POSIX complied unix shell
endef

MAKE_FLAGS += \
	AWK="awk"

define Package/dash/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/dash $(1)/bin/
endef

$(eval $(call BuildPackage,dash))
