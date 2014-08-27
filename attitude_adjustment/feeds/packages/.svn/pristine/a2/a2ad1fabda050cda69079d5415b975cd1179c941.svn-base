# 
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=anyremote
PKG_VERSION:=4.7.1
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/${PKG_NAME}
PKG_MD5SUM:=bd177156faa8a59a5d3e02cada243fc3

include $(INCLUDE_DIR)/package.mk

define Package/anyremote
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=A bluetooth remote control app
  DEPENDS:=+bluez-libs
  URL:=http://anyremote.sourceforge.net/
endef

define Package/anyremote/description
 Remote control your machine via bluetooth
endef

define Build/Configure
    $(call Build/Configure/Default, \
    --disable-xtest \
    )
endef

define Package/anyremote/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/$(PKG_NAME) $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_CONF) ./files/anyremote.cfg $(1)/etc/anyremote.cfg
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/anyremote.init $(1)/etc/init.d/anyremote
endef

define Package/anyremote/conffiles
/etc/anyremote.cfg
endef

$(eval $(call BuildPackage,anyremote))
