#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=celt
PKG_VERSION:=0.7.0
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://downloads.us.xiph.org/releases/celt/
PKG_MD5SUM:=0bb72abec367f4ef12551f79dda11b23

PKG_BUILD_DEPENDS:=libogg

PKG_FIXUP:=autoreconf

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libcelt
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=CELT Codec
  URL:=http://www.celt-codec.org/
endef

define Package/libcelt/description
 The CELT codec is an experimental audio codec for use in low-delay speech and audio communication.
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--without-ogg \
	)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/celt
	$(CP) $(PKG_INSTALL_DIR)/usr/include/celt/celt*.h \
		$(1)/usr/include/celt
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* \
		$(1)/usr/lib/
endef

define Package/libcelt/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcelt.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libcelt))
