#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=taglib
PKG_VERSION:=1.6.3
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://developer.kde.org/~wheeler/files/src/
PKG_MD5SUM:=ddf02f4e1d2dc30f76734df806e613eb

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

PKG_INSTALL:=1

define Package/taglib
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:= $(CXX_DEPENDS) +zlib @!TARGET_avr32
  TITLE:=Audio output library
  URL:=http://developer.kde.org/~wheeler/taglib.html
endef

define Package/taglibc
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+taglib
  TITLE:=Audio output library
  URL:=http://developer.kde.org/~wheeler/taglib.html
endef

TARGET_CFLAGS += $(FPIC)

define Build/InstallDev
	$(INSTALL_DIR) $(2)/bin
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/bin/taglib-config \
		$(2)/bin/
	$(SED) \
		's,^\(prefix\|exec_prefix\)=.*,\1=$(STAGING_DIR)/usr,g' \
		$(2)/bin/taglib-config

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libtag{,_c}.so* \
		$(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/include/taglib
	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/include/taglib/* \
		$(1)/usr/include/taglib/
endef

define Package/taglib/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libtag.so.* $(1)/usr/lib/
endef

define Package/taglibc/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libtag_c.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,taglib))
$(eval $(call BuildPackage,taglibc))
