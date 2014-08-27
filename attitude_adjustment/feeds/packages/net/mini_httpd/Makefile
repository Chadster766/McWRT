#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mini-httpd
PKG_VERSION:=1.19
PKG_RELEASE:=1

PKG_SOURCE:=mini_httpd-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.acme.com/software/mini_httpd/
PKG_MD5SUM:=792a529dfe974355aad8ba6c80e54e7a

PKG_BUILD_DIR:=$(BUILD_DIR)/mini_httpd-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/mini-httpd/Default
  SUBMENU:=Web Servers/Proxies
  SECTION:=net
  CATEGORY:=Network
  TITLE:=A small web server
  URL:=http://www.acme.com/software/mini_httpd/
endef

define Package/mini-httpd/Default/conffiles
/etc/mini_httpd.conf
/etc/default/mini_httpd
endef

define Package/mini-httpd/Default/description
 mini_httpd is a small HTTP server. Its performance is not great, but for 
 low or medium traffic sites it's quite adequate. It implements all the 
 basic features of an HTTP server, including: 
 - GET, HEAD, and POST methods
 - CGI
 - Basic authentication
 - Security against ".." filename snooping
 - The common MIME types
 - Trailing-slash redirection
 - index.html, index.htm, index.cgi
 - Directory listings
 - Multihoming / virtual hosting
 - Standard logging
 - Custom error pages
 It can also be configured to do IPv6.
endef

define Package/mini-httpd
$(call Package/mini-httpd/Default)
endef

define Package/mini-httpd/conffiles
$(call Package/mini-httpd/Default/conffiles)
endef

define Package/mini-httpd/description
$(call Package/mini-httpd/Default/description)
endef

define Package/mini-httpd-matrixssl
$(call Package/mini-httpd/Default)
  DEPENDS:= +libmatrixssl
  TITLE+= (built with MatrixSSL)
endef

define Package/mini-httpd-matrixssl/conffiles
$(call Package/mini-httpd/Default/conffiles)
/etc/mini_httpd.pem
endef

define Package/mini-httpd-matrixssl/description
$(call Package/mini-httpd/Default/description)
 This package is built with SSL/HTTPS using MatrixSSL.
endef

define Package/mini-httpd-openssl
$(call Package/mini-httpd/Default)
  DEPENDS:= +libopenssl
  TITLE+= (built with OpenSSL)
endef

define Package/mini-httpd-openssl/conffiles
$(call Package/mini-httpd/Default/conffiles)
/etc/mini_httpd.pem
endef

define Package/mini-httpd-openssl/description
$(call Package/mini-httpd/Default/description)
 This package is built with SSL/HTTPS using OpenSSL.
endef

define Package/mini-httpd-htpasswd
  $(call Package/mini-httpd/Default)
  TITLE:=Utility to generate HTTP access files
endef

define Package/mini-httpd-htpasswd/description
$(call Package/mini-httpd/Default/description)
 This package generates .htaccess/.htpasswd files to use HTTP access authentication.
endef


define Build/Template

$(STAMP_BUILT)-$(2): $(STAMP_PREPARED)
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		OFLAGS="$(TARGET_CFLAGS)" \
		$(3) \
		all
	( cd $(PKG_BUILD_DIR); mv -f mini_httpd mini_httpd-$(2) )
	touch $$@

$(STAMP_BUILT): $(STAMP_BUILT)-$(2)

define Package/$(1)/install
	$(INSTALL_DIR) $$(1)/etc
ifeq ($(2),without-ssl)
	$(INSTALL_DATA) ./files/mini_httpd.conf $$(1)/etc/mini_httpd.conf
else
	$(INSTALL_DATA) ./files/mini_httpd-ssl.conf $$(1)/etc/mini_httpd.conf
	$(INSTALL_CONF) ./files/mini_httpd.pem $$(1)/etc/mini_httpd.pem
endif
	$(INSTALL_DIR) $$(1)/etc/default
	$(INSTALL_DATA) ./files/mini_httpd.default $$(1)/etc/default/mini_httpd
	$(INSTALL_DIR) $$(1)/etc/init.d
	$(INSTALL_BIN) ./files/mini_httpd.init $$(1)/etc/init.d/mini_httpd
	$(INSTALL_DIR) $$(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mini_httpd-$(2) $$(1)/usr/sbin/mini_httpd
endef

endef


define Build/Prepare
$(call Build/Prepare/Default)
	$(CP) ./files/matrixssl_helper.{c,h} $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
endef

# always build without SSL because we might need htpassd
define Build/without-ssl
  $(call Build/Template,mini-httpd,without-ssl,)
endef
$(eval $(Build/without-ssl))

ifneq ($(SDK)$(CONFIG_PACKAGE_mini-httpd-matrixssl),)
  define Build/with-matrixssl
    $(call Build/Template,mini-httpd-matrixssl,with-matrixssl, \
		SSL_DEFS="-DUSE_SSL -DHAVE_MATRIXSSL" \
		SSL_INC="$(TARGET_CPPFLAGS)" \
		SSL_LIBS="$(TARGET_LDFLAGS) -lmatrixssl" \
		SSL_OBJS="matrixssl_helper.o" \
    )
  endef
endif
$(eval $(Build/with-matrixssl))

ifneq ($(SDK)$(CONFIG_PACKAGE_mini-httpd-openssl),)
  define Build/with-openssl
    $(call Build/Template,mini-httpd-openssl,with-openssl, \
		SSL_DEFS="-DUSE_SSL -DHAVE_OPENSSL" \
		SSL_INC="$(TARGET_CPPFLAGS)" \
		SSL_LIBS="$(TARGET_LDFLAGS) -lcrypto -lssl" \
    )
  endef
endif
$(eval $(Build/with-openssl))

define Package/mini-httpd-htpasswd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/htpasswd $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,mini-httpd))
$(eval $(call BuildPackage,mini-httpd-matrixssl))
$(eval $(call BuildPackage,mini-httpd-openssl))
$(eval $(call BuildPackage,mini-httpd-htpasswd))
