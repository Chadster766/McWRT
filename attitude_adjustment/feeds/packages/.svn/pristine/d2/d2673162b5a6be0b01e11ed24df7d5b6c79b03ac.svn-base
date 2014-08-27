#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dhcp6
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_SOURCE:=dhcpv6-linux-$(PKG_VERSION).zip
PKG_SOURCE_URL:=@SF/dhcpv6-linux
PKG_MD5SUM:=86193dfa62137db3ea459543db4f1102

include $(INCLUDE_DIR)/package.mk

PKG_UNPACK:=unzip -d $(PKG_BUILD_DIR)/ $(DL_DIR)/$(PKG_SOURCE)

define Package/dhcp6/Default
  SECTION:=ipv6
  CATEGORY:=IPv6
  DEPENDS:=+libncurses
  TITLE:=IPv6 DHCP
  URL:=http://sourceforge.net/projects/dhcpv6-linux
endef

define Package/dhcp6-client
  $(call Package/dhcp6/Default)
  TITLE+= client
endef

define Package/dhcp6-client/conffiles
/etc/dhcp6/solicit.conf
endef

define Package/dhcp6-server
  $(call Package/dhcp6/Default)
  TITLE+= server
endef

define Package/dhcp6-server/conffiles
/etc/dhcpd6/dhcpd6.conf
/etc/dhcpd6/leases6.conf
/etc/dhcpd6/partial_leases6.conf
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	# prevent package from rebuilding
	find $(PKG_BUILD_DIR) | xargs touch
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/client \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)"
	$(MAKE) -C $(PKG_BUILD_DIR)/server \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/dhcp6-client/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/dhcp6
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/client/cli $(1)/usr/sbin/dhcp6client
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/client/solicit.conf $(1)/etc/dhcp6/
endef

define Package/dhcp6-server/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/dhcpd6
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/server/serv $(1)/usr/sbin/dhcpd6
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/server/*.conf $(1)/etc/dhcpd6/
endef

$(eval $(call BuildPackage,dhcp6-client))
$(eval $(call BuildPackage,dhcp6-server))
