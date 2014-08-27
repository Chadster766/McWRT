#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rtpproxy
PKG_VERSION:=1.2.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://b2bua.org/chrome/site/
PKG_MD5SUM:=b0b5d6cdce3f17cdbbac473c11a2d0e5

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/rtpproxy
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Telephony
  DEPENDS:=+libpthread
  TITLE:=RTP (Realtime Transport Protocol) proxy
  URL:=http://www.rtpproxy.org/
endef

# uses GNU configure

define Package/rtpproxy/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/rtpproxy $(1)/usr/bin/
endef

$(eval $(call BuildPackage,rtpproxy))
