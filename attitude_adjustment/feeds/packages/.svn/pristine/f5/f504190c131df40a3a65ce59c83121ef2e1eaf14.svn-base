#
# Copyright (C) 2007-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=lcdproc
PKG_VERSION:=0.5.4
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/lcdproc
PKG_MD5SUM:=4d075c3928e84a90bfccf07313b13721

include $(INCLUDE_DIR)/package.mk


define Package/lcdproc
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libusb +libncurses +libftdi
  TITLE:=LCD Display daemon and client
  URL:=http://sourceforge.net/projects/lcdproc/
endef

define Package/lcdproc/description
 LCDProc is a daemon and client for displaying system information on various displays.
endef

define Package/lcdproc/conffiles
/etc/LCDd.conf
/etc/lcdproc.conf
/etc/lcdexec.conf
/etc/lcdvc.conf
endef

define Build/Configure
	$(call Build/Configure/Default,--enable-drivers=all)
endef

define Package/lcdproc/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/usr/lib/lcdproc
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/server/LCDd $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/clients/lcdproc/lcdproc $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/clients/lcdexec/lcdexec $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/clients/lcdvc/lcdvc $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/server/drivers/*.so $(1)/usr/lib/lcdproc/
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/LCDd.conf $(1)/etc/
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/clients/lcdproc/lcdproc.conf $(1)/etc/
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/clients/lcdexec/lcdexec.conf $(1)/etc/
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/clients/lcdvc/lcdvc.conf $(1)/etc/
	$(INSTALL_BIN) ./files/* $(1)/etc/init.d
endef

$(eval $(call BuildPackage,lcdproc))
