#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ipython
PKG_VERSION:=0.8.2
PKG_RELEASE:=1
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://ipython.scipy.org/dist/
PKG_FIXUP:=autoreconf
PKG_MD5SUM:=fd5a1b6e518898a4aee8b8d8b428af70

include $(INCLUDE_DIR)/package.mk

define Package/ipython
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=ipython
  URL:=http://ipython.scipy.org/
endef

define Package/ipython/description
  An enhanced interactive Python shell from the SciPy project
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR); \
		if [ -x $(CONFIGURE_CMD) ]; then \
			$(CP) $(SCRIPT_DIR)/config.{guess,sub} $(PKG_BUILD_DIR)/ && \
			$(CONFIGURE_VARS) \
			PYTHON=/usr/bin/python \
			$(CONFIGURE_CMD) \
			$(CONFIGURE_ARGS) ;\
		fi \
	)
endef

define Build/Compile
	cd $(PKG_BUILD_DIR) ; $(CONFIGURE_VARS) ./setup.py install --prefix="$(PKG_INSTALL_DIR)/usr"
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
endef

define Package/ipython/install
	$(INSTALL_DIR) $(1)
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
endef

$(eval $(call BuildPackage,ipython))
