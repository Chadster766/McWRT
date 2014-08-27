#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=hping3
PKG_VERSION:=20051105
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.hping.org/
PKG_MD5SUM:=ca4ea4e34bcc2162aedf25df8b2d1747

include $(INCLUDE_DIR)/package.mk

define Package/hping3
  SECTION:=net
  CATEGORY:=Network
  TITLE:=TCP/IP packet assembler/analyzer
  URL:=http://www.hping.org/
  DEPENDS:=+libpcap +libpthread
endef

define Package/hping3/description
        hping is a command-line oriented TCP/IP packet assembler/analyzer. The interface is
        inspired to the ping(8) unix command, but hping isn't only able to send ICMP echo
        requests. It supports TCP, UDP, ICMP and RAW-IP protocols, has a traceroute mode, the
        ability to send files between a covered channel, and many other features. 
endef

define Build/Configure
endef 

define Package/hping3/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hping3 $(1)/usr/sbin/hping3
endef

$(eval $(call BuildPackage,hping3))
