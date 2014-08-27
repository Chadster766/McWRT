# 
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ez-ipupdate
PKG_VERSION:=3.0.11b8
PKG_RELEASE:=7

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/e/ez-ipupdate \
	http://ftp.de.debian.org/debian/pool/main/e/ez-ipupdate
PKG_MD5SUM:=000211add4c4845ffa4211841bff4fb0

include $(INCLUDE_DIR)/package.mk

define Package/ez-ipupdate
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=IP Addresses and Names
  TITLE:=Dynamic DNS services client
  URL:=http://ez-ipupdate.com/
endef

# uses GNU configure

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)
endef

define Package/ez-ipupdate/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ez-ipupdate $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/config/
	$(INSTALL_CONF) ./files/ez-ipupdate.config $(1)/etc/config/ddns
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_BIN) ./files/ez-ipupdate.hotplug $(1)/etc/hotplug.d/iface/30-ez-ipupdate
endef

define Package/ez-ipupdate/conffiles
/etc/config/ddns
endef

$(eval $(call BuildPackage,ez-ipupdate))
