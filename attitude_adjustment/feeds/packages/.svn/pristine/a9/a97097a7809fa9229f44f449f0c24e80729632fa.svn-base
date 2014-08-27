#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=id3lib
PKG_VERSION:=3.8.3
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/id3lib
PKG_MD5SUM:=19f27ddd2dda4b2d26a559a4f0f402a7

PKG_FIXUP:=libtool-ucxx
PKG_INSTALL:=1

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk

define Package/id3lib
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+zlib $(CXX_DEPENDS)
  TITLE:=An ID3v1/ID3v2 tag manipulation library
  URL:=http://www.id3lib.org/
endef

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/id3 $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/id3.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libid3.{a,so} $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libid3-*.so.* $(1)/usr/lib/
endef

define Package/id3lib/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libid3-*.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,id3lib))
