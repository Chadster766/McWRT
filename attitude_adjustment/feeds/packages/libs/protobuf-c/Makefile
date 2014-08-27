#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=protobuf-c
PKG_VERSION:=0.15
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://protobuf-c.googlecode.com/files
PKG_MD5SUM:=73ff0c8df50d2eee75269ad8f8c07dc8

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libprotobuf-c
  TITLE:=Protocol Buffers library
  SECTION:=libs
  CATEGORY:=Libraries
  URL:=http://code.google.com/p/protobuf-c/
endef

define Package/libprotobuf-c/description
  Runtime library to use Google Protocol Buffers from C applications.
  Protocol Buffers are a way of encoding structured data in an efficient yet
  extensible format. Google uses Protocol Buffers for almost all of its
  internal RPC protocols and file formats.
endef

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--disable-protoc

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/google/protobuf-c
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/google/protobuf-c/* \
		$(1)/usr/include/google/protobuf-c/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libprotobuf-c.{a,la,so*} $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/* $(1)/usr/lib/pkgconfig/
endef

define Package/libprotobuf-c/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libprotobuf-c.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libprotobuf-c))
