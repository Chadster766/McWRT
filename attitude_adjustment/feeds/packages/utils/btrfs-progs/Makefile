#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=btrfs-progs
PKG_VERSION:=0.19
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.kernel.org/pub/linux/kernel/people/mason/btrfs/
PKG_MD5SUM:=69e29ecd922e3f9dcb74a3a8f80b4f68

PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/btrfs-progs
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Filesystem
  DEPENDS:=+libuuid +zlib
  TITLE:=Btrfs filesystems utilities
  URL:=http://btrfs.wiki.kernel.org/
endef

define Package/btrfs-progs/description
 Btrfs is a new copy on write filesystem for Linux aimed at implementing
 advanced features while focusing on fault tolerance, repair and easy
 administration. Initially developed by Oracle, Btrfs is licensed under the
 GPL and open for contribution from anyone. 
endef

progs = btrfsctl mkfs.btrfs btrfs-debug-tree btrfs-show btrfs-vol btrfsck

MAKE_FLAGS+=\
	CC="$(TARGET_CC)" \
	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)" \
	prefix=/usr \
	DESTDIR=$(PKG_INSTALL_DIR)

define Package/btrfs-progs/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(addprefix $(PKG_INSTALL_DIR)/usr/bin/, $(progs)) $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/btrfs-scan.init $(1)/etc/init.d/btrfs-scan
endef

$(eval $(call BuildPackage,btrfs-progs))
