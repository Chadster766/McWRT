# 
# Copyright (C) 2006-2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=openh323
PKG_VERSION:=cvs-20051230
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://trash.uid0.hu/openwrt/
PKG_MD5SUM:=ba1ffd45016f533bec2bb1539b78e43a

PKG_BUILD_DEPENDS:=pwlib

include $(INCLUDE_DIR)/package.mk

define Package/libopenh323
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=OpenH323 library
  URL:=http://openh323.sourceforge.net/
endef

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--enable-minsize \
	--enable-audio \
	--disable-video \
	--enable-localspeex \

CONFIGURE_VARS += \
	CXXFLAGS="$(TARGET_CFLAGS) -fno-builtin -fno-rtti" \

MAKE_FLAGS += \
	CFLAGS="$(TARGET_CFLAGS) $(FPIC)" \
	CCFLAGS="$(TARGET_CFLAGS) $(FPIC)" \
	optnoshared

define Build/InstallDev
	ln -sf $(PKG_NAME)-$(PKG_VERSION) $(BUILD_DIR)/$(PKG_NAME)
endef

$(eval $(call BuildPackage,libopenh323))
