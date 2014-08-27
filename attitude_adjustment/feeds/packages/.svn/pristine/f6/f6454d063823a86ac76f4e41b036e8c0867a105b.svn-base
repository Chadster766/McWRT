#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=serialoverip
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME).tgz
PKG_SOURCE_URL:=@SF/serialoverip
PKG_MD5SUM:=0931cca65b8f9d32079fa3e55130ee1e

include $(INCLUDE_DIR)/package.mk

define Package/serialoverip
  SECTION:=net
  CATEGORY:=Network
  TITLE:=SerialOverIP
  URL:=http://sourceforge.net/projects/serialoverip/
endef

define Package/serialoverip/description
	Tool for transport of serial interfaces over UDP/IP.
	Usefull for accessing a distant serial device by a local program.
endef

define Build/Configure
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) \
		-o $(PKG_BUILD_DIR)/serialoverip $(PKG_BUILD_DIR)/serialoverip.c
endef

define Package/serialoverip/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/serialoverip $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,serialoverip))
