#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pyOSC
PKG_VERSION:=0.3.5b-5294
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://trac.v2.nl/raw-attachment/wiki/pyOSC
PKG_MD5SUM:=933f85d1ae76f31e711d4a2228507dd8

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DEPENDS:=python

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/python-pyosc
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=python-pyosc
  URL:=http://trac.v2.nl/wiki/pyOSC
  DEPENDS:=+python
  MAINTAINER:=Hamish Guthrie <hcg@openwrt.org>
endef

define Package/python-pyosc/description
  A simple OpenSoundControl implementation in pure Python
endef

define Build/Compile
	$(INSTALL_DIR) $(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)
	$(CP) \
		$(PKG_BUILD_DIR)/OSC.py \
       		$(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)
endef

define Package/python-pyosc/install
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)
	$(CP) \
		$(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/OSC.py \
		$(1)$(PYTHON_PKG_DIR)
endef

$(eval $(call BuildPackage,python-pyosc))
