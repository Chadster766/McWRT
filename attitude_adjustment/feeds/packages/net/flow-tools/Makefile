#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=flow-tools
PKG_VERSION:=0.68
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.eng.oar.net/pub/flow-tools/
PKG_MD5SUM:=c9e0a8b53c79611b6bffcb9d510a5a38

PKG_BUILD_DEPENDS:=+zlib

include $(INCLUDE_DIR)/package.mk

define Package/flow-tools
  SECTION:=net
  CATEGORY:=Network
  TITLE:=flow-tools
  URL:=http://www.splintered.net/sw/flow-tools/
  DEPENDS:=+zlib
endef

define Package/flow-tools/description
	Flow-tools is a software package for collecting and processing 
	NetFlow data from Cisco and Juniper routers.
endef

define Build/Compile
	$(call Build/Compile/Default,all)
endef

define Package/flow-tools/install	
	$(INSTALL_DIR) $(1)/usr/bin/
	$(CP) $(PKG_BUILD_DIR)/src/flow-* $(1)/usr/bin/
endef

$(eval $(call BuildPackage,flow-tools))
