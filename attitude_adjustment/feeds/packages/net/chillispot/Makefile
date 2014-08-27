#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=chillispot
PKG_VERSION:=1.1.0
PKG_RELEASE:=4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.chillispot.info/download
PKG_MD5SUM:=9d2597756af3fa14d7331b4a3651fc9b

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/chillispot
  SUBMENU:=Captive Portals
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+kmod-tun
  TITLE:=Wireless LAN HotSpot controller
  URL:=http://www.chillispot.info/
endef

define Package/chillispot/description
	ChilliSpot is an open source captive portal or wireless LAN 
	access point controller. It is used for authenticating users 
	of a wireless LAN. It supports web based login which is today's 
	standard for public HotSpots and it supports Wireless Protected 
	Access (WPA) which is the standard of the future. Authentication, 
	authorization and accounting (AAA) is handled by your favorite 
	radius server.
endef

define Package/chillispot/conffiles
/etc/chilli.conf
endef

# uses GNU configure

define Package/chillispot/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/$(PKG_NAME).init $(1)/etc/init.d/chilli
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/doc/chilli.conf $(1)/etc/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/chilli $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,chillispot))
