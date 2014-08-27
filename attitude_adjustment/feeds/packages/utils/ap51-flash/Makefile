# 
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=ap51-flash
PKG_REV:=232
PKG_VERSION:=r$(PKG_REV)
PKG_RELEASE:=3

PKG_SOURCE_PROTO:=svn
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=ap51-flash-$(PKG_VERSION)
PKG_SOURCE_URL:=http://dev.cloudtrax.com/downloads/svn/ap51-flash/trunk
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
PKG_MIRROR_MD5SUM:=3bff5b2ffa77ee80a807f3d24165b7cf

include $(INCLUDE_DIR)/package.mk

ifeq ($(CONFIG_BIG_ENDIAN),y)
  STAMP_BUILT:=$(STAMP_BUILT)_big
endif

define Package/ap51-flash
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=A tool for flashing (nearly) all ap51/ap61 based routers
  URL:=http://dev.cloudtrax.com/wiki/ap51-flash-station
endef

TARGET_EXTRA_CFLAGS:=-DFLASH_FROM_FILE -DNO_LIBPCAP
ifeq ($(CONFIG_BIG_ENDIAN),y)
	TARGET_EXTRA_CFLAGS:=$(TARGET_EXTRA_CFLAGS) -DUIP_CONF_BYTE_ORDER=1234
endif

# pass optimization flags
MAKE_FLAGS += \
	OFLAGS="$(TARGET_CFLAGS)" \
	EXTRA_CFLAGS="$(TARGET_EXTRA_CFLAGS)" \
	REVISION="$(PKG_VERSION)" \

define Package/ap51-flash/install
	$(INSTALL_DIR) $(1)/etc/config $(1)/etc/init.d $(1)/usr/sbin \
		$(1)/usr/lib/ap51-flash
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ap51-flash $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/ap51-flash.init $(1)/etc/init.d/ap51-flash
	$(INSTALL_BIN) ./files/ap51-flash.sh $(1)/usr/lib/ap51-flash/ap51-flash.sh
	$(INSTALL_DATA) ./files/ap51-flash.config $(1)/etc/config/ap51-flash
endef

define Package/ap51-flash/conffiles
/etc/config/ap51-flash
endef

$(eval $(call BuildPackage,ap51-flash))
