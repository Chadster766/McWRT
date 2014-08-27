#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=vncrepeater
PKG_VERSION:=0.12
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.rit.edu/~reh5586/openwrt/packages/vncrepeater/src
PKG_MD5SUM:=5951740a8a103d7d9668558b3639c6c4

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk

define Package/vncrepeater
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=$(CXX_DEPENDS)
  TITLE:=UltraVNC repeater for Linux
  URL:=http://www.uvnc.com/addons/repeater.html
endef

define Package/vncrepeater/conffiles
/etc/vncrepeater.conf
endef

define Package/vncrepeater/description
 Viewer can be behind Nat router or directly connected to the internet 
 instead of forwarding serveral ports, you only need to forward 1 port. 
 If the PC that runs the Repeater has access to the local DNS server, 
 you can use your local DNS names instead of 10.10.10.12.
 This could be handy when you have a dynamic DHCP server allocating 
 ip adresses for your PC.
endef

define Build/Compile
	$(call Build/Compile/Default, \
		CC="$(TARGET_CXX)" \
		CFLAGS="$(TARGET_CPPFLAGS) -fno-rtti" \
		repeater \
	)
endef

define Package/vncrepeater/install	
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc
	$(CP) $(PKG_BUILD_DIR)/repeater $(1)/usr/sbin/$(PKG_NAME)
	$(INSTALL_DIR) $(1)/etc
	$(CP) $(PKG_BUILD_DIR)/uvncrepeater.ini $(1)/etc/vncrepeater.conf
endef

$(eval $(call BuildPackage,vncrepeater))
