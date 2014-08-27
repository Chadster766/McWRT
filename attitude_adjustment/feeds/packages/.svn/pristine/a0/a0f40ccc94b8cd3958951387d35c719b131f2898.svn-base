# 
# Copyright (C) 2008-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=st2205tool
PKG_VERSION:=1.4.3
PKG_RELEASE:=2

PKG_SOURCE:=st2205tool-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://downloads.openwrt.org/sources/
PKG_MD5SUM:=827e3b719ebaf6bc537f2ce0ece3fccb

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/st2205tool
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=ST220xu powered photo-frames library
  DEPENDS:=+libgd
  URL:=http://picframe.spritesserver.nl/wiki/
endef

define Package/st2205tool/description
  Library for hacked photo-frames based on st220xu chipset. You may use those photo-frames with
  mplayer or LCD4Linux
endef

TARGET_CFLAGS += $(FPIC)

MAKE_FLAGS += \
	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS) -I$(PKG_BUILD_DIR)/libst2205/" \
	LDFLAGS="$(TARGET_LDFLAGS)" \
	SETPICLIBS="$(TARGET_LDFLAGS) -lgd -ljpeg -lpng -lz -L../libst2205 -lst2205" \


define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/libst2205/st2205.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libst2205/libst2205.{a,so*} $(1)/usr/lib/
endef


define Package/st2205tool/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/setpic/setpic $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libst2205/libst2205.so $(1)/usr/lib/libst2205.so.1
endef

$(eval $(call BuildPackage,st2205tool))
