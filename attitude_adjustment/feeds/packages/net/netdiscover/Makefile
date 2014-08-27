#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=netdiscover
PKG_VERSION:=0.3-beta6
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://nixgeneration.com/~jaime/netdiscover/releases/
PKG_MD5SUM:=0919227a91ecaeeb2443cff249417be2

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/netdiscover
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libpcap +libnet1 +libpthread
  TITLE:=An active/passive address reconnaissance tool
  URL:=http://nixgeneration.com/~jaime/netdiscover/
  MAINTAINER:=Daniel Dickinson <openwrt@cshore.neomailbox.net>
endef

CONFIGURE_VARS+= \
	CPPFLAGS="$(TARGET_CPPFLAGS) -I$(STAGING_DIR)/usr/lib/libnet-1.1.x/include" \
	LDFLAGS="$(TARGET_LDFLAGS) -L$(STAGING_DIR)/usr/lib/libnet-1.1.x/lib" \

define Package/netdiscover/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/netdiscover $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,netdiscover))
