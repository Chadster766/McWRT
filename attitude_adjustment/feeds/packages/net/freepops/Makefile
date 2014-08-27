#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 5624 2006-11-23 00:29:07Z nbd $

include $(TOPDIR)/rules.mk

PKG_NAME:=freepops
PKG_VERSION:=0.2.4
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/freepops
PKG_MD5SUM:=d18848b903ceb68a53ded6b6d9e33047

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk

define Package/freepops
  SECTION:=net
  CATEGORY:=Network
  TITLE:=POP3 interface to several webmails
  DEPENDS:=+libpthread +liblua +luafilesystem +luaexpat @BROKEN # +luacurl
  URL:=http://www.freepops.org
endef

define Package/freepops/description
	FreePOPs is a POP3 daemon that translates local POP3 commands
	to remote HTTP requests to the supported webmail sites.

	FreePOPs can also be used as RSS aggregator and POP3 forwarder.
endef

define Build/Configure
	cd $(PKG_BUILD_DIR) && \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS) -DHAVE_CONFIG_H -I$(PKG_BUILD_DIR)" \
	LDFLAGS="$(TARGET_LDFLAGS) $(EXTRA_LDFLAGS) -llua -llua-filesystem -llua-curl -llua-expat" \
	PKGCONFIG=true \
	./configure.sh openwrt -lua -luafilesystem -luacurl -luaexpat
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) WHERE=/usr/ all
	$(TARGET_CROSS)strip $(PKG_BUILD_DIR)/src/freepopsd
endef

define Package/freepops/install
	$(MAKE) -C $(PKG_BUILD_DIR) DESTDIR=$(PKG_INSTALL_DIR)/ WHERE=/usr/ install
	mkdir -p $(PKG_INSTALL_DIR)/etc/init.d
	$(CP) $(PKG_BUILD_DIR)/buildfactory/openwrt/S50freepopsd \
		$(PKG_INSTALL_DIR)/etc/init.d 
	chmod a+x $(PKG_INSTALL_DIR)/etc/init.d/*
	rm -rf $(PKG_INSTALL_DIR)/usr/share/doc
	rm -rf $(PKG_INSTALL_DIR)/usr/share/man
	rm -rf $(PKG_INSTALL_DIR)/usr/bin/freepops-updater-dialog
	rm -rf $(PKG_INSTALL_DIR)/usr/share/freepops/lua/foo.lua
	rm -rf $(PKG_INSTALL_DIR)/usr/share/freepops/lua/skeleton.lua
	cp -r $(PKG_INSTALL_DIR)/* $(1)
endef

$(eval $(call BuildPackage,freepops))
