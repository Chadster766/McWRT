#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=aircrack-ptw
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.cdc.informatik.tu-darmstadt.de/aircrack-ptw/download
PKG_MD5SUM:=1d7d84d1e69189bc1397db3f565c183a

include $(INCLUDE_DIR)/package.mk

define Package/aircrack-ptw
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libpcap
  TITLE:=A tool using a new method for breaking WEP Keys
  URL:=http://www.cdc.informatik.tu-darmstadt.de/aircrack-ptw/
  SUBMENU:=wireless
endef

define Build/Configure
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_CPPFLAGS) \
		$(TARGET_LDFLAGS) \
		-lpcap -o $(PKG_BUILD_DIR)/aircrack-ptw \
		$(PKG_BUILD_DIR)/aircrack-ptw.c \
		$(PKG_BUILD_DIR)/aircrack-ptw-lib.c 
endef

define Package/aircrack-ptw/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/aircrack-ptw $(1)/usr/bin/
endef

$(eval $(call BuildPackage,aircrack-ptw))
