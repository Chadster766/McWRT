#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pybluez
PKG_VERSION:=0.13
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-src-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://org.csail.mit.edu/pybluez/release/
PKG_MD5SUM:=eb38d423b49255a7b9f5ba1f50844b53

PKG_BUILD_DEPENDS:=python

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/python-bluez
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=Python wrapper for the BlueZ Bluetooth stack
  URL:=http://code.google.com/p/pybluez/
  DEPENDS:=+python-mini +bluez-libs
endef

define Package/python-psycopg/description
 This package contains is a Python wrapper for the BlueZ Bluetooth stack
endef

CONFIGURE_ARGS += \
	--with-python="$(PYTHON)" \

define Build/Compile
	$(if $(Build/Compile/PyMod),,@echo Python packaging code not found.; false)
	$(call Build/Compile/PyMod,., \
		install --prefix="$(PKG_INSTALL_DIR)/usr", \
	)
endef

define PyPackage/python-bluez/filespec
+|$(PYTHON_PKG_DIR)/bluetooth/__init__.py
+|$(PYTHON_PKG_DIR)/bluetooth/_bluetooth.so
+|$(PYTHON_PKG_DIR)/bluetooth/bluez.py
+|$(PYTHON_PKG_DIR)/bluetooth/btcommon.py
endef

$(eval $(call PyPackage,python-bluez))
$(eval $(call BuildPackage,python-bluez))
