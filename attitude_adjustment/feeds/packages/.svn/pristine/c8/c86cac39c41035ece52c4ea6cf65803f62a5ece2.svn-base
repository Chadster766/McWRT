#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ptpd
PKG_VERSION:=2.1.0
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/ptpd
PKG_MD5SUM:=9748f031ed9ebb5c600de354dae53046

include $(INCLUDE_DIR)/package.mk

MAKE_PATH:=src

define Package/ptpd
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Time Synchronization
  TITLE:=Precision Time Protocol (PTP) daemon
  URL:=http://ptpd.sourceforge.net/
  DEPENDS:=+libpthread +librt
endef

define Package/ptpd/description
 The PTP daemon (PTPd) implements version 2 of the Precision Time Protocol (PTP)
 as defined by the IEEE 1588-2008 standard.
 PTP was developed to provide very precise time coordination of LAN connected
 computers.
endef

define Package/ptpd/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/ptpd2 $(1)/usr/sbin/ptpd
	$(INSTALL_BIN) ./files/ptpd.init $(1)/etc/init.d/ptpd
endef

$(eval $(call BuildPackage,ptpd))
