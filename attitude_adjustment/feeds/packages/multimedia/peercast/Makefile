#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=peercast
PKG_VERSION:=0.1214
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.cti.ecp.fr/~beauxir5/peercast
PKG_MD5SUM:=4e8449cde7135ad97f788e67e1bd64ee

PKG_FIXUP:=autoreconf
PKG_REMOVE_FILES:=aclocal.m4 acinclude.m4

PKG_INSTALL:=1

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk

define Package/peercast
  SECTION:=multimedia
  CATEGORY:=Multimedia
  DEPENDS:=+libpthread $(CXX_DEPENDS)
  TITLE:=P2P audio and video streaming server
  URL:=http://www.peercast.org/
endef

define Package/peercast/conffiles
/etc/peercast.ini
endef

define Package/peercast/description
 PeerCast is a fresh new P2P streaming server. It can stream music and 
 video from a broad variety of formats. Many audio players can listen to 
 peercast streams, as it's been built to remain compatible with Nullsoft 
 Shoutcast.
endef

CONFIGURE_ARGS += \
	--enable-staticbin \

CONFIGURE_VARS += \
	LIBS="-lpthread"

TARGET_CFLAGS += -fpermissive

define Package/peercast/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/peercast $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/share/peercast/html
	$(CP) $(PKG_INSTALL_DIR)/usr/share/peercast/html/en $(1)/usr/share/peercast/html/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/peercast.ini $(1)/etc/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/peercast.init $(1)/etc/init.d/peercast
endef

$(eval $(call BuildPackage,peercast))
