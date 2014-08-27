# 
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=seeks
PKG_VERSION:=0.3.4
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=git://seeks.git.sourceforge.net/gitroot/seeks/seeks
PKG_SOURCE_PROTO:=git
PKG_REV:=e8565b98cfe40fba644f587536f15f1a8665d230
PKG_SOURCE_VERSION:=$(PKG_REV)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/seeks
  SUBMENU:=Web Servers/Proxies
  SECTION:=net
  CATEGORY:=Network
  TITLE:=An open P2P design for websearch
  URL:=http://www.seeks-project.info/site/
  DEPENDS:=+libpthread +libevent2 +libstdcpp +libcurl +libpcre +libxml2 $(ICONV_DEPENDS) +zlib +protobuf +tokyocabinet
  MAINTAINER:=Obinou <obconseil@gmail.com>
endef

define Package/seeks/description
 Seeks is a free and open P2P design and application for enabling social
 websearch. Its specific purpose is to regroup users whose queries
 are similar so they can share both the query results and their
 experience on these results. On this basis, Seeks allows for true
 real-time, decentralized, websearch to emerge.

 In the long term, there is no need for web crawlers and third-party
 web indexes as users can push content directly to search groups.
 By hashing queries, Seeks offers a unique solution that allows to
 issue queries and get answers while not revealing the true query itself.

 Seeks is currently under development. Code is available from our git
 repository. Help on any of the social, technical and software matters
 is greatly encouraged and appreciated, see the dedicated page.
endef


CONFIGURE_VARS:= \
	 CXXFLAGS="-I$(STAGING_DIR)/usr/include $(ICONV_CFLAGS)" \
	 LDFLAGS="-L$(STAGING_DIR)/usr/lib      $(ICONV_LDFLAGS)" \

CONFIGURE_ARGS += \
	--prefix=/usr \
	--enable-httpserv-plugin \
	--enable-keepalive \
	--disable-image-websearch-plugin \
	--disable-icu

define Build/Configure
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status}; \
		./autogen.sh \
	);
	$(call Build/Configure/Default)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CXXFLAGS="-I$(STAGING_DIR)/usr/include $(ICONV_CFLAGS)"
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
endef

define Package/seeks/conffiles
/etc/config/seeks
/etc/seeks/config
endef

define Package/seeks/install
# Binary program
	$(INSTALL_DIR)  $(1)/usr/bin
	$(INSTALL_BIN)  $(PKG_INSTALL_DIR)/usr/bin/seeks $(1)/usr/bin/
# Config files
	$(INSTALL_DIR)  $(1)/etc/seeks
	$(INSTALL_CONF) $(PKG_INSTALL_DIR)/etc/seeks/* $(1)/etc/seeks/
# Libs
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib*.so*  $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/lib/seeks/plugins/blocker
	$(INSTALL_DIR) $(1)/usr/lib/seeks/plugins/httpserv
	$(INSTALL_DIR) $(1)/usr/lib/seeks/plugins/websearch
	$(INSTALL_DIR) $(1)/usr/lib/seeks/plugins/query_capture
	$(INSTALL_DIR) $(1)/usr/lib/seeks/plugins/uri_capture
	$(INSTALL_DIR) $(1)/usr/lib/seeks/plugins/cf
	$(INSTALL_DIR) $(1)/usr/lib/seeks/plugins/udb_service
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/seeks/plugins/blocker/lib*.so*    $(1)/usr/lib/seeks/plugins/blocker
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/seeks/plugins/httpserv/lib*.so*   $(1)/usr/lib/seeks/plugins/httpserv
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/seeks/plugins/websearch/lib*.so*  $(1)/usr/lib/seeks/plugins/websearch
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/seeks/plugins/query_capture/lib*.so*  $(1)/usr/lib/seeks/plugins/query_capture
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/seeks/plugins/uri_capture/lib*.so*  $(1)/usr/lib/seeks/plugins/uri_capture
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/seeks/plugins/cf/lib*.so*  $(1)/usr/lib/seeks/plugins/cf
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/seeks/plugins/udb_service/lib*.so*  $(1)/usr/lib/seeks/plugins/udb_service
#Share
	$(INSTALL_DIR) $(1)/usr/share/seeks
	$(CP) -r $(PKG_INSTALL_DIR)/usr/share/seeks/*  $(1)/usr/share/seeks/
# This is OpenWRT specific config & scripts
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/seeks.init $(1)/etc/init.d/seeks
endef

$(eval $(call BuildPackage,seeks))
