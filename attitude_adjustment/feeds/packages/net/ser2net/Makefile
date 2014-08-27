#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ser2net
PKG_VERSION:=2.7
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/ser2net
PKG_MD5SUM:=22977477789868923a5de09a85e847dd

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/ser2net
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Serial port TCP/IP redirector
  URL:=http://sourceforge.net/projects/ser2net/
endef

define Package/ser2net/description
  This is ser2net, a program for allowing network connections to serial ports.
  See the man page for information about using the program. Note that ser2net
  supports RFC 2217 (remote control of serial port parameters), but you must
  have a complient client.
endef

define Package/ser2net/conffiles
/etc/ser2net.conf
endef

# uses GNU configure

define Package/ser2net/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/ser2net $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/ser2net.conf $(1)/etc/
endef

$(eval $(call BuildPackage,ser2net))
