#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=psycopg
PKG_VERSION:=1.1.21
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.initd.org/pub/software/psycopg/
PKG_MD5SUM:=a31f79f68d6d32898d6f24e11369a106

PKG_BUILD_DEPENDS:=python

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/python-psycopg
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=PostgreSQL database adapter for Python
  URL:=http://www.initd.org/
  DEPENDS:=+python-mini +libpq +python-egenix-mx
endef

define Package/python-psycopg/description
 This package contains is a PostgreSQL database adapter for the Python 
 programming language.
endef

CONFIGURE_ARGS += \
	--with-python="$(PYTHON)" \
	--with-postgres-libraries="$(STAGING_DIR)/usr/lib" \
	--with-postgres-includes="$(STAGING_DIR)/usr/include" \
	--with-mxdatetime-includes="$(STAGING_DIR)$(PYTHON_PKG_DIR)/mx/DateTime/mxDateTime" \
	--with-python-prefix=$(STAGING_DIR)/usr
	--with-python-exec-prefix=$(STAGING_DIR)/usr

define Build/Compile
	$(if $(Build/Compile/PyMod),,@echo Python packaging code not found.; false)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		OPT="$(TARGET_CFLAGS)"
endef

define PyPackage/python-psycopg/install
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/psycopgmodule.so $(1)$(PYTHON_PKG_DIR)/
endef

$(eval $(call PyPackage,python-psycopg))
$(eval $(call BuildPackage,python-psycopg))
