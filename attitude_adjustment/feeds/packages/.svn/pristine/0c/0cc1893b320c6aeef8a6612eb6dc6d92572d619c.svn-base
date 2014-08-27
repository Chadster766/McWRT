#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=phidget21
PKG_VERSION:=2.1.6.20091105
PKG_RELEASE:=1

PKG_SOURCE:=Phidgetlinux_$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/Phidgetlinux
PKG_SOURCE_URL:=http://www.phidgets.com/downloads/libraries/
PKG_MD5SUM:=eb48c7fc68ab189a0255c35e71d55ecd

include $(INCLUDE_DIR)/package.mk

define Package/phidget21
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+libpthread
  TITLE:=Phidget API
  URL:=http://www.phidgets.com/
endef

define Package/phidget21/description
 The Phidget API library.
endef

define Package/phidget-service
  SECTION:=misc
  CATEGORY:=Network
  TITLE:=Phidget Web Service
  URL:=http://www.phidgets.com/
  DEPENDS:=phidget21
endef

define Package/phidget-service/description
 The Phidget Web Service.
endef

define Build/Configure
	$(INSTALL_DIR) $(PKG_INSTALL_DIR)/usr/include/
	$(INSTALL_DIR) $(PKG_INSTALL_DIR)/usr/lib/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/phidget21 \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -Wall -Wno-strict-aliasing -fpic -D_LINUX -I. -Iutils" \
		INSTALLPREFIX="$(PKG_INSTALL_DIR)" LDCONFIG="" \
		all install
	$(MAKE) -C $(PKG_BUILD_DIR)/webservice \
		$(TARGET_CONFIGURE_OPTS) LDCONFIG="true" \
		CFLAGS="$(TARGET_CFLAGS) -Wall -Wno-strict-aliasing -fpic -D_LINUX -I. -Iutils" \
		phidget21internal.h
	$(MAKE) -C $(PKG_BUILD_DIR)/webservice \
		$(TARGET_CONFIGURE_OPTS) LDCONFIG="true" \
		LIBS="-lphidget21 -lpthread -ldl -L../phidget21" \
		CFLAGS="$(TARGET_CFLAGS) -static -D_LINUX -I. -I../phidget21 -I../phidget21/utils" \
		webservice21
endef

define Build/InstallDev
	ls $(PKG_INSTALL_DIR)
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/phidget21.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libphidget21.so $(1)/usr/lib/
endef

define Package/phidget21/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/libphidget21.so $(1)/usr/lib/
endef

define Package/phidget-service/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/webservice/phidgetwebservice21 $(1)/usr/bin/
endef

$(eval $(call BuildPackage,phidget21))
$(eval $(call BuildPackage,phidget-service))
