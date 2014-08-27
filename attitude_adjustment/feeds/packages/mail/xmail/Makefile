#
# Copyright (C) 2007-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=xmail
PKG_VERSION:=1.26
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.xmailserver.org/
PKG_MD5SUM:=26d35c2fc104d79ebb6397c7dece5b15

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk

define Package/xmail
  SECTION:=mail
  CATEGORY:=Mail
  DEPENDS:=+libopenssl +libpthread $(CXX_DEPENDS)
  TITLE:=advanced, fast and reliable ESMTP/POP3 mail server
  URL:=http://www.xmailserver.org/
endef

define Package/xmail/description
 XMail is an Internet mail server featuring an SMTP, POP3 and finger server. 
 It's incredibly easy to set up and has lots of features including : 
 multiple domains, virtual users and spam protection.
endef

define Build/Compile
	CFLAGS="$(TARGET_CFLAGS)" \
	$(MAKE) -C $(PKG_BUILD_DIR) -f Makefile.lnx \
		$(TARGET_CONFIGURE_OPTS) \
		LD="$(TARGET_CXX)" \
		all
	tar -cjf $(PKG_BUILD_DIR)/MailRoot.tar.bz2 -C $(PKG_BUILD_DIR)/MailRoot --exclude bin .
endef

define Package/xmail/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/* $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/MailRoot.tar.bz2 $(1)/etc/
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/xmail.config $(1)/etc/config/xmail
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/xmail.init $(1)/etc/init.d/xmail
endef

define Package/xmail/conffiles
/etc/config/xmail
endef

$(eval $(call BuildPackage,xmail))
