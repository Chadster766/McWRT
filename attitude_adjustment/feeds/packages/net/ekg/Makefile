#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=ekg
PKG_VERSION:=1.7
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://ekg.chmurka.net/
PKG_MD5SUM:=2aa92b56517fdf09d75519a105772b74

PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/ekg
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Instant Messaging
  TITLE:=Eksperymentalny Klient Gadu-Gadu (Polish IM)
  URL:=http://ekg.chmurka.net
  DEPENDS:=+zlib +libncurses
endef

CONFIGURE_ARGS+= \
	--without-c99-vsnprintf \
	--without-bind \
	--without-libgsm \
	--without-libjpeg \
	--without-libungif \
	--without-openssl \
	--disable-libgadu-openssl \
	--enable-ui-ncurses \
	--disable-ui-readline \

CONFIGURE_VARS+= \
	CFLAGS="$$$$CFLAGS $(TARGET_CPPFLAGS)" \
	CFLAGS_LIBGADU="$$$$CFLAGS $(TARGET_CPPFLAGS)" \

define Package/ekg/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ekg $(1)/usr/bin/
endef

$(eval $(call BuildPackage,ekg))
