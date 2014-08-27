# 
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libpfm
PKG_VERSION:=3.6
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/perfmon2
PKG_MD5SUM:=5075fa45ddce4aa647b70e9f9446eacc

include $(INCLUDE_DIR)/package.mk

define Package/libpfm
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Perfmon2 Kernel performance monitoring interface
  URL:=http://perfmon2.sourceforge.net/
  DEPENDS:=@BROKEN
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		ARCH=cell \
		CC=$(TARGET_CROSS)gcc \
		AR=$(TARGET_CROSS)ar \
		lib
endef

define Build/InstallDev
	echo "Build/InstallDev"
	$(MAKE) -C $(PKG_BUILD_DIR)/include \
		ARCH=cell \
		DESTDIR=$(1) \
		INCDIR=/usr/include \
		CONFIG_PFMLIB_ARCH_POWERPC=y \
		install
#	$(MAKE) -C $(PKG_BUILD_DIR)/lib \
#		ARCH=cell \
#		CC=$(TARGET_CROSS)gcc \
#		DESTDIR=$(1) \
#		LIBDIR=/usr/lib \
#		CONFIG_PFMLIB_ARCH_POWERPC=y \
#		CONFIG_PFMLIB_CELL=y \
#		CONFIG_PFMLIB_SHARED=y \
#		install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP)	$(PKG_BUILD_DIR)/lib/libpfm* $(1)/usr/lib/
endef

define Package/libpfm/install
	echo "Package/libpfm/install"
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/lib/libpfm* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libpfm))
