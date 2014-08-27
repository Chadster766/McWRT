#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=bing
PKG_VERSION:=1.3.5
PKG_RELEASE:=0

PKG_SOURCE:=$(PKG_NAME)_src-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://fgouget.free.fr/bing/
PKG_MD5SUM:=60aaee55277672d0cc163bf3f0b3d53f
TAR_OPTIONS += || true

include $(INCLUDE_DIR)/package.mk

define Package/bing
  SECTION:=net
  CATEGORY:=Network
  TITLE:=bing bandwidth measuring utility
  URL:=http://fgouget.free.fr/bing/
endef

define Package/bing/description
	bing evaluates the raw bandwidth of a link (PSTN, xDSL, Cable ...)
endef

define Build/Compile
	$(call Build/Compile/Default,bing)
endef

define Package/bing/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bing $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,bing))
