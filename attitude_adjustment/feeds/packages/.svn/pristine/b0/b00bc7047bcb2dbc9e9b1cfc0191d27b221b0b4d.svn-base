#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=macchanger
PKG_VERSION:=1.5.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/macchanger
PKG_MD5SUM:=79b7cdaeca3d8ebafa764c4b0dd03ab7

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/macchanger
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=utility for viewing/manipulating the MAC address
  URL:=http://www.alobbs.com/macchanger
endef

define Package/macchanger/description
	This is a GNU/Linux utility for viewing/manipulating the MAC address 
	of network interfaces.
endef

define Package/macchanger/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/macchanger $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/share
	$(CP) $(PKG_INSTALL_DIR)/usr/share/macchanger $(1)/usr/share/
endef

$(eval $(call BuildPackage,macchanger))
