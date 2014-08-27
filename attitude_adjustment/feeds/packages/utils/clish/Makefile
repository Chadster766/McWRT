#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=clish
PKG_VERSION:=0.7.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=81e20f7a888bcd8e2280e37804d342e4

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/clish
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libstdcpp +libpthread
  TITLE:=Command Line Interface SHell
  URL:=http://clish.sourceforge.net/
endef

define Package/clish/description
 A modular framework for implementing a CISCO-like CLI on a *NIX system.
 Arbitary command menus and actions can be defined using XML files.
 This software handles the user interaction,
 and forks the appropriate system commands to perform any actions.
endef

CONFIGURE_VARS += \
	ac_cv_header_bfd_h=no

MAKE_FLAGS += \
	bin_PROGRAMS=bin/clish

define Package/clish/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/* $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/etc/clish
	$(CP) $(PKG_BUILD_DIR)/xml-examples/*.xml $(1)/etc/clish/
endef

$(eval $(call BuildPackage,clish))
