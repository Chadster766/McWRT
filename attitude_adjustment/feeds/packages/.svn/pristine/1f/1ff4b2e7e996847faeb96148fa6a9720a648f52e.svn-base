# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pen
PKG_VERSION:=0.18.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://siag.nu/pub/pen/
PKG_MD5SUM:=96f6d39e7e7cca11a647e795550f3829

include $(INCLUDE_DIR)/package.mk

define Package/pen
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libopenssl
  TITLE:=Simple tcp load balancer
  URL:=http://siag.nu/pen/
endef

define Package/pen/description
	This is pen, a load balancer for "simple" tcp based protocols
	such as http or smtp. It allows several servers to appear as
	one to the outside and automatically detects servers that are
	down and distributes clients among the available servers.
	This gives high availability and scalable performance.
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--with-poll \
		--with-experimental-only-ssl="$(STAGING_DIR)/usr" \
	)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)
endef

define Package/pen/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mergelogs $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pen $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/penctl $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/penctl.cgi $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/penlog $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/penlogd $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/penstats $(1)/usr/bin/
endef

$(eval $(call BuildPackage,pen))
