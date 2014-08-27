#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=php4
PKG_VERSION:=4.4.9
PKG_RELEASE:=1

PKG_SOURCE:=php-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.php.net/distributions/
PKG_MD5SUM:=2e3b2a0e27f10cb84fd00e5ecd7a1880
PKG_BUILD_DIR:=$(BUILD_DIR)/php-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/php4/Default
  SUBMENU:=PHP
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=PHP4 Hypertext preprocessor
  DEPENDS:=@!TARGET_ixp4xx
  URL:=http://www.php.net/
endef

define Package/php4/Default/description
 PHP is a widely-used general-purpose scripting language that is especially
 suited for Web development and can be embedded into HTML.
endef

define Package/php4
  $(call Package/php4/Default)
  DEPENDS:=+libopenssl +zlib
endef

define Package/php4/description
$(call Package/php4/Default/description)
 This package contains only the PHP config file. You must actually choose
 your PHP flavour (cli, cgi or fastcgi).
endef

define Package/php4-cli
  $(call Package/php4/Default)
  DEPENDS:=php4
  TITLE+= (CLI)
endef

define Package/php4-cli/description
$(call Package/php4/Default/description)
 This package contains the CLI version of the PHP4 interpreter.
endef

define Package/php4-cgi
  $(call Package/php4/Default)
  DEPENDS:=php4
  TITLE+= (CGI)
endef

define Package/php4-cgi/description
$(call Package/php4/Default/description)
 This package contains the CGI version of the PHP4 interpreter.
endef

define Package/php4-fastcgi
  $(call Package/php4/Default)
  DEPENDS:=php4
  TITLE+= (FastCGI)
endef

define Package/php4-fastcgi/description
$(call Package/php4/Default/description)
 This package contains the FastCGI version of the PHP4 interpreter.
endef

define Package/php4-mod-curl
  $(call Package/php4/Default)
  DEPENDS:=php4 +libcurl
  TITLE:=cURL module
endef

define Package/php4-mod-ftp
  $(call Package/php4/Default)
  DEPENDS:=php4
  TITLE:=FTP module
endef

define Package/php4-mod-gd
  $(call Package/php4/Default)
  DEPENDS:=php4 +libgd
  TITLE:=GD graphics module
endef

define Package/php4-mod-gmp
  $(call Package/php4/Default)
  DEPENDS:=php4 +libgmp
  TITLE:=GMP module
endef

define Package/php4-mod-ldap
  $(call Package/php4/Default)
  DEPENDS:=php4 +libopenldap
  TITLE:=LDAP module
endef

define Package/php4-mod-mysql
  $(call Package/php4/Default)
  DEPENDS:=php4 +libmysqlclient
  TITLE:=MySQL module
endef

define Package/php4-mod-openssl
  $(call Package/php4/Default)
  DEPENDS:=php4 +libopenssl
  TITLE:=OpenSSL module
endef

define Package/php4-mod-pcre
  $(call Package/php4/Default)
  DEPENDS:=php4 +libpcre
  TITLE:=PCRE module
endef

define Package/php4-mod-pgsql
  $(call Package/php4/Default)
  DEPENDS:=php4 +libpq
  TITLE:=PostgreSQL module
endef

define Package/php4-mod-session
  $(call Package/php4/Default)
  DEPENDS:=php4
  TITLE:=Session module
endef

define Package/php4-mod-sockets
  $(call Package/php4/Default)
  DEPENDS:=php4
  TITLE:=Sockets module
endef

define Package/php4-mod-xml
  $(call Package/php4/Default)
  DEPENDS:=php4 +libexpat
  TITLE:=XML module
endef

PKG_CONFIGURE_LIBS:= -lcrypto -lssl
PKG_CONFIGURE_OPTS:= \
	--enable-shared \
	--disable-static \
	--disable-rpath \
	--disable-debug \
	--without-pear \
	\
	--with-config-file-path=/etc \
	--enable-magic-quotes \
	--enable-memory-limit \
	--disable-overload \
	--disable-short-tags \
	\
	--disable-ctype \
	--disable-dom \
	--enable-ftp=shared \
	--without-gettext \
	--without-iconv \
	--disable-mbstring \
	--disable-mbregex \
	--with-openssl=shared,"$(STAGING_DIR)/usr" \
	  --with-kerberos=no \
	  --with-openssl-dir="$(STAGING_DIR)/usr" \
	--enable-session=shared \
	--enable-sockets=shared \
	--disable-tokenizer \
	--with-zlib="$(STAGING_DIR)/usr" \
	  --with-zlib-dir="$(STAGING_DIR)/usr" \

