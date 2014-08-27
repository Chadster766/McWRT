#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=cksfv
PKG_VERSION:=1.3.14
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://zakalwe.fi/~shd/foss/cksfv/files
PKG_MD5SUM:=2e15289753ea0b90b6ea86993f93b383

include $(INCLUDE_DIR)/package.mk

define Package/cksfv
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=cksfv CRC32 checksum verification tool
  URL:=http://zakalwe.fi/~shd/foss/cksfv/
endef

define Package/cksfv/description
  cksfv is a tool for verifying CRC32 checksums.
endef

define Package/cksfv/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/cksfv $(1)/usr/sbin/cksfv
endef

$(eval $(call BuildPackage,cksfv))
