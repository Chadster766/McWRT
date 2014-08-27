# 
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=cdp-tools
PKG_VERSION:=20070302
PKG_RELEASE:=1

PKG_SOURCE:=cdp-tools.tar.gz
PKG_SOURCE_URL:=http://gpl.internetconnection.net/files/
PKG_MD5SUM:=94e3a355a5c82a2a5b14114d1f9b0599

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

PKG_BUILD_DEPENDS:=+libpcap +libnet1


define Package/cdp-tools
  SECTION:=net
  CATEGORY:=Network
  TITLE:=CDP tools (CISCO)
  URL:=http://gpl.internetconnection.net
  DEPENDS:=+libpcap
endef

define Package/dhcpcd/description
	Tools to work with the Cisco Discovery Protocol (CDP)
endef

define Build/Compile
	CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/usr/include/pcap -I$(STAGING_DIR)/usr/lib/libnet-1.1.x/include" \
	LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/usr/lib/libnet-1.1.x/lib" \
	CFLAGS="$(TARGET_CFLAGS)" \
	$(MAKE) -C $(PKG_BUILD_DIR) \
	$(TARGET_CONFIGURE_OPTS) \
	all
endef


define Package/cdp-tools/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/{cdp-send,cdp-listen,cdp-routemgr} $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,cdp-tools))
