# $Id: Makefile 4284 2006-07-25 19:07:23Z nbd $

include $(TOPDIR)/rules.mk

PKG_NAME:=webif-hotspot
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk

define Package/webif-hotspot
  SECTION:=admin
  CATEGORY:=Administration
  SUBMENU:=webif
  DEPENDS:=+webif
  TITLE:=XWrt HotSpot control pages
endef

define Build/Compile
endef

define Package/webif-hotspot/install
	$(CP) -a ./files/* $(1)/
	mkdir -p $(1)/CONTROL
	$(INSTALL_BIN) ./ipkg/webif-hotspot.postinst $(1)/CONTROL/preinst
	$(INSTALL_BIN) ./ipkg/webif-hotspot.postrm $(1)/CONTROL/postrm
	$(INSTALL_BIN) ./files/www/cgi-bin/webif/* $(1)/www/cgi-bin/webif/
	$(INSTALL_BIN) ./files/bin/save_hotspot $(1)/bin/save_hotspot
	$(INSTALL_BIN) ./files/usr/lib/webif/apply-hotspot.sh $(1)/usr/lib/webif/apply-hotspot.sh

	rm -rf $(1)/usr/lib/webif/lang/
	rm -f $(1)/www/cgi-bin/webif/*.wip
endef

$(eval $(call BuildPackage,webif-hotspot))
