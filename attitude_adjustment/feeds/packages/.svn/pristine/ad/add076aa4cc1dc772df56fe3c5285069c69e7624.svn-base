#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=deluge
PKG_VERSION:=1.2.0
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://download.deluge-torrent.org/source/
PKG_MD5SUM:=cec6b48f0abd79ea2cff3815a1124192

PKG_BUILD_DEPENDS:=distribute/host

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/deluge/Default
  SUBMENU:=BitTorrent
  SECTION:=net
  CATEGORY:=Network
  TITLE:=A lightweight BitTorrent client
  URL:=http://deluge-torrent.org/
endef

define Package/deluge
$(call Package/deluge/Default)
  TITLE+= (console UI & daemon)
  DEPENDS+=+python +rblibtorrent +python2-chardet +pyxdg +twisted +twisted-web +pyopenssl
endef

define Package/deluge-ui-gtk
$(call Package/deluge/Default)
  TITLE+= (GTK+ UI)
  DEPENDS+=deluge +python-gtk
endef

define Package/deluge-ui-web
$(call Package/deluge/Default)
  TITLE+= (Web UI)
  DEPENDS+=deluge +mako
endef

define Package/deluge/description
  BitTorrent client with a client/server model.
endef

define Package/deluge/conffiles
/etc/config/deluge
endef

define Build/Compile
	$(call Build/Compile/PyMod,,install --prefix=/usr --root=$(PKG_INSTALL_DIR))

	$(SED) 's,#!$(STAGING_DIR)/host/bin/$(PYTHON),#!/usr/bin/$(PYTHON),' $(PKG_INSTALL_DIR)/usr/bin/*
endef

define Package/deluge/install
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)
	$(CP) \
	    $(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/* \
	    $(1)$(PYTHON_PKG_DIR)/
	rm -rf $(1)$(PYTHON_PKG_DIR)/deluge/ui/gtkui
	rm -rf $(1)$(PYTHON_PKG_DIR)/deluge/ui/web

	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) \
	    $(PKG_INSTALL_DIR)/usr/bin/* \
	    $(1)/usr/bin/
	rm -rf $(1)/usr/bin/deluge-gtk
	rm -rf $(1)/usr/bin/deluge-web

	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/deluge.config $(1)/etc/config/deluge

	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/deluge.init $(1)/etc/init.d/deluge
endef

define Package/deluge-ui-gtk/install
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)/deluge/ui
	$(CP) \
	    $(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/deluge/ui/gtkui \
	    $(1)$(PYTHON_PKG_DIR)/deluge/ui/

	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) \
	    $(PKG_INSTALL_DIR)/usr/bin/deluge-gtk \
	    $(1)/usr/bin/
endef

define Package/deluge-ui-web/install
	$(INSTALL_DIR) $(1)$(PYTHON_PKG_DIR)/deluge/ui
	$(CP) \
	    $(PKG_INSTALL_DIR)$(PYTHON_PKG_DIR)/deluge/ui/web \
	    $(1)$(PYTHON_PKG_DIR)/deluge/ui/

	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) \
	    $(PKG_INSTALL_DIR)/usr/bin/deluge-web \
	    $(1)/usr/bin/
endef

$(eval $(call BuildPackage,deluge))
$(eval $(call BuildPackage,deluge-ui-gtk))
$(eval $(call BuildPackage,deluge-ui-web))
