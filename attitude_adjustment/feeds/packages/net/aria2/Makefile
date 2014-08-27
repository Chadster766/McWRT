#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=aria2
PKG_VERSION:=1.15.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/aria2
PKG_MD5SUM:=a749a1f7cfe03a7518630d2310267c42
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/aria2
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=File Transfer
  TITLE:=lightweight download utility
  URL:=http://aria2.sourceforge.net/
  DEPENDS:=+libopenssl +zlib +libxml2 +libstdcpp \
	$(INTL_DEPENDS) $(ICONV_DEPENDS)
endef

define Package/aria2/description
 aria2 is a lightweight multi-protocol & multi-source command-line download
 utility
endef

CONFIGURE_ARGS += \
	--disable-nls \
	--without-gnutls \
	--without-libnettle \
	--without-libgmp \
	--without-libgcrypt \
	--without-libexpat \
	--without-libcares \
	--without-sqlite3 \
	--with-openssl \
	--with-libxml2 \
	--with-libz

CONFIGURE_VARS += \
	ZLIB_CFLAGS="-I$(STAGING_DIR)/usr/include" \
	ZLIB_LIBS="-L$(STAGING_DIR)/usr/lib"

define Package/aria2/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/aria2c $(1)/usr/bin
endef

$(eval $(call BuildPackage,aria2))
