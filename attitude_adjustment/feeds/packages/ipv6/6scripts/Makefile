#
# Copyright (C) 2007-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=6scripts
PKG_VERSION:=0.1
PKG_RELEASE:=7

include $(INCLUDE_DIR)/package.mk

define Package/6scripts
  SECTION:=ipv6
  CATEGORY:=IPv6
  DEPENDS:=+kmod-ipv6
  TITLE:=IPv6 scripts
  PKGARCH:=all
endef

define Package/6scripts/description
IPv6 scripts to setup ipv6 connectivy using 6to4 tunnels
or bridged ipv6 connectivity.
endef

define Package/6scripts/conffiles
/etc/config/6tunnel
/etc/config/6bridge
endef

define Build/Compile
endef

define Build/Configure
endef

define Package/6scripts/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/6tunnel.conf $(1)/etc/config/6tunnel
	$(INSTALL_DATA) ./files/6bridge.conf $(1)/etc/config/6bridge
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/6tunnel.init $(1)/etc/init.d/6tunnel
	$(INSTALL_BIN) ./files/6bridge.init $(1)/etc/init.d/6bridge
	$(INSTALL_DIR) $(1)/etc/ppp/ip-up.d/ 
	$(INSTALL_DATA) ./files/he-tun.ppp $(1)/etc/ppp/ip-up.d/ 
endef

$(eval $(call BuildPackage,6scripts))
