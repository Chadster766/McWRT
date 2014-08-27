#
# Copyright (C) 2010-2011 OpenWrt.org
# Copyright (C) 2010 Gianluigi Tiesi <sherpya@netfarm.it>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ntfsprogs
PKG_VERSION:=2.0.0
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/linux-ntfs
PKG_MD5SUM:=2c402b647bb7aeb1d3f8ce1cc354fd68

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1
PKG_BUILD_DEPENDS:=libgcrypt

include $(INCLUDE_DIR)/package.mk

define Package/ntfsprogs
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Filesystem
  TITLE:=Linux-NTFS
  URL:=http://www.linux-ntfs.org/
  MAINTAINER:=Gianluigi Tiesi <sherpya@netfarm.it>
endef

define Package/ntfsprogs/description
 Tools for managing NTFS volumes from Linux
endef

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--disable-crypto \
	--disable-gnome-vfs \
	--disable-ntfsmount

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/ntfs $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libntfs.{la,a,so*} $(1)/usr/lib/
endef

define Package/ntfsprogs/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libntfs.so.* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ntfsfix $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/* $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,ntfsprogs))
