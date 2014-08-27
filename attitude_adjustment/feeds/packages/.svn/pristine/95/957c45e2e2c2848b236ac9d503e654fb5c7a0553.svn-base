# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v3.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=xmpp4r
PKG_VERSION:=0.5
PKG_RELEASE:=0

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).zip
PKG_SOURCE_URL:=http://download.gna.org/xmpp4r
PKG_MD5SUM:=70eafe960727d750a43ec77ffc1856e9

include $(INCLUDE_DIR)/package.mk

define Package/xmpp4r
	TITLE:=XMPP/Jabber library for Ruby
	SECTION:=libs
	CATEGORY:=Libraries
	DEPENDS:=+ruby
  	MAINTAINER:=Giovanni Di Stasi <giovanni.distasi@unina.it>
  	URL:=http://home.gna.org/xmpp4r/
endef

define Package/xmpp4r/description
	This package contains xmpp4r, an XMPP/Jabber library for Ruby.
endef

define Build/Compile
	(cd $(PKG_BUILD_DIR); \
		ruby $(PKG_BUILD_DIR)/setup.rb config \
			--rbconfig=$(wildcard $(STAGING_DIR)/usr/lib/ruby/*.*/*-linux/rbconfig.rb); \
		ruby $(PKG_BUILD_DIR)/setup.rb install --prefix="$(PKG_INSTALL_DIR)"; \
	)
endef

define Package/xmpp4r/install
	$(INSTALL_DIR) $(1)/usr/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib $(1)/usr
endef

$(eval $(call BuildPackage,xmpp4r))
