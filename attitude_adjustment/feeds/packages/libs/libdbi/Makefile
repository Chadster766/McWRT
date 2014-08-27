#
# Copyright (C) 2009-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libdbi
PKG_VERSION:=0.8.3
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/libdbi
PKG_MD5SUM:=ca66db78d479cbfa727cf3245b5864ae

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libdbi
  NAME:=libdbi
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE+= (library)
  URL:=http://libdbi.sourceforge.net/
endef

define Package/libdbi/description
       libdbi implements a database-independent abstraction layer in C.
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/dbi $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libdbi.{a,so*} $(1)/usr/lib/
endef

define Package/libdbi/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libdbi.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libdbi))
