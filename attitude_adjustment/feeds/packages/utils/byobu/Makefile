#
# Copyright (C) 2009 Sharim At Work, Ltd.
# Author: Xiangfu Liu <xiangfu@sharism.cc>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=byobu
PKG_VERSION:=3.6

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_SOURCE_URL:=http://launchpad.net/byobu/trunk/3.6/+download/
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/byobu
    MAINTAINER:="Xiangfu Liu" <xiangfu@sharism.cc>
    SECTION:=utils
    CATEGORY:=Utilities
    TITLE:=a Japanese term for decorative, multi-panel screens that serve as folding room dividers
    URL:=https://launchpad.net/byobu
endef

define Package/byobu/description
  Byobu is a Japanese term for decorative, multi-panel screens that serve
  as folding room dividers. As an open source project, Byobu is an elegant
  enhancement of the otherwise functional, plain, practical GNU Screen.
  Byobu includes an enhanced profile and configuration utilities for the
  GNU screen window manager, such as toggle-able system status notifications.
endef

define Package/byobu/install
	$(INSTALL_DIR) \
		$(1)/usr/bin \
		$(1)/usr/lib/byobu \
		$(1)/usr/share

	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/bin/* \
		$(1)/usr/bin/

	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/byobu \
		$(1)/usr/lib/

	$(CP) \
		$(PKG_INSTALL_DIR)/usr/share/byobu \
		$(1)/usr/share/
endef

$(eval $(call BuildPackage,byobu))
