# 
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=lsm
PKG_VERSION:=0.131
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://lsm.foobar.fi/download
PKG_MD5SUM:=c3ff15f5e0d3988a1a45f35392639d40

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/lsm
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+ssmtp
  TITLE:=A link state monitor
  URL:=http://lsm.foobar.fi/
endef

define Package/lsm/description
	lsm is a link state monitor for carrying out actions when a link 
	transistions from the up to down state or vice versa.
endef

define Package/lsm/conffiles
/etc/lsm/lsm.conf
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS)"
endef

define Package/lsm/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lsm $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/lsm
	$(INSTALL_DIR) $(1)/etc/lsm/script.d
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DATA) ./files/connections.conf $(1)/etc/lsm/connections.conf
	$(INSTALL_DATA) ./files/lsm.conf $(1)/etc/lsm/lsm.conf
	$(INSTALL_BIN) ./files/lsm_script $(1)/etc/lsm/script
	$(INSTALL_BIN) ./files/lsm.init $(1)/etc/init.d/lsm
endef

define Package/lsm/conffiles
/etc/lsm/connections.conf
/etc/lsm/lsm.conf
endef

$(eval $(call BuildPackage,lsm))
