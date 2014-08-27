#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=php-pear-xmlrpc
PKG_VERSION:=1.5.1
PKG_RELEASE:=1

PKG_SOURCE:=XML_RPC-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=http://download.pear.php.net/package
PKG_MD5SUM:=1b516162ad65971b5fd04a7c279627ec

PKG_BUILD_DIR:=$(BUILD_DIR)/XML_RPC-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/php-pear-xmlrpc
  SUBMENU:=PHP
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=PEAR XML-RPC module
  URL:=http://pear.php.net/package/XML_RPC
  DEPENDS:=php-pear
  PKGARCH:=all
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/php-pear-xmlrpc/install
	$(INSTALL_DIR) $(1)/usr/lib/php/XML
	$(CP)	$(PKG_BUILD_DIR)/Dump.php \
		$(PKG_BUILD_DIR)/RPC.php \
		$(PKG_BUILD_DIR)/Server.php \
		$(1)/usr/lib/php/XML/
endef

$(eval $(call BuildPackage,php-pear-xmlrpc))
