#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=bwping
PKG_VERSION:=1.4
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/bwping
PKG_MD5SUM:=e5d2de8cf61791ad16713bd86609368f

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/bwping
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Open Source bandwidth measurement tool
  URL:=http://bwping.sourceforge.net
endef

define Package/bwping/description
  BWPing is a bandwidth measurement tool based on ICMP echo
  request/reply.  It is especially well suited for lossy links. BWPing
  does not not require any special software on the remote host (only
  the ability to respond on ICMP echo request messages).
endef

define Package/bwping/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/$(PKG_NAME) $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,bwping))
