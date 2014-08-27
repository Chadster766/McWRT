#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=parprouted
PKG_VERSION:=0.64
PKG_RELEASE:=5

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.hazard.maks.net/parprouted/
PKG_MD5SUM:=14133c9cb8dd9c9a90ff37648ae7b31a

include $(INCLUDE_DIR)/package.mk

define Package/parprouted
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Routing and Redirection
  DEPENDS:=+libpthread
  TITLE:=Proxy ARP daemon
 URL:=http://www.hazard.maks.net/parprouted/
endef

define Package/parprouted/description
	parprouted is a daemon for transparent IP (Layer 3) proxy ARP bridging.
	This is useful for creation of transparent firewalls and bridging networks
	with different MAC protocols. Also, unlike standard bridging, proxy ARP
	bridging allows to bridge Ethernet networks behind wireless nodes without
	using WDS or layer 2 bridging.

endef

define Build/Compile
	$(call Build/Compile/Default, \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="" \
		all \
	)
endef

define Package/parprouted/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/parprouted $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_BIN) ./files/parprouted.config $(1)/etc/config/parprouted
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/parprouted.init $(1)/etc/init.d/parprouted
endef

define Package/parprouted/conffiles
/etc/config/parprouted
endef

$(eval $(call BuildPackage,parprouted))
