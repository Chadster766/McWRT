#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=powertop
PKG_VERSION:=1.11
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.lesswatts.org/projects/powertop/download/
PKG_MD5SUM:=3498f5983c683c3a57dce7379a722082

PKG_INSTALL:=1
PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/powertop
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=PowerTOP
  DEPENDS:=+libncurses
  URL:=http://www.lesswatts.org
endef

define Package/powertop/description
 PowerTOP is a Linux tool that helps you find those programs that are 
 misbehaving while your computer is idle.
endef

define Package/powertop/install
	$(CP) $(PKG_INSTALL_DIR)/* $(1)
endef

$(eval $(call BuildPackage,powertop))
