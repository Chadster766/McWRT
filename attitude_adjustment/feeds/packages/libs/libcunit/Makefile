# 
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=libcunit
PKG_VERSION:=2.1-0
PKG_RELEASE:=1

PKG_SOURCE:=CUnit-$(PKG_VERSION)-src.tar.gz
PKG_SOURCE_URL:=http://downloads.sourceforge.net/project/cunit/CUnit/$(PKG_VERSION)
PKG_MD5SUM:=f4f53d2c41e9315a91b4f14a1706fe4f
PKG_BUILD_DIR:=$(BUILD_DIR)/CUnit-$(PKG_VERSION)

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/libcunit
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Unit testing framework for C
  URL:=http://cunit.sourceforge.net/
endef

define Package/libcunit/description
  CUnit, lightweight unit testing for C
  CUnit is a lightweight system for writing, administering, and running unit
  tests in C.  It provides C programmers a basic testing functionality with a
  flexible variety of user interfaces.
endef


define Build/Configure
	$(call Build/Configure/Default, \
		, \
		BUILD_CC="$(TARGET_CC)" \
		HOSTCC="$(HOSTCC)" \
	)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CCOPT="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
endef


define Build/InstallDev
	mkdir -p $(1)/usr/include/CUnit
	$(CP)	$(PKG_INSTALL_DIR)/usr/include/CUnit/* \
		$(1)/usr/include/CUnit

	mkdir -p $(1)/usr/lib
	$(CP)	$(PKG_INSTALL_DIR)/usr/lib/libcunit.* \
		$(1)/usr/lib
endef


define Package/libcunit/install
	$(INSTALL_DIR)	$(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcunit.* $(1)/usr/lib
endef


$(eval $(call BuildPackage,libcunit))
