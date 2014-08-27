# 
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=shorewall-lite
PKG_VERSION:=4.4.27.3
PKG_DIRECTORY:=4.4.27
PKG_RELEASE:=4

PKG_SOURCE_URL:=http://www.shorewall.net/pub/shorewall/4.4/shorewall-$(PKG_DIRECTORY)/ \
	http://www1.shorewall.net/pub/shorewall/4.4/shorewall-$(PKG_DIRECTORY)/ \
	http://slovakia.shorewall.net/pub/shorewall/4.4/shorewall-$(PKG_DIRECTORY)/ \
	http://shorewall.de/pub/shorewall/4.4/shorewall-$(PKG_DIRECTORY)/ \
	http://www.shorewall.com.au/4.4/shorewall-$(PKG_DIRECTORY)/ \
	http://shorewall.infohiiway.com/pub/shorewall/4.4/shorewall-$(PKG_DIRECTORY)/ \
	http://www.shorewall.com.ar/pub/shorewall/shorewall/4.4/shorewall-$(PKG_DIRECTORY)/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_MD5SUM:=40be496c0d512d885b7b0f64204bc235

include $(INCLUDE_DIR)/package.mk

define Package/shorewall-lite
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+ip +iptables
  TITLE:=Shorewall Lite
  URL:=http://www.shorewall.net/
  SUBMENU:=Firewall
endef

define Package/shorewall-lite/description
	Shoreline Firewall Lite is an iptables-based firewall for Linux systems.
endef

define Package/shorewall-lite/conffiles
/etc/shorewall-lite/shorewall-lite.conf
/etc/shorewall-lite/vardir
endef

define Build/Compile
	PREFIX=$(PKG_INSTALL_DIR) $(PKG_BUILD_DIR)/install.sh
endef

define Package/shorewall-lite/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/etc/lsm/script.d
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DIR) $(1)/etc/shorewall-lite
	$(INSTALL_DIR) $(1)/usr/share
	$(INSTALL_BIN) ./files/shorewall-lite.init $(1)/etc/init.d/shorewall-lite
	$(INSTALL_BIN) ./files/hotplug_iface $(1)/etc/hotplug.d/iface/05-shorewall-lite
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/sbin/shorewall-lite $(1)/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/share/shorewall-lite $(1)/usr/share
	$(INSTALL_BIN) ./files/hostname $(1)/usr/share/shorewall-lite
	$(INSTALL_BIN) ./files/lsm_script $(1)/etc/lsm/script.d/45_shorewall-lite
	$(CP) $(PKG_INSTALL_DIR)/etc/shorewall-lite $(1)/etc
	$(CP) ./files/vardir $(1)/etc/shorewall-lite
endef

$(eval $(call BuildPackage,shorewall-lite))
