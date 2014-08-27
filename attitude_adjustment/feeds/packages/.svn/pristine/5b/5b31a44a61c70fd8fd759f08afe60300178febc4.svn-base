#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=crowdcontrol
PKG_VERSION:=0.4b
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://downloads.openwrt.org/sources/
PKG_MD5SUM:=a0991ae3d1f1a361558bed1a4aa66a9c

include $(INCLUDE_DIR)/package.mk

define Package/crowdcontrol
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Web Servers/Proxies
  TITLE:=The extra-light http proxy server
  URL:=http://sourceforge.net/projects/crowdcontrol/
endef

define Package/crowdcontrol/description
	Crowd Control is an small, cross-platform HTTP proxy (a fork of the elhttp project) 
	designed for systems with little RAM, such as a home router. It can filter URLs using 
	SquidGuard blocklists, and has the unique ability to filter HTTPS domains as well.
endef

define Build/Configure
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/crowdcontrol $(PKG_BUILD_DIR)/crowdcontrol.c
endef

define Package/crowdcontrol/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/etc/crowdcontrol
	$(CP) $(PKG_BUILD_DIR)/crowdcontrol $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/*-{domains,expressions,urls} $(1)/etc/crowdcontrol/
endef

define Package/crowdcontrol/conffiles
/etc/crowdcontrol/blocked-domains
/etc/crowdcontrol/blocked-expressions
/etc/crowdcontrol/blocked-urls
/etc/crowdcontrol/permitted-domains
/etc/crowdcontrol/permitted-expressions
/etc/crowdcontrol/permitted-urls
endef

$(eval $(call BuildPackage,crowdcontrol))
