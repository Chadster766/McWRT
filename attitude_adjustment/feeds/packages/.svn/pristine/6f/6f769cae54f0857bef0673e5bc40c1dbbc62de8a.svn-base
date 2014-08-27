#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=cython
PKG_VERSION:=0.10.3
PKG_RELEASE:=1

PKG_SOURCE:=Cython-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.cython.org/
PKG_BUILD_DIR:=$(BUILD_DIR)/Cython-$(PKG_VERSION)
PKG_MD5SUM:=684103ed37472d9beef3a38e0d5c431d

PKG_BUILD_DEPENDS:=python

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/cython
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=cython
  URL:=http://www.cython.org
  DEPENDS:=+python-mini
endef

define Package/cython/description
  Cython is a language that should make writing C extensions for the Python language easier
endef

define Build/Compile
	# <--no-cython-compile> required, otherwise setup.py calls the target-gcc to create so-files which obviously can't be used by <python> on the host system.
	$(call Build/Compile/PyMod,,install --no-cython-compile --prefix="$(PKG_INSTALL_DIR)/usr")
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)
	$(CP) \
	    $(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/* \
	    $(1)$(PYTHON_PKG_DIR)
endef

$(eval $(call BuildPackage,cython))
