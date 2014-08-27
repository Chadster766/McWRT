#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sipsak
PKG_VERSION:=0.9.6
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://download.berlios.de/sipsak/ \
	http://ftp.iptel.org/pub/sipsak/
PKG_MD5SUM:=c4eb8e282902e75f4f040f09ea9d99d5

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/sipsak
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Telephony
  DEPENDS:=+libopenssl
  TITLE:=SIP (Session Initiation Protocol) stress and diagnostics utility
  URL:=http://www.sipsak.org/
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--disable-gnutls \
	)
endef

define Package/sipsak/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/sipsak $(1)/usr/bin/
endef

$(eval $(call BuildPackage,sipsak))
