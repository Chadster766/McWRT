#
# Copyright (C) 2007-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=qolyester
PKG_VERSION:=20090302
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://qolsr.lri.fr/code/
PKG_MD5SUM:=650286fc2ea3e13c246d233aaa46f2a4

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/qolyester
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libpthread +libstdcpp @!TARGET_avr32 @!powerpc
  TITLE:=OLSR implementation with QoS
  URL:=http://qolsr.lri.fr/code/
endef

define Package/qolyester/description
	This is an implementation from scratch of the OLSR protocol.
	It is written in the C++ programming language and its goals
	are modularity, efficiency and conformance to RFC 3626 (OLSR).
endef

# uses GNU configure

define Package/qolyester/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/qolsrd $(1)/usr/bin/
endef

$(eval $(call BuildPackage,qolyester))
