#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libnfsidmap
PKG_VERSION:=0.20
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.citi.umich.edu/projects/nfsv4/linux/libnfsidmap/
PKG_MD5SUM:=9233cb77876eb642374a0d2bcaba1170

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/libnfsidmap
  SECTION:=libs
  CATEGORY:=Libraries
  SUBMENU:=Filesystem
  TITLE:=libnfsidmap, for compiling mount.nfs4
  URL:=http://www.citi.umich.edu/projects/nfsv4
endef

define Package/libnfsidmap/description
 needed for nfs-utils
endef

TARGET_CFLAGS += $(FPIC)

TARGET_CFLAGS += -I$(PKG_BUILD_DIR)/lib

MAKE_FLAGS += \
	OPT="$(TARGET_CFLAGS)" \
	INSTALLSUID="install -m 4755" \
	DESTDIR="$(PKG_INSTALL_DIR)" \
	all install

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/nfsidmap.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnfsidmap.{a,so*} $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libnfsidmap.pc $(1)/usr/lib/pkgconfig
endef

$(eval $(call BuildPackage,libnfsidmap))
