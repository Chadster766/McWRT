#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=uob-nomad
PKG_VERSION:=0.11
PKG_RELEASE:=2

PKG_SOURCE_URL:=http://www.comnets.uni-bremen.de/~mab/openwrt/source/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_MD5SUM:=9c7445cefaf8d6160017aef606f2e558

PKG_BUILD_PARALLEL:=0

include $(INCLUDE_DIR)/package.mk

define Package/uob-nomad
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+kmod-iptunnel4 +libpthread
  TITLE:=UoB Nomad
  URL:=http://www.comnets.uni-bremen.de/~mab/openwrt/source/
endef

define Package/uob-nomad/description
This is an implementation of NOMADv4 (Filters for
	Mobile IP, IETF Draft
	draft-mobileip-nomad-filters-05.txt) for Linux,
	based on the Sun Labs Mobile IP software. It is
	released under the Sun Public License (SPL).
	
	This version (0.11) has been modified to provide
	status information to the UoB-NOMAD-GUI (ver 0.1).
endef

TARGET_CFLAGS += -include stddef.h -include string.h

MAKE_FLAGS = \
	CC="$(TARGET_CC) $(TARGET_CFLAGS)"

define Build/Compile
$(call Build/Compile/Default,-C $(PKG_BUILD_DIR)/Agent clean mipagent)
$(call Build/Compile/Default,-C $(PKG_BUILD_DIR)/Client/net-tools clean)
$(call Build/Compile/Default,-C $(PKG_BUILD_DIR)/Client/net-tools/lib clean)
$(call Build/Compile/Default,-C $(PKG_BUILD_DIR)/Client/net-tools all)
$(call Build/Compile/Default,-C $(PKG_BUILD_DIR)/Client clean mipmn)
endef

define Package/uob-nomad/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/usr/lib $(1)/etc/
	$(CP) $(PKG_BUILD_DIR)/Agent/mipagent $(1)/usr/bin/
	$(CP) $(PKG_BUILD_DIR)/Client/mipmn $(1)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/conf/mip{agent,mn,mn-filters}.conf $(1)/etc/
endef

define Package/uob-nomad/conffiles
/etc/mip{agent,mn,mn-filters}.conf
endef

$(eval $(call BuildPackage,uob-nomad))
