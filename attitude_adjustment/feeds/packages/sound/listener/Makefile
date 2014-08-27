#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=listener
PKG_VERSION:=1.7.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=http://www.vanheusden.com/listener/
PKG_MD5SUM:=8ad9c24c45c7d28aed606304a2bf80bf

include $(INCLUDE_DIR)/package.mk

define Package/listener
  SECTION:=sound
  CATEGORY:=Sound
  DEPENDS:=+libncurses +libsndfile
  TITLE:=Listener automatically records when sound is detected
  URL:=http://www.vanheusden.com/listener/
endef

define Package/listener/description
  This program listens for sound. If it detects any, it starts recording
  automatically and also automatically stops when things become silent
  again.
endef

MAKE_FLAGS+= \
	INSTALL_PREFIX= \

define Package/listener/conffiles
/etc/listener.conf
endef

define Package/listener/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/{listener,listenersoundpipe,setlistener} $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_CONF) ./files/listener.conf $(1)/etc/
endef

$(eval $(call BuildPackage,listener))
