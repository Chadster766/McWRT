#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dudders
PKG_VERSION:=1.04
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=1d368a86f3c284e95d260a94767a8ded

PKG_BUILD_DEPENDS:=+libopenssl +libgcrypt
PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/dudders
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=IP Addresses and Names
  DEPENDS:=+libopenssl +libgcrypt +libgpg-error
  TITLE:=DNS Update SIG(0) client
  MAINTAINER:=Dean Scarff <dos@scarff.id.au>
  URL:=http://dudders.sourceforge.net/
endef

define Package/dudders/description
  Dudders is a utility for updating DNS records.  It points
  a domain name to a given IP address, using the RFC2136 DNS
  UPDATE protocol and a SIG(0) signature.  It is designed with
  embedded systems in mind.

  You must have either libopenssl or libgcrypt installed to
  use this package.
endef

define Package/dudders/conffiles
/etc/config/dudders
endef

EXTRA_CFLAGS:= -DNDEBUG -std=gnu99

CONFIGURE_ARGS+= \
	--with-libgcrypt-prefix="$(STAGING_DIR)/usr"

define Package/dudders/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/dudders $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib/dudders
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/dudders/crypt_gcrypt.so \
		       $(1)/usr/lib/dudders/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/dudders/crypt_openssl.so \
		       $(1)/usr/lib/dudders/
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/dudders.config $(1)/etc/config/dudders
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_BIN) ./files/dudders.hotplug $(1)/etc/hotplug.d/iface/50-dudders
endef

$(eval $(call BuildPackage,dudders))
