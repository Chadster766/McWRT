#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=clinkc
PKG_VERSION:=101
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/clinkc/
PKG_MD5SUM:=4c8ac54a15da47203a86daf77fbc2664

PKG_BUILD_DIR:=$(BUILD_DIR)/CyberLinkC

PKG_BUILD_DEPENDS:=libexpat
PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libclinkc
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+libexpat
  TITLE:=Embedded UPnP library
  URL:=http://sourceforge.net/projects/clinkc
endef

define Package/libclinkc/description
CyberLink for C is a UPnP library using C for small and embedded platforms.
endef

# uses GNU configure

TARGET_CFLAGS += $(FPIC)

define Build/Compile
	chmod +x $(PKG_BUILD_DIR)/config/install-sh
	$(call Build/Compile/Default)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/cybergarage $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libclinkc.a $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libclinkc))
