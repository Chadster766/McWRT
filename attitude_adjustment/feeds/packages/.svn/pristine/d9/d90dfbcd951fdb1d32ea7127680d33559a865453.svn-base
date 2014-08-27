# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=devio
PKG_VERSION:=1.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/devio
PKG_MD5SUM:=5d332c2bffc0791367bcf3368ba1a0d1

include $(INCLUDE_DIR)/package.mk

define Package/devio
  SUBMENU:=disc
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=correctly read (or write) a region of a block device
  URL:=http://sourceforge.net/projects/devio/
endef

define Package/devio/description
	A command line program to correctly read (or write) a region of a block device.
	Primary use is for extracting formatted data from MTD partitions.
endef

define Package/devio/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/devio $(1)/usr/sbin
endef

$(eval $(call BuildPackage,devio))
