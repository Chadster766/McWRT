#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=xsupplicant
PKG_VERSION:=1.2.8
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/open1x
PKG_MD5SUM:=75f89731b905eb8d36a52621661e1226

PKG_BUILD_DEPENDS:=PACKAGE_kmod-madwifi:madwifi
PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=0

include $(INCLUDE_DIR)/package.mk

define Package/xsupplicant
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=wireless
  DEPENDS:=+libopenssl +libiw
  TITLE:=A fully compliant 802.1x authenticator
  URL:=http://open1x.sourceforge.net/
endef

define Package/xsupplicant/description
 This software allows a GNU/Linux or BSD workstation to authenticate with 
 a RADIUS server using 802.1x and various EAP protocols.
endef

define Package/xsupplicant/conffiles
/etc/xsupplicant.conf
endef

TARGET_CFLAGS += --std=gnu99

CONFIGURE_ARGS+= \
	--with-openssl="$(STAGING_DIR)/usr" \
	--with-madwifi-path="$(STAGING_DIR)/usr/include/madwifi" \

CONFIGURE_VARS+= \
	CPPFLAGS="$$$$CPPFLAGS -I$(STAGING_DIR)/usr/include/madwifi" \

define Build/Prepare
	$(call Build/Prepare/Default)
	(cd $(PKG_BUILD_DIR); touch \
		configure.in \
		aclocal.m4 \
		Makefile.in \
		configure \
	);
endef

define Package/xsupplicant/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/etc/xsupplicant.conf $(1)/etc/
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/xsup_* $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/xsupplicant $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,xsupplicant))
