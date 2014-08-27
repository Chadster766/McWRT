#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ucarp
PKG_VERSION:=1.5.2
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://download.pureftpd.org/pub/ucarp
PKG_MD5SUM:=723636dbf79fc6abd329a71ec4ddf79d

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/ucarp
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Lets several hosts share one IP
  DEPENDS:=+ip +libpcap
  URL:=http://www.ucarp.org/project/ucarp
endef

define Package/ucarp/description
 ucarp allows a couple of hosts to share common virtual IP addresses in order
 to provide automatic failover. It is a portable userland implementation of the
 secure and patent-free Common Address Redundancy Protocol
endef

define Package/ucarp/conffiles
/etc/ucarp.conf
endef

define Package/ucarp/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_CONF) ./files/ucarp.conf $(1)/etc/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/ucarp.init $(1)/etc/init.d/ucarp
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/ucarp $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,ucarp))
