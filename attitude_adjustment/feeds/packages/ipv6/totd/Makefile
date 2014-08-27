#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=totd
PKG_VERSION:=1.5.1
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.dillema.net/software/totd
PKG_MD5SUM:=7edaedae9f6aca5912dd6c123582cf08

include $(INCLUDE_DIR)/package.mk

define Package/totd
  SECTION:=ipv6
  CATEGORY:=IPv6
  DEPENDS:=+kmod-ipv6
  TITLE:=Small DNS proxy that supports IPv6/IPv4 record translation
  URL:=http://www.dillema.net/software/totd.html
endef

define Package/totd/description
	totd is a small DNS proxy nameserver which supports IPv6 and enable IPv6
	only sites to access IPv4 sites by using some translation mechanism such
	as NAT-PT, KAME faith, etc...
endef

define Package/totd/conffiles
/etc/totd.conf
endef

# uses GNU configure

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		CC="$(TARGET_CC)" \
		all
endef

define Package/totd/install
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/
	$(INSTALL_CONF) ./files/totd.conf $(1)/etc/
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/totd.init $(1)/etc/init.d/totd
endef

define Package/totd/conffiles
/etc/totd.conf
endef

$(eval $(call BuildPackage,totd))
