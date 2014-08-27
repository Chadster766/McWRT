#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=logilab-common
PKG_VERSION:=0.29.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.logilab.fr/pub/common/
PKG_MD5SUM:=90f679e0d4ed19e2bf083fb932cde922

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/logilab-common
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=logilab-common
  URL:=http://www.logilab.fr
  DEPENDS:=+python-mini
endef

define Package/logilab-common/description
  a bunch of modules providing low level functionnalities shared among some python projects devel
endef

define Build/Compile
	$(if $(Build/Compile/PyMod),,@echo Python packaging code not found.; false)
	$(call Build/Compile/PyMod,., \
		install --prefix="$(PKG_INSTALL_DIR)/usr", \
	)
endef

define Package/logilab-common/install
	$(INSTALL_DIR) $(1)
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
endef

$(eval $(call BuildPackage,logilab-common))
