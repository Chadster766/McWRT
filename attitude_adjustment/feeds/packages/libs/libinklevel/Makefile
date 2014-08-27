#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libinklevel
PKG_VERSION:=0.8.0
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/libinklevel
PKG_MD5SUM:=83464cb23fe46a1d1adbe10f08b247be

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libinklevel
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=$(if $(WITH_PARPORT), +libieee1284)
  TITLE:=Printers Ink Level Library
  URL:=http://libinklevel.sourceforge.net/
endef

define Package/libinklevel/description
  Libinklevel is a library for checking the ink level of your printer on a system which runs Linux.
endef

TARGET_CFLAGS += $(FPIC)
TARGET_LIBS += $(if $(WITH_PARPORT), -lieee1284)
MAKE_ARGS += LIBS="$(TARGET_LIBS)"

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/include/inklevel.h \
		$(1)/usr/include/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libinklevel.so* \
		$(1)/usr/lib/
endef

define Package/libinklevel/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libinklevel.so* \
		$(1)/usr/lib/
endef

$(eval $(call BuildPackage,libinklevel))
