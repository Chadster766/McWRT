#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=web.py
PKG_VERSION:=0.37
PKG_RELEASE:=2

PKG_SOURCE:=web.py-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://webpy.org/static/
PKG_MD5SUM:=93375e3f03e74d6bf5c5096a4962a8db

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DEPENDS:=python

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/python-webpy
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=python-webpy
  URL:=http://webpy.org
  DEPENDS:=+python
  MAINTAINER:=Hamish Guthrie <hcg@openwrt.org>
endef

define Package/python-web.py/description
  Think about the ideal way to write a web app. Write the code to make it happen.
endef

define Build/Compile
	$(INSTALL_DIR) $(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)
	$(CP) \
		$(PKG_BUILD_DIR)/web \
       		$(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)
endef

define Package/python-webpy/install
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)
	$(CP) \
		$(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/web \
		$(1)$(PYTHON_PKG_DIR)
endef

$(eval $(call BuildPackage,python-webpy))
