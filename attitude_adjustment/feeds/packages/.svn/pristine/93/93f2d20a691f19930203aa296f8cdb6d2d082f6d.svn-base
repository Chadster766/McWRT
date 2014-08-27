#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=elinks
PKG_VERSION:=0.11.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://elinks.or.cz/download
PKG_MD5SUM:=fcd087a6d2415cd4c6fd1db53dceb646

include $(INCLUDE_DIR)/package.mk

define Package/elinks
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libopenssl
  TITLE:=A text based web browser
  URL:=http://elinks.or.cz/
endef

define Package/elinks/description
An advanced text based web browser
endef

SHVAR_TRANS:= \
	$(STAGING_DIR_HOST)/bin/sed -e \
	'y%*+%pp%;s%[^_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789]%_%g'

define Build/Configure
 (cd $(PKG_BUILD_DIR); rm -rf config.cache; \
                x=`echo ac_cv_file_./features.conf | $(SHVAR_TRANS)` && export $$$${x}=yes && \
                x=`echo ac_cv_file_$(abspath $(PKG_BUILD_DIR)/features.conf) | $(SHVAR_TRANS)` && export $$$${x}=yes && \
                $(TARGET_CONFIGURE_OPTS) \
                CFLAGS="$(TARGET_CFLAGS)" \
                CPPFLAGS="$(TARGET_CPPFLAGS)" \
                LDFLAGS="$(TARGET_LDFLAGS)" \
                ./configure \
                        --target=$(GNU_TARGET_NAME) \
                        --host=$(GNU_TARGET_NAME) \
                        --build=$(GNU_HOST_NAME) \
                        --program-prefix="" \
                        --program-suffix="" \
                        --prefix=/usr \
                        --exec-prefix=/usr \
                        --bindir=/usr/bin \
                        --datadir=/usr/share \
                        --includedir=/usr/include \
                        --infodir=/usr/share/info \
                        --libdir=/usr/lib \
                        --libexecdir=/usr/lib \
                        --localstatedir=/var \
                        --mandir=/usr/share/man \
                        --sbindir=/usr/sbin \
                        --sysconfdir=/etc \
                        $(DISABLE_NLS) \
                        --enable-small\
                        --enable-256-colors\
                        --disable-backtrace\
                        --disable-data\
                        --disable-formhist\
                        --disable-leds \
                        --disable-mailcap\
                        --disable-mouse \
                        --disable-smb \
                        --disable-uri-rewrite\
                        --disable-xbel \
                        --without-bzlib \
                        --without-gpm \
                        --without-idn \
                        --without-gnutls \
                        --without-libiconv \
                        --without-lua \
                        --with-openssl="$(STAGING_DIR)/usr" \
                        --without-spidermonkey\
                        --without-x\
                        --without-zlib \
 	);
endef

define Package/elinks/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/elinks $(1)/usr/bin/
endef

$(eval $(call BuildPackage,elinks))
