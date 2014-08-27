#
# Copyright (C) 2007-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pciutils
PKG_VERSION:=3.1.10
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@KERNEL/software/utils/pciutils
PKG_MD5SUM:=3a5d8b2784c8571dc5557efda4a8c157

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/pciutils
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Linux PCI Utilities
  URL:=http://mj.ucw.cz/pciutils.shtml
  DEPENDS:=+zlib
endef

define Package/pciutils/description
 contains collection of programs for inspecting and manipulating configuration
 of PCI devices
endef

define Package/pciutils/postinst
#!/bin/sh
[ -z "$${IPKG_INSTROOT}" ] || \
(cd $${PKG_ROOT}/usr/share; $${PKG_ROOT}/usr/sbin/update-pciids; rm pci.ids.gz.old)
exit 0
endef

MAKE_FLAGS += \
	CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include" \
	PREFIX="/usr" \
	HOST="$(GNU_TARGET_NAME)" \
	ZLIB="yes" \

ifneq ($(CONFIG_USE_EGLIBC),)
TARGET_LDFLAGS += -lresolv
endif

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/lib/libpci.a $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/include/pci
	$(CP) $(foreach i,pci.h config.h header.h types.h, \
		$(PKG_BUILD_DIR)/lib/$(i)) $(1)/usr/include/pci
endef

define Package/pciutils/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/{lspci,setpci,update-pciids} $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/share
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/share/pci.ids.gz $(1)/usr/share/
endef

$(eval $(call BuildPackage,pciutils))
