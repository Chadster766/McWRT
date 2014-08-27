#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=vpnc-scripts
PKG_VERSION:=20120423
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.infradead.org/pub/vpnc-scripts/
PKG_MD5SUM:=3265dc7fe57ae9b4c905961a28699c4c

include $(INCLUDE_DIR)/package.mk

define Package/vpnc-scripts
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+ip
  TITLE:=VPN configuration script for vpnc and OpenConnect
  URL:=http://www.infradead.org/openconnect/vpnc-script.html
  SUBMENU:=VPN
endef

define Package/vpnc/vpnc-scripts
	This package contains the vpnc-script which is used by vpnc
	and OpenConnect to configure the tunnel interface.
endef

define Package/vpnc-scripts/conffiles
/etc/vpnc/connect.d/
/etc/vpnc/post-connect.d/
/etc/vpnc/disconnect.d/
/etc/vpnc/post-connect.d/
/etc/vpnc/reconnect.d/
endef

define Build/Compile
endef

define Package/vpnc-scripts/install
	$(INSTALL_DIR) $(1)/etc/vpnc
	$(INSTALL_DIR) $(1)/etc/vpnc/connect.d
	$(INSTALL_DIR) $(1)/etc/vpnc/post-connect.d
	$(INSTALL_DIR) $(1)/etc/vpnc/disconnect.d
	$(INSTALL_DIR) $(1)/etc/vpnc/post-disconnect.d
	$(INSTALL_DIR) $(1)/etc/vpnc/reconnect.d
	$(INSTALL_BIN) ./files/etc/vpnc/connect.d/ifstate $(1)/etc/vpnc/connect.d/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/vpnc-script $(1)/etc/vpnc/
endef

$(eval $(call BuildPackage,vpnc-scripts))
