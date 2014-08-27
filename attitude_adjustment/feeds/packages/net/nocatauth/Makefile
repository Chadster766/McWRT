#
# Copyright (C) 2009-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nocatauth
PKG_VERSION:=nightly
PKG_RELEASE:=2

PKG_SOURCE:=NoCatAuth-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=http://nocat.net/downloads/NoCatAuth/
PKG_MD5SUM:=b3a5f8d62b322a9409806037ab8d374e

PKG_BUILD_DIR:=$(BUILD_DIR)/NoCatAuth-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/nocatauth
  SUBMENU:=Captive Portals
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+perl +perlbase-essential +perlbase-posix +perlbase-autoloader +perlbase-xsloader +perlbase-fcntl +perlbase-tie +perlbase-getopt +perlbase-findbin +perlbase-cwd +perlbase-config +perlbase-file +perlbase-sigtrap +perlbase-io +perlbase-symbol +perlbase-selectsaver +perlbase-socket +perlbase-errno +kmod-ipt-ipopt +kmod-ipt-nat-extra +iptables-mod-ipopt +iptables-mod-nat-extra
  TITLE:=Open public network perl-based gateway daemon
  URL:=http://nocat.net
endef

define Package/nocatauth/description
        NoCatAuth is the original "catch and release" captive portal 
	implementation. It provides a simple splash screen web page for 
	clients on your network, as well as a variety of authenticated 
	modes. It is written in Perl.
endef

define Build/Configure
endef

define Build/Compile
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		PREFIX="$(PKG_INSTALL_DIR)" \
		gateway
endef

define Package/nocatauth/install
	$(INSTALL_DIR) $(1)
	$(INSTALL_DIR) $(1)/www
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/htdocs/{status,splash}.html $(1)/www
	$(INSTALL_DIR) $(1)/www/images
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/htdocs/images/auth_logo.gif $(1)/www/images
	$(INSTALL_DIR) $(1)/usr/nocatauth/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/* $(1)/usr/nocatauth/lib
	$(INSTALL_DIR) $(1)/usr/nocatauth/bin
	$(CP) $(PKG_INSTALL_DIR)/bin/* $(1)/usr/nocatauth/bin
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_CONF) $(PKG_INSTALL_DIR)/nocat.conf $(1)/etc/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/nocatauth-gateway $(1)/etc/init.d/nocatauth-gateway
endef

define Package/nocatauth/conffiles
/etc/nocat.conf
endef

$(eval $(call BuildPackage,nocatauth))
