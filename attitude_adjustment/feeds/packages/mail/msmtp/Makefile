#
# Copyright (C) 2009 David Cooper <dave@kupesoft.com>
# Copyright (C) 2009-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=msmtp
PKG_VERSION:=1.4.27
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/msmtp
PKG_MD5SUM:=2d6d10d9c59ed2b2635554ed35fb9226

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/msmtp/Default
  SECTION:=mail
  CATEGORY:=Mail
  TITLE:=Simple sendmail SMTP forwarding
  URL:=http://msmtp.sourceforge.net/
endef

define Package/msmtp/Default/description
 msmtp is an SMTP client. In the default mode, it transmits a mail to
 an SMTP server (for example at a free mail provider) which does the
 delivery. To use this program with your mail user agent (MUA), create
 a configuration file with your mail account(s) and tell your MUA to
 call msmtp instead of /usr/sbin/sendmail.
endef

define Package/msmtp
$(call Package/msmtp/Default)
  DEPENDS+= +libopenssl
  TITLE+= (with SSL support)
  VARIANT:=ssl
endef

define Package/msmtp/conffiles
/etc/msmtprc
endef

define Package/msmtp/description
$(call Package/msmtp/Default/description)
 This package is built with SSL support.
endef

define Package/msmtp-nossl
$(call Package/msmtp/Default)
  TITLE+= (without SSL support)
  VARIANT:=nossl
endef

define Package/msmtp-nossl/description
$(call Package/msmtp/Default/description)
 This package is built without SSL support.
endef

define Package/msmtp-queue
$(call Package/msmtp/Default)
  DEPENDS+= +bash
  TITLE+= (queue scripts)
endef

define Package/msmtp-queue/description
$(call Package/msmtp/Default/description)
 This package contains the msmtp queue scripts.
endef

CONFIGURE_ARGS += \
	--disable-rpath \
	--without-libintl-prefix \
	--without-libgsasl \
	--without-libidn

MAKE_FLAGS :=

ifeq ($(BUILD_VARIANT),ssl)
	CONFIGURE_ARGS += \
		--with-ssl=openssl
endif

ifeq ($(BUILD_VARIANT),nossl)
	CONFIGURE_ARGS += \
		--with-ssl=no
endif

define Package/msmtp/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/doc/msmtprc-system.example \
		$(1)/etc/msmtprc
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/msmtp $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/sbin
endef

define Package/msmtp/postinst
	ln -sf ../bin/msmtp $${IPKG_INSTROOT}/usr/sbin/sendmail
endef

Package/msmtp-nossl/conffiles = $(Package/msmtp/conffiles)
Package/msmtp-nossl/install = $(Package/msmtp/install)
Package/msmtp-nossl/postinst = $(Package/msmtp/postinst)

define Package/msmtp-queue/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/msmtpq/msmtp{q,-queue} $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/scripts/msmtpqueue/msmtp-{en,list,run}queue.sh $(1)/usr/bin/
endef

$(eval $(call BuildPackage,msmtp))
$(eval $(call BuildPackage,msmtp-nossl))
$(eval $(call BuildPackage,msmtp-queue))
