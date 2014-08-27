#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ssltunnel
PKG_VERSION:=1.16
PKG_RELEASE:=1

PKG_SOURCE:=${PKG_NAME}-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.hsc.fr/ressources/outils/ssltunnel/download
PKG_MD5SUM:=4039b50566a33a4077883ea974e476d5

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/ssltunnel
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+ppp +libopenssl
  TITLE:=A PPP over SSL VPN tool
  URL:=http://www.hsc.fr/ressources/outils/ssltunnel/index.html
  SUBMENU:=VPN
endef

define Package/ssltunnel/description
	SSLTunnel is a client/server program to create a PPP connection over a
	SSL/TLS session.
	Client and server are mutually authenticated using a X509 certificate,
	PPP packets are then encrypted/decrypted realtime on each side.
	It's possible to use ssltunnel to connect a machine to another, a
	machine with a network or two networks sets.
endef

define Build/Configure
	$(call Build/Configure/Default, \
		, \
		BUILD_CC="$(TARGET_CC)" \
		HOSTCC="$(HOSTCC)" \
	)
endef

define Package/ssltunnel/install	
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/pppclient $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/pppwho $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/pppserver $(1)/usr/lib/
endef

$(eval $(call BuildPackage,ssltunnel))
