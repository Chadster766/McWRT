#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=lftp
PKG_VERSION:=4.3.6
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=http://lftp.cybermirror.org \
  http://lftp.cybermirror.org/old
PKG_MD5SUM:=ffcdbaec5d1416cb6dd754efd43fa61a

PKG_INSTALL:=1

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk

define Package/lftp
  SUBMENU:=File Transfer
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libncurses +libopenssl +libreadline $(CXX_DEPENDS) +libexpat
  TITLE:=Sophisticated file transfer program
  URL:=http://lftp.yar.ru/
endef

define Package/lftp/description
  lftp is sophisticated file transfer program with command line interface. It
  supports FTP, HTTP, FISH, SFTP and FILE (local FS) protocols. GNU Readline
  library is used for input. BitTorrent protocol is supported as built-in
  `torrent' command.
endef

CONFIGURE_ARGS += \
	--without-libiconv-prefix \
	--without-libintl-prefix \
	--without-gnutls \
	--without-libresolv \
	--with-openssl="$(STAGING_DIR)/usr" \
	--disable-static

CONFIGURE_VARS += \
	LIBS="-lz -lutil -lcurses -ldl" \
	i_cv_posix_fallocate_works=no

MAKE_VARS += \
	LD="$(TARGET_CXX)"

define Package/lftp/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/lftp $(1)/usr/bin/
endef

$(eval $(call BuildPackage,lftp))
