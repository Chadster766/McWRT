# 
# Copyright (C) 2006 Michele Favara Pedarsi <mik.fp@commentaria.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=webif-mesh
PKG_VERSION:=0.1
PKG_RELEASE:=beta

# PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
# PKG_SOURCE_URL:=http://
# PKG_MD5SUM:=90b2b2bef15e307b42a04702385be25d
# PKG_CAT:=zcat
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk

define Package/webif-mesh
  SECTION:=admin
  CATEGORY:=Administration
  SUBMENU:=webif
  DEPENDS:=+webif
  TITLE:=XWrt mesh installation page
endef

define Build/Compile
endef


define Package/webif-mesh/install
	$(CP) -a ./files/* $(1)/
	mkdir -p $(1)/CONTROL
	$(INSTALL_BIN) ./files/www/cgi-bin/webif/* $(1)/www/cgi-bin/webif/
	$(INSTALL_BIN) ./files/etc/init.d/mesh $(1)/etc/init.d/mesh
	$(CP) -a ./ipkg/webif-mesh.postinst $(1)/CONTROL/preinst
	$(CP) -a ./ipkg/webif-mesh.postrm $(1)/CONTROL/postrm
endef

$(eval $(call BuildPackage,webif-mesh))
