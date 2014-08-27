#
# Copyright (C) 2009-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=lsqlite3
PKG_VERSION:=7
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://luaforge.net/frs/download.php/4472
PKG_MD5SUM:=ef7afdd3ea7a29333daa951cda3175bb

include $(INCLUDE_DIR)/package.mk

define Package/lsqlite3
  SUBMENU:=Lua
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=LuaSQLite3
  URL:=http://luasqlite.luaforge.net/lsqlite3.html
  DEPENDS:=+lua +libsqlite3
endef

define Package/lsqlite3/description
 LuaSQLite 3 is a thin wrapper around the public domain SQLite3 database engine.
endef

define Build/Configure
endef

TARGET_CFLAGS += $(FPIC) $(TARGET_CPPFLAGS) -std=c99

TARGET_LDFLAGS += -llua -lsqlite3

MAKE_FLAGS += \
	CC="$(TARGET_CC)" \
	CFLAGS="$(TARGET_CFLAGS)" \
	INCS="$(TARGET_CPPFLAGS)" \
	LIBS="$(TARGET_LDFLAGS)" \
	SHFLAGS="-shared" \
	UNAME="Linux"

define Package/lsqlite3/install
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(CP) $(PKG_BUILD_DIR)/lsqlite3.so $(1)/usr/lib/lua/
endef

$(eval $(call BuildPackage,lsqlite3))
