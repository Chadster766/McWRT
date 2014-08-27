#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=python-imaging-library
PKG_VERSION:=1.1.7
PKG_RELEASE:=2

PKG_SOURCE:=Imaging-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://effbot.org/downloads
PKG_MD5SUM:=fc14a54e1ce02a0225be8854bfba478e

PKG_BUILD_DIR:=$(BUILD_DIR)/Imaging-$(PKG_VERSION)
PKG_BUILD_DEPENDS:=python

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/python-imaging-library
  SECTION:=language-python
  CATEGORY:=Languages
  SUBMENU:=Python
  TITLE:=Python Imaging Library (PIL)
  URL:=http://www.pythonware.com/products/pil/
  DEPENDS:=+python +libfreetype +libjpeg +zlib
endef

define Package/python-imaging-library/description
 The Python Imaging Library adds image processing capabilities to your
 Python interpreter.
 .
 This library provides extensive file format support, an efficient
 internal representation, and fairly powerful image processing
 capabilities.
 .
 The core image library is designed for fast access to data stored in a
 few basic pixel formats. It should provide a solid foundation for a
 general image processing tool.
endef

define Build/Compile
	$(call Build/Compile/PyMod,., \
		install --prefix="/usr" --root="$(PKG_INSTALL_DIR)", \
	)
endef

define Package/python-imaging-library/install
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)
	$(CP) \
		$(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/* \
		$(1)$(PYTHON_PKG_DIR)/
endef

$(eval $(call BuildPackage,python-imaging-library))
