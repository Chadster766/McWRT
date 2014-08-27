#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pysqlite
PKG_VERSION:=2.3.5
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://initd.org/pub/software/pysqlite/releases/2.3/2.3.5/
PKG_MD5SUM:=b4a185e936848370fcc1a5b17755b641

PKG_BUILD_DEPENDS:=python

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/python-sqlite
 SUBMENU:=Python
 SECTION:=lang
 CATEGORY:=Languages
 TITLE:=SQLite database adapter for Python
 URL:=http://www.initd.org/tracker/pysqlite/wiki/pysqlite
 DEPENDS:=+python-mini +libsqlite3
endef

define Package/python-sqlite/description
 This package contains an SQLite database adapter for the Python 
 programming language.
endef

define PyPackage/python-sqlite/filespec
+|$(PYTHON_PKG_DIR)/pysqlite2
-|$(PYTHON_PKG_DIR)/pysqlite2/test
endef

define Build/Configure
	cp ./files/setup.cfg.in $(PKG_BUILD_DIR)/setup.cfg
	$(SED) \
		's,@INCLUDE_DIRS@,$(STAGING_DIR)/usr/include,g' \
		-e 's,@INCLUDE_DIRS@,$(STAGING_DIR)/usr/include,g' \
		$(PKG_BUILD_DIR)/setup.cfg
endef

define Build/Compile
	$(if $(Build/Compile/PyMod),,@echo Python packaging code not found.; false)
	$(call Build/Compile/PyMod,., \
		install --prefix="$(PKG_INSTALL_DIR)/usr", \
	)
endef

$(eval $(call PyPackage,python-sqlite))
$(eval $(call BuildPackage,python-sqlite))
