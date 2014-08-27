#
# Copyright (C) 2006-2008 X-Wrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=webif-freeloader
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/webif-freeloader
  SECTION:=admin
  CATEGORY:=Administration
  SUBMENU:=webif
  DEPENDS:=+webif
  TITLE:=A download manager for the webif^2
  VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
endef

define Package/webif-freeloader-lang
  $(call Package/webif-freeloader)
  DEPENDS:=+webif-freeloader
  PKGARCH:=all
endef

define Package/webif-freeloader-lang-cs
  $(call Package/webif-freeloader-lang)
  TITLE:=Czech language file for webif-freeloader
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/webif-freeloader/install
	$(CP) -a ./files/* $(1)/
	mkdir -p $(1)/CONTROL
	$(INSTALL_BIN) ./ipkg/webif-freeloader.postinst $(1)/CONTROL/postinst
	$(INSTALL_BIN) ./ipkg/webif-freeloader.postrm $(1)/CONTROL/postrm
	$(INSTALL_BIN) ./files/www/cgi-bin/webif/* $(1)/www/cgi-bin/webif/
	$(INSTALL_BIN) ./files/usr/lib/webif/freeloader-include.sh $(1)/usr/lib/webif/freeloader-include.sh
	$(INSTALL_BIN) ./files/usr/sbin/* $(1)/usr/sbin/
	rm -rf $(1)/usr/lib/webif/lang/
	rm -f $(1)/www/cgi-bin/webif/*.wip
endef

define BuildLang
  define Package/webif-freeloader-lang-$(1)/install
	$(INSTALL_DIR) $$(1)/usr/lib/webif/lang/$(1)/
	$(INSTALL_DATA) ./files/usr/lib/webif/lang/$(1)/*.txt \
		$$(1)/usr/lib/webif/lang/$(1)/
  endef

  $$(eval $$(call BuildPackage,webif-freeloader-lang-$(1)))
endef

$(eval $(call BuildPackage,webif-freeloader))
$(eval $(call BuildLang,cs))
