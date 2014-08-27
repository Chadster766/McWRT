#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 9173 2007-10-07 03:20:26Z blogic $

include $(TOPDIR)/rules.mk

PKG_NAME:=rblibtorrent
PKG_VERSION:=0.14.7
PKG_RELEASE:=1

PKG_SOURCE:=libtorrent-rasterbar-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/libtorrent
PKG_MD5SUM:=6959f719245d82758235fd73d93373b2
PKG_BUILD_DIR:=$(BUILD_DIR)/libtorrent-rasterbar-$(PKG_VERSION)
PKG_BUILD_DEPENDS:=libtool
BOOSTPOSTFIX=mt

include $(INCLUDE_DIR)/package.mk

define Package/rblibtorrent
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Rasterbar BitTorrent library
  URL:=http://www.rasterbar.com/products/libtorrent/
  DEPENDS:=+boost +boost-python +boost-filesystem +boost-regex +boost-thread +boost-program_options +boost-system +libopenssl @BROKEN
endef

define Package/rblibtorrent/description
Rasterbar libtorrent is a C++ library that aims to be a good alternative to
all the other bittorrent implementations around. It is a library and not a
full featured client, although it comes with a working example client.
endef

CONFIGURE_ARGS+= \
	--enable-shared \
	--disable-debug \
	--enable-python-binding \
	--with-ssl=$(STAGING_DIR)/usr \
	--with-boost=$(STAGING_DIR)/usr/include \
	--with-boost-libdir=$(STAGING_DIR)/usr/lib \
	--with-zlib=detect \
	--with-boost-system=boost_system-$(BOOSTPOSTFIX) \
	--with-boost-filesystem=boost_filesystem-$(BOOSTPOSTFIX) \
	--with-boost-thread=boost_thread-$(BOOSTPOSTFIX) \
	--with-boost-regex=boost_regex-$(BOOSTPOSTFIX) \
	--with-boost-python=boost_python-$(BOOSTPOSTFIX) \
	--with-boost-program_options=boost_program_options-$(BOOSTPOSTFIX)

CONFIGURE_VARS+=CC="$(TARGET_CXX)"

EXTRA_LDFLAGS+=-lz -lpthread

define Build/Compile
	$(call Build/Compile/Default)
	$(call Build/Install/Default)
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libtorrent $(1)/usr/include/
	mkdir -p $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so* $(1)/usr/lib/
	mkdir -p $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libtorrent-rasterbar.pc $(1)/usr/lib/pkgconfig/
endef

define Package/rblibtorrent/install
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/lib/python2.6/site-packages/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so* $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/python2.6/site-packages/*.so* $(1)/usr/lib/python2.6/site-packages/
endef

$(eval $(call BuildPackage,rblibtorrent))
