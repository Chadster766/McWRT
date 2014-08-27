#
# Copyright (C) 2009-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#


include $(TOPDIR)/rules.mk

PKG_NAME:=digitemp
PKG_VERSION:=3.6.0
PKG_RELEASE:=5

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.digitemp.com/software/linux
PKG_MD5SUM:=9be2e48db37920f21925ae6e88f83b84

include $(INCLUDE_DIR)/package.mk

define Package/digitemp/Default
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=DigiTemp is a simple to use program for reading values from 1-wire devices.
  URL:=http://www.digitemp.com
endef

define Package/digitemp/Default/description
	DigiTemp is a simple to use program for reading values from 1-wire devices.
	Its main use is for reading temperature sensors, but it also reads counters,
	and understands the 1-wire hubs with devices on different branches of the network.
endef

define Package/digitemp
$(call Package/digitemp/Default)
  TITLE += (serial version)
endef

define Package/digitemp/description
$(call Package/digitemp/Default/description)
	This package is build without USB support.
endef

define Package/digitemp-usb
$(call Package/digitemp/Default)
  TITLE += (USB version)
  DEPENDS += +libusb
endef

define Package/digitemp-usb/description
$(call Package/digitemp/Default/description)
	This package is build with USB support.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -DLINUX $(TARGET_CPPFLAGS) \
			-I./src -I./userial" \
		LIBS="$(TARGET_LDFLAGS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		ds9097 ds9097u
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -DLINUX $(TARGET_CPPFLAGS) \
			-I./src -I./userial -DOWUSB" \
		LIBS="$(TARGET_LDFLAGS) -lusb -lm -DOWUSB" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		clean ds2490
endef

define Package/digitemp/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(CP) $(PKG_BUILD_DIR)/digitemp_{DS9097,DS9097U} $(1)/usr/bin/
endef

define Package/digitemp-usb/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(CP) $(PKG_BUILD_DIR)/digitemp_DS2490 $(1)/usr/bin/
endef

$(eval $(call BuildPackage,digitemp))
$(eval $(call BuildPackage,digitemp-usb))
