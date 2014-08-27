#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=hasciicam
PKG_VERSION:=1.1.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.dyne.org/hasciicam/releases/
PKG_MD5SUM:=2c7393270e18ab913043683fa3390d8c

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/hasciicam
  SECTION:=multimedia
  CATEGORY:=Multimedia
  TITLE:=Live ASCII video feeds
  URL:=http://ascii.dyne.org/
  DEPENDS:= +libaa +ftplib
endef


CONFIGURE_VARS += \
	LIBS="-lncurses" \

define Package/hasciicam/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/hasciicam $(1)/usr/bin/
endef

$(eval $(call BuildPackage,hasciicam))
