# 
# Copyright (C) 2010 segal.di.ubi.pt 
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=open2300
PKG_RELEASE:=1.11
PKG_VERSION:=1.11

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.ccccoimbra.com/open2300/
PKG_MD5SUM:=804576790840c3bc4a288f3e0f83ce4d

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)


include $(INCLUDE_DIR)/package.mk


define Package/open2300
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=WS23XX weather station utilities
  URL:=http://www.lavrsen.dk/twiki/bin/view/Open2300/WebHome
  MAINTAINER:=Nuno Goncalves <nunojpg@gmail.com>
endef

define Package/open2300/description
  This is an utility suite used to communicate with and collect data from a WS23XX wheather station.
endef

define Package/open2300/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/open2300 $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dump2300 $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/log2300 $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/fetch2300 $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wu2300 $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/cw2300 $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/history2300 $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/histlog2300 $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin2300 $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/xml2300 $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/light2300 $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/interval2300 $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/minmax2300 $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/open2300-dist.conf $(1)/etc/
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib2300.so.1.11 $(1)/usr/lib/
	ln -sf /usr/lib/lib2300.so.1.11 $(1)/usr/lib/lib2300.so
endef

$(eval $(call BuildPackage,open2300))
