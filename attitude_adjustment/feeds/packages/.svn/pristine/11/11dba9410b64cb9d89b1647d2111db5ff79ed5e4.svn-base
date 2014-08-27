# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ifstat
PKG_VERSION:=1.1
PKG_RELEASE:=1

PKG_SOURCE_URL:=http://gael.roualland.free.fr/ifstat
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_MD5SUM:=b655642c33a626cfe976792fbcd9b6e1

include $(INCLUDE_DIR)/package.mk

define Package/ifstat
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Network interface bandwidth usage
  URL:=http://gael.roualland.free.fr/ifstat/
endef

define Package/ifstat/description
 Network interface bandwidth usage, with support for snmp targets
endef

CONFIGURE_ARGS += \
	--with-proc=yes \
	--with-snmp=no \

define Build/Configure
	(cd $(PKG_BUILD_DIR); touch \
		config.h.in \
		configure \
	);
	$(call Build/Configure/Default)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) all
endef

define Package/ifstat/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ifstat $(1)/usr/bin/
endef

$(eval $(call BuildPackage,ifstat))
