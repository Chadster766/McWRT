# 
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luaprofiler
PKG_VERSION:=2.0.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://luaforge.net/frs/download.php/3400
PKG_MD5SUM:=611435c8e61e1a330ee0aa8ddc9c124a

include $(INCLUDE_DIR)/package.mk

define Package/luaprofiler
  SUBMENU:=Lua
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=Lua Profiler
  URL:=http://luaprofiler.luaforge.net
  DEPENDS:=+lua
endef

define Package/luaprofiler/description
 Profile any Lua application
endef

define Build/Configure

endef

TARGET_CFLAGS += $(FPIC) $(TARGET_CPPFLAGS)

TARGET_LDFLAGS += -llua

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -std=gnu99" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		-f Makefile.linux
endef

define Package/luaprofiler/install
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(CP) $(PKG_BUILD_DIR)/bin/profiler.so $(1)/usr/lib/lua/
endef

$(eval $(call BuildPackage,luaprofiler))
