#
# Copyright (C) 2009-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rinetd
PKG_VERSION:=0.62
PKG_RELEASE:=2

PKG_SOURCE:=rinetd.tar.gz
PKG_SOURCE_URL:=http://www.boutell.com/rinetd/http
PKG_MD5SUM:=28c78bac648971724c46f1a921154c4f
PKG_UNPACK=$(HOST_TAR) -C $(PKG_BUILD_DIR) --strip-components=1 -xzf $(DL_DIR)/$(PKG_SOURCE)

include $(INCLUDE_DIR)/package.mk

define Package/rinetd
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Routing and Redirection
  TITLE:=rinetd - Internet Redirection Server
  URL:=http://www.boutell.com/rinetd/
endef

define Package/rinetd/conffiles
/etc/config/rinetd
endef

define Package/rinetd/description
  Rinetd redirects TCP connections from one IP address and port to another.
  Rinetd is a single-process server which handles any number of connections
  to the address/port pairs specified in the configuration.
  Since rinetd runs as a single process using nonblocking I/O, it is able to
  redirect a large number of connections without a severe impact on the
  machine.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		rinetd
endef

define Package/rinetd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rinetd $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/rinetd.config $(1)/etc/config/rinetd
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/rinetd.init $(1)/etc/init.d/rinetd

endef

$(eval $(call BuildPackage,rinetd))
