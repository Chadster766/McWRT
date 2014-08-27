#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ndyndns
PKG_VERSION:=2.1
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://ndyndns.googlecode.com/files
PKG_MD5SUM:=0cdb4dcba5f8c1664f53884b3c2179e9

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

MAKE_FLAGS:=PLATFORM=linux

define Package/ndyndns
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=IP Addresses and Names
  TITLE:=DynDNS.com update daemon
  URL:=http://ndyndns.googlecode.com/
  DEPENDS:=+libcurl +libopenssl
endef

define Package/ndyndns/description
	The ndyndns daemon is an update client for DynDNS.com. It
	checks for an IP address change every two minutes, avoids
	unnecessary updates, reports errors, logs to syslog, and
	generally complies with everything required of a proper dyndns
	client.
endef

define Package/ndyndns/conffiles
/etc/ndyndns/ndyndns.conf.sample
endef

define Package/ndyndns/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ndyndns $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_BIN) files/ndyndns.init $(1)/etc/init.d/ndyndns
	$(INSTALL_DIR) $(1)/etc/ndyndns
	$(INSTALL_CONF) files/ndyndns.conf.sample $(1)/etc/ndyndns
endef

$(eval $(call BuildPackage,ndyndns))
