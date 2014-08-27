# 
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pepsal
PKG_VERSION:=1.2.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=5107f0b922716276e8493b26b80fb4b6

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/pepsal
  TITLE:=Performance Enhancing Proxy
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Web Servers/Proxies
  URL:=http://pepsal.sourceforge.net/
  DEPENDS:=+libnetfilter-queue
endef

define Package/pepsal/description
 PEPsal is an integrated, multi-layer, transparent TCP Performance Enhancing 
 Proxy which splits the connection into two parts, making use of Linux TCP 
 enhancements when sending data, and largely improving performance in 
 links with different characteristics 
endef


define Package/pepsal/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/pepsal $(1)/usr/bin/
endef

$(eval $(call BuildPackage,pepsal))
