#
# Copyright (C) 2010-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wide-dhcpv6
PKG_VERSION:=20080615
PKG_RELEASE:=11

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=1011e165850fe40d3e332dad2bfd30b3

include $(INCLUDE_DIR)/package.mk

define Package/wide-dhcpv6/Default
  SECTION:=ipv6
  CATEGORY:=IPv6
  TITLE:=WIDE-DHCPv6
  SUBMENU:=wide-dhcpv6
  DEPENDS:=+kmod-ipv6 
  URL:=https://sourceforge.net/projects/wide-dhcpv6/
  MAINTAINER:=Andrew Byrne <openwrt@andy.id.au>
endef

define Package/wide-dhcpv6-client
  $(call Package/wide-dhcpv6/Default)
  TITLE+= client
endef

define Package/wide-dhcpv6-client/description
	WIDE-DHCPv6 is an open-source implementation of Dynamic Host Configuration Protocol
	for IPv6 (DHCPv6) originally developed by the KAME project. The implementation
	mainly conforms to the following standards: RFC3315,3319,3633,3646,4075,4272,etc.

	This package installs the client component.
endef

define Package/wide-dhcpv6-server
  $(call Package/wide-dhcpv6/Default)
  TITLE+= server
endef

define Package/wide-dhcpv6-server/description
	WIDE-DHCPv6 is an open-source implementation of Dynamic Host Configuration Protocol
	for IPv6 (DHCPv6) originally developed by the KAME project. The implementation
	mainly conforms to the following standards: RFC3315,3319,3633,3646,4075,4272,etc.

	This package installs the server component.
endef

define Package/wide-dhcpv6-relay
  $(call Package/wide-dhcpv6/Default)
  TITLE+= relay
endef

define Package/wide-dhcpv6-relay/description
	WIDE-DHCPv6 is an open-source implementation of Dynamic Host Configuration Protocol
	for IPv6 (DHCPv6) originally developed by the KAME project. The implementation
	mainly conforms to the following standards: RFC3315,3319,3633,3646,4075,4272,etc.

	This package installs the relay component.
endef

define Package/wide-dhcpv6-control
  $(call Package/wide-dhcpv6/Default)
  TITLE+= client and server control utility
endef

define Package/wide-dhcpv6-control/description
	WIDE-DHCPv6 is an open-source implementation of Dynamic Host Configuration Protocol
	for IPv6 (DHCPv6) originally developed by the KAME project. The implementation
	mainly conforms to the following standards: RFC3315,3319,3633,3646,4075,4272,etc.

	This package installs the client and server control utility.
endef

TARGET_CFLAGS+=-D_GNU_SOURCE

define Build/Configure
	$(call Build/Configure/Default,--with-localdbdir=/var)
endef

define Build/Compile	
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		EXTRA_CFLAGS="-include debug.h" \
		all 
endef

define Package/wide-dhcpv6-client/conffiles
/etc/config/dhcp6c
endef

define Package/wide-dhcpv6-client/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dhcp6c $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/dhcp6c.config $(1)/etc/config/dhcp6c
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/dhcp6c.init $(1)/etc/init.d/dhcp6c
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) ./files/dhcp6c.hotplug $(1)/etc/hotplug.d/iface/40-dhcp6c
	$(INSTALL_DIR) $(1)/etc/hotplug.d/dhcp6c
	$(INSTALL_DATA) ./files/dhcp6c-dnsmasq.hotplug $(1)/etc/hotplug.d/dhcp6c/10-dnsmasq
	$(INSTALL_DATA) ./files/dhcp6c-radvd.hotplug $(1)/etc/hotplug.d/dhcp6c/20-radvd
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) ./files/dhcp6c-state $(1)/usr/bin/dhcp6c-state
endef

define Package/wide-dhcpv6-server/conffiles
/etc/config/dhcp6s
endef

define Package/wide-dhcpv6-server/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dhcp6s $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/dhcp6s.conf.sample $(1)/etc
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/dhcp6s.config $(1)/etc/config/dhcp6s
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/dhcp6s.init $(1)/etc/init.d/dhcp6s
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) ./files/dhcp6s.hotplug $(1)/etc/hotplug.d/iface/50-dhcp6s
endef

define Package/wide-dhcpv6-relay/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dhcp6relay $(1)/usr/sbin
endef

define Package/wide-dhcpv6-control/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dhcp6ctl $(1)/usr/sbin
endef

$(eval $(call BuildPackage,wide-dhcpv6-client))
$(eval $(call BuildPackage,wide-dhcpv6-server))
$(eval $(call BuildPackage,wide-dhcpv6-relay))
$(eval $(call BuildPackage,wide-dhcpv6-control))
