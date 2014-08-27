# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ustl
PKG_VERSION:=0.8
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-0.8-0.tar.bz2
PKG_SOURCE_URL:=@SF/ustl
PKG_MD5SUM:=dd59483c327f1bd1356273bc5ae4dde0

include $(INCLUDE_DIR)/package.mk

define Package/ustl
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=C++ STL for embedded systems
  DEPENDS:=@!TARGET_avr32
  URL:=http://ustl.sourceforge.net/
endef

define Package/ustl/description
	C++ Standard Template Library (STL) for embedded systems.\\\
    uSTL is a partial implementation of the STL (Standard Template \\\
    Library) that reduces code size by factoring memory management \\\
    code into a non-template base class and deriving containers \\\
    from it.
endef

define Build/Configure
	$(CP) ./files/config.h $(PKG_BUILD_DIR)/
	$(CP) ./files/Common.mk $(PKG_BUILD_DIR)/
endef

TARGET_CFLAGS += $(FPIC)

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CROSS="$(TARGET_CROSS)" \
		OPTFLAGS="$(TARGET_CFLAGS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Package/ustl/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libustl.so.* $(1)/usr/lib/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/ustl* $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libustl.{a,so*} $(1)/usr/lib/
endef

$(eval $(call BuildPackage,ustl))
