#
# Copyright (C) 2006,2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dircproxy
PKG_VERSION:=1.2.0-RC1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://dircproxy.googlecode.com/files/
PKG_MD5SUM:=4fe2e82619c7b575599bfb236adbd872

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/dircproxy
  SUBMENU:=Instant Messaging
  SECTION:=net
  CATEGORY:=Network
  TITLE:=An IRC proxy server
  URL:=http://dircproxy.googlecode.com/
endef

# uses Build/Configure/Default

define Package/dircproxy/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/dircproxy{,-crypt} $(1)/usr/bin/
endef

$(eval $(call BuildPackage,dircproxy))
