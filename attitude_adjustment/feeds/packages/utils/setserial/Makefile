#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=setserial
PKG_VERSION:=2.17
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/setserial
PKG_MD5SUM:=c4867d72c41564318e0107745eb7a0f2

include $(INCLUDE_DIR)/package.mk

define Package/setserial
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Serial port configuration utility
  URL:=http://setserial.sourceforge.net/
  SUBMENU:=Terminal
endef

define Package/setserial/description
	setserial is a program designed to set and/or report the 
	configuration information associated with a serial port. This 
	information includes what I/O port and IRQ a particular 
	serial port is using, and whether or not the break key should 
	be interpreted as the Secure Attention Key, and so on.
endef

# uses GNU configure

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		setserial
endef

define Package/setserial/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/setserial $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/setserial.config $(1)/etc/config/setserial
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/setserial.init $(1)/etc/init.d/setserial
endef

define Package/setserial/conffiles
/etc/config/setserial
endef

$(eval $(call BuildPackage,setserial))
