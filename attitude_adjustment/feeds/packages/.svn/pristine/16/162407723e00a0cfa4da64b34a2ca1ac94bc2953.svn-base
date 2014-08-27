#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=python-ifconfig
PKG_VERSION:=0.1
PKG_RELEASE:=2

PKG_SOURCE:=python-ifconfig-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://python-ifconfig.googlecode.com/files/
PKG_MD5SUM:=6d780d29c1df1acc17195bde7be0e394

PKG_BUILD_DIR:=$(BUILD_DIR)/python-ifconfig-$(PKG_VERSION)
PKG_BUILD_DEPENDS:=python

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/python-ifconfig
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=python-ifconfig
  URL:=http://code.google.com/p/python-ifconfig/
  DEPENDS:=+python
endef

define Package/python-ifconfig/description
  Display network interface status.
endef

define Build/Compile
	$(call Build/Compile/PyMod,,install --prefix="$(PKG_INSTALL_DIR)/usr")
endef

define Package/python-ifconfig/install
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)
	$(CP) \
	    $(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/* \
	    $(1)$(PYTHON_PKG_DIR)
endef

$(eval $(call BuildPackage,python-ifconfig))
