# 
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
#
include $(TOPDIR)/rules.mk

PKG_NAME:=openrrcp
PKG_VERSION:=0.2.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://nording.ru/projects/download/openrrcp/stable
PKG_MD5SUM:=e3d6097732af812b03dbbbedeced34d2

include $(INCLUDE_DIR)/package.mk

define Package/openrrcp
  SECTION:=net
  CATEGORY:=Network
  TITLE:=OpenRRCP toolset for managing RRCP-capable Ethernet switches
  URL:=http://openrrcp.org.ru/
endef

define Package/openrrcp/description
	OpenRRCP toolset for managing RRCP-capable Ethernet switches
endef

define Build/Compile
	$(call Build/Compile/Default, \
		PREFIX="/usr" \
	)
endef

define Package/openrrcp/install
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(1)" \
		PREFIX="/usr" \
		install
endef

$(eval $(call BuildPackage,openrrcp))
