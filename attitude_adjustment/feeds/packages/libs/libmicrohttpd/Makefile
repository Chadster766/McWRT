# 
# Copyright (C) 2011-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libmicrohttpd
PKG_VERSION:=0.9.19
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/libmicrohttpd
PKG_MD5SUM:=0f4c55ba224ce75b3b993b558e072a6b

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libmicrohttpd
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Microhttpd
  URL:=http://www.gnu.org/software/libmicrohttpd
  DEPENDS:=+libpthread
endef

define Package/libmicrohttpd/description
 GNU libmicrohttpd is a small C library that is supposed to make it easy to run an HTTP server as part of another application.
endef

CONFIGURE_ARGS+= \
	--disable-curl \
	--disable-https \

define Build/InstallDev 
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/microhttpd.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmicrohttpd.{a,so*} $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libmicrohttpd.pc $(1)/usr/lib/pkgconfig/
endef

define Package/libmicrohttpd/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmicrohttpd.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libmicrohttpd))
