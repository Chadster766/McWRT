#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wx200d
PKG_VERSION:=1.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/wx200d
PKG_MD5SUM:=0f90c099dddcdd46ac568d707822ec4c

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/wx200d
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libpq
  TITLE:=weather station data collector
  URL:=http://wx200d.sourceforge.net/
endef

define Package/wx200d/description
	wx200d is a weather station data collector and server daemon 
	for the WX200, WM918, WMR918 and WMR968 weather station 
	hardware.
endef

# uses GNU configure

define Package/wx200d/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP)	$(PKG_INSTALL_DIR)/usr/bin/wx200 \
		$(PKG_INSTALL_DIR)/usr/bin/wxstdout \
		$(PKG_INSTALL_DIR)/usr/bin/wxfilter \
		$(PKG_INSTALL_DIR)/usr/bin/wxdebug \
		$(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(CP)	$(PKG_INSTALL_DIR)/usr/sbin/wx200d \
		$(PKG_INSTALL_DIR)/usr/sbin/wxread \
		$(1)/usr/sbin/
endef

$(eval $(call BuildPackage,wx200d))
