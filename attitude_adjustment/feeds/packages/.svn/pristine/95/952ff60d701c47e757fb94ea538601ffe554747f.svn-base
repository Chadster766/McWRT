#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=jtg
PKG_VERSION:=1.95
PKG_RELEASE:=1
PKG_SOURCE:=$(PKG_NAME)-v$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.netlab.tkk.fi/~jmanner/jtg

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-v$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/jtg
  TITLE:=Jugi's Traffic Generator
  SECTION:=net
  CATEGORY:=Network
  MAINTAINER:=Roberto Riggio <roberto.riggio@create-net.org>
  URL:=https://hoslab.cs.helsinki.fi/savane/projects/jtg/
endef

define Package/wing/Description
	The JTG generator is a simple, yet powerful and, if required, accurate
	traffic generator. jtg differs from other traffic generators in that one
	instance of a jtg process only sends one stream (e.g. MGEN can send tens
	of streams) and the characteristics of the stream are given only with
	command line arguments. On the other hand, a receiving jtg process can
	be set to loop and receive several streams but, again, only one at a
	time. The received packets can be logged and the log files can be
	analyzed with jtg_calc (see later in this file for instructions). jtg6
	is the binary for IPv6 networks.
endef

define Build/Compile
	$(MAKE) $(TARGET_CONFIGURE_OPTS) \
		-C $(PKG_BUILD_DIR)
endef

define Package/jtg/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/jtg $(1)/usr/bin/
endef

$(eval $(call BuildPackage,jtg))
