#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=python-django
PKG_VERSION:=1.3.1
PKG_RELEASE:=1

PKG_SOURCE:=Django-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://media.djangoproject.com/releases/1.3/
PKG_MD5SUM:=62d8642fd06b9a0bf8544178f8500767

PKG_BUILD_DIR:=$(BUILD_DIR)/Django-$(PKG_VERSION)
PKG_BUILD_DEPENDS:=python

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/python-django
  SECTION:=language-python
  CATEGORY:=Languages
  SUBMENU:=Python
  TITLE:=Django Web framework
  URL:=https://www.djangoproject.com/
  DEPENDS:=+python
endef

define Package/python-django/description
 Django is a high-level Python Web framework that encourages rapid
 development and clean, pragmatic design.
endef

define Build/Compile
	$(call Build/Compile/PyMod,., \
		install --prefix="/usr" --root="$(PKG_INSTALL_DIR)" \
	)
endef

define Package/python-django/install
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)
	$(CP) \
		$(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/* \
		$(1)$(PYTHON_PKG_DIR)/
endef

$(eval $(call BuildPackage,python-django))
