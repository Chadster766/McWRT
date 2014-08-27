#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pypcap
PKG_VERSION:=1.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://pypcap.googlecode.com/files
PKG_MD5SUM:=034c3cbbfa81aa19e8f685b767c65764

PKG_BUILD_DEPENDS:=python

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/python-pcap
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=Python interface to lipcap
  URL:=http://code.google.com/p/pypcap/
  DEPENDS:=+python-mini +libpcap
endef

define PyPackage/python-pcap/filespec
+|$(PYTHON_PKG_DIR)/pcap.so
endef

TARGET_CFLAGS += -DHAVE_PCAP_FILE=1

define Build/Compile
	$(if $(Build/Compile/PyMod),,@echo Python packaging code not found.; false)
	$(call Build/Compile/PyMod,., \
		install --prefix="$(PKG_INSTALL_DIR)/usr", \
		PCAP_HOME="$(STAGING_DIR)/usr" \
	)
endef

$(eval $(call PyPackage,python-pcap))
$(eval $(call BuildPackage,python-pcap))
