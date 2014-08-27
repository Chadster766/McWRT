#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rp-l2tp
PKG_VERSION:=0.4
PKG_RELEASE:=4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/rp-l2tp
PKG_MD5SUM:=0e45d11cb4fa6c56cce6b1d119733ed9

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/rp-l2tpd
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Roaring Penguin L2TP
  URL:=http://sourceforge.net/projects/rp-l2tp/
  TITLE+= daemon
  SUBMENU:=dial-in/up
endef

define Package/rp-l2tpd/description
	Roaring Penguin L2TP (Layer 2 Tunneling Protocol) implementation.
endef

TARGET_CFLAGS := -I$(PKG_BUILD_DIR)/libevent $(TARGET_CFLAGS)

define Package/rp-l2tpd/install
	$(INSTALL_DIR) $(1)/etc
	$(CP) $(PKG_INSTALL_DIR)/etc/l2tp.conf.example $(1)/etc/l2tp.conf
	$(INSTALL_DIR) $(1)/usr/lib/l2tp
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/l2tpd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/rp-l2tpd.init $(1)/etc/init.d/rp-l2tpd
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/l2tp/cmd.so $(1)/usr/lib/l2tp/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/l2tp/sync-pppd.so $(1)/usr/lib/l2tp/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/l2tp-control $(1)/usr/sbin/
endef

define Package/rp-l2tpd/conffiles
/etc/l2tp.conf
endef

#define Package/rp-l2tpd-mod-cmd/install
#	$(INSTALL_DIR) $(1)/usr/lib/l2tp
#	$(CP) $(PKG_INSTALL_DIR)/usr/lib/l2tp/cmd.so $(1)/usr/lib/l2tp/
#	$(INSTALL_DIR) $(1)/usr/sbin
#	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/l2tp-control $(1)/usr/sbin/
#endef

#define Package/rp-l2tpd-mod-ppp/install
#	$(INSTALL_DIR) $(1)/usr/lib/l2tp
#	$(CP) $(PKG_INSTALL_DIR)/usr/lib/l2tp/sync-pppd.so $(1)/usr/lib/l2tp/
#endef

$(eval $(call BuildPackage,rp-l2tpd))
#$(eval $(call BuildPackage,rp-l2tpd-mod-cmd))
#$(eval $(call BuildPackage,rp-l2tpd-mod-ppp))
