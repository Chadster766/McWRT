#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=fxload
PKG_VERSION:=2008_10_13
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/linux-hotplug
PKG_MD5SUM:=4477a2457f064228bef4a93ba2f21692

include $(INCLUDE_DIR)/package.mk

define Package/fxload
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@USB_SUPPORT +udev
  TITLE:=Downloading firmware into EZ-USB microcontrollers
  URL:=http://linux-hotplug.sourceforge.net/
endef

define Build/Compile
	$(call Build/Compile/Default, \
		CFLAGS="$(TARGET_CFLAGS) -I$(LINUX_DIR)/include" \
	)
endef

define Package/fxload/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/sbin/
	$(INSTALL_DIR) $(1)/usr/share/usb
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/a3load.hex $(1)/usr/share/usb/
endef

$(eval $(call BuildPackage,fxload))
