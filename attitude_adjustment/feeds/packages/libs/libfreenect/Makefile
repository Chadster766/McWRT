#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libfreenect
PKG_VERSION:=2012-01-29
PKG_RELEASE:=$(PKG_SOURCE_VERSION)

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=git://github.com/OpenKinect/libfreenect.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=bbc109a589a1dd2c229f8bc98536dbc184dd73f9
PKG_MIRROR_MD5SUM:=08f8cb77dd8b9bce21fc7440f62110e7
CMAKE_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/libfreenect
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Xbox Kinect library
  DEPENDS:=+libusb-1.0
endef

CMAKE_OPTIONS += -DBUILD_EXAMPLES="OFF"
TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include

define Package/libfreenect/description
       Drivers and libraries for the Xbox Kinect device
endef

define Package/libfreenect/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libfreenect.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libfreenect))
