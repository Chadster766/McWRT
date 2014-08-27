#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=hiawatha
PKG_VERSION:=7.8.2
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.hiawatha-webserver.org/files/
PKG_MD5SUM:=8aff3f8c759871ea1d1ff22e98030332

PKG_CONFIG_DEPENDS:= \
	CONFIG_IPV6 \

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/hiawatha
  CATEGORY:=Network
  SECTION:=net
  SUBMENU:=Web Servers/Proxies
  TITLE:=A very lightweight web server
  URL:=http://www.hiawatha-webserver.org/
  MAINTAINER:=Raphaël HUCK <rhk@cksum.org>
  DEPENDS:=+libpthread +zlib
endef

define Package/hiawatha/description
  Hiawatha is an open source webserver with a focus on security.
endef

CONFIGURE_ARGS+= \
	$(call autoconf_bool,CONFIG_IPV6,ipv6) \
	--disable-cache \
	--disable-monitor \
	--disable-ssl \
	--disable-xslt

CONFIGURE_VARS+= \
	ac_cv_file__dev_urandom=yes \
	webrootdir=/www

define Package/hiawatha/conffiles
/etc/hiawatha/hiawatha.conf
/etc/hiawatha/mimetype.conf
endef

define Package/hiawatha/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/hiawatha $(1)/usr/sbin/

	$(INSTALL_DIR) $(1)/etc/hiawatha
	$(INSTALL_CONF) $(PKG_INSTALL_DIR)/etc/hiawatha/{hiawatha,mimetype}.conf $(1)/etc/hiawatha/
endef

$(eval $(call BuildPackage,hiawatha))
