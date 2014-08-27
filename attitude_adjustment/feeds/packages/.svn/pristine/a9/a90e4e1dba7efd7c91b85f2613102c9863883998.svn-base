#
# Copyright (C) 2005 Fabian Schneider, 2010 Florian Sesser, TU Muenchen
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=cpusage
PKG_VERSION:=0.31

PKG_BUILD_DIR:=$(BUILD_DIR)/cpusage

include $(INCLUDE_DIR)/package.mk

define Package/cpusage
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Outputs CPU usage statistics once per second
  URL:=http://www.net.t-labs.tu-berlin.de/~fabian/software_en.html\#cpusage
endef

define Package/cpusage/description
  CPUsage outputs CPU usage statistics once per second.
  Optionally writes CSV output (see '-o' option).
  Written by Fabian Schneider (TUM, TUB) in 2005.
  Timestamp and CSV-compliance by Florian Sesser (TUM), 2010.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) src/* $(PKG_BUILD_DIR)/
endef

TARGET_CFLAGS += $(FPIC)

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -D__Linux26__ -D_BSD_SOURCE=1 -c -I$(PKG_BUILD_DIR) -I$(STAGING_DIR)/usr/include -o $(PKG_BUILD_DIR)/cpusage.o $(PKG_BUILD_DIR)/cpusage.c
	$(TARGET_CC) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/cpusage -L$(STAGING_DIR)/usr/lib $(PKG_BUILD_DIR)/cpusage.o
endef

define Package/cpusage/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/cpusage $(1)/usr/bin/
endef

$(eval $(call BuildPackage,cpusage))
