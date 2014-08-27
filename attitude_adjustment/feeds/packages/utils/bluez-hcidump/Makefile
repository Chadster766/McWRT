#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=bluez-hcidump
PKG_VERSION:=1.42
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://bluez.sourceforge.net/download
PKG_MD5SUM:=5704737aaf72104eeaf77335218a1827

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/bluez-hcidump
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+bluez-libs
  TITLE:=Bluetooth packet analyzer
  URL:=http://www.bluez.org/
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--with-bluez="$(STAGING_DIR)/usr/include" \
	)
endef

define Package/bluez-hcidump/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/hcidump $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,bluez-hcidump))
