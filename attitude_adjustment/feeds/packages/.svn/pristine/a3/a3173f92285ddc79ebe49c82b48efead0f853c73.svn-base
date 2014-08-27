#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=nvramtool
PKG_REV:=4887
PKG_VERSION:=$(PKG_REV)
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=svn
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=svn://coreboot.org/coreboot/trunk/util/nvramtool
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz

include $(INCLUDE_DIR)/package.mk

define Package/nvramtool
  SECTION:=utils
  CATEGORY:=Utilities
  URL:=http://www.coreboot.org/Nvramtool
  TITLE:=Coreboot nvramtool
  DEPENDS:=@TARGET_x86 +@PACKAGE_pciutils +zlib
endef

define Package/nvramtool/description
nvramtool (previously known as lxbios and cmos_util) is a utility for
reading/writing coreboot parameters and displaying information from the coreboot
table in CMOS/NVRAM.
The program is intended only for x86-based Linux systems that use coreboot, but
can also be used for non-coreboot systems (e.g. for dumping all NVRAM bytes).
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		LIBS="-lpci -lz" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		nvramtool
endef

define Package/nvramtool/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/nvramtool $(1)/sbin/nvramtool
endef

$(eval $(call BuildPackage,nvramtool))
