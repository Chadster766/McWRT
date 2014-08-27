include $(TOPDIR)/rules.mk

PKG_NAME:=webif-applications
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk

define Package/webif-applications
  SECTION:=admin
  CATEGORY:=Administration
  SUBMENU:=webif
  DEPENDS:=+webif
  TITLE:=XWrt applications plugin
endef

define Build/Compile
endef


define Package/webif-applications/install
	$(CP) -a ./files/* $(1)/
	$(INSTALL_BIN) ./files/www/cgi-bin/webif/* $(1)/www/cgi-bin/webif/
endef

$(eval $(call BuildPackage,webif-applications))
