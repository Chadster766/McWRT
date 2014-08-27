#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=peervpn
PKG_VERSION:=0-023
PKG_RELEASE:=1

PKG_BUILD_DIR :=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL :=http://www.peervpn.net/files

include $(INCLUDE_DIR)/package.mk

define Package/peervpn
  SECTION:=net
  CATEGORY:=Network
  TITLE:=PeerVPN the open source peer-to-peer VPN
  URL:=http://www.peervpn.net/
  DEPENDS:=+libopenssl +ip
endef

define Package/peervpn/description
  PeerVPN is software that builds virtual ethernet networks between multiple
  computers. Adding a new node to an existing network doesn't require the
  existing nodes to alter their configuration, because its IP address is
  automatically distributed across the whole virtual network.
endef

define Package/peervpn/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/peervpn $(1)/sbin/
	$(INSTALL_DIR) $(1)/etc/peervpn
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/peervpn.conf $(1)/etc/peervpn
endef

$(eval $(call BuildPackage,peervpn))
