#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=md5deep
PKG_VERSION:=3.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=d103b738721c54a9bf535470b63d49dc

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/md5deep
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=A program to compute, match and audit hashsets
  URL:=http://md5deep.sourceforge.net/
endef

define Package/md5deep/description
 md5deep is a set of programs to compute MD5, SHA-1, SHA-256, Tiger, or
 Whirlpool message digests on an arbitrary number of files.
endef

define Package/md5deep/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/{hash,md5,sha1,sha256,tiger,whirlpool}deep $(1)/usr/bin/
endef

$(eval $(call BuildPackage,md5deep))
