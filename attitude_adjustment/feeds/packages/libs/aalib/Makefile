#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=aalib
PKG_VERSION:=1.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/aa-project
PKG_MD5SUM:=790434e0f2005fc95559ab3eab42e746

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libaa
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Ascii-Art rendering library
  URL:=http://aa-project.sourceforge.net/
  DEPENDS:=+libncurses
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--enable-shared \
		--enable-static \
		--without-x \
		--without-x11-driver \
		--without-slang-driver \
		--with-ncurses=$(STAGING_DIR)/usr \
	)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/aalib.h \
		$(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libaa.{a,so*} \
		$(1)/usr/lib/
endef

define Package/libaa/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libaa.so.* \
		$(1)/usr/lib/
endef

$(eval $(call BuildPackage,libaa))
