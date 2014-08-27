#
# Copyright (C) 2006-2011 OpenWrt.org
# Copyright (C) 2011 SMBPhone Inc.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=newt
PKG_VERSION:=0.52.12
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://fedorahosted.org/releases/n/e/newt/
PKG_MD5SUM:=51b04128d9e1bf000fa769c417b74486

include $(INCLUDE_DIR)/package.mk

define Package/libnewt
  URL:=http://fedorahosted.org/newt/
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Not Erik's Windowing Toolkit
  DEPENDS:= +libslang2 +libpopt
endef

define Package/libnewt/description
  Programming library for colour text mode widget-based user interfaces,
  based on S-Lang.
endef

define Package/whiptail
  URL:=http://fedorahosted.org/newt/
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Dialog boxes from shell scripts
  DEPENDS:= +libnewt
endef

define Package/whiptail/description
  A lightweight replacement for the dialog command (dialog boxes from shell
  scripts), based on libnewt.
endef

TARGET_CFLAGS += $(FPIC) -I$(STAGING_DIR)/usr/include -L$(STAGING_DIR)/usr/lib

CONFIGURE_ARGS += \
	--disable-nls \
	--without-tcl \
	--without-gpm-support

MAKE_FLAGS += AR="$(TARGET_CROSS)ar"

define Package/libnewt/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libnewt.so* $(1)/usr/lib/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/newt.h $(1)/usr/include/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libnewt.{a,so*} $(1)/usr/lib/
endef

define Package/whiptail/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/whiptail $(1)/usr/bin/	
endef

$(eval $(call BuildPackage,libnewt))
$(eval $(call BuildPackage,whiptail))

