#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=apf
PKG_VERSION:=0.8.4
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://gray-world.net/projects/af/
PKG_MD5SUM:=4411785b05ec59e955152fe4e61705b1

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

CONFIGURE_VARS += \
	LDFLAGS="$$$$LDFLAGS -lcrypto -lssl -lz"

define Package/apf
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Active port forwarder
  URL:=http://gray-world.net/pr_af.shtml
  DEPENDS:=+libopenssl +libpthread
endef

define Package/apf/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/af{client,server} $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/doc/afclient_example.conf $(1)/etc/afclient.conf
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/doc/afserver_example.conf $(1)/etc/afserver.conf
endef

define Package/apf/conffiles
/etc/afclient.conf
/etc/afserver.conf
endef

$(eval $(call BuildPackage,apf))
