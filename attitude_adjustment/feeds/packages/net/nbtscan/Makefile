# 
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nbtscan
PKG_VERSION:=1.5.1

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/n/nbtscan/
PKG_MD5SUM:=85bb085077c380b82a6ff73e0de0c154

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)a

include $(INCLUDE_DIR)/package.mk

define Package/nbtscan
  SECTION:=net
  CATEGORY:=Network
  TITLE:=NBTscan is a program for scanning IP networks for NetBIOS name information
  URL:=http://www.inetcat.net/software/nbtscan.html
endef

define Package/nbtscan/description
	NBTscan is a program for scanning IP networks for NetBIOS name information.
	It sends NetBIOS status query to each address in supplied range and lists
	received information in human readable form. For each responded host it lists
	IP address, NetBIOS computer name, logged-in user name and MAC address.
endef

CONFIGURE_ARGS += \
	--enable-static \
	--enable-shared

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)"
endef


define Package/nbtscan/install
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/nbtscan $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,nbtscan))
