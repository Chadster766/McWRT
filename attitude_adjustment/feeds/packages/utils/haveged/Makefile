#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 1 2012-01-12 03:50:18Z Olipro $ 

include $(TOPDIR)/rules.mk

PKG_NAME:=haveged
PKG_VERSION:=1.4
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.issihosts.com/$(PKG_NAME)
PKG_MD5SUM:=39a03d4b8a4225706f78dbef3f623b0e

#ensure this is consistent with the dir in the tarball!!!
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/haveged
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Feeds the kernel entropy pool by timing CPU loops.
  DEPENDS:=+librt +libpthread
  URL:=http://www.issihosts.com/haveged/
endef

define Package/haveged/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/haveged.init $(1)/etc/init.d/haveged
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/haveged $(1)/sbin/
endef

$(eval $(call BuildPackage,haveged))
