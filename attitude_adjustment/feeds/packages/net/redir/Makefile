# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=redir
PKG_VERSION:=2.2.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://sammy.net/~sammy/hacks
PKG_MD5SUM:=4342fadac30504c86c8db7beefe01995

include $(INCLUDE_DIR)/package.mk

define Package/redir
	SECTION:=net
	CATEGORY:=Network
	SUBMENU:=Routing and Redirection
	TITLE=Redirect TCP connections
	URL:=http://sammy.net/~sammy/hacks
endef

define Package/redir/description
 A port redirector, used to forward incoming connections to somewhere else.
endef

define Package/redir/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/redir $(1)/usr/bin/
endef

$(eval $(call BuildPackage,redir))
