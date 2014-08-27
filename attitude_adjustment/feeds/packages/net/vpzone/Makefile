#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=vpzone
PKG_VERSION:=0.1.2
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/vpzone
PKG_MD5SUM:=9b8a142a217f69ece0163b2026dc8585

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/vpzone
  CATEGORY:=Network
  SECTION:=net
  SUBMENU:=VPN
  TITLE:=A flexible multipurpose layer 3 VPN daemon
  URL:=http://vpzone.sourceforge.net/
  DEPENDS:=+libcares +libevent2 +libgnutls +kmod-ipv6 +kmod-tun
endef

define Package/vpzone/conffiles
/etc/vpzone/vpzone.conf
endef

define Package/vpzone/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/vpzone $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib/vpzone
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/vpzone/*.so $(1)/usr/lib/vpzone/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/vpzone.init $(1)/etc/init.d/vpzone
	$(INSTALL_DIR) $(1)/etc/vpzone
	$(INSTALL_CONF) ./files/vpzone.conf $(1)/etc/vpzone/
endef

$(eval $(call BuildPackage,vpzone))
