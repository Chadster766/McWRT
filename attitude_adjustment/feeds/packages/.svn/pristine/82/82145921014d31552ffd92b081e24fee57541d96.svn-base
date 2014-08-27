#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tctool
PKG_VERSION:=0.9.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).zip
PKG_SOURCE_URL:=http://download2.berlios.de/speedmodem50b
PKG_MD5SUM:=df38334206d4cad106e58bd71c6f1d9a

PKG_BUILD_DIR=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/tctool
  SECTION:=net
  CATEGORY:=Network
  TITLE:=TrendChip-Modem-Utility
  URL:=http://speedmodem50b.berlios.de/
  DEPENDS:=+libstdcpp
endef

define Package/tctool/description
 tcTool provides access to the console-service of TrendChip-based 
 ADSL-Modems.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/src $(TARGET_CONFIGURE_OPTS) tctool
endef

define Package/tctool/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/tctool $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,tctool))
