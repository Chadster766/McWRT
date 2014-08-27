include $(TOPDIR)/rules.mk

PKG_NAME:=credis
PKG_VERSION:=0.2.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://credis.googlecode.com/files/
PKG_MD5SUM:=bbc498beb22b8590005520405766cf6e

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/credis
  SECTION:=libs
  CATEGORY:=Libraries
	TITLE:=C library for communicating with Redis servers.
  URL:=http://code.google.com/p/credis
endef

define Package/credis/description
 Credis is a client library in plain C for communicating with Redis
 servers. Redis is a high performance key-value database, refer to
 Redis project page for more information. Credis aims to be fast
 and minimalistic with respect to memory usage. It supports
 connections to multiple Redis servers.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC='$(TARGET_CC)' \
		CFLAGS='$(TARGET_CFLAGS)' \
		LDFLAGS='$(TARGET_LDFLAGS)' \
		AR='$(TARGET_CROSS)ar' \
		all
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/credis.h $(1)/usr/include/credis.h
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libcredis.{a,so*} $(1)/usr/lib/
endef

define Package/credis/install
	$(INSTALL_DIR)  $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libcredis.so $(1)/usr/lib
endef

$(eval $(call BuildPackage,credis))
