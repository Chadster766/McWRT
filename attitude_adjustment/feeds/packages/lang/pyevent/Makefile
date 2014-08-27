#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pyevent
PKG_VERSION:=0.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://pyevent.googlecode.com/files/
PKG_MD5SUM:=584912c92d08bf005283fb29a47a6e4d

PKG_BUILD_DEPENDS:=python

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/python-event
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=Python interface to libevent
  URL:=http://code.google.com/p/pyevent/
  DEPENDS:=+python-mini +libevent2
endef

define PyPackage/python-event/filespec
+|$(PYTHON_PKG_DIR)/event.so
endef

define Build/Compile
	$(if $(Build/Compile/PyMod),,@echo Python packaging code not found.; false)
	$(call Build/Compile/PyMod,., \
		install --prefix="$(PKG_INSTALL_DIR)/usr", \
		LIBEVENT_HOME="$(STAGING_DIR)/usr/lib" \
	)
endef

$(eval $(call PyPackage,python-event))
$(eval $(call BuildPackage,python-event))
