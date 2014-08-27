# 
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=shorewall6-lite
PKG_VERSION:=4.4.22.2
PKG_DIRECTORY:=4.4.22
PKG_RELEASE:=2

PKG_SOURCE_URL:=http://www.shorewall.net/pub/shorewall/4.4/shorewall-$(PKG_DIRECTORY)/ \
	http://www1.shorewall.net/pub/shorewall/4.4/shorewall-$(PKG_DIRECTORY)/ \
	http://slovakia.shorewall.net/pub/shorewall/4.4/shorewall-$(PKG_DIRECTORY)/ \
	http://shorewall.de/pub/shorewall/4.4/shorewall-$(PKG_DIRECTORY)/ \
	http://www.shorewall.com.au/4.4/shorewall-$(PKG_DIRECTORY)/ \
	http://shorewall.infohiiway.com/pub/shorewall/4.4/shorewall-$(PKG_DIRECTORY)/ \
	http://www.shorewall.com.ar/pub/shorewall/shorewall/4.4/shorewall-$(PKG_DIRECTORY)/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_MD5SUM:=3ad7ba3790388bb562d3c937ba02fa31

include $(INCLUDE_DIR)/package.mk

define Package/shorewall6-lite
  SECTION:=ipv6
  CATEGORY:=IPv6
  DEPENDS:=+ip +ip6tables
  TITLE:=Shorewall6 Lite
  URL:=http://www.shorewall.net/
  SUBMENU:=Firewall
endef

define Package/shorewall6-lite/description
	Shoreline Firewall 6 Lite is an iptables-based firewall for Linux systems.
endef

define Package/shorewall6-lite/conffiles
/etc/shorewall6-lite/shorewall6-lite.conf
/etc/shorewall6-lite/vardir
endef

define Build/Compile
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	PREFIX=$(PKG_INSTALL_DIR) $(PKG_BUILD_DIR)/install.sh
endef

define Package/shorewall6-lite/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DIR) $(1)/etc/shorewall6-lite
	$(INSTALL_DIR) $(1)/usr/share
	$(INSTALL_BIN) ./files/shorewall-lite.init $(1)/etc/init.d/shorewall6-lite
	$(INSTALL_BIN) ./files/hotplug_iface $(1)/etc/hotplug.d/iface/05-shorewall6-lite
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/sbin/shorewall6-lite $(1)/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/share/shorewall6-lite $(1)/usr/share
	$(INSTALL_BIN) ./files/hostname $(1)/usr/share/shorewall6-lite
	$(CP) $(PKG_INSTALL_DIR)/etc/shorewall6-lite $(1)/etc
	$(CP) ./files/vardir $(1)/etc/shorewall6-lite
endef

$(eval $(call BuildPackage,shorewall6-lite))
