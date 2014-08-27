#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libol
PKG_VERSION:=0.3.18
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.balabit.com/downloads/files/libol/0.3
PKG_MD5SUM:=cbadf4b7ea276dfa85acc38a1cc5ff17

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libol
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Support library for syslog-ng
  URL:=http://www.balabit.com/products/syslog_ng/
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--enable-shared \
		--enable-static \
	)
endef

TARGET_CFLAGS += $(FPIC)

# pass CFLAGS again to override -O2 set by configure
MAKE_ARGS += CFLAGS="$(TARGET_CFLAGS)"

define Build/InstallDev
	$(INSTALL_DIR) $(2)/bin
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/bin/libol-config \
		$(2)/bin/
	$(SED) \
		's,^\(prefix\|exec_prefix\)=.*,\1=$(STAGING_DIR)/usr,g' \
		$(2)/bin/libol-config

	$(INSTALL_DIR) $(1)/usr/include/libol
	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/include/libol/*.h \
		$(1)/usr/include/libol/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libol.{a,la,so*} \
		$(1)/usr/lib/
endef

define Package/libol/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libol.so* \
		$(1)/usr/lib/
endef

$(eval $(call BuildPackage,libol))
