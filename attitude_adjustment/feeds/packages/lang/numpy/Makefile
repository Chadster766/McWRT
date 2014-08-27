#
# Copyright (C) 2008-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=numpy
PKG_VERSION:=1.0.4
PKG_RELEASE:=1
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/numpy
PKG_MD5SUM:=8f011e9d7697da570efaac61c51725e0

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/numpy
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=numpy
  URL:=http://numpy.sf.net
  DEPENDS:=+python-mini @TARGET_x86||TARGET_olpc||TARGET_uml @BROKEN
endef

define Package/numpy/description
  API to GTK and glade library from python
endef

define Build/Compile
	cd $(PKG_BUILD_DIR) ; $(CONFIGURE_VARS) $(MAKE_VARS) python2.5 ./setup.py install --prefix="$(PKG_INSTALL_DIR)/usr"
endef

define Package/numpy/install
	$(INSTALL_DIR) $(1)
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
endef

define Require/python25-dev
[ -e /usr/include/python2.5/pyconfig.h ]
endef

$(eval $(call Require,python25-dev, \
	Please install Python 2.5 (including development headers and libraries) \
))

$(eval $(call BuildPackage,numpy))

