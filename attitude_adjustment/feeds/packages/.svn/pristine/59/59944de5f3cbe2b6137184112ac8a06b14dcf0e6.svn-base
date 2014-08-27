#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mimms
PKG_VERSION:=3.2.1
PKG_RELEASE:=1

PKG_SOURCE:=mimms-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://download.savannah.gnu.org/releases/mimms
PKG_MD5SUM:=ec629d8899551b4789ba15c17402c36f

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DEPENDS:=python

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/python-mimms
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=python-mimms
  URL:=http://savannah.nongnu.org/projects/mimms
  DEPENDS:=+python +libmms
  MAINTAINER:=Hamish Guthrie <hcg@openwrt.org>
endef

define Package/python-mimms/description
  mimms is an mms (e.g. mms://) stream downloader
endef

define Build/Compile
	$(INSTALL_DIR) $(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/libmimms
	$(CP) \
		$(PKG_BUILD_DIR)/libmimms \
       		$(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/libmimms
	$(CP) \
		$(PKG_BUILD_DIR)/mimms \
       		$(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)
endef

define Package/python-mimms/install
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)/libmimms
	$(CP) \
		$(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/libmimms \
		$(1)$(PYTHON_PKG_DIR)/libmimms
	$(CP) \
		$(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/mimms \
		$(1)$(PYTHON_PKG_DIR)
endef

$(eval $(call BuildPackage,python-mimms))
