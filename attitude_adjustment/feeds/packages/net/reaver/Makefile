#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=reaver
PKG_REV:=113
PKG_VERSION:=r$(PKG_REV)
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://reaver-wps.googlecode.com/svn/trunk
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_PROTO:=svn

include $(INCLUDE_DIR)/package.mk

define Package/reaver
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=wireless
  TITLE:=Efficient brute force attack against Wifi Protected Setup
  URL:=https://code.google.com/p/reaver-wps/
  DEPENDS:=+libpcap +libsqlite3
endef

define Package/reaver/description
  Reaver targets the external registrar functionality mandated by the WiFi
  Protected Setup specification.
  Access points will provide authenticated registrars with their current
  wireless configuration (including the WPA PSK), and also accept a new
  configuration from the registrar.
endef

CONFIGURE_PATH:=src

MAKE_PATH:=src

TARGET_CFLAGS+=$(TARGET_CPPFLAGS)

define Package/reaver/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/{reaver,wash} $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/reaver
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/src/reaver.db $(1)/etc/reaver/
endef

$(eval $(call BuildPackage,reaver)) 
