#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=php-pear
PKG_VERSION:=1.5.4
PKG_RELEASE:=1

PKG_SOURCE:=PEAR-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=http://download.pear.php.net/package
PKG_MD5SUM:=06432c4768fdd7df2c4982c973d3f429

PKG_BUILD_DIR:=$(BUILD_DIR)/PEAR-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/php-pear
  SUBMENU:=PHP
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=PEAR
  URL:=http://pear.php.net/package/PEAR
  DEPENDS:=+php5
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/php-pear/install
	$(INSTALL_DIR) $(1)/usr/lib/php
	$(CP)	$(PKG_BUILD_DIR)/PEAR.php $(PKG_BUILD_DIR)/PEAR \
		$(1)/usr/lib/php/
endef

$(eval $(call BuildPackage,php-pear))

