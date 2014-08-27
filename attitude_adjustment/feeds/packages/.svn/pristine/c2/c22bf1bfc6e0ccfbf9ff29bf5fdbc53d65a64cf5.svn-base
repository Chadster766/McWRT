#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libdnet
PKG_VERSION:=1.11
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=04c394ed8e1e7fc455456e79e908916d

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

PKG_CONFIG_DEPENDS:=\
	CONFIG_PACKAGE_libdnet \
	CONFIG_PACKAGE_pydnet

PKG_BUILD_DEPENDS:=PACKAGE_pydnet:python

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/libdnet/Default
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Low-level network library
  URL:=http://sourceforge.net/projects/libdnet/
endef

define Package/libdnet/Default/description
	libdnet is a library of simplified, portable interface to several
	low-level networking routines.
endef

define Package/libdnet
  $(call Package/libdnet/Default)
endef

define Package/libdnet/description
  $(call Package/libdnet/Default/description)
endef

define Package/pydnet
  $(call Package/libdnet/Default)
  DEPENDS:=libdnet +python-mini
endef

define Package/pydnet/description
  $(call Package/libdnet/Default/description)
  This package contains the python dnet library.
endef

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--without-check \
	--without-python

CONFIGURE_VARS += \
	ac_cv_dnet_bsd_bpf=no

MAKE_FLAGS += \
	CFLAGS="$(TARGET_CFLAGS) -D_GNU_SOURCE"

define Build/Configure
	(cd $(PKG_BUILD_DIR); \
		ln -sf config/acinclude.m4 . ; \
		autoreconf -v --install || exit 1 \
	);
	$(call Build/Configure/Default)
endef

define Build/Compile
	$(call Build/Compile/Default)
	$(if $(CONFIG_PACKAGE_pydnet), \
		$(call Build/Compile/PyMod, \
			python/, \
			build \
		) \
	)
endef

define Build/Install
	$(call Build/Install/Default)
	$(if $(CONFIG_PACKAGE_pydnet), \
		$(call Build/Compile/PyMod, \
			python/, \
			install --no-compile --prefix $(PKG_INSTALL_DIR)/usr \
		) \
	)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(2)/bin
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/bin/dnet-config \
		$(2)/bin/
	$(SED) \
		's,^\(prefix\|exec_prefix\)=.*,\1=$(STAGING_DIR)/usr,g' \
		$(2)/bin/dnet-config

	$(INSTALL_DIR) $(1)/usr/include
	$(INSTALL_DATA)\
		$(PKG_INSTALL_DIR)/usr/include/dnet.h \
		$(1)/usr/include/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/dnet \
		$(1)/usr/include/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libdnet.{la,a,so*} \
		$(1)/usr/lib/
endef

define Package/libdnet/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libdnet.so.* \
		$(1)/usr/lib/

	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/sbin/dnet \
		$(1)/usr/sbin/
endef

define Package/pydnet/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/python* $(1)/usr/lib/
endef


$(eval $(call BuildPackage,libdnet))
$(eval $(call BuildPackage,pydnet))
