#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ptunnel
PKG_VERSION:=0.71
PKG_RELEASE:=4

PKG_SOURCE:=PingTunnel-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.cs.uit.no/~daniels/PingTunnel
PKG_MD5SUM:=9b04771d4fa50abc15a6af690b81c71a

PKG_BUILD_DIR:=$(BUILD_DIR)/PingTunnel

include $(INCLUDE_DIR)/package.mk

define Package/ptunnel
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Firewall Tunnel
  DEPENDS:=+libpthread +libpcap
  TITLE:=Tunnel TCP connections over ICMP packets
  URL:=http://www.cs.uit.no/~daniels/PingTunnel/
endef

define Package/ptunnel/description
  ptunnel is an application that allows you to reliably tunnel TCP connections to 
  a remote host using ICMP echo request and reply packets, commonly known as ping 
  requests and replies. It acts as a proxy and can handle sockets and secured 
  identification.
endef

define Package/ptunnel/conffiles
/etc/config/ptunnel
endef

# uses GNU configure

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall -g -I$(STAGING_DIR)/usr/include" \
		LDOPTS="$(TARGET_LDFLAGS) -lpthread -lpcap" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Package/ptunnel/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/ptunnel $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/ptunnel.init $(1)/etc/init.d/ptunnel
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/ptunnel.config $(1)/etc/config/ptunnel
endef

$(eval $(call BuildPackage,ptunnel))
