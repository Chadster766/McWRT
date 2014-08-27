# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nstx
PKG_VERSION:=1.1-beta6
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/n/nstx/
PKG_MD5SUM:=275abd75a856b07a798b1f2360088cf5

include $(INCLUDE_DIR)/package.mk

define Package/nstx/Default
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Firewall Tunnel
  TITLE:=allows you to pass IP packets via DNS queries
  URL:=http://savannah.nongnu.org/projects/nstx/
endef

define Package/nstx
  $(call Package/nstx/Default)
  TITLE+= client version
endef

define Package/nstx/description
	nstx client version
endef

define Package/nstxd
  $(call Package/nstx/Default)
  TITLE+= server version
endef

define Package/nstxd/description
	nstx server version
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)"
endef

define Package/nstx/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/nstxcd $(1)/usr/sbin/
endef

define Package/nstxd/install
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/nstxd $(1)/usr/sbin
endef

$(eval $(call BuildPackage,nstx))
$(eval $(call BuildPackage,nstxd))