ifneq ($(SDK),)
  PKG_BUILD_DEPENDS:=libopenssl zlib libcurl libgd libgmp libopenldap libmysqlclient libpq libexpat
# force PKG_CONFIGURE_{LIBS,OPTS} below when built in the SDK
  CONFIG_PACKAGE_php4-cli:=m
  CONFIG_PACKAGE_php4-cgi:=m
  CONFIG_PACKAGE_php4-fastcgi:=m
  CONFIG_PACKAGE_php4-mod-curl:=m
  CONFIG_PACKAGE_php4-mod-gd:=m
  CONFIG_PACKAGE_php4-mod-gmp:=m
  CONFIG_PACKAGE_php4-mod-ldap:=m
  CONFIG_PACKAGE_php4-mod-mysql:=m
  CONFIG_PACKAGE_php4-mod-pcre:=m
  CONFIG_PACKAGE_php4-mod-pgsql:=m
  CONFIG_PACKAGE_php4-mod-xml:=m
endif

ifneq ($(CONFIG_PACKAGE_php4-mod-curl),)
  PKG_CONFIGURE_OPTS+= --with-curl=shared,"$(STAGING_DIR)/usr"
else
  PKG_CONFIGURE_OPTS+= --without-curl
endif
ifneq ($(CONFIG_PACKAGE_php4-mod-gd),)
  PKG_CONFIGURE_OPTS+= --with-gd=shared,"$(STAGING_DIR)/usr" \
	--without-freetype-dir \
	--with-jpeg-dir="$(STAGING_DIR)/usr" \
	--with-png-dir="$(STAGING_DIR)/usr" \
	--without-xpm-dir \
	--without-ttf \
	--without-t1lib	\
	--enable-gd-native-ttf \
	--disable-gd-jis-conv
else
  PKG_CONFIGURE_OPTS+= --without-gd
endif
ifneq ($(CONFIG_PACKAGE_php4-mod-gmp),)
  PKG_CONFIGURE_OPTS+= --with-gmp=shared,"$(STAGING_DIR)/usr"
else
  PKG_CONFIGURE_OPTS+= --without-gmp
endif
ifneq ($(CONFIG_PACKAGE_php4-mod-ldap),)
  PKG_CONFIGURE_OPTS+= --with-ldap=shared,"$(STAGING_DIR)/usr"
	--with-ldap-sasl="$(STAGING_DIR)/usr"
else
  PKG_CONFIGURE_OPTS+= --without-ldap
endif
ifneq ($(CONFIG_PACKAGE_php4-mod-mysql),)
  PKG_CONFIGURE_OPTS+= --with-mysql=shared,"$(STAGING_DIR)/usr"
else
  PKG_CONFIGURE_OPTS+= --without-mysql
endif
ifneq ($(CONFIG_PACKAGE_php4-mod-pcre),)
  PKG_CONFIGURE_OPTS+= --with-pcre-regex=shared,"$(STAGING_DIR)/usr"
else
  PKG_CONFIGURE_OPTS+= --without-pcre-regex
endif
ifneq ($(CONFIG_PACKAGE_php4-mod-pgsql),)
  PKG_CONFIGURE_OPTS+= --with-pgsql=shared,"$(STAGING_DIR)/usr"
else
  PKG_CONFIGURE_OPTS+= --without-pgsql
endif
ifneq ($(CONFIG_PACKAGE_php4-mod-xml),)
  PKG_CONFIGURE_OPTS+= --enable-xml=shared \
	--with-expat-dir="$(STAGING_DIR)/usr"
else
  PKG_CONFIGURE_OPTS+= --disable-xml
endif

define Build/Configure
endef

TARGET_CFLAGS += $(if $(CONFIG_BIG_ENDIAN),-DIEEE_BIG_ENDIAN,-DIEEE_LITTLE_ENDIAN) $(FPIC)

