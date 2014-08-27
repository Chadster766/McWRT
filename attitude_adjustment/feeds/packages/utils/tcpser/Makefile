# 
# Copyright (C) 2009 Marek Drapal <marek.drapal@touchware.cz>
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tcpser
PKG_VERSION:=1.0rc12
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.jbrain.com/pub/linux/serial/
PKG_MD5SUM:=e712ae9fe6893bc825026b6dee6da9c0


include $(INCLUDE_DIR)/package.mk

define Package/tcpser
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Hayes compatible modem emulator
  URL:=http://www.jbrain.com/pub/linux/serial/
  DEPENDS:=+libpthread
endef

define Package/tcpser/description
 TCPSER turns a PC serial port into an emulated Hayes compatible modem 
 that uses TCP/IP for incoming and outgoing connections. It can be used 
 to allow older applications and systems designed for modem use 
 to operate on the Internet. TCPSER supports all standard Hayes commands,
 and understands extended and vendor proprietary commands (though it does
 not implement many of them). TCPSER can be used for both inbound and 
 outbound connections. 
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) $(PKG_BUILD_DIR)/../$(PKG_NAME)/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
	$(TARGET_CONFIGURE_OPTS) CFLAGS="$(TARGET_CFLAGS) -I$(LINUX_DIR)/include"
endef

define Package/tcpser/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tcpser $(1)/usr/bin/
endef


$(eval $(call BuildPackage,tcpser))
