#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=php-pear-db
PKG_VERSION:=1.7.11
PKG_RELEASE:=1

PKG_SOURCE:=DB-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=http://download.pear.php.net/package
PKG_MD5SUM:=a57302138757500706e075a4d3d87ef7

PKG_BUILD_DIR:=$(BUILD_DIR)/DB-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/php-pear-db
  SUBMENU:=PHP
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=PEAR DB module
  URL:=http://pear.php.net/package/DB
  DEPENDS:=php-pear
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/php-pear-db/install
	$(INSTALL_DIR) $(1)/usr/lib/php
	$(CP)	$(PKG_BUILD_DIR)/DB.php $(PKG_BUILD_DIR)/DB \
		$(1)/usr/lib/php/
endef

$(eval $(call BuildPackage,php-pear-db))
