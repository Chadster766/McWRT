#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wrt-radauth
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.zerfleddert.de/wrt54g/
PKG_MD5SUM:=1713b96a157f9e209cf7986f38ef57d9

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/wrt-radauth
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+kmod-brcm-wl +nvram
  TITLE:=A Radius MAC authenticator for Broadcom based access points
  URL:=http://www.zerfleddert.de/wrt54g/
endef

define Package/wrt-radauth/description
	This software watches the stations associating to a broadcom based 
	AccessPoint (e.g. Linksys WRT54G) and sends an authentication request 
	to the radius-server specified in nvram.
endef
  
define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) clean
	$(call Build/Compile/Default, \
		CFLAGS="$(EXTRA_CFLAGS)" \
		LDFLAGS="$(EXTRA_LDFLAGS)" \
	)
endef

define Package/wrt-radauth/install	
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/radius-client $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wrt-radauth $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,wrt-radauth))
