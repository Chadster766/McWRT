#
# Copyright (C) 2009-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nzbget
PKG_VERSION:=0.8.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/nzbget
PKG_MD5SUM:=c47f464fe988deeb4fc5381506c1fbe3

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

TARGET_LDFLAGS+=-Wl,-rpath-link="$(STAGING_DIR)/usr/lib" -lcrypto -lsigc-2.0
TARGET_CFLAGS += $(FPIC)

define Package/nzbget
  SUBMENU:=NNTP
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libxml2 +libpar2 +libopenssl +libpthread +libncurses
  URL:=http://nzbget.sourceforge.net/
  TITLE:=Binary newsgrabber for nzb-files
  MAINTAINER:=Artur Wronowski <arteqw@gmail.com>
endef

define Package/nzbget/description
A command-line client/server based binary newsgrabber for nzb-files.
endef

CONFIGURE_ARGS += \
	--disable-nls \
	--with-tlslib=OpenSSL \
	--with-libpar2-includes=$(STAGING_DIR)/usr/include/libpar2 \
	--with-libpar2-libraries=$(STAGING_DIR)/usr/lib \
	--with-libsigc-includes=$(STAGING_DIR)/usr/include/sigc++-2.0 \
	--with-libsigc-libraries=$(STAGING_DIR)/usr/lib \
	--with-openssl-includes=$(STAGING_DIR)/usr/include \
	--with-openssl-libraries=$(STAGING_DIR)/usr/lib \
	--with-libcurses-includes=$(STAGING_DIR)/usr/include \
	--with-libcurses-libraries=$(STAGING_DIR)/usr/lib \
	, \
	PKG_CONFIG_PATH="$(STAGING_DIR)/usr/lib/pkgconfig"

CONFIGURE_VARS += \
	ac_cv_header_regex_h=no \
	CPPFLAGS="-I$(STAGING_DIR)/usr/include/libxml2 $$$$CPPFLAGS"

define Package/nzbget/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/nzbget $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/nzbget.conf.example $(1)/etc/nzbget.conf
endef

define Package/nzbget/conffiles
/etc/nzbget.conf
endef

$(eval $(call BuildPackage,nzbget))
