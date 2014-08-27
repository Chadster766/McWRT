# 
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=curl
PKG_VERSION:=7.29.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://curl.haxx.se/download/ \
	http://www.mirrorspace.org/curl/ \
	http://curl.mirror.internet.tp/download/ \
	ftp://ftp.sunet.se/pub/www/utilities/curl/ \
	ftp://ftp.planetmirror.com/pub/curl/ \
	http://www.mirrormonster.com/curl/download/ \
	http://curl.mirrors.cyberservers.net/download/
PKG_MD5SUM:=fa5f37f38a8042020e292ce7ec5341ce

PKG_FIXUP:=autoreconf
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/curl/Default
  SECTION:=net
  CATEGORY:=Network
  URL:=http://curl.haxx.se/
endef

define Package/curl
  $(call Package/curl/Default)
  SUBMENU:=File Transfer
  DEPENDS:=+libcurl
  TITLE:=A client-side URL transfer utility
endef

define Package/libcurl
  $(call Package/curl/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+libopenssl +zlib
  TITLE:=A client-side URL transfer library
endef

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--disable-thread \
	--enable-cookies \
	--enable-crypto-auth \
	--enable-nonblocking \
	--enable-file \
	--enable-ftp \
	--enable-http \
	--disable-ares \
	--disable-debug \
	--disable-dict \
	--disable-gopher \
	--disable-ldap \
	--disable-manual \
	--disable-sspi \
	--disable-telnet \
	--enable-tftp \
	--disable-verbose \
	--with-random="/dev/urandom" \
	--with-ssl="$(STAGING_DIR)/usr" \
	--without-ca-bundle \
	--without-gnutls \
	--without-krb4 \
	--without-libidn \
	--without-nss \
	--without-libssh2 \
	--with-zlib="$(STAGING_DIR)/usr" \
	$(call autoconf_bool,CONFIG_IPV6,ipv6) \

CONFIGURE_VARS += \
	LIBS="-lcrypto -lssl -lz" \
	CC="$(filter-out ccache,$(TARGET_CC))"

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		CC="$(TARGET_CC)" \
		install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(2)/bin $(1)/usr/bin $(1)/usr/include $(1)/usr/lib $(1)/usr/lib/pkgconfig
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/curl-config $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/curl $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcurl.{a,so*} $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/libcurl.pc $(1)/usr/lib/pkgconfig/
	$(SED) 's,-L$$$${exec_prefix}/lib,,g' $(1)/usr/bin/curl-config
	[ -n "$(TARGET_LDFLAGS)" ] && $(SED) 's#$(TARGET_LDFLAGS)##g' $(1)/usr/lib/pkgconfig/libcurl.pc || true
	ln -sf $(STAGING_DIR)/usr/bin/curl-config $(2)/bin/
endef

define Package/curl/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/curl $(1)/usr/bin/
endef

define Package/libcurl/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcurl.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,curl))
$(eval $(call BuildPackage,libcurl))
