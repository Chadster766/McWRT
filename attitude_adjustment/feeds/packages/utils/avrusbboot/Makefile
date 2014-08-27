#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=avrusbboot
PKG_VERSION:=2006-06-25
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME).$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.fischl.de/avrusbboot/
PKG_MD5SUM:=3e2f5e2c257918410a66c3463f583fdf

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME).$(PKG_VERSION)

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk

define Package/avrusbboot
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libusb $(CXX_DEPENDS)
  TITLE:=USB bootloader for Atmel AVR controllers
  URL:=http://www.fischl.de/avrusbboot/
endef

define Package/avrusbboot/description
  AVRUSBBoot is a bootloader for the Atmel AVR controllers.
  It uses a firmware-only USB driver to transfer binary data
  from the PC to the flash of the controller. Once the AVR
  is flashed with the bootloader, no other ISP programmer is
  needed; then the microcontroller can be reprogrammed over USB.
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/software \
		CXX="$(TARGET_CXX)" \
		CPPFLAGS="$(TARGET_CPPFLAGS) -fno-rtti" \
		LFLAGS="$(TARGET_LDFLAGS) -lusb" \
		all
endef

define Package/avrusbboot/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/software/avrusbboot $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,avrusbboot))
