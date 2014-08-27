#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=orbit2
PKG_VERSION:=2.14.16
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/ORBit2-$(PKG_VERSION)
HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/ORBit2-$(PKG_VERSION)
PKG_SOURCE:=ORBit2-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@GNOME/ORBit2/2.14
#PKG_MD5SUM:=d7fb9eb41e8196f1aacf2f08375ae555

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

PKG_INSTALL:=1

HOST_BUILD_DEPENDS:=libIDL2/host
PKG_BUILD_DEPENDS:=orbit2/host

BIGENDIAN=no
ifeq ($(CONFIG_BIG_ENDIAN),y)
	BIGENDIAN:=yes
endif

CONFIGURE_VARS += \
	ac_cv_c_bigendian=$(BIGENDIAN) \
	ac_cv_alignof_CORBA_octet=1 \
	ac_cv_alignof_CORBA_boolean=1 \
	ac_cv_alignof_CORBA_char=1 \
	ac_cv_alignof_CORBA_wchar=2 \
	ac_cv_alignof_CORBA_short=2 \
	ac_cv_alignof_CORBA_long=4 \
	ac_cv_alignof_CORBA_long_long=4 \
	ac_cv_alignof_CORBA_float=4 \
	ac_cv_alignof_CORBA_double=4 \
	ac_cv_alignof_CORBA_long_double=4 \
	ac_cv_alignof_CORBA_struct=1 \
	ac_cv_alignof_CORBA_pointer=4

define Package/orbit2
  SECTION:=xorg-libraries
  CATEGORY:=Xorg
  SUBMENU:=libraries
  TITLE:=ORBit2
  URL:=http://www.gnome.org/projects/ORBit2/
  DEPENDS:=+glib2 +libIDL2
endef

define Package/orbit2/description
ORBit2 is a CORBA 2.4-compliant Object Request Broker (ORB).
endef

define Build/Configure
	$(call Build/Configure/Default)
	find $(PKG_BUILD_DIR) -name Makefile -exec sed -i 's,IDL_COMPILER.*orbit-idl-2,IDL_COMPILER = $(STAGING_DIR_HOST)/bin/orbit-idl-2,g' {} \; # tell orbit2 to use the host-built idl-compiler
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/{include/orbit-2.0,lib/pkgconfig}

	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/*.{so*,a,la} \
		$(1)/usr/lib/

	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/* \
		$(1)/usr/lib/pkgconfig/

	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/orbit-2.0/* \
		$(1)/usr/include/orbit-2.0/

	$(INSTALL_DIR) $(1)/usr/share/aclocal
	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/share/aclocal/* \
		$(1)/usr/share/aclocal/

	$(INSTALL_DIR) $(1)/usr/share/idl/orbit-2.0
	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/share/idl/orbit-2.0/* \
		$(1)/usr/share/idl/orbit-2.0/
endef

define Package/orbit2/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libORBit*.so* \
		$(1)/usr/lib/
endef

$(eval $(call HostBuild))
$(eval $(call BuildPackage,orbit2))
