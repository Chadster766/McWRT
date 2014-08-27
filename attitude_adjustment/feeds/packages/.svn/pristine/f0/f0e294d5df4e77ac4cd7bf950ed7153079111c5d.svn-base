#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=netstat-nat
PKG_VERSION:=1.4.10
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://tweegy.nl/download
PKG_MD5SUM:=c1bbb3ee9774ebfa27c440e2d9dd7286

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/netstat-nat
  SECTION:=net
  CATEGORY:=Network
  TITLE:=A netstat variant for NAT connections
  URL:=http://tweegy.nl/projects/netstat-nat/
endef

define Package/netstat-nat/description
	Netstat-nat is a small program written in C.
	It displays NAT connections, managed by netfilter/iptables which comes
	with the > 2.4.x linux kernels.
	The program reads its information from '/proc/net/ip_conntrack', which
	is the temporary conntrack-storage of netfilter.
endef

define Package/netstat-nat/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/netstat-nat $(1)/usr/bin/
endef

$(eval $(call BuildPackage,netstat-nat))
