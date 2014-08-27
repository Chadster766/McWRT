#
# Copyright (C) 2008-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=telepathy-python
PKG_VERSION:=0.15.3
PKG_RELEASE:=1
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://telepathy.freedesktop.org/releases/telepathy-python/
PKG_MD5SUM:=594a66a70764052e30df9428b5be52b2

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/telepathy-python
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=telepathy-python
  URL:=http://telepathy.freedesktop.org
  DEPENDS:=+python-mini +dbus-python
endef

define Package/telepathy-python/description
  telepathy python bindings
endef

define Build/Compile
	$(call Build/Compile/PyMod,,install --prefix="$(PKG_INSTALL_DIR)/usr")
endef

define Package/telepathy-python/install
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)
	$(CP) \
	    $(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/* \
	    $(1)$(PYTHON_PKG_DIR)
endef

$(eval $(call RequireCommand,xsltproc, \
	$(PKG_NAME) requires xsltproc installed on the host-system. \
))

$(eval $(call BuildPackage,telepathy-python))

