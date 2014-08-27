#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libpthread-stubs
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://xcb.freedesktop.org/dist/
PKG_MD5SUM:=774eabaf33440d534efe108ef9130a7d
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libpthread-stubs
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=libpthread-stubs
  URL:=http://xcb.freedesktop.org/
  BUILDONLY:=1
endef

define Package/libpthread-stubs/description
This library provides weak aliases for pthread functions not provided in libc
or otherwise available by default.  Libraries like libxcb rely on pthread
stubs to use pthreads optionally, becoming thread-safe when linked to
libpthread, while avoiding any performance hit when running single-threaded.
libpthread-stubs supports this behavior even on platforms which do not supply
all the necessary pthread stubs.  On platforms which already supply all the
necessary pthread stubs, this package ships only the pkg-config file
pthread-stubs.pc, to allow libraries to unconditionally express a dependency
on pthread-stubs and still obtain correct behavior.
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/* \
		$(1)/usr/lib/pkgconfig/
endef

$(eval $(call BuildPackage,libpthread-stubs))
