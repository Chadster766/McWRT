include $(TOPDIR)/rules.mk

PKG_NAME:=webif-iw-lua-openvpn
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/webif-iw-lua-openvpn
  SECTION:=admin
  CATEGORY:=Administration
  SUBMENU:=webif
  DEPENDS:=+webif-iw-lua +ip +openvpn +ntpclient
  TITLE:=OpenVPN Configuration Script in LUA
endef

define Build/Compile
endef

define Package/webif-iw-lua-openvpn/install
	$(CP) -a ./files/* $(1)/
	$(INSTALL_DIR) $(1)/www/cgi-bin/webif
	$(INSTALL_BIN) ./files/www/cgi-bin/webif/* $(1)/www/cgi-bin/webif/
endef

$(eval $(call BuildPackage,webif-iw-lua-openvpn))