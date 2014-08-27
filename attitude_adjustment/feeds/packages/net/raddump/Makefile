#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=raddump
PKG_VERSION:=0.3.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/raddump
PKG_MD5SUM:=f8c29c67141ea78bb6ae8b97d5149480

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/raddump
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libpcap +libopenssl
  TITLE:=raddump interprets captured RADIUS packets.
  URL:=http://sourceforge.net/projects/raddump
endef

define Package/raddump/description
	raddump interprets captured RADIUS packets to print a 
	timestamp, packet length, RADIUS packet type, source and 
	destination hosts and ports, and included attribute names and 
	values for each packet.
endef

# uses GNU configure

define Package/raddump/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/raddump $(1)/usr/bin/
endef

$(eval $(call BuildPackage,raddump))
