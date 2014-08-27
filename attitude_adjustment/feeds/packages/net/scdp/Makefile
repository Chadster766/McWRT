#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=scdp
PKG_VERSION:=1.0b
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/scdp
PKG_MD5SUM:=7eafaf5a422e37d04715613993ed5d95

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/scdp
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libnet0
  TITLE:=An utility to send CDP (Cisco Discovery Protocol) packets
  URL:=http://sourceforge.net/projects/scdp
endef

define Package/scdp/description
	This program sends CDP (Cisco Discovery Protocol) packets out on
	selected interfaces and tells the connected switch (cisco only)
	where the host is connected.
endef

define Build/Configure
	$(call Build/Configure/Default, \
		, \
		CPPFLAGS="$$$$CPPFLAGS -I$(STAGING_DIR)/usr/lib/libnet-1.0.x/include" \
		LDFLAGS="$$$$LDFLAGS -L$(STAGING_DIR)/usr/lib/libnet-1.0.x/lib" \
		PATH="$(STAGING_DIR)/usr/lib/libnet-1.0.x/bin:$$$$PATH" \
	)
endef

define Package/scdp/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/scdp $(1)/usr/bin/
endef

$(eval $(call BuildPackage,scdp))
