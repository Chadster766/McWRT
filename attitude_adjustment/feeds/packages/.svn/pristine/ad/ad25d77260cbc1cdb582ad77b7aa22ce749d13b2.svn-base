#
# Copyright (C) 2007-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=6tunnel
PKG_VERSION:=0.11rc2
PKG_RELEASE:=3
PKG_MD5SUM:=74e02d4f0704b3083a01feda66033449

PKG_SOURCE_URL:=http://toxygen.net/6tunnel/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz

include $(INCLUDE_DIR)/package.mk

define Package/6tunnel
  SECTION:=ipv6
  CATEGORY:=IPv6
  DEPENDS:=+libpcap +kmod-sit
  TITLE:=IPv4 / IPv6 tunnel proxy
  URL:=http://toxygen.net/6tunnel/
endef

define Package/6tunnel/description
IPv4 / IPv6 tunnel proxy
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/$(PKG_NAME) $(PKG_BUILD_DIR)/$(PKG_NAME).c
endef

define Package/6tunnel/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/6tunnel $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,6tunnel))
