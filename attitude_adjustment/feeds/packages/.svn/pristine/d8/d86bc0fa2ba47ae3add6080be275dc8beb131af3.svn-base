#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libsml
PKG_VERSION:=0.1
PKG_RELEASE:=1
PKG_REV:=4e546fb0db0bd4c0a2c3c2f3d29ac0a0ba6651a3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=git://github.com/dailab/libsml.git
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_PROTO:=git

include $(INCLUDE_DIR)/package.mk

define Package/libsml
  SECTION:=libs
  CATEGORY:=Libraries
  URL:=https://github.com/dailab/libsml
  TITLE:=Library which implements the Smart Messaging Language (SML) protocol
  DEPENDS:=+libuuid
endef

define Package/libsml/description
  libSML is a library which implements the Smart Messaging Language (SML) protocol specified by
  VDE's Forum Netztechnik/Netzbetrieb (FNN). It can be utilized to communicate to FNN specified
  Smart Meters or Smart Meter components (EDL/MUC).
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/sml/include/sml $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/sml/lib/libsml.{so*,a} $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_BUILD_DIR)/sml.pc $(1)/usr/lib/pkgconfig/
endef

define Package/libsml/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/sml/lib/libsml.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libsml))
