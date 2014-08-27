#
# Copyright (C) 2009-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tmux
PKG_VERSION:=1.6
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/tmux
PKG_MD5SUM:=3e37db24aa596bf108a0442a81c845b3

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/tmux
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Terminal multiplexer
  DEPENDS:=+libncurses +libevent2 +libpthread +librt
  URL:=http://tmux.sourceforge.net/
endef

define Package/tmux/description
  tmux is a modern, BSD-licensed alternative to GNU screen.
endef

MAKE_FLAGS += \
	PREFIX="/usr" \
	PLATFORM="linux"

define Package/tmux/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/tmux $(1)/usr/bin/
endef

$(eval $(call BuildPackage,tmux))
