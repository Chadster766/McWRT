#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=l2tpd
PKG_VERSION:=0.70pre
PKG_UPSTREAM_VERSION:=0.70-pre20031121
PKG_RELEASE:=4.1

PKG_SOURCE:=$(PKG_NAME)_$(PKG_UPSTREAM_VERSION).orig.tar.gz
PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/l/l2tpd
PKG_MD5SUM:=3f2707b6e16a8cb72e7bf64f574202fa

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_UPSTREAM_VERSION).orig

include $(INCLUDE_DIR)/package.mk

define Package/l2tpd
  SECTION:=net
  CATEGORY:=Network
  TITLE:=An L2TP (Layer 2 Tunneling Protocol) daemon
  URL:=http://l2tpd.sourceforge.net/
  SUBMENU:=VPN
endef

define Package/l2tpd/description
	l2tpd is the open source implementation of the L2TP tunneling 
	protocol (RFC2661). It does implement both LAC and LNS role 
	in a L2TP networking architecture. The main goal of this protocol is 
	to tunnel PPP frame trough an ip network.
endef

define Package/l2tpd/conffiles
/etc/l2tpd/l2tpd.conf
/etc/ppp/options.l2tpd
endef

define Build/Compile
	$(call Build/Compile/Default, \
		CFLAGS="$(TARGET_CFLAGS) -I. -DLINUX -DSANITY -DIP_ALLOCATION" \
	)
endef

define Package/l2tpd/install	
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/l2tpd
	$(INSTALL_DATA) ./files/l2tpd.conf $(1)/etc/l2tpd/
	$(INSTALL_DIR) $(1)/etc/ppp
	$(INSTALL_DATA) ./files/options.l2tpd $(1)/etc/ppp/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/l2tpd.init $(1)/etc/init.d/l2tpd
endef

$(eval $(call BuildPackage,l2tpd))

