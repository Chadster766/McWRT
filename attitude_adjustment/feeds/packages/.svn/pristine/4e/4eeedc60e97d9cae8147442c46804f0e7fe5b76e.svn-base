# 
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ptrtd
PKG_VERSION:=0.5.2
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://v6web.litech.org/ptrtd/dist/
PKG_MD5SUM:=bfe026445fdc4fe509a9c70ec4551744

include $(INCLUDE_DIR)/package.mk

define Package/ptrtd
  SECTION:=ipv6
  CATEGORY:=IPv6
  DEPENDS:=+kmod-ipv6 +kmod-tun +ip
  TITLE:=Portable Transport Relay Translator Daemon
  URL:=http://v6web.litech.org/ptrtd/
endef

define Package/ptrtd/description
	The Portable Transport Relay Translator Daemon (pTRTd)
	is a method of allowing IPv6 hosts to communicate with IPv4 hosts.
endef

MAKE_FLAGS += \
	CFLAGS="$(TARGET_CFLAGS)"

define Package/ptrtd/install
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/ptrtd.init $(1)/etc/init.d/ptrtd
endef

$(eval $(call BuildPackage,ptrtd))
