#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dovecot
PKG_VERSION:=1.2.13
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.dovecot.org/releases/1.2
PKG_MD5SUM:=aaee3b5fd59e01780305553248f686bc

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/dovecot
  SECTION:=mail
  CATEGORY:=Mail
  DEPENDS:=+libopenssl +librt +zlib +libbz2
  TITLE:=An IMAP and POP3 daemon
  URL:=http://www.dovecot.org/
endef

define Package/dovecot/description
 Dovecot is a program which provides POP3 and IMAP services.
endef

CONFIGURE_ARGS += \
	--without-gssapi \
	--without-pam \
	--without-libcap \
	--with-moduledir=/usr/lib/dovecot/modules \
	--with-notify=dnotify \

CONFIGURE_VARS += \
	RPCGEN= \
	i_cv_signed_size_t=no \
	i_cv_signed_time_t=no \
	i_cv_gmtime_max_time_t=32 \
	i_cv_mmap_plays_with_write=yes \
	i_cv_fd_passing=yes \
	i_cv_c99_vsnprintf=yes \
	lib_cv_va_copy=yes \
	lib_cv_va_copy=yes \
	lib_cv___va_copy=yes \
	lib_cv_va_val_copy=yes \

define Package/dovecot/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/dovecot.init $(1)/etc/init.d/dovecot
	$(INSTALL_CONF) $(PKG_INSTALL_DIR)/etc/dovecot-example.conf $(1)/etc/dovecot.conf
	$(INSTALL_DIR) $(1)/usr/lib/dovecot
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/dovecot/* $(1)/usr/lib/dovecot/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/* $(1)/usr/sbin/
	find $(1)/usr/lib/dovecot/modules/ \
		-name "*.a" -o -name "*.la" | xargs rm
endef

define Package/dovecot/conffiles
/etc/dovecot.conf
endef

$(eval $(call BuildPackage,dovecot))
