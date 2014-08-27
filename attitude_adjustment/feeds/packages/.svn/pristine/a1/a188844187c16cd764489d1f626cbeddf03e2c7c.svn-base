#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=reiserfsprogs
PKG_VERSION:=3.6.21
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@KERNEL/linux/utils/fs/reiserfs/
PKG_MD5SUM:=bc00c7c4e047902575dc4e1c64ab3ba4

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/reiserfsprogs
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Filesystem
  TITLE:=ReiserFS filesystems utilities
  URL:=http://www.namesys.com/
endef

define Package/reiserfsprogs/description
	This package contains utilities to create, check,
	resize, and debug ReiserFS filesystems.
endef

define Package/reiserfsprogs/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(foreach bin,debugreiserfs mkreiserfs reiserfsck reiserfstune resize_reiserfs,$(PKG_INSTALL_DIR)/usr/sbin/$(bin)) $(1)/usr/sbin/
	ln -sf mkreiserfs $(1)/usr/sbin/mkfs.reiserfs
	ln -sf reiserfsck $(1)/usr/sbin/fsck.reiserfs
endef

$(eval $(call BuildPackage,reiserfsprogs))
