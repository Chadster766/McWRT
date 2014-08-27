# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=cbtt
PKG_VERSION:=20060727
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)80-$(PKG_VERSION)-src.tar.gz
PKG_SOURCE_URL:=@SF/bnbtusermods
PKG_MD5SUM:=d6edf0a11d7e68fa3d121e91308f22f5

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)80-$(PKG_VERSION)-src

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk

define Package/cbtt/Default
  SUBMENU:=BitTorrent
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libpthread $(CXX_DEPENDS) +zlib
  TITLE:=Bittorrent tracker
  URL:=http://bnbtusermods.sourceforge.net/
endef

define Package/cbtt
$(call Package/cbtt/Default)
endef

define Package/cbtt-mysql
$(call Package/cbtt/Default)
  DEPENDS+= +libmysqlclient
  TITLE+= (with mysql support)
endef


define Build/Template

$(STAMP_BUILT)-$(2): $(STAMP_PREPARED)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
		LFLAGS="$(TARGET_LDFLAGS) -L. $(4) -lz -lm -lpthread" \
		$(3)
	touch $$@

$(STAMP_BUILT): $(STAMP_BUILT)-$(2)

define Package/$(1)/install
	$(INSTALL_DIR) $$(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(3) $$(1)/usr/bin/
endef

endef

define Build/Configure
endef

define Build/Compile
endef

ifneq ($(SDK)$(CONFIG_PACKAGE_cbtt),)
  define Build/without-mysql
    $(call Build/Template,cbtt,without-mysql,bnbt,)
  endef
endif
$(eval $(Build/without-mysql))

ifneq ($(SDK)$(CONFIG_PACKAGE_cbtt-mysql),)
  define Build/with-mysql
    $(call Build/Template,cbtt-mysql,with-mysql,bnbtmysql, \
	-L$(STAGING_DIR)/usr/lib/mysql -lmysqlclient \
    )
  endef
endif
$(eval $(Build/with-mysql))

$(eval $(call BuildPackage,cbtt))
$(eval $(call BuildPackage,cbtt-mysql))
