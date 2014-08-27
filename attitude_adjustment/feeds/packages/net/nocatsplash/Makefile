#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nocatsplash
PKG_VERSION:=0.93pre2
PKG_RELEASE:=2

PKG_SOURCE:=NoCatSplash-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://nocat.net/downloads/NoCatSplash/
PKG_MD5SUM:=b0aa5cc33a3bbdd8e65726ff3cfe3cd1

PKG_BUILD_DIR:=$(BUILD_DIR)/NoCatSplash-$(PKG_VERSION)

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/nocatsplash
  SUBMENU:=Captive Portals
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+glib1 +iptables-mod-ipopt +iptables-mod-nat-extra
  TITLE:=Open public network gateway daemon
  URL:=http://nocat.net
endef

define Package/nocatsplash/description
	NoCatSplash is an Open Public Network Gateway Daemon.
	It performs as a [captive/open/active] portal. When run on a gateway/router 
	on a network, all web requests are redirected until the client either logs 
	in or clicks "I Accept" to an AUP. The gateway daemon then changes the 
	firewall rules on the gateway to pass traffic for that client (based on IP 
	address and MAC address).
endef

define Package/nocatsplash/conffiles
/etc/nocat.conf
endef

define Build/Configure
	$(call Build/Configure/Default,	\
		--with-firewall=iptables \
		--with-glib-prefix="$(STAGING_DIR)/usr" \
	)
endef

define Package/nocatsplash/install
	$(INSTALL_DIR) $(1)
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
endef

$(eval $(call BuildPackage,nocatsplash))
