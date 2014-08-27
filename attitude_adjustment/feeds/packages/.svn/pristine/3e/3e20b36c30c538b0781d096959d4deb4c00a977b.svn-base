#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=python-cjson
PKG_VERSION:=1.0.5
PKG_RELEASE:=1

PKG_SOURCE:=python-cjson-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://pypi.python.org/packages/source/p/python-cjson/
PKG_MD5SUM:=4d55b66ecdf0300313af9d030d9644a3

PKG_BUILD_DIR:=$(BUILD_DIR)/python-cjson-$(PKG_VERSION)
PKG_BUILD_DEPENDS:=python

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/python-cjson
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=python-cjson
  URL:=http://pypi.python.org/pypi/python-cjson/
  DEPENDS:=+python
endef

define Package/python-cjson/description
  Fast JSON encoder/decoder for Python
endef

define Build/Compile
	$(call Build/Compile/PyMod,,install --prefix="$(PKG_INSTALL_DIR)/usr")
endef

define Package/python-cjson/install
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)
	$(CP) \
	    $(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/* \
	    $(1)$(PYTHON_PKG_DIR)
endef

$(eval $(call BuildPackage,python-cjson))
