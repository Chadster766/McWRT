# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=expat
PKG_VERSION:=2.0.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_MD5SUM:=ee8b492592568805593f81f8cdf2a04c
PKG_SOURCE_URL:=@SF/expat

PKG_FIXUP:=autoreconf
PKG_REMOVE_FILES:=conftools/libtool.m4

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk

define Package/libexpat
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=An XML parsing library
  URL:=http://expat.sourceforge.net/
endef

define Package/libexpat/description
 A fast, non-validating, stream-oriented XML parsing library.
endef


TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) DESTDIR="$(PKG_INSTALL_DIR)" install
endef

define Host/Install
	$(MAKE) -C $(HOST_BUILD_DIR) install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/expat{,_external}.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libexpat.{a,so*} $(1)/usr/lib/
endef

define Package/libexpat/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libexpat.so.* $(1)/usr/lib/
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage,libexpat))

