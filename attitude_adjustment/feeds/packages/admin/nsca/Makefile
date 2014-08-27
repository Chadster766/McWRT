# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nsca
PKG_VERSION:=2.7.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/nagios
PKG_MD5SUM:=33a98e7975f633a9489d7a8938ed6131

include $(INCLUDE_DIR)/package.mk

define Package/send-nsca
  SECTION:=admin
  CATEGORY:=Administration
  DEPENDS:=+libmcrypt
  TITLE:=Nagios Service Check Acceptor (NSCA) client
  URL:=http://www.nagios.org/
endef

define Package/send-nsca/description
	This is the client program that is used to send service check
	information from a remote machine to the nsca daemon on the central
	machine that runs Nagios.
endef

define Package/send-nsca/conffiles
/etc/send_nsca.cfg
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--with-nsca-user="root" \
		--with-nsca-grp="root" \
		--with-libmcrypt-prefix="$(STAGING_DIR)/usr" \
		, \
		CFLAGS="$$$$CFLAGS $$$$CPPFLAGS" \
	)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		send_nsca
endef

define Package/send-nsca/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/sample-config/send_nsca.cfg $(1)/etc/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/send_nsca $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,send-nsca))
