#
# Copyright (C) 2007-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=updatedd
PKG_VERSION:=2.6
PKG_RELEASE:=9

PKG_SOURCE_URL:=http://downloads.openwrt.org/sources/
PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).tar.bz2
PKG_MD5SUM:=f03446105ed043cd5dbd1ab95484d27f

PKG_FIXUP:=autoreconf

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/updatedd
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=IP Addresses and Names
  MENU:=1
  TITLE:=A tool to update dynamic dns services
  URL:=http://savannah.nongnu.org/projects/updatedd/
endef

define Package/updatedd/description
Updatedd is a small tool that will update one of many dynamic 
	dns services on boot. 
	Please look at the /etc/init.d/ddns script for more info. 

endef

define Package/updatedd/conffiles
/etc/config/updatedd
endef

define Package/updatedd-mod-changeip
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=IP Addresses and Names
  DEPENDS:=updatedd
  TITLE:=changeip.com
endef

define Package/updatedd-mod-dyndns
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=IP Addresses and Names
  DEPENDS:=updatedd
  TITLE:=dyndns.org
endef

define Package/updatedd-mod-eurodyndns
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=IP Addresses and Names
  DEPENDS:=updatedd
  TITLE:=eurodyndns.org
endef

define Package/updatedd-mod-hn
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=IP Addresses and Names
  DEPENDS:=updatedd
  TITLE:=hn.org
endef

define Package/updatedd-mod-noip
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=IP Addresses and Names
  DEPENDS:=updatedd
  TITLE:=no-ip.com
endef

define Package/updatedd-mod-ods
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=IP Addresses and Names
  DEPENDS:=updatedd
  TITLE:=ods.org
endef

define Package/updatedd-mod-ovh
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=IP Addresses and Names
  DEPENDS:=updatedd
  TITLE:=ovh.com
endef

define Package/updatedd-mod-regfish
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=IP Addresses and Names
  DEPENDS:=updatedd
  TITLE:=regfish.com
endef

define Package/updatedd-mod-tzo
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=IP Addresses and Names
  DEPENDS:=updatedd
  TITLE:=tzo.com
endef

define Package/updatedd-mod-zoneedit
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=IP Addresses and Names
  DEPENDS:=updatedd
  TITLE:=zoneedit.com
endef

define BuildPlugin
  define Package/$(1)/install
	$(INSTALL_DIR) $$(1)/usr/lib/updatedd
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/updatedd/$(2).so* $$(1)/usr/lib/updatedd/
  endef

  $$(eval $$(call BuildPackage,$(1)))
endef

define Package/updatedd/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/updatedd.init $(1)/etc/init.d/updatedd
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/updatedd.config $(1)/etc/config/updatedd
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/updatedd $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib/updatedd
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) ./files/updatedd.hotplug $(1)/etc/hotplug.d/iface/30-updatedd
endef

$(eval $(call BuildPackage,updatedd))
$(eval $(call BuildPlugin,updatedd-mod-changeip,libchangeip))
$(eval $(call BuildPlugin,updatedd-mod-dyndns,libdyndns))
$(eval $(call BuildPlugin,updatedd-mod-eurodyndns,libeurodyndns))
$(eval $(call BuildPlugin,updatedd-mod-hn,libhn))
$(eval $(call BuildPlugin,updatedd-mod-noip,libnoip))
$(eval $(call BuildPlugin,updatedd-mod-ods,libods))
$(eval $(call BuildPlugin,updatedd-mod-ovh,libovh))
$(eval $(call BuildPlugin,updatedd-mod-regfish,libregfish))
$(eval $(call BuildPlugin,updatedd-mod-tzo,libtzo))
$(eval $(call BuildPlugin,updatedd-mod-zoneedit,libzoneedit))
