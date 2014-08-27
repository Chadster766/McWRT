#
# Copyright (C) 2011-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libmbus
PKG_VERSION:=0.7.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.freescada.com/public-dist/
PKG_MD5SUM:=2f1a7ba0ccd683e6708364a13310a6d1

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libmbus/Default
  SECTION:=utils
  CATEGORY:=Utilities
  URL:=http://www.freescada.com/libmbus/
  TITLE:=mbus-tools
endef

define Package/libmbus/Default/description
  libmbus is an open source M-bus (Meter-Bus) library.
  The Meter-Bus is a standard for reading out meter data from
  electricity meters, heat meters, gas meters, etc.
endef

define Package/libmbus
  $(call Package/libmbus/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=libmbus
endef

define Package/libmbus/description
  $(call Package/libmbus/Default/description)

  This package contains the M-bus shared library, used by other programs.
endef

define Package/mbus-serial
  $(call Package/libmbus/Default)
  DEPENDS+=+libmbus
  TITLE+= (serial)
endef

define Package/mbus-serial/description
  $(call Package/libmbus/Default/description)

  This package contains command line tools for scanning the M-bus
  and retrieving data from meters which are connected through a
  serial (e.g. RS232) interface.
endef

define Package/mbus-tcp
  $(call Package/libmbus/Default)
  DEPENDS+=+libmbus
  TITLE+= (tcp)
endef

define Package/mbus-tcp/description
  $(call Package/libmbus/Default/description)

  This package contains command line tools for scanning the M-bus
  and retrieving data from meters which are accessible through
  M-bus gateways via TCP.
endef

TARGET_CFLAGS += $(FPIC)

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/mbus $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmbus.{so*,la} $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libmbus.pc $(1)/usr/lib/pkgconfig/
endef

define Package/libmbus/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmbus.so* $(1)/usr/lib/
endef

define Package/mbus-serial/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/mbus-serial-* $(1)/usr/bin/
endef

define Package/mbus-tcp/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/mbus-tcp-* $(1)/usr/bin/
endef

$(eval $(call BuildPackage,libmbus))
$(eval $(call BuildPackage,mbus-serial))
$(eval $(call BuildPackage,mbus-tcp))
