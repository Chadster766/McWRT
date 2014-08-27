#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libmpd
PKG_VERSION:=0.20.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://download.sarine.nl/Programs/gmpc/$(PKG_VERSION)/
PKG_MD5SUM:=2f1c99e12c69f7d95cfd1e27368056ed

PKG_FIXUP:=autoreconf
PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

TARGET_CFLAGS += $(FPIC)

define Package/libmpd
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+glib2
  TITLE:=Libmpd is an a library to easily connect to a mpd server
  URL:=http://gmpc.wikia.com/wiki/LIBMPD_RELEASES
endef

define Package/libmpd/description
It's wraps around libmpdclient and provides a higher level api.
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libmpd-1.0 $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmpd.{a,so*} $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libmpd.pc $(1)/usr/lib/pkgconfig/
endef

define Package/libmpd/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmpd.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libmpd))
