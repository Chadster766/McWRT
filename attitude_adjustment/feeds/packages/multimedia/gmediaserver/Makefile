#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gmediaserver
PKG_VERSION:=0.13.0
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://savannah.nongnu.org/download/gmediaserver/
PKG_MD5SUM:=c422de386331e2a1a859d45f6fa270a3

PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/gmediaserver
  SECTION:=multimedia
  CATEGORY:=Multimedia
  DEPENDS:=+id3lib +libmagic +file +libupnp +libuuid +zlib $(ICONV_DEPENDS)
  TITLE:=An UPnP music media server
  URL:=http://www.nongnu.org/gmediaserver/
endef

define Package/gmediaserver/description
 GMediaServer is a server for UPnP media players like the
 Netgear MP101, Linksys WMLS11B etc. It exports one or more
 directories using the UPnP protocol so the mediaserver can
 browse through them and play audio or video files.
endef

CONFIGURE_ARGS += \
	--disable-rpath \
	--enable-threads=posix \
	--with-libiconv-prefix="$(ICONV_PREFIX)" \
	--without-libintl-prefix \
	--with-id3lib="$(STAGING_DIR)/usr" \
	--with-libupnp="$(STAGING_DIR)/usr" \
	, \
	LIBS="-lz" \

MAKE_FLAGS += \
	AR="$(TARGET_CROSS)ar" \
	LTLIBICONV="" \
	DESTDIR="$(PKG_INSTALL_DIR)"

define Package/gmediaserver/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/$(PKG_NAME) $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/$(PKG_NAME).config $(1)/etc/config/$(PKG_NAME)
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/$(PKG_NAME).init $(1)/etc/init.d/$(PKG_NAME)
endef

define Package/gmediaserver/conffiles
/etc/config/gmediaserver
endef

$(eval $(call BuildPackage,gmediaserver))
