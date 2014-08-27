# 
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=icmptx
PKG_REV:=33e0b826f99df290ee792f6aa587af273d674309
PKG_VERSION:=0.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://github.com/jakkarth/icmptx.git
PKG_SOURCE_VERSION:=$(PKG_REV)

include $(INCLUDE_DIR)/package.mk

define Package/icmptx
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Firewall Tunnel
  TITLE:=IP-over-ICMP tunnel 
  URL:=http://github.com/jakkarth/icmptx
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		OPT_FLAGS="$(TARGET_CFLAGS)" \
		icmptx
endef

define Package/icmptx/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/icmptx $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,icmptx))
