#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dtach
PKG_VERSION:=0.8
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/dtach
PKG_MD5SUM:=ec5999f3b6bb67da19754fcb2e5221f3

include $(INCLUDE_DIR)/package.mk

define Package/dtach
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=A program that emulates the dtach feature of screen
  URL:=http://dtach.sourceforge.net/
endef

define Package/dtach/description
	dtach is a tiny program that emulates the detach feature of screen,
	allowing you to run a program in an environment that is protected from
	the controlling terminal and attach to it later. dtach does not keep
	track of the contents of the screen, and thus works best with programs
	that know how to redraw themselves. 
endef

define Build/Configure	
	$(call Build/Configure/Default)
endef

define Build/Compile	
	$(MAKE) -C $(PKG_BUILD_DIR)
endef

define Package/dtach/install	
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dtach $(1)/usr/bin/
endef

$(eval $(call BuildPackage,dtach))
