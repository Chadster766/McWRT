# 
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sg3-utils
PKG_VERSION:=1.24
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_SOURCE_URL:=http://ftp.de.debian.org/debian/pool/main/s/$(PKG_NAME)
PKG_MD5SUM:=9e1b8811b013d8f97d8ea2e0942cebad

include $(INCLUDE_DIR)/package.mk

define Package/sg3-utils
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+kmod-usb-storage
  TITLE:=sg3 scsi utilities
  URL:=http://sg.danny.cz/sg/sg3_utils.html
endef

define Package/sg3-utils/description
  The package includes a number of utilities to allow a user
  to use the sg interface to find out low-level information
  about any SCSI device.
endef

define Build/Configure
endef


define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) -f no_lib/Makefile.linux \
		DESTDIR=$(STAGING_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -DSG3_UTILS_LINUX" \
		sg_start
endef


define Package/sg3-utils/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sg_start $(1)/usr/bin
endef


$(eval $(call BuildPackage,sg3-utils))
