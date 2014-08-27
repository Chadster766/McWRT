#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nullmailer
PKG_VERSION:=1.05
PKG_RELEASE:=4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://untroubled.org/nullmailer/
PKG_MD5SUM:=35124cc05f893efba1310e2ec7c876ff

include $(INCLUDE_DIR)/package.mk

define Package/nullmailer
  SECTION:=mail
  CATEGORY:=Mail
  DEPENDS:=+libstdcpp
  TITLE:=A minimal MTA for hosts which relay to a fixed set of smart relays
  URL:=http://untroubled.org/nullmailer/
endef

define Package/nullmailer/description
 This is nullmailer, a sendmail/qmail/etc replacement MTA for hosts
 which relay to a fixed set of smart relays.  It is designed to be
 simple to configure, secure, and easily extendable.
endef

TARGET_CFLAGS += $(TARGET_CPPFLAGS)

define Package/nullmailer/install
	$(INSTALL_DIR) $(1)/etc/nullmailer
	echo "# List of smart relays" > $(1)/etc/nullmailer/remotes
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/mailq $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/nullmailer-inject $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/nullmailer-queue $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/nullmailer-send $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/sendmail $(1)/usr/sbin/nullmailer-sendmail
	$(INSTALL_DIR) $(1)/usr/lib/nullmailer
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/protocols/qmqp $(1)/usr/lib/nullmailer/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/protocols/smtp $(1)/usr/lib/nullmailer/
endef

define Package/nullmailer/conffiles
/etc/nullmailer/remotes
endef

define Package/nullmailer/postinst
	ln -sf ../sbin/sendmail $${IPKG_INSTROOT}/usr/lib/sendmail
	ln -sf nullmailer-sendmail $${IPKG_INSTROOT}/usr/sbin/sendmail
endef

$(eval $(call BuildPackage,nullmailer))
