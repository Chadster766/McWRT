# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=microcom
PKG_VERSION:=1.02
PKG_RELEASE:=1

PKG_SOURCE:=m102.tar.gz
PKG_SOURCE_URL:=http://microcom.port5.com/
PKG_MD5SUM:=c7817035dc41cb02e7cfb565cf9b7401

include $(INCLUDE_DIR)/package.mk

PKG_UNPACK:=zcat $(DL_DIR)/$(PKG_SOURCE) | $(TAR) -C $(PKG_BUILD_DIR) $(TAR_OPTIONS)

define Package/microcom
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=A serial terminal emulation program
  URL:=http://microcomlinux.homestead.com/
  SUBMENU:=Terminal
endef

define Package/microcom/description
	microcom is a minicom-like serial terminal emulator with scripting 
	support.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		microcom
endef

define Package/microcom/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/microcom $(1)/usr/bin/
endef

$(eval $(call BuildPackage,microcom))
