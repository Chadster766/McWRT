# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libamsel
PKG_VERSION:=20040621-cvs
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://ftp.amselinux.net/source/
PKG_MD5SUM:=2077e620047404881a65d39c53c9917e

include $(INCLUDE_DIR)/package.mk

define Package/libamsel
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=The amwall library
  URL:=http://amselinux.de/
endef

define Package/libamsel/description
	A library for amwall configuration files.
endef

define Build/Configure
endef

TARGET_CFLAGS += $(FPIC)

MAKE_FLAGS += \
	CFLAGS="$(TARGET_CFLAGS) -I$(PKG_BUILD_DIR) -fPIC" \
	LDIR=$(PKG_BUILD_DIR) \
	PREFIX="$(PKG_INSTALL_DIR)/usr" \
	all install

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/amsel/compat.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/include/compat
	$(CP) $(PKG_INSTALL_DIR)/usr/include/amsel/compat/queue.h $(1)/usr/include/compat/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libamsel.so $(1)/usr/lib/
endef

define Package/libamsel/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib
endef

$(eval $(call BuildPackage,libamsel))
