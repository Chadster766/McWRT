#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=iptables-snmp
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.nobiscuit.com/iptables-snmp
PKG_MD5SUM:=8370d2f0c899461a053da491400119d1

include $(INCLUDE_DIR)/package.mk

define Package/iptables-snmp
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+snmpd +libiptc
  TITLE:=An snmpd plugin to access iptables rules
  URL:=http://www.nobiscuit.com/iptables-snmp/
  SUBMENU:=Firewall
endef

define Package/iptables-snmp/description
	This is a plugin that makes iptables rules accessable from snmpd.
endef

define Build/Compile	
	$(call Build/Compile/Default, \
		INSTALL_PREFIX="$(PKG_INSTALL_DIR)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		all install \
	)
endef
 
define Package/iptables-snmp/install	
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/iptables-snmp.so $(1)/usr/lib
endef 

$(eval $(call BuildPackage,iptables-snmp))
