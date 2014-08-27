#
# Copyright (C) 2009-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=uhub
PKG_VERSION:=0.2.8
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-src.tar.gz
PKG_SOURCE_URL:=http://www.extatic.org/downloads/uhub/
PKG_MD5SUM:=45d35d9757bc0a84297dec51520f725b

include $(INCLUDE_DIR)/package.mk

define Package/uhub
  SUBMENU:=P2P
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libevent
  TITLE:=High performance ADC hub
  URL:=http://www.extatic.org/uhub/
endef

define Package/uhub/description
	uhub is a high performance peer-to-peer hub for the ADC network.
        Its low memory footprint allows it to handle several thousand users
        on high-end servers, or a small private hub on embedded hardware.
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include/libevent
TARGET_LDFLAGS = -L$(STAGING_DIR)/usr/lib/libevent -L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib

ifdef CONFIG_USE_EGLIBC
TARGET_LDFLAGS += -lrt
endif

define Package/uhub/install
	$(INSTALL_DIR) $(1)/usr/local/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/uhub $(1)/usr/local/bin/
	$(INSTALL_DIR) $(1)/etc/uhub/
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/doc/uhub.conf $(1)/etc/uhub/
endef

define Package/uhub/conffiles
/etc/uhub/uhub.conf
endef

$(eval $(call BuildPackage,uhub))
