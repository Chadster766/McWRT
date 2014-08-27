#
# Copyright (C) 2010 OpenWrt.org 
#
# This is free software, licensed under the GNU General Public License v2. 
# See /LICENSE for more information. 
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pwcrypt
PKG_VERSION:=1.2.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://xjack.org/pwcrypt/downloads
PKG_MD5SUM:=381a60c8debe5b21c935b19684de

include $(INCLUDE_DIR)/package.mk

define Package/pwcrypt
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=A utility to create crypted passwords from plaintext
  URL:=http://xjack.org/pwcrypt
endef

define Package/pwcrypt/description
 A wrapper for the unix crypt() function. Supports crypt and MD5 hashing
endef

define Package/pwcrypt/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/pwcrypt $(1)/sbin/
endef

$(eval $(call BuildPackage,pwcrypt))
