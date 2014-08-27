#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=httptunnel
PKG_VERSION:=3.3
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.nocrew.org/pub/nocrew/unix/
PKG_MD5SUM:=493cc0f5f21e9955db27ee9cd9a976d5

include $(INCLUDE_DIR)/package.mk

define Package/httptunnel
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libopenssl
  SUBMENU:=Firewall Tunnel
  TITLE:=bidirectional tunnel in HTTP requests
  URL:=http://www.nocrew.org/software/httptunnel.html
endef

define Package/httptunnel/description
	httptunnel creates a bidirectional virtual data connection tunnelled in HTTP requests.
	The HTTP requests can be sent via an HTTP proxy if so desired.
endef

define Package/httptunnel/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hts $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/htc $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/httptunnel.init $(1)/etc/init.d/httptunnel
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/httptunnel.config $(1)/etc/config/httptunnel
endef

define Package/httptunnel/conffiles
/etc/config/httptunnel
endef

$(eval $(call BuildPackage,httptunnel))
