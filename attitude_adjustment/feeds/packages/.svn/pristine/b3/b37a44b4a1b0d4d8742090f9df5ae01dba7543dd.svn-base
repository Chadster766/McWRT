#
# Copyright (C) 2009-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=kissdx
PKG_REV:=193
PKG_VERSION:=r$(PKG_REV)
PKG_RELEASE:=3

PKG_SOURCE_PROTO:=svn
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=http://www.famille-kahn.com/svn/kissdx/kissdx/trunk
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_VERSION:=$(PKG_REV)

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_SOURCE_SUBDIR)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/kissdx
  SECTION:=multimedia
  CATEGORY:=Multimedia
  DEPENDS:=+libjpeg +libdvdread $(ICONV_DEPENDS)
  TITLE:=PC-Link clone for KiSS media players
  URL:=http://kissdx.vidartysse.net
endef

define Package/kissdx/description
	kissdx is a PC-Link clone for KiSS media players, based for
	the most part on kissd (which it now replaces), with added
	features for media playback, management, flexibility and more.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CFLAGS="$(TARGET_CFLAGS) -DLinux -DUSE_INTERNAL_SENDFILE -std=gnu99 -Wall -Wstrict-prototypes -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -DFILE_OFFSET_BITS=64 -D_GNU_SOURCE -I$(ICONV_PREFIX)/include -I$(STAGING_DIR)/usr/include/dvdread $(TARGET_CPPFLAGS) $(TARGET_LDFLAGS) -L$(ICONV_PREFIX)/lib" \
		LDFLAGS="-L$(ICONV_PREFIX)/lib $(TARGET_LDFLAGS)" \
		STRIP=$(STRIP) \
		IconvExternal=defined \
		CC="$(TARGET_CROSS)gcc" all
endef

define Package/kissdx/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/kissdx $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/kissdx.config $(1)/etc/config/kissdx
	$(INSTALL_BIN) ./files/kissdx.init $(1)/etc/init.d/kissdx
endef

define Package/kissdx/conffiles
/etc/config/kissdx
endef

$(eval $(call BuildPackage,kissdx))
