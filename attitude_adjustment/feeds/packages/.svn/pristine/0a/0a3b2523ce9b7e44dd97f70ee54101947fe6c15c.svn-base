#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wknock
PKG_VERSION:=1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME).tar.gz
PKG_SOURCE_URL:=http://blackhat.com/presentations/bh-europe-05/BH_EU_05-Oudot/
PKG_MD5SUM:=6b7a1f3cbb0af88da02e5d30cefd8e7f

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/wknock
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=kmod-brcm-wl +libpcap
  TITLE:=A tool to hide your Access Point against opportunistic attackers.
  URL:=http://www.rstack.org/oudot/wknock/
endef

define Package/wknock/description
tool to hide your Access Point against opportunistic attackers.
endef

define Build/Compile	
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS) -lpcap" \
		all
endef

define Package/wknock/install	
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wknock $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wknock-ng $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,wknock))
