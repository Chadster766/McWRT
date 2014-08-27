#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=lft
PKG_VERSION:=3.32
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://pwhois.org/get
PKG_MD5SUM:=5f09c9ad2475cb237b8277a2d69c569c

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/lft/Default
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libpcap
  TITLE:=The alternative traceroute and whois tools for network engineers
  URL:=http://pwhois.org/lft
endef

define Package/lft/Default/description
	LFT, short for Layer Four Traceroute, is a sort of 'traceroute' that
	often works much faster (than the commonly-used Van Jacobson method)
	and goes through many configurations of packet-filters (firewalls).
	More importantly, LFT implements numerous other features including AS
	number lookups through several reliable sources, loose source routing,
	netblock name lookups, et al.
endef

define Package/lft
  $(call Package/lft/Default)
endef

define Package/lft/description
	$(call Package/lft/Default/description)
	This package contains the lft 'traceroute' program.
endef

define Package/whob
  $(call Package/lft/Default)
endef

define Package/whob/description
	$(call Package/lft/Default/description)
	This package contains the whob 'whois' program.
endef

CONFIGURE_ARGS += \
	--with-pcap="$(STAGING_DIR)/usr"

define Package/lft/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/lft $(1)/usr/bin/
endef

define Package/whob/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/whob $(1)/usr/bin/
endef

$(eval $(call BuildPackage,lft))
$(eval $(call BuildPackage,whob))
