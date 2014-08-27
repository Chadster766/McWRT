#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libdlna
PKG_VERSION:=0.2.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://libdlna.geexbox.org/releases/
PKG_MD5SUM:=2c974f95b711e5fd07f78fc4ebfcca66

PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libdlna
 SECTION:=libs
 CATEGORY:=Libraries
 TITLE:= Reference DLNA implementation
 URL:=http://libdlna.geexbox.org/
 DEPENDS:=+libffmpeg
endef

define Package/libdlna/description
 libdlna aims at being the reference open-source implementation of DLNA 
 (Digital Living Network Alliance) standards. Its primary goal is to 
 provide DLNA support to uShare, an embedded DLNA & UPnP A/V Media Server, 
 but it will be used to build both DLNA servers and players in the long 
 term.
endef

define Build/Configure
	# this is *NOT* GNU configure
	( cd $(PKG_BUILD_DIR); \
		CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS) -Wl,-rpath-link=$(STAGING_DIR)/usr/lib -lpthread" \
		HOST_CC="$(HOSTCC)" \
		./configure \
		--cross-compile \
		--cross-prefix=$(TARGET_CROSS) \
		--prefix=/usr \
		--enable-shared \
		--enable-static \
		--disable-debug \
		--disable-optimize \
		--disable-strip \
	)
endef

# XXX: fix build on systems where install is not at its usual location
# INSTALL defaults to /usr/bin/install
MAKE_INSTALL_FLAGS += \
	INSTALL="install" \

TARGET_CFLAGS += $(FPIC)

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/dlna.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libdlna.{a,so*} $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libdlna.pc $(1)/usr/lib/pkgconfig/
endef

define Package/libdlna/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libdlna.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libdlna))
