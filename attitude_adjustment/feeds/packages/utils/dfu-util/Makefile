#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dfu-util
PKG_VERSION:=r3095
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://sources.nslu2-linux.org/sources
PKG_MD5SUM:=2346dddceeab0e7090109f04d9c3a6be

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/dfu-util
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=OpenMoko device flash utility
  URL:=http://wiki.openmoko.org/wiki/Dfu-util
  DEPENDS:=+libusb
endef

define Package/dfu-util/description
	Dfu-util is a program that implements the host side of the USB DFU
	(Universal Serial Bus Device Firmware Upgrade) protocol.
	This version is specifically used to flash the FIC Neo 1973 mobile phone.
endef

# CONFIGURE_VARS += USB_CFLAGS="-I $(STAGING_DIR)/usr/include" \
# 		  USB_LIBS="-L $(STAGING_DIR)/usr/lib"

define Build/Configure
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status}; \
		./autogen.sh \
	);
	$(call Build/Configure/Default)
endef

define Package/dfu-util/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/dfu-util $(1)/usr/bin/
endef

$(eval $(call BuildPackage,dfu-util))
