# 
# Copyright (C) 2011 segal.ubi.pt 
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ntripclient
PKG_VERSION:=1.5.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME).zip
PKG_SOURCE_URL:=http://igs.bkg.bund.de/root_ftp/NTRIP/software/
PKG_MD5SUM:=4b357fa648d74e4e13c4b5a2a39f5624

include $(INCLUDE_DIR)/package.mk

define Package/ntripclient
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=NTRIP
  TITLE:=Ntrip Version 2.0 Command Line Client
  URL:=http://igs.bkg.bund.de/ntrip/download
  MAINTAINER:=Nuno Goncalves <nunojpg@gmail.com>
endef

define Package/ntripclient/description
  Ntrip Version 2.0 Command Line Client, reading from Ntrip Version 1.0 or 2.0 Caster
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	unzip $(DL_DIR)/$(PKG_SOURCE) -d $(PKG_BUILD_DIR)
endef

define Package/ntripclient/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ntripclient $(1)/usr/bin/
endef

$(eval $(call BuildPackage,ntripclient))
