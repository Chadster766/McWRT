#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sslstrip
PKG_VERSION:=0.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.thoughtcrime.org/software/sslstrip
PKG_MD5SUM:=7fedb46e682c2d8ef3fbc52da5e002da

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/sslstrip
  SECTION:=net
  CATEGORY:=Network
  TITLE:=sslstrip
  URL:=http://www.thoughtcrime.org/software/sslstrip/index.html
  DEPENDS:=+python +twisted +twisted-web +pyopenssl
endef

define Package/sslstrip/description
  sslstrip will transparently hijack HTTP traffic on a network,
  watch for HTTPS links and redirects, then map those links
  into either look-alike HTTP links or homograph-similar HTTPS
  links. It also supports modes for supplying a favicon which
  looks like a lock icon, selective logging, and session denial.
endef

define Build/Compile
	$(call Build/Compile/PyMod,,install --prefix="$(PKG_INSTALL_DIR)/usr")
endef

define Package/sslstrip/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) \
	    $(PKG_INSTALL_DIR)/usr/bin/sslstrip \
	    $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/share/sslstrip
	$(CP) \
	    $(PKG_INSTALL_DIR)/usr/share/sslstrip/* \
	    $(1)/usr/share/sslstrip
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)
	$(CP) \
	    $(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/* \
	    $(1)$(PYTHON_PKG_DIR)
endef

$(eval $(call BuildPackage,sslstrip))
