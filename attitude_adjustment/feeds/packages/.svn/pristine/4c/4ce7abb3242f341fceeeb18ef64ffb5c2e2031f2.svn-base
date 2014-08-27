#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=cppunit
PKG_VERSION:=1.12.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/cppunit
PKG_MD5SUM:=bd30e9cf5523cdfc019b94f5e1d7fd19

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk

define Package/cppunit
  SECTION:=devel
  CATEGORY:=Development
  TITLE:=Unit Testing Library for C++
  URL:=http://cppunit.sourceforge.net/
  DEPENDS:=+libstdcpp
endef

define Package/cppunit/description
  Unit Testing Library for C++
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) DESTDIR="$(PKG_INSTALL_DIR)" install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/cppunit
	$(CP) $(PKG_INSTALL_DIR)/usr/include/cppunit $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcppunit*so* $(1)/usr/lib/
endef

define Package/cppunit/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcppunit*so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,cppunit))
