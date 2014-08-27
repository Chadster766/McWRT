#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 10812 2008-04-13 12:06:07Z olli $
#
# http://ftp.gnu.org/gnu/parted/parted-1.8.8.tar.bz2

include $(TOPDIR)/rules.mk

PKG_NAME:=parted
PKG_VERSION:=3.1
PKG_RELEASE:=1

PKG_INSTALL:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@GNU/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/parted
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=parted Partition editor
  URL:=http://www.gnu.org/software/parted/index.shtml
  DEPENDS:=+libpopt
endef

define Package/parted/description
	parted Partition editor
	http://www.gnu.org/software/parted/index.shtml
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--without-readline \
		--disable-device-mapper \
		--disable-nls \
	)
endef

define Package/parted/install	
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP)          $(PKG_INSTALL_DIR)/usr/lib/*.so*      $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/parted    $(1)/usr/sbin	
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/partprobe $(1)/usr/sbin
endef

$(eval $(call BuildPackage,parted))
