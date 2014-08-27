#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mini-sendmail
PKG_VERSION:=1.3.6
PKG_RELEASE:=4

PKG_SOURCE:=mini_sendmail-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.acme.com/software/mini_sendmail/
PKG_MD5SUM:=fb1585d2ad81c519a26d83bfd783dee8

PKG_BUILD_DIR:=$(BUILD_DIR)/mini_sendmail-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/mini-sendmail
  SECTION:=mail
  CATEGORY:=Mail
  TITLE:=A small sendmail-compatible mail forwarder
  URL:=http://www.acme.com/software/mini_sendmail/
endef

define Package/mini-sendmail/description
 mini_sendmail reads its standard input up to an end-of-file and sends a
 copy of the message found there to all of the addresses listed.
 The message is sent by connecting to a local SMTP server. This means
 mini_sendmail can be used to send email from inside a chroot(2) area.
endef

MAKE_FLAGS += \
	OFLAGS="$(TARGET_CFLAGS)"

define Package/mini-sendmail/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mini_sendmail $(1)/usr/sbin/
endef

define Package/mini-sendmail/postinst
	ln -sf mini_sendmail $${IPKG_INSTROOT}/usr/sbin/sendmail
endef

$(eval $(call BuildPackage,mini-sendmail))
