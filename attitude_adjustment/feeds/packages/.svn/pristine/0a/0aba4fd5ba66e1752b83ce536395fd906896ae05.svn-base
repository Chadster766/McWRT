#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dhcp-forwarder
PKG_VERSION:=0.10
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=http://savannah.nongnu.org/download/dhcp-fwd
PKG_MD5SUM:=3b96dd27f6d05502d4048acab453808d

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--disable-dietlibc

define Package/dhcp-forwarder
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=IP Addresses and Names
  TITLE:=DHCP relay agent
  URL:=http://www.nongnu.org/dhcp-fwd/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		cfg_filename="/etc/dhcp-fwd.conf" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Package/dhcp-forwarder/conffiles
/etc/dhcp-fwd.conf
endef

define Package/dhcp-forwarder/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/etc $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/dhcp-fwd $(1)/usr/bin/
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/contrib/dhcp-fwd.conf $(1)/etc/
	$(INSTALL_BIN) ./files/dhcp-fwd.init $(1)/etc/init.d/dhcp-fwd
endef

$(eval $(call BuildPackage,dhcp-forwarder))
