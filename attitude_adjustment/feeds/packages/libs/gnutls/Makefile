#
# Copyright (C) 2005-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gnutls
PKG_VERSION:=2.8.6
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@GNU/gnutls \
	ftp://ftp.mirrors.wiretapped.net/pub/security/network-security/gnutls/
PKG_MD5SUM:=eb0a6d7d3cb9ac684d971c14f9f6d3ba

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1
PKG_LIBTOOL_PATHS:=. lib libextra

include $(INCLUDE_DIR)/package.mk

define Package/gnutls/Default
  SUBMENU:=SSL
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=GNU TLS
  URL:=http://www.gnu.org/software/gnutls/
endef

define Package/gnutls/Default/description
 GnuTLS is a project that aims to develop a library which provides a
 secure layer, over a reliable transport layer. Currently the GnuTLS
 library implements the SSL 3.0, TLS 1.0, and TLS 1.1 protocols.
endef


define Package/certtool
$(call Package/gnutls/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE+= (certool utility)
  DEPENDS+= +libgnutls +libgnutls-extra +libncurses +libreadline
endef

define Package/certtool/description
$(call Package/gnutls/Default/description)
 This package contains the GnuTLS certtool utility.
endef


define Package/gnutls-utils
$(call Package/gnutls/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE+= (utilities)
  DEPENDS+= +libgnutls +libgnutls-extra
endef

define Package/gnutls-utils/description
$(call Package/gnutls/Default/description)
 This package contains the GnuTLS gnutls-cli, gnutls-serv, psktool,
 and srptool utilities.
endef


define Package/libgnutls
$(call Package/gnutls/Default)
  TITLE+= (library)
  DEPENDS+= +zlib +libgcrypt +libgpg-error
endef

define Package/libgnutls/description
$(call Package/gnutls/Default/description)
 This package contains the GnuTLS shared library, needed by other programs.
endef


define Package/libgnutls-extra
$(call Package/gnutls/Default)
  TITLE+= (extra library)
  DEPENDS+= +libgnutls
endef

define Package/libgnutls-extra/description
$(call Package/gnutls/Default/description)
 This package contains the GnuTLS extra shared library.
endef


define Package/libgnutls-openssl
$(call Package/gnutls/Default)
  TITLE+= (OpenSSL compat library)
  DEPENDS+= +libgnutls
endef

define Package/libgnutls-openssl/description
$(call Package/gnutls/Default/description)
 This package contains the GnuTLS OpenSSL compatibility layer shared library.
endef

CONFIGURE_ARGS+= \
	--enable-shared \
	--enable-static \
	--disable-openpgp-authentication \
	--disable-guile \
	--with-included-libcfg \
	--with-included-libtasn1 \
	--with-libgcrypt-prefix="$(STAGING_DIR)/usr" \
	--with-libreadline-prefix="$(STAGING_DIR)/usr" \
	--with-libz-prefix="$(STAGING_DIR)/usr" \
	--without-lzo \

# XXX: AM_CFLAGS duplicates with CFLAGS
MAKE_FLAGS:= \
	AM_CFLAGS="" \

TARGET_CFLAGS += $(FPIC)
TARGET_LDFLAGS += -Wl,-rpath-link=$(STAGING_DIR)/usr/lib

define Build/Configure
	$(SED) 's,-I$$$${includedir},,g' $(PKG_BUILD_DIR)/configure
	$(SED) 's,-L$$$${libdir},,g' $(PKG_BUILD_DIR)/configure
	$(call Build/Configure/Default)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include $(1)/usr/lib/pkgconfig
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/gnutls \
		$(1)/usr/include/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libgnutls{,-extra,-openssl}.{a,so*} \
		$(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/gnutls{,-extra}.pc \
		$(1)/usr/lib/pkgconfig/
endef

define Package/certtool/conffiles
/etc/gnutls/certtool.cfg
endef

define Package/certtool/install
	$(INSTALL_DIR) $(1)/etc/gnutls
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/doc/certtool.cfg $(1)/etc/gnutls/
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/certtool $(1)/usr/bin/
endef


define Package/gnutls-utils/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/bin/gnutls-{cli,serv} \
		$(PKG_INSTALL_DIR)/usr/bin/psktool \
		$(PKG_INSTALL_DIR)/usr/bin/srptool \
		$(1)/usr/bin/
endef


define Package/libgnutls/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgnutls.so.* $(1)/usr/lib/
endef


define Package/libgnutls-extra/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgnutls-extra.so.* $(1)/usr/lib/
endef


define Package/libgnutls-openssl/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgnutls-openssl.so.* $(1)/usr/lib/
endef


$(eval $(call BuildPackage,certtool))
$(eval $(call BuildPackage,gnutls-utils))
$(eval $(call BuildPackage,libgnutls))
$(eval $(call BuildPackage,libgnutls-extra))
$(eval $(call BuildPackage,libgnutls-openssl))
