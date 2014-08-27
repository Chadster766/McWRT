# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
#
# Sample usage:
#   env LD_PRELOAD="/usr/lib/libptmalloc3.so" <application>

include $(TOPDIR)/rules.mk

PKG_NAME:=libptmalloc3
PKG_VERSION:=20060531
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/ptmalloc3

PKG_SOURCE:=ptmalloc3-current.tar.gz
PKG_SOURCE_URL:=http://www.malloc.de/malloc/
PKG_MD5SUM:=c0b9dd5f16f8eae979166dc74b60015c

include $(INCLUDE_DIR)/package.mk

define Package/libptmalloc3
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Efficient malloc implementation
  URL:=http://www.malloc.de/en/
endef

# flags: taken from ptmalloc3/Makefile by combining
# 'linux-shared' and 'linux-nothreads' targets
define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		SYS_FLAGS="$(TARGET_CFLAGS)" \
		linux-shared-nothreads
endef

define Package/libptmalloc3/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libptmalloc3.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libptmalloc3))
