#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# Author: Carlos Cesario
#

include $(TOPDIR)/rules.mk

PKG_NAME:=imspector
PKG_VERSION:=0.9
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.imspector.org/downloads
PKG_MD5SUM:=626abf7c2b8f15d56df679ad66624575

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/imspector
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Instant Messaging
  TITLE:=IMSpector is an Instant Messenger proxy
  URL:=http://www.imspector.org
  DEPENDS:=+libopenssl +libstdcpp +libmysqlclient +libsqlite3 +libpq \
	+libpthread $(ICONV_DEPENDS)
endef

define Package/imspector/description
	IMSpector is an Instant Messenger proxy with monitoring and blocking,
	and content-filtering capabilities.
endef

define Package/imspector/conffiles
/etc/config/imspector
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR); \
		sed -ie 's/#ADD_PLUGINS/ADD_PLUGINS/' Makefile \
	);
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
	    DESTDIR="$(PKG_INSTALL_DIR)" \
	    $(TARGET_CONFIGURE_OPTS) \
	    SSL_DIR="$(STAGING_DIR)/usr" \
	    SSL_LIBS="$(TARGET_LDFLAGS) -lssl -lcrypto" \
	    SSL_FLAGS="$(TARGET_CPPFLAGS) -DHAVE_SSL" \
	    SSL_OBJS=sslstate.o \
	    PREFIX="$(STAGING_DIR)/usr" \
	    CXX=$(TARGET_CXX) \
	    CXXFLAGS="$(TARGET_CXXFLAGS) $(TARGET_CFLAGS) $(FPIC) $(TARGET_CPPFLAGS) \
			-I$(ICONV_PREFIX)/include \
			$(TARGET_LDFLAGS) -DHAVE_SSL" \
	    all
endef

define Build/Install
	$(MAKE) -C $(PKG_BUILD_DIR) \
	    DESTDIR="$(PKG_INSTALL_DIR)" \
	    install
endef

define Package/imspector/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/imspector $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/libimspector.so $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/$(PKG_NAME)
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/$(PKG_NAME)/*.so $(1)/usr/lib/$(PKG_NAME)/
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/imspector.config $(1)/etc/config/imspector
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/imspector.init $(1)/etc/init.d/imspector
	$(INSTALL_DIR) $(1)/etc/$(PKG_NAME)
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/etc/$(PKG_NAME)/badwords.txt $(1)/etc/$(PKG_NAME)/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/etc/$(PKG_NAME)/acl.txt $(1)/etc/$(PKG_NAME)/
endef

$(eval $(call BuildPackage,imspector))
