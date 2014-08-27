#
# Copyright (C) 2009-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libpar2
PKG_VERSION:=0.2
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/parchive
PKG_MD5SUM:=94c6df4e38efe08056ecde2a04e0be91

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libpar2
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+libsigcxx
  TITLE:=A library for performing common tasks related to PAR recovery sets
  URL:=http://parchive.sourceforge.net/
endef

define Package/libpar2/description
A library for performing common tasks related to PAR recovery sets
endef

CONFIGURE_ARGS += \
	--disable-static \
	--disable-nls \

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/libpar2
	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/include/libpar2/*.h \
		$(1)/usr/include/libpar2/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libpar2.{la,so*} \
		$(1)/usr/lib/
endef

define Package/libpar2/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libpar2.so* \
		$(1)/usr/lib/
endef

$(eval $(call BuildPackage,libpar2))
