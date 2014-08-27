# 
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sslcat
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME).tgz
PKG_SOURCE_URL:=http://www.bindshell.net/tools/sslcat/
PKG_MD5SUM:=27a88e2143cbfe216e13e9493bacd32e

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/sslcat
  SECTION:=net
  CATEGORY:=Network
  TITLE:=A netcat like utility with SSL support
  URL:=http://www.bindshell.net/tools/sslcat
  DEPENDS:= +libopenssl
endef

define Package/sslcat/description
 SSLCat is a simple Unix utility that reads and writes data across an 
 SSL enable network connection.
endef

CONFIGURE_ARGS+= \
	--with-openssl=$(STAGING_DIR)/usr \

define Package/sslcat/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sslcat $(1)/usr/bin/
endef

$(eval $(call BuildPackage,sslcat))
