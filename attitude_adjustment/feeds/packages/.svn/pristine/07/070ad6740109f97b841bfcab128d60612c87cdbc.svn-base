#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=firmwarehotplug
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/linux-hotplug
PKG_MD5SUM:=e5cf352b172f498e27b160167b981f24

PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

# workaround for fedora sdcc naming changes
ifneq ($(wildcard /usr/libexec/sdcc),)
  export PATH:=$(PATH):/usr/libexec
endif

define Package/firmwarehotplug
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@USB_SUPPORT +fxload
  TITLE:=Hotplug firmware for EZ-USB microcontrollers
  URL:=http://linux-hotplug.sourceforge.net/
endef

MAKE_PATH:=ezusb/midi/original

MAKE_FLAGS+=\
	OPT="$(TARGET_CFLAGS)"

define Package/firmwarehotplug/install
	$(INSTALL_DIR) $(1)/etc/hotplug.d/usb
	install -m755 $(PKG_BUILD_DIR)/ezusb/midi/original/Hotplug/ezusbmidi $(1)/etc/hotplug.d/usb
	install -m644 $(PKG_BUILD_DIR)/ezusb/midi/original/Hotplug/ezusbmidi.usermap $(1)/etc/hotplug.d
	$(INSTALL_DIR) $(1)/usr/share/usb/ezusbmidi
	install -m644 $(PKG_BUILD_DIR)/ezusb/midi/original/ezusbmidi1x1.ihx $(1)/usr/share/usb/ezusbmidi
	install -m644 $(PKG_BUILD_DIR)/ezusb/midi/original/ezusbmidi2x2.ihx $(1)/usr/share/usb/ezusbmidi
endef

$(eval $(call BuildPackage,firmwarehotplug))

