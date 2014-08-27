include $(TOPDIR)/rules.mk

PKG_NAME:=webif-iw-lua-chillispot
PKG_VERSION:=1.0
PKG_RELEASE:=2

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DEPENDS:=+lua/host

include $(INCLUDE_DIR)/package.mk

define Package/webif-iw-lua-chillispot
  SECTION:=admin
  CATEGORY:=Administration
  SUBMENU:=webif
  DEPENDS:=+webif-iw-lua +chillispot
  TITLE:=ChilliSpot Configuration Script in LUA
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) -a files $(PKG_BUILD_DIR)
	find $(PKG_BUILD_DIR)/files -name 'CVS' -o -name '.svn' -o -name '*~' | $(XARGS) rm -rf
endef

define Build/Configure
endef

define Build/Compile
	find $(PKG_BUILD_DIR)/files -type f -name '*.lua' -exec $(STAGING_DIR_HOST)/bin/luac -s -o {} {} \;
endef

define Package/webif-iw-lua-chillispot/install
	$(CP) -a $(PKG_BUILD_DIR)/files/* $(1)/
	$(INSTALL_DIR) $(1)/www/cgi-bin/webif/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/www/cgi-bin/webif/* $(1)/www/cgi-bin/webif/
	$(INSTALL_DIR) $(1)/www/cgi-bin/login/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/www/cgi-bin/login/* $(1)/www/cgi-bin/login/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/etc/init.d/* $(1)/etc/init.d/

endef

$(eval $(call BuildPackage,webif-iw-lua-chillispot))
