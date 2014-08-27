#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=yapsnmp
PKG_VERSION:=0.7.8
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=8a3e9a5b814388d7907c639105cb2365

PKG_BUILD_DEPENDS:=python

PKG_FIXUP:=autoreconf
PKG_REMOVE_FILES:=aclocal.m4 acinclude.m4

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/python-yapsnmp
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=Python interface to Net-SNMP
  URL:=http://yapsnmp.sourceforge.net/
  DEPENDS:=+python-mini +libnetsnmp
endef

define Package/python-yapsnmp/description
 This package contains a Python SNMP module based on the net-snmp (formerly 
 known as ucd-snmp) library. It's composed of a low level interface to the 
 library, created using SWIG, and a higher level python module removing all 
 the complexity out of dealing with SNMP.
endef

define PyPackage/python-yapsnmp/filespec
+|$(PYTHON_PKG_DIR)/netsnmp.py
+|$(PYTHON_PKG_DIR)/netsnmpc.so
+|$(PYTHON_PKG_DIR)/yapsnmp.py
endef

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--enable-site-packages-prefix="$(PYTHON_PKG_DIR)" \

CONFIGURE_VARS += \
	ac_cv_file__usr_include_net_snmp_library="yes" \
	ac_cv_prog_SWIG="echo MISSING SWIG" \

define Build/Compile
	$(if $(Build/Compile/PyMod),,@echo Python packaging code not found.; false)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		INCLUDES="-I$(PYTHON_INC_DIR)" \
		SNMPINCDIR="$(STAGING_DIR)/usr/include/net-snmp/library" \
		all
	$(MAKE) -C $(PKG_BUILD_DIR) DESTDIR="$(PKG_INSTALL_DIR)" install
endef

$(eval $(call PyPackage,python-yapsnmp))
$(eval $(call BuildPackage,python-yapsnmp))
