#
# Copyright (C) 2010-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#


include $(TOPDIR)/rules.mk

PKG_NAME:=spandsp
PKG_VERSION:=0.0.6
PKG_VERSION_SUFFIX:=pre17
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)$(PKG_VERSION_SUFFIX).tgz
PKG_SOURCE_URL:=http://www.soft-switch.org/downloads/spandsp/
PKG_MD5SUM:=cc860f77270cf3b881f8ccd42d2d0238

PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1
PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/libspandsp
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=spandsp library
  DEPENDS:=+libtiff
endef

TARGET_CFLAGS += $(FPIC)

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/{lib,include}
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libspandsp* $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/
endef

define Package/libspandsp/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libspandsp*so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libspandsp))
