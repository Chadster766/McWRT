#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=jsonpath
PKG_VERSION:=0.52.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.ultimate.com/phil/python/download/
PKG_MD5SUM:=4cd0c3a70acbc67d2915d64f132cdb4f
PKG_BUILD_DEPENDS:=python

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/jsonpath
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=JsonPath - XPath implementation
  URL:=http://goessner.net/articles/JsonPath/
  DEPENDS:=+python
endef

define Package/jsonpath/description
  An XPath implementation for JSON
endef

define Build/Compile
	$(call Build/Compile/PyMod,,install --prefix="$(PKG_INSTALL_DIR)/usr")
endef

define Package/jsonpath/install
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)
	$(CP) \
	    $(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/* \
	    $(1)$(PYTHON_PKG_DIR)
endef

$(eval $(call BuildPackage,jsonpath))
