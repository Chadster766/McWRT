#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=horst
PKG_VERSION:=3.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://br1.einfach.org/horst_dl/
PKG_MD5SUM:=076d836733fb6ce53a590eddf92ad358

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/horst
	SECTION:=net
	CATEGORY:=Network
        SUBMENU:=wireless
	DEPENDS:=+libncurses
	MAINTAINER:=Bruno Randolf <br1@einfach.org>
	TITLE:=Highly Optimized 802.11 Radio Scanning Tool
	URL:=http://br1.einfach.org/horst/
endef

define Package/horst/description
	[horst] is a scanning and analysis tool for 802.11 wireless networks
	and especially IBSS (ad-hoc) mode and mesh networks (OLSR).
endef

define Package/horst/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/horst $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/horst.sh $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,horst))
