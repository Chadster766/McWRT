#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=udpcast
PKG_RELEASE:=1
PKG_VERSION:=20100130

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://udpcast.linux.lu/download/
PKG_MD5SUM:=f9ad942dfe3ecbd2da5d91479957e5a9

include $(INCLUDE_DIR)/package.mk

define Package/udpcast
  SECTION:=net
  CATEGORY:=Network
  TITLE:=UDP Cast
  URL:=http://udpcast.linux.lu/
  DEPENDS:=+libpthread
endef

define Package/udpcast/description
  UDPcast is a file transfer tool that can send data simultaneously to many destinations on a LAN.
endef

define Package/udpcast/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) \
		$(PKG_BUILD_DIR)/udp-receiver \
		$(1)/usr/sbin/
	$(INSTALL_BIN) \
		$(PKG_BUILD_DIR)/udp-sender \
		$(1)/usr/sbin/
endef

$(eval $(call BuildPackage,udpcast))
