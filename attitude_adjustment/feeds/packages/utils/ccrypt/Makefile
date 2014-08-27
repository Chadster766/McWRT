#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ccrypt
PKG_VERSION:=1.9
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/ccrypt
PKG_MD5SUM:=c3f78019d7a166dd66f1d4b1390c62c2

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/ccrypt
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=ccrypt is a utility for encrypting and decrypting files and streams
  URL:=http://ccrypt.sourceforge.net/
endef

define Package/ccrypt/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/ccrypt $(1)/usr/bin/
	ln -s ccrypt $(1)/usr/bin/ccencrypt
	ln -s ccrypt $(1)/usr/bin/ccdecrypt
	ln -s ccrypt $(1)/usr/bin/ccat
endef

$(eval $(call BuildPackage,ccrypt))