ifneq ($(CONFIG_PACKAGE_php4-cli),)
  define Build/Compile/php4-cli
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	$(call Build/Configure/Default, \
		$(PKG_CONFIGURE_OPTS) \
		--enable-cli \
		--disable-cgi \
		--disable-fastcgi \
		--enable-force-cgi-redirect \
		--enable-discard-path \
		, \
		LIBS="$(PKG_CONFIGURE_LIBS)" \
		php_cv_cc_rpath="no" \
	)
	LD_LIBRARY_PATH="" $(MAKE) -C $(PKG_BUILD_DIR)
	mv $(PKG_BUILD_DIR)/sapi/cli/php $(PKG_BUILD_DIR)/php-cli
  endef
endif

ifneq ($(CONFIG_PACKAGE_php4-cgi),)
  define Build/Compile/php4-cgi
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	$(call Build/Configure/Default, \
		$(PKG_CONFIGURE_OPTS) \
		--disable-cli \
		--enable-cgi \
		--disable-fastcgi \
		--enable-force-cgi-redirect \
		--enable-discard-path \
		, \
		LIBS="$(PKG_CONFIGURE_LIBS)" \
		php_cv_cc_rpath="no" \
	)
	LD_LIBRARY_PATH="" $(MAKE) -C $(PKG_BUILD_DIR)
	mv $(PKG_BUILD_DIR)/sapi/cgi/php $(PKG_BUILD_DIR)/php-cgi
  endef
endif

ifneq ($(CONFIG_PACKAGE_php4-fastcgi),)
  define Build/Compile/php4-fastcgi
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	$(call Build/Configure/Default, \
		$(PKG_CONFIGURE_OPTS) \
		--disable-cli \
		--enable-cgi \
		--enable-fastcgi \
		--enable-force-cgi-redirect \
		--enable-discard-path \
		, \
		LIBS="$(PKG_CONFIGURE_LIBS)" \
		php_cv_cc_rpath="no" \
	)
	LD_LIBRARY_PATH="" $(MAKE) -C $(PKG_BUILD_DIR)
	mv $(PKG_BUILD_DIR)/sapi/cgi/php $(PKG_BUILD_DIR)/php-fastcgi
  endef
endif

define Build/Compile
	$(call Build/Compile/php4-cli)
	$(call Build/Compile/php4-cgi)
	$(call Build/Compile/php4-fastcgi)
endef

define Package/php4/conffiles
/etc/php.ini
endef

define Package/php4/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/php.ini $(1)/etc/
endef

define Package/php4-cli/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/php-cli $(1)/usr/bin/php
endef

define Package/php4-cgi/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/php-cgi $(1)/usr/bin/php
endef

define Package/php4-fastcgi/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/php-fastcgi $(1)/usr/sbin/php
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/php.init $(1)/etc/init.d/php
endef

define BuildPlugin
  define Package/$(1)/install
	[ -z "$(2)" ] || $(INSTALL_DIR) $$(1)/usr/lib/php
	for m in $(2); do \
		$(INSTALL_BIN) $(PKG_BUILD_DIR)/modules/$$$$$$$${m}.so $$(1)/usr/lib/php/ ; \
	done
  endef

  $$(eval $$(call BuildPackage,$(1)))
endef

$(eval $(call BuildPackage,php4))
$(eval $(call BuildPackage,php4-cli))
$(eval $(call BuildPackage,php4-cgi))
$(eval $(call BuildPackage,php4-fastcgi))
$(eval $(call BuildPlugin,php4-mod-curl,curl))
$(eval $(call BuildPlugin,php4-mod-ftp,ftp))
$(eval $(call BuildPlugin,php4-mod-gd,gd))
$(eval $(call BuildPlugin,php4-mod-gmp,gmp))
$(eval $(call BuildPlugin,php4-mod-ldap,ldap))
$(eval $(call BuildPlugin,php4-mod-mysql,mysql))
$(eval $(call BuildPlugin,php4-mod-openssl,openssl))
$(eval $(call BuildPlugin,php4-mod-pcre,pcre))
$(eval $(call BuildPlugin,php4-mod-pgsql,pgsql))
$(eval $(call BuildPlugin,php4-mod-session,session))
$(eval $(call BuildPlugin,php4-mod-sockets,sockets))
$(eval $(call BuildPlugin,php4-mod-xml,xml))

