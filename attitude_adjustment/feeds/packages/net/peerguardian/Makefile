#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=peerguardian
PKG_VERSION:=1.5beta
PKG_RELEASE:=3

PKG_SOURCE:=pglinux-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/peerguardian
PKG_MD5SUM:=0fb2bc5501b031604fc56eec3bd35fa4

PKG_BUILD_DIR:=$(BUILD_DIR)/pglinux-$(PKG_VERSION)
PKG_BUILD_PARALLEL:=1

PKG_BUILD_DEPENDS:=iptables

include $(INCLUDE_DIR)/package.mk

define Package/peerguardian
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=P2P
  DEPENDS:= +libncurses +libpthread +libnetfilter-queue +libipq +libstdcpp
  TITLE:=PeerGuardian for Linux
  URL:=http://phoenixlabs.org/
endef

define Package/peerguardian/description
 PeerGuardian helps protect your privacy by blocking many ranges  of aggressive 
 IPs while you use P2P.
endef

define Package/peerguardian/conffiles
/etc/PG.conf
/etc/p2p.p2b.p2p
endef

EXTRA_CFLAGS:=-fno-rtti -I$(STAGING_DIR)/usr/include/libnetfilter_queue/

CONFIGURE_VARS+= \
	LIBS="-lm"

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
		CXXLD="$(TARGET_CXX)"
endef

define Package/peerguardian/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/PG.conf $(1)/etc/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/p2p.p2b.p2p $(1)/etc/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/peerguardnf $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pgtext $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/peerguardian.init $(1)/etc/init.d/peerguardian
endef

$(eval $(call BuildPackage,peerguardian))
