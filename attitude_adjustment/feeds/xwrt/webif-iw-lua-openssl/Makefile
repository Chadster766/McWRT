include $(TOPDIR)/rules.mk

PKG_NAME:=webif-iw-lua-openssl
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/webif-iw-lua-openssl
  SECTION:=admin
  CATEGORY:=Administration
  SUBMENU:=webif
  DEPENDS:=+webif-iw-lua +openssl-util
  TITLE:=OpenSSL to handle certificates Script in LUA
endef

define Build/Compile
endef

define Package/webif-iw-lua-openssl/install
	$(CP) -a ./files/* $(1)/
	$(INSTALL_DIR) $(1)/www/cgi-bin/webif
	$(INSTALL_BIN) ./files/www/cgi-bin/webif/* $(1)/www/cgi-bin/webif/
	$(INSTALL_BIN) ./files/etc/openssl/ca-build $(1)/etc/openssl/ca-build
	$(INSTALL_BIN) ./files/etc/openssl/cert-build $(1)/etc/openssl/cert-build
	$(INSTALL_BIN) ./files/etc/openssl/cert-revoke $(1)/etc/openssl/cert-revoke
endef

$(eval $(call BuildPackage,webif-iw-lua-openssl))