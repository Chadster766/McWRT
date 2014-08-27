#
# Copyright (C) 2009-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=prosody
PKG_VERSION:=0.6.2
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://prosody.im/downloads/source
PKG_MD5SUM:=5da59bc906419ad3b4faa21516a6ca18

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/prosody
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Instant Messaging
  DEPENDS:=+luafilesystem +libidn +luaexpat +luasec
  TITLE:=XMPP server
  URL:=http://prosody.im/
endef

define Package/prosody/description
  Prosody is an exciting new server for Jabber/XMPP
  written in Lua. It aims to be easy to use, and light
  on resources
endef

define Package/prosody/conffiles
/etc/prosody/prosody.cfg.lua
endef

TARGET_CFLAGS += $(FPIC)

define Build/Configure
	# this is *NOT* GNU autoconf stuff
	(cd $(PKG_BUILD_DIR); ./configure \
		--prefix=/usr \
		--with-lua="$(STAGING_DIR)/host/bin" \
		--with-lua-include="$(STAGING_DIR)/usr/include" \
		--with-lua-lib="$(STAGING_DIR)/usr/lib" \
		--cflags="$(TARGET_CFLAGS)" \
		--datadir="/etc/prosody/data" \
	)
endef

MAKE_FLAGS += \
	CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS) -std=gnu99" \
	LDFLAGS="$(TARGET_LDFLAGS)" \
	PREFIX="/usr" \

define Package/prosody/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/prosody.init $(1)/etc/init.d/prosody
	$(INSTALL_DIR) $(1)/etc/prosody
	$(INSTALL_CONF) $(PKG_INSTALL_DIR)/etc/prosody/prosody.cfg.lua $(1)/etc/prosody/
	$(INSTALL_DIR) $(1)/etc/prosody/certs
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/prosody/certs/localhost.{cert,key} $(1)/etc/prosody/certs/
	$(INSTALL_DIR) $(1)/etc/prosody/data
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/prosody{,ctl} $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib/prosody
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/prosody/prosody.version $(1)/usr/lib/prosody/
	$(INSTALL_DIR) $(1)/usr/lib/prosody/core
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/prosody/core/*.lua $(1)/usr/lib/prosody/core/
	$(INSTALL_DIR) $(1)/usr/lib/prosody/fallbacks
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/prosody/fallbacks/*.lua $(1)/usr/lib/prosody/fallbacks/
	$(INSTALL_DIR) $(1)/usr/lib/prosody/modules
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/prosody/modules/*.lua $(1)/usr/lib/prosody/modules/
	$(INSTALL_DIR) $(1)/usr/lib/prosody/modules/muc
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/prosody/modules/muc/*.lua $(1)/usr/lib/prosody/modules/muc/
	$(INSTALL_DIR) $(1)/usr/lib/prosody/net
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/prosody/net/*.lua $(1)/usr/lib/prosody/net/
	$(INSTALL_DIR) $(1)/usr/lib/prosody/util
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/prosody/util/*.lua $(1)/usr/lib/prosody/util/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/prosody/util/*.so $(1)/usr/lib/prosody/util/
endef

$(eval $(call BuildPackage,prosody))
