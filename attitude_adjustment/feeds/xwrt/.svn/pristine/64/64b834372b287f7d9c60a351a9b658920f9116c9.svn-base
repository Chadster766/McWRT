#
# Copyright (C) 2006-2008 X-Wrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=webif
PKG_VERSION:=0.3
PKG_RELEASE:=$(shell LC_ALL=C svn info ${PKG_SOURCE_URL} | sed -ne's/^Last Changed Rev: //p')

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/webif
  SECTION:=admin
  CATEGORY:=Administration
  SUBMENU:=webif
  DEPENDS:=+haserl +opkg +uhttpd
  TITLE:=A HTTP administrative console for OpenWrt
  VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
endef

define Package/webif/conffiles
/etc/config/webif
/etc/config/cron
/etc/httpd.conf
/etc/init.d/custom-user-startup
endef

define Package/webif-subpackage
  $(call Package/webif)
  DEPENDS:=+webif
  PKGARCH:=all
endef

define Package/webif-lang-ca
  $(call Package/webif-subpackage)
  TITLE:=Catalan language file for webif^2
endef

define Package/webif-lang-cs
  $(call Package/webif-subpackage)
  TITLE:=Czech language file for webif^2
endef

define Package/webif-lang-da
  $(call Package/webif-subpackage)
  TITLE:=Danish language file for webif^2
endef

define Package/webif-lang-de
  $(call Package/webif-subpackage)
  TITLE:=German language file for webif^2
endef

define Package/webif-lang-ee
  $(call Package/webif-subpackage)
  TITLE:=Estonian language file for webif^2
endef

define Package/webif-lang-es
  $(call Package/webif-subpackage)
  TITLE:=Spanish language file for webif^2
endef

define Package/webif-lang-fr
  $(call Package/webif-subpackage)
  TITLE:=French language file for webif^2
endef

define Package/webif-lang-hr
  $(call Package/webif-subpackage)
  TITLE:=Croatian language file for webif^2
endef

define Package/webif-lang-hu
  $(call Package/webif-subpackage)
  TITLE:=Hungarian language file for webif^2
endef

define Package/webif-lang-it
  $(call Package/webif-subpackage)
  TITLE:=Italian language file for webif^2
endef

define Package/webif-lang-nl
  $(call Package/webif-subpackage)
  TITLE:=Dutch language file for webif^2
endef

define Package/webif-lang-no
  $(call Package/webif-subpackage)
  TITLE:=Norwegian language file for webif^2
endef

define Package/webif-lang-pl
  $(call Package/webif-subpackage)
  TITLE:=Polish language file for webif^2
endef

define Package/webif-lang-pt
  $(call Package/webif-subpackage)
  TITLE:=Portuguese language file for webif^2
endef

define Package/webif-lang-ru
  $(call Package/webif-subpackage)
  TITLE:=Russian language file for webif^2
endef

define Package/webif-lang-sq
  $(call Package/webif-subpackage)
  TITLE:=Albanian language file for webif^2
endef

define Package/webif-lang-sv
  $(call Package/webif-subpackage)
  TITLE:=Swedish language file for webif^2
endef

define Package/webif-lang-uk
  $(call Package/webif-subpackage)
  TITLE:=Ukrainian language file for webif^2
endef

define Package/webif-lang-zh
  $(call Package/webif-subpackage)
  TITLE:=Chinese language file for webif^2
endef

define Package/webif-theme-zephyr
  $(call Package/webif-subpackage)
  TITLE:=Zephyr theme for webif^2
endef

define Package/webif-theme-xwrt
  $(call Package/webif-subpackage)
  TITLE:=Original theme for webif^2
endef

define Package/webif-theme-xwrt-mini
  $(call Package/webif-subpackage)
  TITLE:=Variant of the original theme (blank header)
endef

define Package/webif-theme-sedky1
  $(call Package/webif-subpackage)
  TITLE:=Sedky theme for webif^2
endef

define Package/webif-theme-clubman
  $(call Package/webif-subpackage)
  TITLE:=Clubman theme for webif^2
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) -a files src ipkg/* $(PKG_BUILD_DIR)
	find $(PKG_BUILD_DIR)/files $(PKG_BUILD_DIR)/src -name 'CVS' -o -name '.svn' -o -name '*~' | $(XARGS) rm -rf
endef

define Build/Configure
	$(SED) 's,$$$$S,$(BOARD),g' $(PKG_BUILD_DIR)/webif.postinst
	./create_lang_list.sh $(PKG_BUILD_DIR) $(PKG_BUILD_DIR)/files/etc
	./embed_rev.sh $(PKG_BUILD_DIR)/webif.*inst $(PKG_BUILD_DIR)/files
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/src \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/include-host -I$(STAGING_DIR_HOST)/include -include endian.h  $(TARGET_CFLAGS_EXTRA)" \
	LDFLAGS="$(TARGET_LDFLAGS)"
	$(STRIP) $(PKG_BUILD_DIR)/src/webifmetabin
endef

define Package/webif/install
	$(CP) -a $(PKG_BUILD_DIR)/files/* $(1)/
	mkdir -p $(1)/CONTROL
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/webif.postinst $(1)/CONTROL/postinst
	rm -rf \
		$(1)/usr/lib/webif/lang/ \
		$(1)/www/themes/zephyr/ \
		$(1)/www/themes/xwrt-mini/ \
		$(1)/www/themes/sedky1/ \
		$(1)/www/themes/xwrt/
	$(SED) 's,$$$$S,$(PKGARCH),g' $(1)/etc/opkg/xwrt.conf
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/webifmetabin $(1)/usr/bin/webifmetabin
endef

define BuildLang
  define Package/webif-lang-$(1)/install
	$(INSTALL_DIR) $$(1)/usr/lib/webif/lang/$(1)/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/usr/lib/webif/lang/$(1)/common.txt \
		$$(1)/usr/lib/webif/lang/$(1)/
  endef

  $$(eval $$(call BuildPackage,webif-lang-$(1)))
endef

define BuildTheme
  define Package/webif-theme-$(1)/install
	$(INSTALL_DIR) $$(1)/www/themes/$(1)/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/www/themes/$(1)/* \
		$$(1)/www/themes/$(1)/
  endef

  $$(eval $$(call BuildPackage,webif-theme-$(1)))
endef

$(eval $(call BuildPackage,webif))
$(eval $(call BuildLang,ca))
$(eval $(call BuildLang,cs))
$(eval $(call BuildLang,da))
$(eval $(call BuildLang,de))
$(eval $(call BuildLang,ee))
$(eval $(call BuildLang,es))
$(eval $(call BuildLang,fr))
$(eval $(call BuildLang,hr))
$(eval $(call BuildLang,hu))
$(eval $(call BuildLang,it))
$(eval $(call BuildLang,nl))
$(eval $(call BuildLang,no))
$(eval $(call BuildLang,pl))
$(eval $(call BuildLang,pt))
$(eval $(call BuildLang,ru))
$(eval $(call BuildLang,sq))
$(eval $(call BuildLang,sv))
$(eval $(call BuildLang,uk))
$(eval $(call BuildLang,zh))
$(eval $(call BuildTheme,zephyr))
$(eval $(call BuildTheme,xwrt))
$(eval $(call BuildTheme,xwrt-mini))
$(eval $(call BuildTheme,sedky1))
$(eval $(call BuildTheme,clubman))
