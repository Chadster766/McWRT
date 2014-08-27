#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tcl
PKG_VERSION:=8.5.8
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)$(PKG_VERSION)-src.tar.gz
PKG_SOURCE_URL:=@SF/tcl
PKG_MD5SUM:=7f123e53b3daaaba2478d3af5a0752e3

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)$(PKG_VERSION)

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/tcl
  SECTION:=lang
  CATEGORY:=Languages
  DEPENDS:=+libpthread
  TITLE:=TCL Lang
  URL:=http://www.tcl.tk
endef

define Package/tcl/description
Tcl (Tool Command Language) is a very powerful but easy to learn dynamic
programming language, suitable for a very wide range of uses, including web and
desktop applications, networking, administration, testing and many more. Open
source and business-friendly, Tcl is a mature yet evolving language that is truly
cross platform, easily deployed and highly extensible.
endef

CONFIGURE_PATH := unix

CONFIGURE_VARS += \
	tcl_cv_strtod_unbroken=ok

CONFIGURE_ARGS += \
	--enable-threads

MAKE_PATH := unix

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/*.h $(1)/usr/include/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libtcl*.{a,so*} $(1)/usr/lib/
endef

define Package/tcl/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so $(1)/usr/lib

	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/* $(1)/usr/bin
endef

$(eval $(call BuildPackage,tcl))
