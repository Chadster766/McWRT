#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=airpwn
PKG_RELEASE:=1
PKG_VERSION:=1.3

PKG_SOURCE:=airpwn-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=@SF/airpwn
PKG_MD5SUM:=15b6f1cd287b4bb20dd586bb6c83628d

PKG_BUILD_DEPENDS:=libnet1 libopenssl libpcap libpcre

PKG_FIXUP:=autoreconf
PKG_LIBTOOL_PATHS:=. lorcon

include $(INCLUDE_DIR)/package.mk

define Package/airpwn
	SECTION:=net
	CATEGORY:=Network
        SUBMENU:=wireless
	TITLE:=Packet injection pwnage
	DEPENDS:=+libnet1 +libopenssl +libpcap +libpcre +libpthread
	URL:=http://airpwn.sourceforge.net/Airpwn.html
endef

define Package/airpwn/description
  Airpwn is a framework for 802.11 (wireless) packet injection.
  Airpwn listens to incoming wireless packets, and if the data
  matches a pattern specified in the config files, custom content
  is injected "spoofed" from the wireless access point. From the
  perspective of the wireless client, airpwn becomes the server.
endef

CONFIGURE_VARS:= \
	CC="$(TARGET_CC)" \
	LD="$(TARGET_LD)" \
	CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/lib/libnet-1.1.x/include -I$(STAGING_DIR)/usr/include -I$(PKG_BUILD_DIR)/lorcon" \
	LDFLAGS="$(TARGET_LDFLAGS) -L$(STAGING_DIR)/usr/lib/libnet-1.1.x/lib -L$(PKG_BUILD_DIR)/lorcon/.libs -lcrypto -lssl" \
	LIBTOOL="$(STAGING_DIR)/host/bin/libtool"

define Build/Configure
	( cd $(PKG_BUILD_DIR); tar -xzf lorcon-current.tgz; cd lorcon; \
	$(CONFIGURE_VARS) ./configure $(CONFIGURE_ARGS); \
	$(MAKE) -C $(PKG_BUILD_DIR)/lorcon $(CONFIGURE_VARS); cd ..; \
	$(CONFIGURE_VARS) ./configure $(CONFIGURE_ARGS) )
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) $(CONFIGURE_VARS)
endef

define Package/airpwn/install
	$(INSTALL_DIR) $(1)/etc/airpwn
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/conf/* $(1)/etc/airpwn/
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/airpwn $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lorcon/.libs/liborcon-1.0.0.so $(1)/usr/lib/
	( cd $(1)/usr/lib; ln -s liborcon-1.0.0.so liborcon.so )
endef

$(eval $(call BuildPackage,airpwn))
