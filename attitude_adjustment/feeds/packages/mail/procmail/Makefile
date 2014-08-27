#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=procmail
PKG_VERSION:=3.22
PKG_RELEASE:=1

PKG_SOURCE_URL:=http://www.procmail.org/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_MD5SUM:=1678ea99b973eb77eda4ecf6acae53f1

include $(INCLUDE_DIR)/package.mk

define Package/procmail
  SECTION:=mail
  CATEGORY:=Mail
  TITLE:=Mail processing tool
  URL:=http://www.procmail.org
endef

define Package/procmail/description
 Procmail is able to process and classify incoming mail into 
 different mail boxes.
 Very useful in conjunction with mutt.
endef

TARGET_CFLAGS += -DPROCMAIL

define Build/Compile	
	$(MAKE) -C $(PKG_BUILD_DIR)/src -f ../Makefile.openwrt  \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)"
endef

define Package/procmail/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/src/procmail $(1)/usr/bin/ 
	$(CP) $(PKG_BUILD_DIR)/src/formail $(1)/usr/bin/
endef

$(eval $(call BuildPackage,procmail))
