#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gatling
PKG_VERSION:=0.11
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://dl.fefe.de
PKG_MD5SUM:=a0fb49ccb6b910bdbd7e76a9960394e3

include $(INCLUDE_DIR)/nls.mk
include $(INCLUDE_DIR)/package.mk

define Package/gatling
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Web
  TITLE:=gatling - a high performance web server
  VERSION:=$(PKG_VERSION)-$(PKG_RELEASE)
  URL:=http://www.fefe.de/gatling/
  DEPENDS:=$(ICONV_DEPENDS) +zlib +libowfat +libopenssl
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include/libowfat \
		 -I$(ICONV_PREFIX)/include

TARGET_LDFLAGS += -L$(STAGING_DIR)/usr/lib \
		  -L$(ICONV_PREFIX)/lib \
		  -Wl,-Bdynamic,$(LIBGCC_S)

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/gatling $(1)/bin
endef

$(eval $(call BuildPackage,gatling))
