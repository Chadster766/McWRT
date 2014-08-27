#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=l2tpns
PKG_VERSION:=2.1.21
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/l2tpns
PKG_MD5SUM:=385c58055723ebc6c38062acd2db9c2c

include $(INCLUDE_DIR)/package.mk

define Package/l2tpns
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libcli
  TITLE:=An L2TP enhanced server
  URL:=http://sourceforge.net/projects/l2tpns
  SUBMENU:=VPN
endef

define Package/l2tpns/description
	l2tpns is a layer 2 tunneling protocol network server (LNS).
	it supports up to 65535 concurrent sessions per server/cluster
	plus ISP features such as rate limiting, walled garden, usage
	accounting, and more.
endef

define Package/l2tpns/conffiles
/etc/l2tpns/ip_pool
/etc/l2tpns/startup-config
/etc/l2tpns/users
endef
	
define Build/Compile
	$(call Build/Compile/Default, \
		LD="$(TARGET_CC)" \
		OPTIM="$(TARGET_CFLAGS)" \
		STAGING_DIR="$(STAGING_DIR)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install \
	)
endef

define Package/l2tpns/install	
	$(INSTALL_DIR) $(1)/etc/l2tpns
	$(CP) $(PKG_INSTALL_DIR)/etc/l2tpns/* $(1)/etc/l2tpns/
	$(INSTALL_DIR) $(1)/usr/lib/l2tpns
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/l2tpns/* $(1)/usr/lib/l2tpns/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/* $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,l2tpns))
