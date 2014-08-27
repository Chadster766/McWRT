#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ipupdate
PKG_VERSION:=1.1.1
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/ipupdate
PKG_MD5SUM:=88ad3ac097190055db4ba816b9d26d13


include $(INCLUDE_DIR)/package.mk

define Package/ipupdate
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=IP Addresses and Names
  TITLE:=IPUpdate-Daemon
  URL:=http://ipupdate.sourceforge.net
endef

define Package/ipupdate/description
  Lightweight remote DNS update-daemon which implements RFC 2136 dynamic updates.
  Compatible with BIND, Simple DNS, and Microsoft DNS Server.
endef


define Build/Configure
$(call Build/Configure/Default,, \
	UNAME_S="Linux" \
	UNAME_R="$(LINUX_VERSION)" \
	UNAME_M="$(ARCH)" \
)
endef

define Build/Compile
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)/usr
	$(MAKE) -C $(PKG_BUILD_DIR) \
		 $(TARGET_CONFIGURE_OPTS) \
		 CC="$(TARGET_CC) $(TARGET_CFLAGS) -s" \
		 PRECOMPILED_FILTER=1 \
		 STAGING_DIR="$(STAGING_DIR)" \
		 DESTDIR="$(PKG_INSTALL_DIR)/usr" \
		 all
endef

define Package/ipupdate/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ipupdate $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/ipupdate
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dollardns $(1)/etc/ipupdate/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/failover $(1)/etc/ipupdate/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ifip $(1)/etc/ipupdate/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/whatismyip $(1)/etc/ipupdate/
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/ipupdate.conf $(1)/etc/
endef

define Package/ipupdate/conffiles
/etc/ipupdate.conf
endef

$(eval $(call BuildPackage,ipupdate))
