#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=frickin
PKG_VERSION:=1.3
PKG_RELEASE:=4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/frickin
PKG_MD5SUM:=5b2e98da2310c71c703d4617b88a173f

include $(INCLUDE_DIR)/package.mk

define Package/frickin
  SECTION:=net
  CATEGORY:=Network
  TITLE:=PPTP (Point-to-Point Tunneling Protocol) proxy
  URL:=http://sourceforge.net/projects/frickin/
  SUBMENU:=VPN
endef

define Package/frickin/description
	Frickin PPTP Proxy allows a Point-to-Point Tunneling Protocol (PPTP) 
	client to connect to a PPTP server through Network Address 
	Translation (NAT).
endef

define Build/Configure
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		all
endef

define Package/frickin/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/frickin.config $(1)/etc/config/frickin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/frickin.init $(1)/etc/init.d/frickin
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/frickin $(1)/usr/sbin/
endef

define Package/frickin/conffiles
/etc/config/frickin
endef

$(eval $(call BuildPackage,frickin))
