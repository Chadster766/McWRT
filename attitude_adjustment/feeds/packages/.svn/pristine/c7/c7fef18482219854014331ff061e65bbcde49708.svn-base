# 
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=axtls
PKG_VERSION:=1.4.3
PKG_RELEASE:=1

PKG_SOURCE:=axTLS-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/axtls
PKG_MD5SUM:=f0e50d11fec81714b40ae21cca23b827

include $(INCLUDE_DIR)/package.mk

define Package/libaxtls
  SUBMENU:=SSL
  SECTION:=libs
  CATEGORY:=Libraries
  SUBMENU:=SSL
  TITLE:=Embedded client/server TLSv1 SSL library
  URL:=http://sourceforge.net/projects/axtls
endef

define Package/axhttpd
  SUBMENU:=Web Servers/Proxies
  SECTION:=net
  CATEGORY:=Network
  TITLE:=A small embedded web server using the axTLS library
  URL:=http://www.axtls.co.nr/
  DEPENDS:=+libaxtls
endef

define Package/axtlswrap
  SUBMENU:=SSL
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Web Servers/Proxies
  TITLE:=A simple TLS/SSL wrapper using the axTLS library
  URL:=http://sourceforge.net/projects/axtls
  DEPENDS:=+libaxtls
endef

define Build/Configure
	$(CP) ./files/config $(PKG_BUILD_DIR)/config/.config
	$(MAKE) -C $(PKG_BUILD_DIR) oldconfig
endef

TAR_CMD:=$(HOST_TAR) -C $(PKG_BUILD_DIR) --strip-components 1 $(TAR_OPTIONS)

TARGET_CFLAGS += $(FPIC)

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		STRIP="/bin/true" \
		OPT_CFLAGS="$(TARGET_CFLAGS)" OPT_LDFLAGS="" \
		all
	$(MAKE) -C $(PKG_BUILD_DIR) \
		PREFIX="$(PKG_INSTALL_DIR)/usr" \
		install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/axTLS $(1)/usr/include/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libaxtls.{a,so*} $(1)/usr/lib/
endef

define Package/libaxtls/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libaxtls.so.* $(1)/usr/lib/
endef

define Package/axhttpd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/axhttpd $(1)/usr/sbin/
endef

define Package/axtlswrap/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/_stage/axtlswrap $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,libaxtls))
$(eval $(call BuildPackage,axhttpd))
$(eval $(call BuildPackage,axtlswrap))
