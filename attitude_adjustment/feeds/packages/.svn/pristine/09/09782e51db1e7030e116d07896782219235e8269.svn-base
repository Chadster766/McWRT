#
# Copyright (C) 2009-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=btpd
PKG_VERSION:=0.16
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/downloads/btpd/btpd
PKG_MD5SUM:=fe042aae8d7c515ecd855673d1c2b33e

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/btpd
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libpthread +librt
  SUBMENU:=BitTorrent
  TITLE:=btpd - BitTorrent Protocol Daemon
  URL:=https://github.com/btpd/btpd/wiki/
endef

define Package/btpd/description
	BTPD is a bittorrent client consisting of a daemon and client commands,
	which can be used to read and/or manipulate the daemon state.
	The daemon is capable of running several torrents simultaneously and
	only uses one tcp port.
	It's fairly low on resource usage and should be perfect for file
	distribution sites.
	Efficient downloads and ease of use makes this client a good choice for
	the casual user as well.
endef

define Package/btpd/conffiles
/etc/config/btpd
endef

CONFIGURE_ARGS += -C --with-evloop-method=poll

define Package/btpd/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/bt{cli,info,pd} $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/btpd.init $(1)/etc/init.d/btpd
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/btpd.config $(1)/etc/config/btpd
endef

$(eval $(call BuildPackage,btpd))
