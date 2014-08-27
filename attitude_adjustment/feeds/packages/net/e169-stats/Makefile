#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=e169-stats
PKG_VERSION:=0.6
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.mavetju.org/download
PKG_MD5SUM:=2f6f5b984faeec368a867e59cd2cc36d

include $(INCLUDE_DIR)/package.mk

define Package/e169-stats
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Huawei USB modem statistics
  URL:=http://www.mavetju.org/weblog/html/00282.html
  DEPENDS:=+libncurses
endef

define Package/e169-stats/description
  Huawei E169 statistics, works with other Huawei's devices.
endef

define Package/e169-stats/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/e169-stats $(1)/usr/bin/
endef

$(eval $(call BuildPackage,e169-stats))
