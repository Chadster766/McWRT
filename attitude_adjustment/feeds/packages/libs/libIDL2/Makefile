#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#


include $(TOPDIR)/rules.mk

PKG_NAME:=libIDL2
PKG_VERSION:=0.8.13
PKG_RELEASE:=1

PKG_SOURCE:=libIDL-$(PKG_VERSION).tar.bz2
PKG_BUILD_DIR:=$(BUILD_DIR)/libIDL-$(PKG_VERSION)
HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/libIDL-$(PKG_VERSION)
PKG_SOURCE_URL:=@GNOME/libIDL/0.8
PKG_MD5SUM:=b43b289a859eb38a710f70622c46e571
PKG_FIXUP:=autoreconf
HOST_BUILD_DEPENDS:=glib2/host
PKG_INSTALL=1

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/libIDL2
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=IDL Compiling Library
  URL:=http://andrewtv.org/libIDL/
  DEPENDS:=+glib2
endef

define Package/libIDL2/description
  libIDL is a library licensed under the GNU LGPL for creating trees of
  CORBA Interface Definition Language (IDL) files, which is a
  specification for defining portable interfaces.
endef

define Build/Configure
	$(call Build/Configure/Default,libIDL_cv_long_long_format=ll)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/*.{so*,a,la} \
		$(1)/usr/lib/

	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/* \
		$(1)/usr/lib/pkgconfig/

	$(INSTALL_DIR) $(1)/usr/include
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/* \
		$(1)/usr/include/
endef

define Package/libIDL2/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/*.so* \
		$(1)/usr/lib/
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage,libIDL2))
