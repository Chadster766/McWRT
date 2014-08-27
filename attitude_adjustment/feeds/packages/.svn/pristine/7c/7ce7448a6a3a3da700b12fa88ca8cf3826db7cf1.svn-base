#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rrs
PKG_VERSION:=1.70
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.cycom.se/uploads/36/19 
PKG_MD5SUM:=b400d03c0e39e3e78a7327ba78f789f0

include $(INCLUDE_DIR)/package.mk

define Package/rrs/Default
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+uclibcxx
  TITLE:=A reverse (connecting) remote shell
  URL:=http://www.cycom.se/dl/rrs/
endef

define Package/rrs
$(call Package/rrs/Default)
  SECTION:=net
  CATEGORY:=Network
  DEPENDS+= +libopenssl
  TITLE+= (with SSL support)
  URL:=http://www.cycom.se/dl/rrs/
endef

define Package/rrs-nossl
$(call Package/rrs/Default)
  SECTION:=net
  CATEGORY:=Network
  TITLE+= (without SSL support)
endef


define Build/Template

$(STAMP_BUILT)-$(2): $(STAMP_PREPARED)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS) -I$(STAGING_DIR)/usr/include/uClibc++ -fno-builtin -fno-rtti -nostdinc++" \
		LDFLAGS="$(TARGET_LDFLAGS) -nodefaultlibs -lc $(4) -luClibc++ -lm -lutil $(LIBGCC_S)" \
		LDFLAGSNOSSL="$(TARGET_LDFLAGS) -nodefaultlibs -lc $(4) -luClibc++ -lm -lutil $(LIBGCC_S)" \
		$(3)
	( cd $(PKG_BUILD_DIR); mv -f rrs rrs-$(2); )
	touch $$@

$(STAMP_BUILT): $(STAMP_BUILT)-$(2)

define Package/$(1)/install
	$(INSTALL_DIR) $$(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rrs-$(2) $$(1)/usr/bin/rrs
endef

endef


define Build/Configure
endef

define Build/Compile
endef

ifneq ($(SDK)$(CONFIG_PACKAGE_rrs),)
  define Build/with-ssl
    $(call Build/Template,rrs,with-ssl,generic,-lcrypto -lssl)
  endef
endif
$(eval $(Build/with-ssl))

ifneq ($(SDK)$(CONFIG_PACKAGE_rrs-nossl),)
  define Build/without-ssl
    $(call Build/Template,rrs-nossl,without-ssl,generic-nossl,)
  endef
endif
$(eval $(Build/without-ssl))


$(eval $(call BuildPackage,rrs))
$(eval $(call BuildPackage,rrs-nossl))
