#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luasoap
PKG_VERSION:=2.0.2
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/downloads/tomasguisasola/luasoap/
PKG_MD5SUM:=3aafa06f3a65e65f4ee08e12b89ec583

include $(INCLUDE_DIR)/package.mk

define Package/luasoap
  SUBMENU:=Lua
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=LuaSOAP
  URL:=https://github.com/tomasguisasola/luasoap
  DEPENDS:=+lua +luaexpat +luasec +luasocket
endef

define Package/luasoap/description
  LuaSOAP is a library of functions to deal with SOAP.
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/luasoap/install
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/src/soap.lua $(1)/usr/lib/lua/
	$(INSTALL_DIR) $(1)/usr/lib/lua/soap
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/src/{client,server}.lua $(1)/usr/lib/lua/soap/
	$(INSTALL_DIR) $(1)/usr/lib/lua/soap/client
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/src/client/https.lua $(1)/usr/lib/lua/soap/client/
	$(INSTALL_DIR) $(1)/usr/lib/lua/soap/tests
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/tests/test{,-http,-server}.lua $(1)/usr/lib/lua/soap/tests/
endef

$(eval $(call BuildPackage,luasoap))
