#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=etherpuppet
PKG_REV:=90decf83d1d4
PKG_VERSION:=0.3_r$(PKG_REV)
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://hg.secdev.org/etherpuppet
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_PROTO:=hg

include $(INCLUDE_DIR)/package.mk

define Package/etherpuppet
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+kmod-tun
  TITLE:=A voodoo doll for an Ethernet interface
  URL:=http://www.secdev.org/projects/etherpuppet/
  SUBMENU:=VPN
endef

define Package/etherpuppet/description
  Etherpuppet is a small program for Linux that will create a virtual interface
  (TUN/TAP) on one machine from the ethernet interface of another machine
  through a TCP connection.
endef

define Build/Configure
endef

define Build/Compile
  $(TARGET_CC) $(TARGET_CFLAGS) $(PKG_BUILD_DIR)/$(PKG_NAME).c -o $(PKG_BUILD_DIR)/$(PKG_NAME)
endef

define Package/etherpuppet/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/bin/
endef

$(eval $(call BuildPackage,etherpuppet))
