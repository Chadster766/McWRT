#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=kid
PKG_VERSION:=0.9.6
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.kid-templating.org/dist/0.9.6/dist
PKG_MD5SUM:=31e7156698a6f214b4067afb9e13728f

PKG_BUILD_DEPENDS:=python

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/python-kid
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  DEPENDS:=+python
  TITLE:=XML-based Templating
  URL:=http://www.kid-templating.org/
endef

define Package/python-kid/description
 Kid is a simple template language for XML based vocabularies written in 
 Python.
endef

define PyPackage/python-kid/filespec
+|$(PYTHON_PKG_DIR)/kid
-|$(PYTHON_PKG_DIR)/kid/test
endef

define Build/Compile
	$(if $(Build/Compile/PyMod),,@echo Python packaging code not found.; false)
	$(call Build/Compile/PyMod,., \
		install --prefix="$(PKG_INSTALL_DIR)/usr", \
	)
endef

$(eval $(call PyPackage,python-kid))
$(eval $(call BuildPackage,python-kid))
