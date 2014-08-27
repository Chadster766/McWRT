#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=paris-traceroute
PKG_VERSION:=0.92
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-dev.tar.gz
PKG_SOURCE_URL:=http://www.paris-traceroute.net/downloads
PKG_MD5SUM:=18643a62fdcabd038cdeb14b5cbedb8b

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-current
PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk

define Package/paris-traceroute
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Paris Traceroute
  URL:=http://www.paris-traceroute.net/
  DEPENDS:=+libpthread $(CXX_DEPENDS)
endef

define Package/paris-traceroute/description
  Paris traceroute is a new version of the well-known network diagnosis and
  measurement tool. It addresses problems caused by load balancers with the
  initial implementation of traceroute.
endef

TARGET_LDFLAGS += -lm

define Package/paris-traceroute/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/bin/paris-traceroute \
		$(1)/usr/bin/
endef

$(eval $(call BuildPackage,paris-traceroute))
