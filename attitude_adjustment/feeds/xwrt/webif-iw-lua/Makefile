include $(TOPDIR)/rules.mk

PKG_NAME:=webif-iw-lua
PKG_VERSION:=1.1
PKG_RELEASE:=2

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)-$(PKG_RELEASE)
PKG_BUILD_DEPENDS:=+lua/host

include $(INCLUDE_DIR)/package.mk

define Package/webif-iw-lua
  SECTION:=admin
  CATEGORY:=Administration
  SUBMENU:=webif
  DEPENDS:=+webif +lua +libuci-lua
  TITLE:=Librerias para scripts de lua
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) -a files $(PKG_BUILD_DIR)
	find $(PKG_BUILD_DIR)/files -name 'CVS' -o -name '.svn' -o -name '*~' | $(XARGS) rm -rf
endef

define Build/Compile
	find $(PKG_BUILD_DIR)/files -type f -name '*.lua' -exec $(STAGING_DIR_HOST)/bin/luac -s -o {} {} \;
endef

define Package/webif-iw-lua/install
	$(CP) -a $(PKG_BUILD_DIR)/files/* $(1)/
#	$(INSTALL_BIN) ./file/usr/lib/lua/* $(1)/usr/lib/lua/
#	$(INSTALL_BIN) ./file/usr/lib/lua/lua-xwrt/* $(1)/usr/lib/lua/lua-xwrt/
#	$(INSTALL_BIN) ./file/usr/lib/lua/lua-xwrt/html/* $(1)/usr/lib/lua/lua-xwrt/html/
#	$(INSTALL_BIN) ./file/usr/lib/lua/lua-xwrt/addon/* $(1)/usr/lib/lua/lua-xwrt/addon/
#	$(INSTALL_BIN) ./file/usr/lib/lua/lua-xwrt/xwrt/* $(1)/usr/lib/lua/lua-xwrt/xwrt/
endef

$(eval $(call BuildPackage,webif-iw-lua))
