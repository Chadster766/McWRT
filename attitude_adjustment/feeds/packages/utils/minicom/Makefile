#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=minicom
PKG_VERSION:=2.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://alioth.debian.org/download.php/2332/
PKG_MD5SUM:=0ebe7a91898384ca906787cc4e2c3f25

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/minicom
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libncurses
  TITLE:=Terminal emulation program
  URL:=http://alioth.debian.org/projects/minicom/
  SUBMENU:=Terminal
endef

define Package/minicom/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/$(PKG_NAME) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/ascii-xfr $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,minicom))
