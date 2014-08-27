#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=unzip
PKG_VERSION:=5.52
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)552.zip
PKG_SOURCE_URL:=@SF/infozip
PKG_MD5SUM:=3d55b4e7375c3d0b81feca52dc0e4d48

include $(INCLUDE_DIR)/package.mk

PKG_UNPACK=unzip -d $(PKG_BUILD_DIR)/ $(DL_DIR)/$(PKG_SOURCE)

define Package/unzip
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=
  TITLE:=De-archiver for .zip files
  URL:=http://infozip.sourceforge.net/UnZip.html
  SUBMENU:=compression
endef

define Package/unzip/description
	InfoZIP's unzip program. With the exception of multi-volume archives 
	(ie, .ZIP files that are split across several disks using PKZIP's /& option), 
	this can handle any file produced either by PKZIP, or the corresponding 
	InfoZIP zip program.
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) -f unix/Makefile generic \
		$(TARGET_CONFIGURE_OPTS) \
		prefix="$(PKG_INSTALL_DIR)/usr" \
		CFLAGS="$(TARGET_CFLAGS)" \
		CF="$(TARGET_CFLAGS) -O $(TARGET_CPPFLAGS) -I. -DUNIX" \
		LD="$(TARGET_CC) $(TARGET_LDFLAGS)" \
		install
endef

define Package/unzip/install	
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/* $(1)/usr/bin/
endef

$(eval $(call BuildPackage,unzip))
