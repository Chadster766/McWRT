#
# Copyright (C) 2005-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libtasn1
PKG_VERSION:=1.4
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.gnutls.org/pub/gnutls/libtasn1/ \
	ftp://ftp.gnupg.org/gcrypt/alpha/gnutls/libtasn1/ \
	http://www.mirrors.wiretapped.net/security/network-security/gnutls/libtasn1/ \
	ftp://ftp.mirrors.wiretapped.net/pub/security/network-security/gnutls/libtasn1/ \
	http://josefsson.org/gnutls/releases/libtasn1/
PKG_MD5SUM:=a733ecdfed3517910e627d24e050979f

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libtasn1
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=An ASN.1 and DER structures manipulation library
  URL:=ftp://ftp.gnutls.org/pub/gnutls/libtasn1/
endef

define Package/libtasn1/description
 This is a library for Abstract Syntax Notation One (ASN.1) and 
 Distinguish Encoding Rules (DER) manipulation.
endef

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
		--enable-shared \
		--enable-static

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/libtasn1-config $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libtasn1.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libtasn1.{a,so*} $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libtasn1*.pc $(1)/usr/lib/pkgconfig/
	$(SED) 's,-I$$$${prefix}/include,,g' $(1)/usr/bin/libtasn1-config
	$(SED) 's,-L$$$${exec_prefix}/lib,,g' $(1)/usr/bin/libtasn1-config
endef

define Package/libtasn1/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libtasn1.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libtasn1))
