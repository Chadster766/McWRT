#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gnokii
PKG_VERSION:=0.6.21
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://gnokii.org/download/gnokii/0.6.x
PKG_MD5SUM:=bbf0aabe2436d8e927f3e2edb4efcac8

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=0

include $(INCLUDE_DIR)/package.mk

define Package/gnokii
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libusb
  TITLE:=Gnokii - Datasuite for the mobile phones
  URL:=http://www.gnokii.org/
endef

define Package/gnokii/description
	Gnokii is a suite of programs linked against a backend
	library that allows communication with the phones. It
	currently supports the most mobile phones, all AT
	capable mobiles as well as Symbian phones.
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--without-libiconv-prefix \
		--without-libintl-prefix \
		--disable-rlpdebug \
		--disable-xdebug \
		--disable-debug \
		--enable-shared \
		--disable-static \
		--disable-libpcsclite \
		--disable-libical \
		--disable-irda \
		--disable-bluetooth \
		--without-x \
		)
endef

define Package/gnokii/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/gnokii $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/gnokiid $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/mgnokiidev $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgnokii.so.3.5.0 $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgnokii.so.3 $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgnokii.so $(1)/usr/lib
endef

$(eval $(call BuildPackage,gnokii))
