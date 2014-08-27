#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=netio
PKG_VERSION:=1.31
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)$(subst .,,$(PKG_VERSION)).zip
PKG_SOURCE_URL:=http://www.ars.de/ars/ars.nsf/f24a6a0b94c22d82862566960071bf5a/aa577bc4be573b05c125706d004c75b5/%24FILE/
PKG_MD5SUM:=6b81e3c71b9dfa3bd60c53fa82e89085

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk

PKG_UNPACK:=mkdir -p $(PKG_BUILD_DIR); unzip $(DL_DIR)/$(PKG_SOURCE) -d $(PKG_BUILD_DIR)

define Package/netio
  $(call Package/netio/Default)
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=$(CXX_DEPENDS) +libpthread
  TITLE:=Network throughput benchmark
  URL:=http://www.ars.de/ars/ars.nsf/docs/netio
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) linux CC="$(TARGET_CC)"
endef

define Package/netio/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,netio))
