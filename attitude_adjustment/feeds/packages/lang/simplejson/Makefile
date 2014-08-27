#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=simplejson
PKG_VERSION:=2.2.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://pypi.python.org/packages/source/s/simplejson/
PKG_MD5SUM:=070c6467462bd63306f1756b01df6d70
PKG_BUILD_DEPENDS:=python

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/simplejson
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=Simple, fast, extensible JSON encoder/decoder for Python
  URL:=http://simplejson.readthedocs.org/
  DEPENDS:=+python +distribute
endef

define Package/simplejson/description
  Simple, fast, extensible JSON encoder/decoder for Python
endef

define Build/Compile
	$(call Build/Compile/PyMod,,install --prefix=/usr --root=$(PKG_INSTALL_DIR))
endef

define Package/simplejson/install
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)
	$(CP) \
	    $(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/* \
	    $(1)$(PYTHON_PKG_DIR)
endef

$(eval $(call BuildPackage,simplejson))
