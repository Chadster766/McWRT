#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=usbmuxd
PKG_VERSION:=1.0.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://marcansoft.com/uploads/usbmuxd
PKG_MD5SUM:=6f431541f3177fa06aa6df9ceecb2da0

CMAKE_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/usbmuxd/Default
  TITLE:=USB multiplexing daemon
  URL:=http://marcansoft.com/blog/iphonelinux/usbmuxd/
endef

define Package/usbmuxd/Default/description
  This daemon is in charge of multiplexing connections over USB to an iPhone or
  iPod touch. To users, it means you can sync your music, contacts, photos, etc.
  over USB. To developers, it means you can connect to any listening localhost
  socket on the device. usbmuxd is not used for tethering data transfer, which
  uses a dedicated USB interface as a virtual network device.
endef

define Package/usbmuxd
  $(call Package/usbmuxd/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+librt +libusb-1.0 +libusbmuxd
endef

define Package/usbmuxd/description
  $(call Package/usbmuxd/Default/description)
endef

define Package/libusbmuxd
  $(call Package/usbmuxd/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE+= library
  DEPENDS:=+libplist +libpthread +libxml2 +zlib
endef

define Package/libusbmuxd/description
  $(call Package/usbmuxd/Default/description)
  This package contains the usbmuxd shared library.
endef

TARGET_CFLAGS += $(FPIC)

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/*.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib$(LIB_SUFFIX)/libusbmuxd.so* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib$(LIB_SUFFIX)/pkgconfig/libusbmuxd.pc $(1)/usr/lib/pkgconfig/
	$(SED) 's,lib$(LIB_SUFFIX),lib,g' $(1)/usr/lib/pkgconfig/libusbmuxd.pc
	$(SED) 's,\-dirty,$(PKG_VERSION),g' $(1)/usr/lib/pkgconfig/libusbmuxd.pc
endef

define Package/usbmuxd/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/iproxy $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/usbmuxd $(1)/usr/sbin/
endef

define Package/libusbmuxd/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib$(LIB_SUFFIX)/libusbmuxd.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,usbmuxd))
$(eval $(call BuildPackage,libusbmuxd))
