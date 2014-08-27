#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=aprx
PKG_REV:=421
PKG_VERSION:=2.00_r$(PKG_REV)
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://repo.ham.fi/svn/aprx/trunk/
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_PROTO:=svn

include $(INCLUDE_DIR)/package.mk

define Package/aprx
  SECTION:=net
  CATEGORY:=Network
  TITLE:=APRS RX only I-gate
  URL:=http://ham.zmailer.org/oh2mqk/aprx/
endef

define Package/aprx/description
	This daemon listens for traffic on the specified serial interfaces.
	It then forwards appropriate packets to APRS-IS servers.
endef

CONFIGURE_ARGS += \
	--with-embedded \

define Package/aprx/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/aprx.init $(1)/etc/init.d/aprx
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/aprx.conf $(1)/etc/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/aprx $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/aprx-stat $(1)/usr/sbin/
endef

define Package/aprx/conffiles
/etc/aprx.conf
endef

$(eval $(call BuildPackage,aprx))
