#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luasocket
PKG_VERSION:=2.0.2
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://luaforge.net/frs/download.php/2664
PKG_MD5SUM:=41445b138deb7bcfe97bff957503da8e

include $(INCLUDE_DIR)/package.mk

define Package/luasocket
  SUBMENU:=Lua
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=LuaSocket
  URL:=http://luasocket.luaforge.net/
  DEPENDS:=+lua
endef

define Package/luasocket/description
  LuaSocket is the most comprehensive networking support
  library for the Lua language. It provides easy access to
  TCP, UDP, DNS, SMTP, FTP, HTTP, MIME and much more.
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/ \
		LIBDIR="$(TARGET_LDFLAGS)" \
		CC="$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_CPPFLAGS) -std=gnu99" \
		LD="$(TARGET_CROSS)ld -shared" \
		all
endef


define Package/luasocket/install
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/src/{ltn12,mime,socket}.lua $(1)/usr/lib/lua
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/mime.so.1.0.2 $(1)/usr/lib/lua
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/socket.so.2.0.2 $(1)/usr/lib/lua
	$(INSTALL_DIR) $(1)/usr/lib/lua/mime
	ln -sf ../mime.so.1.0.2 $(1)/usr/lib/lua/mime/core.so
	$(INSTALL_DIR) $(1)/usr/lib/lua/socket
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/src/{ftp,http,smtp,tp,url}.lua $(1)/usr/lib/lua/socket
	ln -sf ../socket.so.2.0.2 $(1)/usr/lib/lua/socket/core.so
endef

$(eval $(call BuildPackage,luasocket))
