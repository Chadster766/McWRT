# 
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=eggdrop
PKG_VERSION:=1.6.17
PKG_RELEASE:=1

PKG_SOURCE:=eggdrop$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://ftp.eggheads.org/pub/eggdrop/source/1.6
PKG_MD5SUM:=68e181201483dee398f9904e8652e6ca

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)$(PKG_VERSION)
PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/eggdrop
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=EggDrop IRC BOT
  DEPENDS:=+tcl +zlib
  URL:=http://www.eggheads.org
endef

define Package/eggdrop/description
	EggDrop IRC BOT
endef

CONFIGURE_ARGS+=\
	--prefix="$(PKG_INSTALL_DIR)/eggdrop" \
	--enable-threads \
	--with-tcllib="$(STAGING_DIR)/usr/lib/libtcl8.5.so" \
	--with-tclinc="$(STAGING_DIR)/usr/include/tcl.h"

define Build/Compile
	$(MAKE) config -C $(PKG_BUILD_DIR)
	$(call Build/Compile/Default)
endef

define Build/Install
	$(call Build/Install/Default)
	rm -Rvf $(PKG_INSTALL_DIR)/eggdrop/doc
endef

define Package/eggdrop/install
	$(INSTALL_DIR) $(1)/eggdrop
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/eggdrop
endef

$(eval $(call BuildPackage,eggdrop))
