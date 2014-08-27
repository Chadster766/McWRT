#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=smtptrapd
PKG_VERSION:=1.5
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=edc5f2f3ea9f9bdd9e0f479a58903bf1

include $(INCLUDE_DIR)/package.mk

define Package/smtptrapd
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libpthread
  TITLE:=SMTP Trap Daemon
  URL:=http://smtptrapd.sourceforge.net/
endef

define Package/smtptrapd/description
	The smtptrapd program is a multi-threaded daemon that provides 
	a RFC 2821 compliant SMTP service that always returns a 4xx soft 
	error to the RCPT TO verb.
endef

define Build/Compile
	$(TARGET_CC) \
		$(TARGET_CFLAGS) \
		$(TARGET_CPPFLAGS) \
		-D_REENTRANT \
		$(PKG_BUILD_DIR)/smtptrapd.c \
		-o $(PKG_BUILD_DIR)/smtptrapd \
		$(TARGET_LDFLAGS) \
		-lpthread
endef
	
define Package/smtptrapd/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/smtptrapd.init $(1)/etc/init.d/smtptrapd
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/smtptrapd $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,smtptrapd))
