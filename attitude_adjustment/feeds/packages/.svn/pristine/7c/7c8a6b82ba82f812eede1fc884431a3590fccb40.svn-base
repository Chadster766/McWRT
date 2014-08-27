#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=reaim
PKG_VERSION:=0.8
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/reaim
PKG_MD5SUM:=647d2ab72ec454f89294accfb3182c09

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/reaim
  SUBMENU:=Instant Messaging
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+iptables
  TITLE:=A transparent AIM / MSN proxy
  URL:=http://reaim.sourceforge.net/
endef

define Package/reaim/description
ReAIM is a proxy for file transfers with MSN/AIM.
endef

define Package/reaim/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/reaim $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/reaim.init $(1)/etc/init.d/reaim
endef

$(eval $(call BuildPackage,reaim))
