# 
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=make
PKG_VERSION:=3.81
PKG_RELEASE:=1
PKG_SOURCE_URL:=@GNU/make
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_MD5SUM:=354853e0b2da90c527e35aabb8d6f1e6
PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/make
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:=+libelf
  TITLE:=make
  URL:=http://www.gnu.org/software/make/
endef

define Package/make/description
	The Make package contains a program for compiling packages
endef

define Package/make/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/make $(1)/usr/bin/
endef

$(eval $(call BuildPackage,make))
