# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tcptraceroute
PKG_VERSION:=1.5beta7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://michael.toren.net/code/tcptraceroute
PKG_MD5SUM:=65d1001509f971ea986fcbc2dd009643

include $(INCLUDE_DIR)/package.mk

define Package/tcptraceroute
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libpcap +libnet1
  TITLE:=A traceroute implementation using TCP packets.
  URL:=http://michael.toren.net/code/tcptraceroute/
endef

define Build/Configure
        $(call Build/Configure/Default, \
		, \
	        CPPFLAGS="$$$$CPPFLAGS -I$(STAGING_DIR)/usr/lib/libnet-1.1.x/include" \
	        LDFLAGS="$$$$LDFLAGS -L$(STAGING_DIR)/usr/lib/libnet-1.1.x/lib" \
        	PATH="$(STAGING_DIR)/usr/lib/libnet-1.1.x/bin:$$$$PATH" \
        )
endef

define Build/Compile	
	$(MAKE) -C $(PKG_BUILD_DIR)
endef

define Package/tcptraceroute/install	
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tcptraceroute $(1)/usr/bin/
endef

$(eval $(call BuildPackage,tcptraceroute))
