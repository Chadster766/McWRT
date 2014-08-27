#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=smcroute
PKG_VERSION:=0.92
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=http://www.cschill.de/smcroute
PKG_MD5SUM:=2b50e7166606ec90e09682399e69c4a6

include $(INCLUDE_DIR)/package.mk

define Package/smcroute
	SECTION:=net
	CATEGORY:=Network
	SUBMENU:=Routing and Redirection
	TITLE:=Static multicast routing daemon
	URL:=http://www.cschill.de/smcroute/
endef

define Package/smcroute/description
 SMCRoute is a command line tool to manipulate the multicast routes of the Linux kernel.
endef


ifeq ($(HOST_OS),FreeBSD)
  TARGET_CFLAGS += -fno-builtin-log
  TARGET_LDFLAGS += -fno-builtin-log
endif

define Build/Compile
	 $(MAKE) CC="$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS)" -C $(PKG_BUILD_DIR)/src all
endef

define Package/smcroute/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/smcroute $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/mcsender $(1)/usr/bin/
	$(INSTALL_BIN) ./files/smcroute.init $(1)/etc/init.d/smcroute
endef

$(eval $(call BuildPackage,smcroute))
