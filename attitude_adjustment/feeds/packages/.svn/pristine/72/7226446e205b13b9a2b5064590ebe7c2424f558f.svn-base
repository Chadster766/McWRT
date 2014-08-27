#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libart
PKG_VERSION:=2.3.21
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)_lgpl-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@GNOME/$(PKG_NAME)_lgpl/2.3
PKG_MD5SUM:=08559ff3c67fd95d57b0c5e91a6b4302

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)_lgpl-$(PKG_VERSION)

PKG_FIXUP:=autoreconf

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libart
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=An high-performance 2D graphics library
  URL:=http://www.levien.com/libart/
endef

define Package/libart/description
 A library of functions for 2D graphics supporting a superset of the
 PostScript imaging model, designed to be integrated with graphics,
 artwork, and illustration programs. It is written in optimized C, and
 is fully compatible with C++. With a small footprint of 10,000 lines of
 code, it is especially suitable for embedded applications.
endef

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \

TARGET_CFLAGS += $(FPIC)

define Build/InstallDev
	$(INSTALL_DIR) $(2)/bin $(1)/usr/include $(1)/usr/lib/pkgconfig
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/bin/libart2-config \
		$(2)/bin/
	$(SED) \
		's,^\(prefix\|exec_prefix\)=.*,\1=$(STAGING_DIR)/usr,g' \
		$(2)/bin/libart2-config

	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/libart-2.0 \
			$(1)/usr/include/

	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libart_lgpl_2.{la,a,so*} \
		$(1)/usr/lib/

	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libart-2.0.pc \
		$(1)/usr/lib/pkgconfig/
endef

define Package/libart/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libart_lgpl_2.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libart))
