#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mvprelay
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://downloads.openwrt.org/sources/
PKG_MD5SUM:=189fb792470c966834d5204705ebee83

include $(INCLUDE_DIR)/package.mk

define Package/mvprelay
  SECTION:=net
  CATEGORY:=Network
  TITLE:=relays requests from the MVP
  URL:=http://mvpmc.wikispaces.com/mvprelay
endef

define Package/mvprelay/description
	Relays requests from the MediaMVP to the TFTP server
endef

define Build/Configure
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(PKG_BUILD_DIR)/$(PKG_NAME).c -o $(PKG_BUILD_DIR)/$(PKG_NAME)
endef

define Package/mvprelay/install	
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,mvprelay))
