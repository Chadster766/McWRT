#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=ink
PKG_VERSION:=0.5.1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/ink
PKG_MD5SUM:=df09c4a1a404dd37ffff80ce2491ab6d

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/ink
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=A utility for reading ink levels
  URL:=http://ink.sourceforge.net/
  DEPENDS:=+libinklevel
endef

define Package/ink/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/ink $(1)/usr/bin/
endef

$(eval $(call BuildPackage,ink))
