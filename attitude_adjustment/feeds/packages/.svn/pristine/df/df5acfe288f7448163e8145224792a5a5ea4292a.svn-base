#
# Copyright (C) 2008-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gst-python
PKG_VERSION:=0.10.13
PKG_RELEASE:=1
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://gstreamer.freedesktop.org/src/gst-python/
PKG_MD5SUM:=870440c6aad304112961ef5700d7698f
PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk
$(call include_mk, python-package.mk)


TARGET_CFLAGS += $(FPIC)
CONFIGURE_VARS += PYTHON=$(HOST_PYTHON)

define Package/gst-python
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=python bindings for gstreamer
  URL:=http://gstreamer.freedesktop.org/
  DEPENDS:=+gstreamer +gst-plugins-base +python-gobject +python-mini
endef

define Package/gst-python/description
  python bindings for gstreamer
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig/
	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/* \
		$(1)/usr/lib/pkgconfig/

	$(SED) \
		"s,^defsdir=.*,defsdir=$(STAGING_DIR)/usr/share/gst-python/0.10/defs/,g" \
		$(1)/usr/lib/pkgconfig/gst-python-0.10.pc

	$(INSTALL_DIR) $(1)/usr/share/gst-python/0.10/defs
	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/share/gst-python/0.10/defs/* \
		$(1)/usr/share/gst-python/0.10/defs/
endef

define Package/gst-python/install
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)/gst-0.10/gst/extend
	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/{gstoption.so,pygst.pth,pygst.py} \
		$(1)$(PYTHON_PKG_DIR)/

	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/gst-0.10/gst/*.{so,py} \
		$(1)$(PYTHON_PKG_DIR)/gst-0.10/gst/

	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/gst-0.10/gst/extend/*.py \
		$(1)$(PYTHON_PKG_DIR)/gst-0.10/gst/extend/
endef

$(eval $(call BuildPackage,gst-python))
