#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=shat
PKG_VERSION:=0.9.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/shat
PKG_MD5SUM:=da06846f35838b14d1513523dad84d24

include $(INCLUDE_DIR)/package.mk

define Package/shat
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Source Hardware Address Translation
  URL:=http://sourceforge.net/projects/shat/
endef

define Package/shat/description
	A tool which allows hosts with arbitrary IP configuration
	to connect to the internet over a SHAT gateway.

endef

define Build/Compile	
	$(MAKE) -C $(PKG_BUILD_DIR)/src \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall -I$(PKG_BUILD_DIR)/include/linux/include -I$(PKG_BUILD_DIR)/include -DVERSION=\\\"$(PKG_VERSION)\\\" -DDROP_NETBIOS"
endef

define Package/shat/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/shatd $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/shatc $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,shat))
