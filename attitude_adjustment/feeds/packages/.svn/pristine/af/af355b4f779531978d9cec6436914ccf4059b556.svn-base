#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gadgetfs-test
PKG_VERSION:=2007-11-29
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME).tar.bz2
PKG_SOURCE_URL:=http://avr32linux.org/twiki/pub/Main/GadgetFsTest
PKG_MD5SUM:=49476a74c29f1281c8a4c035aa57a5bd

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/gadgetfs-test
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Filesystem
  TITLE:=Test program for gadgetfs
  DEPENDS:=@DEVEL +libpthread
  URL:=http://linux-usb.org/gadget
endef

define Package/gadgetfs-test/description
 Test program for gadgetfs from linux-usb.org.
endef

define Package/gadgetfs-test-bulk
$(call Package/gadgetfs-test)
  TITLE+= (BULK transfer)
endef

define Package/gadgetfs-test-bulk/description
$(call Package/gadgetfs-test/description)
 This package is built with BULK transfer support.
endef

define Package/gadgetfs-test-isoc
$(call Package/gadgetfs-test)
  DEPENDS+= +libaio
  TITLE+= (ISOCHRONOUS transfer)
endef

define Package/gadgetfs-test-isoc/description
$(call Package/gadgetfs-test/description)
 This package is built with ISOCHRONOUS transfer support using libaio.
endef


GADGETFS_CFLAGS:=$(TARGET_CFLAGS) $(EXTRA_CFLAGS) $(TARGET_CPPFLAGS) $(EXTRA_CPPFLAGS)
GADGETFS_LDFLAGS:=$(TARGET_LDFLAGS) $(EXTRA_LDFLAGS)
GADGETFS_LIBS:=-lpthread

define Build/Template

$(STAMP_BUILT)-$(1): $(STAMP_PREPARED)
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	$(MAKE_VARS) \
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CC="$(TARGET_CC)" \
		LDFLAGS="$(GADGETFS_LDFLAGS)" \
		$(2) \
		all
	mv -f $(PKG_BUILD_DIR)/gadgetfs-test $(PKG_BUILD_DIR)/gadgetfs-test-$(1)
	touch $$@

$(STAMP_BUILT): $(STAMP_BUILT)-$(1)

define Package/gadgetfs-test-$(1)/install
	$(INSTALL_DIR) $$(1)/usr/sbin
	$(INSTALL_BIN) $$(PKG_BUILD_DIR)/gadgetfs-test-$(1) $$(1)/usr/sbin/
endef

endef

define Build/Configure
endef

define Build/Compile
endef

ifneq ($(SDK)$(CONFIG_PACKAGE_gadgetfs-test-bulk),)
  define Build/bulk
    $(call Build/Template,bulk, \
		CFLAGS="$(GADGETFS_CFLAGS)" \
		LIBS="$(GADGETFS_LIBS)" \
    )
  endef
endif
$(eval $(Build/bulk))

ifneq ($(SDK)$(CONFIG_PACKAGE_gadgetfs-test-isoc),)
  define Build/isoc
    $(call Build/Template,isoc, \
		CFLAGS="$(GADGETFS_CFLAGS) -DAIO" \
		LIBS="$(GADGETFS_LIBS) -laio" \
    )
  endef
endif
$(eval $(Build/isoc))

$(eval $(call BuildPackage,gadgetfs-test-bulk))
$(eval $(call BuildPackage,gadgetfs-test-isoc))

