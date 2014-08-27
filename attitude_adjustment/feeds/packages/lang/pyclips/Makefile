#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pyclips
PKG_VERSION:=1.0.7.348
PKG_RELEASE:=1

PKG_SOURCE:=pyclips-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/pyclips
PKG_MD5SUM:=c548d62ec951bf9f98c86c70815ed27c

PKG_BUILD_DIR:=$(BUILD_DIR)/pyclips
PKG_BUILD_DEPENDS:=python

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/pyclips
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=pyclips
  URL:=http://sourceforge.net/projects/pyclips/
  DEPENDS:=+python
endef

define Package/pyclips/description
  Python module to interface the CLIPS expert system shell library.
endef

define Build/Compile
	$(call Build/Compile/PyMod,,install --prefix="$(PKG_INSTALL_DIR)/usr")
endef

define Package/pyclips/install
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)
	$(CP) \
	    $(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/* \
	    $(1)$(PYTHON_PKG_DIR)
endef

$(eval $(call BuildPackage,pyclips))
