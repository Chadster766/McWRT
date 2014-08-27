#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libtwin
PKG_VERSION:=0.0.3
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://ozlabs.org/~jk/projects/petitboot/downloads/
PKG_MD5SUM:=ebd7c5cca3c9e108208a7bf17b256a6c
PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

PKG_INSTALL=1

define Package/libtwin
	SECTION:=libs
	CATEGORY:=Libraries
	DEPENDS:=+zlib +libpng +libjpeg +libfreetype
	TITLE:=The tiny windowing system library
	URL:=http://ozlabs.org/~jk/projects/petitboot/
	MAINTAINER:=Geoff Levand <geoffrey.levand@am.sony.com>
endef

define Package/libtwin/description
	Keith Packard's Tiny Windowing System Library
endef

CONFIGURE_ARGS += --enable-shared --enable-static --disable-x11

ifdef CONFIG_powerpc
  EXTRA_CFLAGS += -flax-vector-conversions
endif

TARGET_LDFLAGS += \
	-Wl,-rpath-link=$(STAGING_DIR)/usr/lib

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/libtwin
	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/include/libtwin/*.h \
		$(1)/usr/include/libtwin

	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/lib/libtwin.{la,a,so*} \
		$(1)/usr/lib/

	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libtwin.pc \
		$(1)/usr/lib/pkgconfig/
endef

define Package/libtwin/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/lib/libtwin.so* \
		$(1)/usr/lib/
endef

$(eval $(call BuildPackage,libtwin))
