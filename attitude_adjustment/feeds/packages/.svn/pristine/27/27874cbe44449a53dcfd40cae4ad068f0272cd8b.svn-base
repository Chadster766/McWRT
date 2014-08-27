#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=json-py
PKG_VERSION:=3_4
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).zip
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=921ebfede886a10ff32d6d4b4e216f8f

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

PKG_UNPACK:=unzip -d $(PKG_BUILD_DIR) $(DL_DIR)/$(PKG_SOURCE)

define Package/python-json
	SECTION:=language-python
	CATEGORY:=Languages
	SUBMENU:=Python
	TITLE:=json-py
	URL:=http://sourceforge.net/projects/json-py/
	DEPENDS:=+python-mini
endef

define Build/Configure
endef

define Build/Compile
	$(INSTALL_DIR) $(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/json.py \
		$(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)
endef

define Package/python-json/install
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/json.py \
		$(1)$(PYTHON_PKG_DIR)
endef

$(eval $(call BuildPackage,python-json))
