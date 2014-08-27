#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dibbler
PKG_VERSION:=0.8.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://klub.com.pl/dhcpv6/dibbler
PKG_MD5SUM:=313044c95af3091607127a05c338d8ec

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk

define Package/dibbler/Default
  SECTION:=ipv6
  CATEGORY:=IPv6
  DEPENDS:=$(CXX_DEPENDS) +libpthread
  TITLE:=Dibbler, a portable DHCPv6 implementation
  URL:=http://klub.com.pl/dhcpv6/
endef

define Package/dibbler/Default/description
	Dibbler is a portable DHCPv6 implementation. It supports stateful (i.e.
	IPv6 address granting) as well as stateless (i.e. option granting)
	autoconfiguration for IPv6.
endef

define Package/dibbler-client
  $(call Package/dibbler/Default)
  TITLE+= (client)
endef

define Package/dibbler-client/conffiles
/etc/dibbler/client.conf
endef

define Package/dibbler-client/description
	$(call Package/dibbler/Default/description)
	This package contains the dibbler client.
endef

define Package/dibbler-relay
  $(call Package/dibbler/Default)
  TITLE+= (relay)
endef

define Package/dibbler-relay/conffiles
/etc/dibbler/relay.conf
endef

define Package/dibbler-relay/description
	$(call Package/dibbler/Default/description)
	This package contains the dibbler relay.
endef

define Package/dibbler-requestor
  $(call Package/dibbler/Default)
  TITLE+= (requestor)
endef

define Package/dibbler-requestor/description
	$(call Package/dibbler/Default/description)
	This package contains the dibbler requestor.
endef

define Package/dibbler-server
  $(call Package/dibbler/Default)
  TITLE+= (server)
endef

define Package/dibbler-server/conffiles
/etc/dibbler/server.conf
endef

define Package/dibbler-server/description
	$(call Package/dibbler/Default/description)
	This package contains the dibbler server.
endef

define Package/dibbler-client/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dibbler-client $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/dibbler
	$(INSTALL_CONF) \
		$(PKG_BUILD_DIR)/doc/examples/client.conf \
		$(1)/etc/dibbler/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) \
		./files/dibbler-client.init \
		$(1)/etc/init.d/dibbler-client
endef

define Package/dibbler-relay/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dibbler-relay $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/dibbler
	$(INSTALL_CONF) \
		$(PKG_BUILD_DIR)/doc/examples/relay.conf \
		$(1)/etc/dibbler/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) \
		./files/dibbler-relay.init \
		$(1)/etc/init.d/dibbler-relay
endef

define Package/dibbler-requestor/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dibbler-requestor $(1)/usr/sbin/
endef

define Package/dibbler-server/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dibbler-server $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/dibbler
	$(INSTALL_CONF) \
		$(PKG_BUILD_DIR)/doc/examples/server.conf \
		$(1)/etc/dibbler/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) \
		./files/dibbler-server.init \
		$(1)/etc/init.d/dibbler-server
endef

$(eval $(call BuildPackage,dibbler-client))
$(eval $(call BuildPackage,dibbler-relay))
$(eval $(call BuildPackage,dibbler-requestor))
$(eval $(call BuildPackage,dibbler-server))
