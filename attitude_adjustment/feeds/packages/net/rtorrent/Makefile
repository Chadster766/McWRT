#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rtorrent
PKG_VERSION:=0.8.6
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://libtorrent.rakshasa.no/downloads
PKG_MD5SUM:=b804c45c01c40312926bcea6b55bb084

PKG_FIXUP:=autoreconf
PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/rtorrent
  SUBMENU:=BitTorrent
  SECTION:=net
  CATEGORY:=Network
  TITLE:=BitTorrent client for ncurses
  URL:=http://libtorrent.rakshasa.no/
  DEPENDS:=+libcurl +libtorrent +libncursesw +libsigcxx +xmlrpc-c-server +libpthread
  MAINTAINER:=Luka Perkov <openwrt@lukaperkov.net>
endef

define Package/rtorrent/description
 rTorrent is a BitTorrent client for ncurses, using the libtorrent library.
 The client and library is written in C++ with emphasis on speed and
 efficiency, while delivering equivalent features to those found in GUI based
 clients in an ncurses client. 
endef

TARGET_CPPFLAGS += -fno-strict-aliasing -fno-inline
TARGET_LDFLAGS += -lpthread -Wl,-rpath-link=$(STAGING_DIR)/usr/lib

CONFIGURE_ARGS+= \
	--enable-shared \
	--disable-static \
	--disable-debug \
	--with-xmlrpc-c

define Build/Configure
	( cd $(PKG_BUILD_DIR); ./autogen.sh );
	$(call Build/Configure/Default)
endef

define Package/rtorrent/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/rtorrent $(1)/usr/bin/
endef

$(eval $(call BuildPackage,rtorrent))
