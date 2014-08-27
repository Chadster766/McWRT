#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tmsnc
PKG_VERSION:=0.3.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/tmsnc
PKG_MD5SUM:=337dae91d4775e03b6681f61db56d2a7

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/tmsnc
  SUBMENU:=Instant Messaging
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libncurses +libopenssl +libpthread
  TITLE:=text-based MSN client
  URL:=http://tmsnc.sourceforge.net
endef

define Package/tmsnc/description
	TMSNC is a textbased (console) MSN client written in C.
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--disable-rpath \
		--with-openssl="$(STAGING_DIR)/usr" \
		--with-ncurses="$(STAGING_DIR)/usr" \
		--without-libiconv-prefix \
	)
endef

define Package/tmsnc/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/tmsnc $(1)/usr/bin/
endef

$(eval $(call BuildPackage,tmsnc))
