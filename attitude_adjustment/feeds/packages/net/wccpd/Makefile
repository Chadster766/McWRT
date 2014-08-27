#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wccpd
PKG_VERSION:=0.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/wccpd
PKG_MD5SUM:=5f15c274de61dfb88e0dbfc1ccbe6b67

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/wccpd
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Web Servers/Proxies
  TITLE:=a Web Cache Coordination daemon
  URL:=http://wccpd.sourceforge.net/
endef

define Package/wccpd/description
	wccpd is a WCCP (Web Cache Coordination Protocol) server daemon 
	It allows a router (running Linux) to redirect web traffic to a 
	group of Squid servers using WCCP as the monitoring/controling 
	protocol.
endef

define Package/wccpd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/wccpd $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,wccpd))
