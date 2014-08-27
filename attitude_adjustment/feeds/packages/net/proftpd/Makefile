#
# Copyright (C) 2009-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=proftpd
PKG_VERSION:=1.3.4b
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.proftpd.org/distrib/source
PKG_MD5SUM:=0871e0b93c9c3c88ca950b6d9a04aed2

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/proftpd
  SUBMENU:=File Transfer
  SECTION:=net
  CATEGORY:=Network
  TITLE:=ProFTPD FTP server
  URL:=http://www.proftpd.org/
  MAINTAINER:=Cezary Jackiewicz <cezary@eko.one.pl>
endef

define Package/proftpd/conffiles
/etc/proftpd.conf
endef

TARGET_CPPFLAGS += \
	-I$(LINUX_DIR)/include

MAKE_FLAGS += \
	INSTALL_USER=$(shell id -u) \
	INSTALL_GROUP=$(shell id -g)

CONFIGURE_ARGS += \
	--disable-cap \
	--enable-devel \
	ac_cv_header_curses_h=no \
	ac_cv_header_ncurses_h=no

define Package/proftpd/install
	$(INSTALL_DIR) $(1)/etc
	$(if $(CONFIG_IPV6),,$(SED) 's/^UseIPv6/#UseIPv6/' $(PKG_INSTALL_DIR)/etc/proftpd.conf)
	$(INSTALL_CONF) $(PKG_INSTALL_DIR)/etc/proftpd.conf $(1)/etc/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/proftpd.init $(1)/etc/init.d/proftpd
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/ftp{count,top,who} $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/{ftpshut,proftpd} $(1)/usr/sbin/
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/in.proftpd $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,proftpd))
