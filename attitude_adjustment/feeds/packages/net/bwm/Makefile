#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=bwm
PKG_VERSION:=1.1.0
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/b/bwm
PKG_MD5SUM:=51021a036acb92d2bda0c0c0483f9552

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION).orig

include $(INCLUDE_DIR)/package.mk

define Package/bwm
  SECTION:=net
  CATEGORY:=Network
  TITLE:=A very tiny bandwidth monitor
  URL:=http://ftp.debian.org/debian/pool/main/b/bwm
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(PKG_BUILD_DIR)/bwm.c -o $(PKG_BUILD_DIR)/bwm
endef

define Package/bwm/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bwm $(1)/usr/bin/
endef

$(eval $(call BuildPackage,bwm))
