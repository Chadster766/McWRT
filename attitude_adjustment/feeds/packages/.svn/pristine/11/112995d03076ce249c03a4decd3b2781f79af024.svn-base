# 
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=kismet
PKG_VERSION:=2010-07-R1
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.kismetwireless.net/code
PKG_MD5SUM:=85e59186eb529889118b5635f35cf57d

PKG_BUILD_DEPENDS:=libpcap libncurses libpcre

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk

define Package/kismet/Default
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Kismet
  DEPENDS:= $(CXX_DEPENDS) +libnl
  URL:=http://www.kismetwireless.net/
  SUBMENU:=wireless
endef

define Package/kismet/Default/description
 An 802.11 layer2 wireless network detector, sniffer, and intrusion
 detection system.
endef

define Package/kismet-client
$(call Package/kismet/Default)
  TITLE+= client
  DEPENDS+= +libncurses
endef

define Package/kismet-client/conffiles
/etc/kismet/kismet.conf
endef

define Package/kismet-client/description
$(call Package/kismet/Default/description)
 This package contains the kismet text interface client.
endef

define Package/kismet-drone
$(call Package/kismet/Default)
  DEPENDS+= +libpcap +libpcre +libcap
  TITLE+= drone
endef

define Package/kismet-drone/conffiles
/etc/kismet/kismet_drone.conf
endef

define Package/kismet-drone/description
$(call Package/kismet/Default/description)
 This package contains the kismet remote sniffing.and monitoring drone.
endef

define Package/kismet-server
$(call Package/kismet/Default)
  DEPENDS+= +libpcap +libpcre +libcap
  TITLE+= server
endef

define Package/kismet-server/conffiles
/etc/kismet/kismet.conf
endef

define Package/kismet-server/description
$(call Package/kismet/Default/description)
 This package contains the kismet server.
endef

CONFIGURE_ARGS += \
	--sysconfdir=/etc/kismet \

CONFIGURE_VARS += \
	CXXFLAGS="$$$$CXXFLAGS -fno-rtti"  \
	LIBS="-lm" \
	CLIENTCLIBS="-lm"

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		LD="$(TARGET_CXX)" \
		all
endef

#FIXME: remove this package?
define Package/kismet/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/kismet $(1)/usr/bin/kismet
endef

define Package/kismet-client/install
	$(INSTALL_DIR) $(1)/etc/kismet/
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/conf/kismet.conf $(1)/etc/kismet/
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/kismet_client $(1)/usr/bin/
endef

define Package/kismet-drone/install
	$(INSTALL_DIR) $(1)/etc/kismet/
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/conf/kismet_drone.conf $(1)/etc/kismet/
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/kismet_drone $(1)/usr/bin/
endef

define Package/kismet-server/install
	$(INSTALL_DIR) $(1)/etc/kismet/
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/conf/kismet.conf $(1)/etc/kismet/
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/kismet_server $(1)/usr/bin/
endef

$(eval $(call BuildPackage,kismet-client))
$(eval $(call BuildPackage,kismet-drone))
$(eval $(call BuildPackage,kismet-server))
