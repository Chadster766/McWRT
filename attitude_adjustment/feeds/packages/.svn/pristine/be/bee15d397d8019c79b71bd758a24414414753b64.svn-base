#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=elfutils
PKG_VERSION:=0.152
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://fedorahosted.org/releases/e/l/$(PKG_NAME)/$(PKG_VERSION)
PKG_MD5SUM:=39739ed58a0fa1862eff8735f111fe5c

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/elfutils/Default
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=$(INTL_DEPENDS) +USE_UCLIBC:argp-standalone @BROKEN
  TITLE:=ELF manipulation libraries
  URL:=https://fedorahosted.org/elfutils/
endef

define Package/libdw
  $(call Package/elfutils/Default)
  DEPENDS:=libelf1 +zlib
  TITLE+= (libdw)
endef

define Package/libelf1
  $(call Package/elfutils/Default)
  TITLE+= (libelf)
endef

ifdef CONFIG_USE_UCLIBC
CONFIGURE_VARS += \
	LIBS="-largp"
endif

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libdw/libdw.{a,so*} $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/libelf/libelf.{a,so*} $(1)/usr/lib/
endef

define Package/libdw/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libdw/libdw.so* $(1)/usr/lib/
endef

define Package/libelf1/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libelf/libelf.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libdw))
$(eval $(call BuildPackage,libelf1))
