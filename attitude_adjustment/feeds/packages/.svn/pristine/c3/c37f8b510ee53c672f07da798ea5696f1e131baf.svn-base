#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libzip
PKG_VERSION:=0.10.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.nih.at/libzip/
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libzip/default
  TITLE:=libzip
  URL:=http://www.nih.at/libzip/
endef

define Package/libzip
$(call Package/libzip/default)
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+zlib
endef

define Package/libzip-utils
$(call Package/libzip/default)
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=compression
  TITLE+= utilities
  DEPENDS:=+libzip
endef

define Package/libzip/description
 C library for reading, creating, and modifying zip archives
endef

define Package/libzip-utils/description
 package contains binaries from libzip library (zipcmp, ziptorrent, zipmerge)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib $(1)/usr/lib/pkgconfig

	$(CP) $(PKG_INSTALL_DIR)/usr/include/* \
		$(PKG_INSTALL_DIR)/usr/lib/libzip/include/* \
		$(1)/usr/include/

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.{a,la,so*} \
		$(1)/usr/lib/

	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libzip.pc \
		$(1)/usr/lib/pkgconfig/
endef

define Package/libzip/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so* $(1)/usr/lib/
endef

define Package/libzip-utils/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/* $(1)/usr/bin/
endef

$(eval $(call BuildPackage,libzip))
$(eval $(call BuildPackage,libzip-utils))
