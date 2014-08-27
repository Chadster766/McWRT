#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pyrrd
PKG_VERSION:=0.1.0
PKG_RELEASE:=2

PKG_SOURCE:=PyRRD-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://pypi.python.org/packages/source/P/PyRRD/
PKG_MD5SUM:=c33a0760b42a23e45e423b8b9f2cd0b0

PKG_BUILD_DEPENDS:=python
PKG_BUILD_DIR:=$(BUILD_DIR)/PyRRD-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/pyrrd
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=PyRRD - An Object-Oriented Python Interface for RRDTool
  URL:=http://code.google.com/p/pyrrd/
  DEPENDS:=+python +distribute +rrdtool
endef

define Package/pyrrd/description
  An Object-Oriented Python Interface for RRDTool
endef

define Build/Compile
	$(call Build/Compile/PyMod,., \
		install --prefix="/usr" --root="$(PKG_INSTALL_DIR)" \
	)
endef

define Package/pyrrd/install
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)
	$(CP) \
		$(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/* \
		$(1)$(PYTHON_PKG_DIR)/
endef

$(eval $(call BuildPackage,pyrrd))
