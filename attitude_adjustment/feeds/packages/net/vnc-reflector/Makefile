#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=vnc-reflector
PKG_VERSION:=1.2.4
PKG_RELEASE:=1

PKG_SOURCE:=vnc_reflector-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/vnc-reflector
PKG_MD5SUM:=c3f88bc62f228b335c25c07f9744ab0c

PKG_BUILD_DIR:=$(BUILD_DIR)/vnc_reflector

include $(INCLUDE_DIR)/package.mk

define Package/vnc-reflector
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libjpeg +zlib
  TITLE:=VNC proxy for multiple clients
  URL:=http://sourceforge.net/projects/vnc-reflector
endef

define Package/vnc-reflector/description
	VNC Reflector is a specialized VNC server which acts as 
	a proxy sitting between real VNC server (a host) and a 
	number of VNC clients. It was designed to work efficiently 
	with large number of clients.
endef

define Build/Compile
	$(call Build/Compile/Default, \
		CC="$(TARGET_CC)" \
		OFLAGS="$(TARGET_CFLAGS)" \
		STAGING_DIR="$(STAGING_DIR)" \
		default \
	)
endef

define Package/vnc-reflector/install	
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/vncreflector $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,vnc-reflector))
