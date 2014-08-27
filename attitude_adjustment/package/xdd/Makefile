#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=xdd65.013007
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME).tgz
PKG_SOURCE_URL:=http://openbsd.mirrors.tds.net/pub/FreeBSD/distfiles

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/xdd
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=xdd benchmark
  URL:=http://openbsd.mirrors.tds.net/pub/FreeBSD/distfiles
  DEPENDS:=+libpthread
endef

define Package/xdd/description
	Tool for testing perfomance
endef

define Package/xdd/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/xdd $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/timeserver $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/gettime $(1)/usr/bin/
endef

$(eval $(call BuildPackage,xdd))
