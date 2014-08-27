#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=maradns
PKG_VERSION:=1.3.07.09
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.maradns.org/download/1.3/$(PKG_VERSION)/
PKG_MD5SUM:=1d221438fb4d9317263555262b9c652c

include $(INCLUDE_DIR)/package.mk

define Package/maradns
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libpthread
  TITLE:=Small and secure DNS Server
  URL:=http://www.maradns.org/
  SUBMENU:=IP Addresses and Names
endef

define Package/maradns/description
	maradns is a DNS server written with security, simplicity, and
	performance in mind.
endef

define Package/maradns/conffiles
/etc/mararc
endef

define Build/Configure
	# it's not GNU autoconf stuff
	(cd $(PKG_BUILD_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		./configure \
	);
endef

define Build/Compile	
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CFLAGS="$(TARGET_CFLAGS)" \
		FLAGS="$(TARGET_CFLAGS)" \
		CC="$(TARGET_CC)" \
		HOSTCC="$(HOSTCC)" \
		V=
	mkdir -p $(PKG_INSTALL_DIR)/usr/{bin,sbin}
	mkdir -p $(PKG_INSTALL_DIR)/usr/man/man{1,5,8}
	$(MAKE) -C $(PKG_BUILD_DIR) \
		PREFIX="$(PKG_INSTALL_DIR)/usr" \
		RPM_BUILD_ROOT="$(PKG_INSTALL_DIR)" \
		V= \
		install
endef

define Package/maradns/install	
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/mararc $(1)/etc/mararc
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/maradns.init $(1)/etc/init.d/maradns
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/{askmara,getzone,fetchzone,duende} $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/{maradns,zoneserver} $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,maradns))
