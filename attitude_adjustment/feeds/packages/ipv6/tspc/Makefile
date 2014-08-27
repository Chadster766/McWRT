# 
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tspc
PKG_VERSION:=2.1.1
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_SOURCE_URL:=http://ftp.fr.debian.org/debian/pool/main/t/tspc/
PKG_MD5SUM:=65183cae002feaacd8bc92d6a5404cc2

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)2

include $(INCLUDE_DIR)/package.mk

define Package/tspc
  SECTION:=ipv6
  CATEGORY:=IPv6
  TITLE:=Client for Hexago's (Freenet6's) IPv6 service
  URL:=http://www.hexago.com
endef

define Package/tspc/description
	Client to configure an IPv6 tunnel to 
	Hexago's (Freenet6's) migration broker.
endef

MAKE_FLAGS += \
	CFLAGS="$(TARGET_CFLAGS) -I$(PKG_BUILD_DIR)/include -I$(PKG_BUILD_DIR)/platform/linux" \
	target=linux \
	all

define Package/tspc/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/tspc $(1)/bin/
	$(INSTALL_DIR) $(1)/etc/tsp
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/bin/tspc.conf.sample $(1)/etc/tsp/tspc.conf
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/template/linux.sh $(1)/etc/tsp/setup.sh
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/template/checktunnel.sh $(1)/etc/tsp/
endef

define Package/tspc/conffiles
/etc/tsp/tspc.conf
endef

$(eval $(call BuildPackage,tspc))
