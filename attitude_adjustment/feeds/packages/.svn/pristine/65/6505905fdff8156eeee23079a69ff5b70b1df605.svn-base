#
# Copyright (C) 2007-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libmpcdec
PKG_VERSION:=1.2.6
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://files.musepack.net/source/
PKG_MD5SUM:=7f7a060e83b4278acf4b77d7a7b9d2c0
PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

PKG_INSTALL=1

define Package/libmpcdec
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Musepack decoder library
  URL:=http://www.musepack.net
endef

define Package/libmpcdec/description
	Musepack decoder library
endef

TARGET_CFLAGS += $(FPIC)

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/mpcdec/
	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/include/mpcdec/*.h \
		$(1)/usr/include/mpcdec/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/*.{so*,a,la} \
		$(1)/usr/lib/
endef

define Package/libmpcdec/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libmpcdec.so* \
		$(1)/usr/lib/
endef

$(eval $(call BuildPackage,libmpcdec))
