#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=deco
PKG_VERSION:=39
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)$(PKG_VERSION).tgz
PKG_SOURCE_URL:=@SF/deco
PKG_MD5SUM:=f77f60e8be0cae1f814cba1ef61bf4d0

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/deco
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libncurses
  TITLE:=Text-based, full featured file manager
  URL:=http://deco.sourceforge.net/
  SUBMENU:=filemanager
endef

define Package/deco/description
	A clone of Norton Commander for Unix. Text-based, full featured 
	file manager.
endef
  
define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)
endef

define Package/deco/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/deco $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib/deco
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/menu $(1)/usr/lib/deco/menu
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/profile $(1)/usr/lib/deco/profile
endef

$(eval $(call BuildPackage,deco))
