#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=matrixtunnel
PKG_VERSION:=0.2
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://znerol.ch/files
PKG_MD5SUM:=af169751efc7d87d500716a48d74ddc5

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

MAKE_PATH:=src

define Package/matrixtunnel
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=VPN
  DEPENDS:=+libmatrixssl
  TITLE:=SSL tunnel based on matrixssl
  URL:=http://znerol.ch/svn/matrixtunnel/trunk/
endef

define Package/matrixtunnel/description
	matrixtunnel is a stripped down version of stunnel 
	based on matrixssl library.
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--without-libiconv-prefix \
		--without-libintl-prefix \
		--with-matrixssl-src="$(BUILD_DIR)/matrixssl" \
	)
endef

define Package/matrixtunnel/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/matrixtunnel $(1)/usr/bin/
endef

$(eval $(call BuildPackage,matrixtunnel))
