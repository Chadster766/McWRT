#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=xapian-bindings
PKG_VERSION:=1.2.8
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://oligarchy.co.uk/xapian/$(PKG_VERSION)
PKG_MD5SUM:=4bfe673c71ac074e349ef691d7ba56d0

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

PKG_INSTALL=1

define Package/python-xapian
  SECTION:=lang
  CATEGORY:=Languages
  SUBMENU:=Python
  TITLE:=xapian python bindings
  URL:=http://xapian.org/
  DEPENDS:=+python-mini +libxapian
endef

MAKE_FLAGS += \
	pkgpylib_DATA=xapian/__init__.py

CONFIGURE_ARGS+= \
	--with-python \

CONFIGURE_VARS+= \
	PYTHON_LIB="$(PYTHON_PKG_DIR)" \

define Package/python-xapian/install
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)
	$(CP) $(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/* $(1)$(PYTHON_PKG_DIR)/
endef

$(eval $(call BuildPackage,python-xapian))


