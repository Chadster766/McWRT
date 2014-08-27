#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luaposix
PKG_VERSION:=5.1.11
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://luaforge.net/frs/download.php/4813
PKG_MD5SUM:=edb76911dbdabe98dec49e3d8a126227
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/luaposix
  SUBMENU:=Lua
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=luaposix
  URL:=http://luaposix.luaforge.net/
  DEPENDS:=+lua +librt
endef

define Package/luaposix/description
  luaposix is a general POSIX library for Lua providing access
  to various low level libc functions.
endef

define Build/Configure
endef

TARGET_CFLAGS += -DLUA_USE_LINUX $(FPIC) -std=gnu99

ifneq ($(CONFIG_USE_EGLIBC),)   
  ifeq ($(CONFIG_EGLIBC_OPTION_EGLIBC_UTMP),)
    TARGET_CFLAGS += -DNO_GETLOGIN
  endif
endif


define Package/luaposix/install
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/posix.so $(1)/usr/lib/lua
endef

$(eval $(call BuildPackage,luaposix))
