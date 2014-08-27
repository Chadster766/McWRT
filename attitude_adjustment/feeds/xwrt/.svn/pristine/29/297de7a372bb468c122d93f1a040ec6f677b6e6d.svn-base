# 
# Copyright (C) 2007 Meganetwork.org <info@meganetwork.org>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=webif-netsukuku
PKG_VERSION:=0.1
PKG_RELEASE:=beta

# PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
# PKG_SOURCE_URL:=http://
# PKG_MD5SUM:=90b2b2bef15e307b42a04702385be25d
# PKG_CAT:=zcat
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk

define Package/webif-netsukuku
  SECTION:=admin
  CATEGORY:=Administration
  SUBMENU:=webif
  DEPENDS:=+webif +webif-mesh
  TITLE:=XWrt Netsukuku mesh plugin
endef

define Build/Compile
endef


define Package/webif-netsukuku/install
#	$(CP) -a ./files/* $(1)/
#	chmod -R 755 $(1)/*
endef

$(eval $(call BuildPackage,webif-netsukuku))
