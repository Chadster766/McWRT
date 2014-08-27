#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libatomicops
PKG_VERSION:=1.2
PKG_EXTRA_VERSION:=+cvs20080819.orig
PKG_RELEASE:=2

PKG_SOURCE:=libatomic-ops_$(PKG_VERSION)$(PKG_EXTRA_VERSION).tar.gz
PKG_SOURCE_URL:=http://ftp.de.debian.org/debian/pool/main/liba/libatomic-ops/
PKG_MD5SUM:=bf26c062f28796e3daaab586a38abec1

PKG_BUILD_DIR:=$(BUILD_DIR)/libatomic_ops-$(PKG_VERSION)

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libatomicops
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=implementations for atomic memory update operations
  DEPENDS:=@!TARGET_avr32
  URL:=http://www.hpl.hp.com/research/linux/atomic_ops/
endef

define Package/libatomicops/description
  Provides implementations for atomic memory update
  operations on a number of architectures. This allows
  direct use of these in reasonably portable code. Unlike
  earlier similar packages, this one explicitly considers
  memory barrier semantics, and allows the construction of
  code that involves minimum overhead across a variety of
  architectures.
endef

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/src/atomic_ops $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libatomic_ops*.a $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libatomicops))
