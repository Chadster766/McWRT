#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rarpd
PKG_VERSION:=1.1
PKG_RELEASE:=4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.dementia.org/pub/net-tools
PKG_MD5SUM:=04e2ca849e758d0b88c8281775ec3b58

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/rarpd
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=IP Addresses and Names
  DEPENDS:=+libnet0
  TITLE:=Reverse ARP Daemon
  URL:=ftp://ftp.dementia.org/pub/net-tools/
endef

define Package/rarpd/description
	Rarpd is a daemon which responds to RARP requests.
	RARP is used by some machines at boot time to discover their
	IP address. They provide their Ethernet address and rarpd
	responds with their IP address if it finds it in the ethers
	database.
endef

CONFIGURE_ARGS+= \
	--with-libnet="$(STAGING_DIR)/usr/lib/libnet-1.0.x" \
	--with-pcap="$(STAGING_DIR)/usr" \

CONFIGURE_VARS+= \
	CFLAGS="$$$$CFLAGS -DNEW_LIBNET_INTERFACE" \

define Package/rarpd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/rarpd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/rarpd.config $(1)/etc/config/rarpd
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/rarpd.init $(1)/etc/init.d/rarpd
endef

define Package/rarpd/conffiles
/etc/config/rarpd
endef

$(eval $(call BuildPackage,rarpd))
