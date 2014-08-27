#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=vgp
PKG_VERSION:=0.2.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://vgpd.freaknet.org/files/
PKG_MD5SUM:=b8c25b5dfcb944f78bbc584be9c230c7

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/vgp
  SECTION:=net
  CATEGORY:=Network
  TITLE:=A daemon working like Cisco(c) HSRP
  URL:=http://vgpd.freaknet.org/
endef

define Package/vgp/description
	VGP (Virtual Gateway Protocol)is a very simple protocol able to 
	work in a way similar to Cisco(c) HSRP. It implements the idea 
	of Virtual Gateway to provide fault tollerance (and load 
	balancing) on your net. Each client on your net has as its 
	default gateway set to the virtual gateway and not the real router.
endef

# uses GNU configure

define Package/vgp/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/vgpd $(1)/usr/bin/
endef

$(eval $(call BuildPackage,vgp))
