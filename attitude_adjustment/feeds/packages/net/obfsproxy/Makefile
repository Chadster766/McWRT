#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=obfsproxy
PKG_VERSION:=0.1.4
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://www.torproject.org/dist/obfsproxy \
        https://archive.torproject.org/tor-package-archive/obfsproxy
PKG_MD5SUM:=d3b898b1f5ea21c611bceaeb3c20e0d7

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/obfsproxy
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Web Servers/Proxies
  TITLE:=A pluggable transports proxy
  URL:=https://www.torproject.org/projects/obfsproxy
  DEPENDS:=+libevent2 +libopenssl
endef

define Package/obfsproxy/description
  obfsproxy is a tool that attempts to circumvent censorship, by transforming
  the Tor traffic between the client and the bridge. This way, censors, who
  usually monitor traffic between the client and the bridge, will see
  innocent-looking transformed traffic instead of the actual Tor traffic.
endef

define Package/obfsproxy/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/obfsproxy $(1)/usr/bin/
endef

$(eval $(call BuildPackage,obfsproxy))
