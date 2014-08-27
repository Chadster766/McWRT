#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=lcd4linux-tew673gru
PKG_VERSION:=1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/lcd4linux-tew673gru
	SECTION:=utils
	CATEGORY:=Utilities
	DEPENDS:=@TARGET_ar71xx +kmod-spi-gpio +kmod-spi-dev +lcd4linux-custom
	TITLE:=TRENDnet TEW673GRU LCD support
endef

define Build/Compile
endef

define Package/lcd4linux-tew673gru/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/openwrt_220x176.png $(1)/etc/
	$(INSTALL_BIN) ./files/lcd4linux_tew673gru.conf $(1)/etc/
	$(INSTALL_BIN) ./files/tew673gru-lcd $(1)/etc/uci-defaults
endef

$(eval $(call BuildPackage,lcd4linux-tew673gru))
