# 
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libspe2
PKG_VERSION:=2.3.0
PKG_MINOR_VERSION:=135
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).$(PKG_MINOR_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/libspe
PKG_MD5SUM:=c77dc69d3585d45f980e661232ae2b0b

include $(INCLUDE_DIR)/package.mk

define Package/libspe2
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=SPE Runtime Management Library for the Cell Broadband Engine
  DEPENDS:=@TARGET_ps3||TARGET_ps3chk
  URL:=http://sourceforge.net/projects/libspe/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) DESTDIR="$(PKG_INSTALL_DIR)" SYSROOT="" CROSS=$(TARGET_CROSS) all install
endef

define Build/InstallDev
	echo "Build/InstallDev"
	$(INSTALL_DIR) $(1)/usr/include
	$(CP)	$(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP)	$(PKG_INSTALL_DIR)/usr/lib/libspe2* $(1)/usr/lib/
endef

define Package/libspe2/install
	echo "Package/libspe2/install"
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libspe2* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libspe2))
