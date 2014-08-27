#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=openconnect
PKG_VERSION:=4.08
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.infradead.org/pub/openconnect/
PKG_MD5SUM:=3dd065194d87c08084675d255c8e29ef

include $(INCLUDE_DIR)/package.mk

define Package/openconnect
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libxml2 +libopenssl +kmod-tun +resolveip
  TITLE:=VPN client for Cisco's AnyConnect SSL VPN
  URL:=http://www.infradead.org/openconnect/
  SUBMENU:=VPN
endef

define Package/openconnect/description
	A VPN client compatible with Cisco's AnyConnect SSL VPN.

        OpenConnect is a client for Cisco's AnyConnect SSL VPN, which is
        supported by IOS 12.4(9)T or later on Cisco SR500, 870, 880, 1800, 2800,
        3800, 7200 Series and Cisco 7301 Routers.  
endef

CONFIGURE_ARGS += \
	--disable-shared \
	--with-vpnc-script=/lib/netifd/vpnc-script

define Package/openconnect/install
	$(INSTALL_DIR) $(1)/lib/netifd/proto
	$(INSTALL_BIN) ./files/openconnect.sh $(1)/lib/netifd/proto/
	$(INSTALL_BIN) ./files/vpnc-script $(1)/lib/netifd/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/openconnect $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,openconnect))
