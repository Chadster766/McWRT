#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=dns2tcp
PKG_VERSION:=0.5.2
PKG_RELEASE:=1
PKG_MD5SUM:=51c5dc69f5814c2936ce6832217d292d

PKG_SOURCE_URL:=http://www.hsc.fr/ressources/outils/dns2tcp/download/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz

include $(INCLUDE_DIR)/package.mk

define Package/dns2tcp/Default
  SECTION:=net
  CATEGORY:=Network
  TITLE:=TCP over DNS tunnel
  URL:=http://www.hsc.fr/ressources/outils/dns2tcp/
endef

define Package/dns2tcp
  $(call Package/dns2tcp/Default)
  TITLE+= (client)
endef

define Package/dns2tcpd
  $(call Package/dns2tcp/Default)
  TITLE+= (server)
endef

define Package/dns2tcp/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/client/dns2tcpc $(1)/usr/bin
endef

define Package/dns2tcpd/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/server/dns2tcpd $(1)/usr/bin
endef

$(eval $(call BuildPackage,dns2tcp))
$(eval $(call BuildPackage,dns2tcpd))
