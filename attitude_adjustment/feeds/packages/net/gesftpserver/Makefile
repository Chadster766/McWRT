#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sftpserver
PKG_REV:=228
PKG_VERSION:=r$(PKG_REV)
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.greenend.org.uk/rjk/bzr/sftpserver.dev
PKG_SOURCE_PROTO:=bzr
PKG_SOURCE_SUBDIR:=$(PKG_NAME)
PKG_SOURCE_VERSION:=$(PKG_REV)

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/gesftpserver
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=SSH
  DEPENDS:=+libpthread $(ICONV_DEPENDS)
  TITLE:=SFTP server
  URL:=http://www.greenend.org.uk/rjk/sftpserver/
endef

define Package/gesftpserver/description
This is an experimental free SFTP server. It is currently still under development
(so don't trust it with critical data). It should be possible to use it as a
drop-in replacement for the widely-used OpenSSH SFTP server. It does not include
an SSH server, but rather runs as a subsystem of one.
endef

CONFIGURE_CMD:=./prepare

TAR_CMD:=$(HOST_TAR) -C $(PKG_BUILD_DIR) --strip-components 1 $(TAR_OPTIONS)

TARGET_LDFLAGS+=-lpthread

define Package/gesftpserver/install	
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/gesftpserver $(1)/usr/bin/
endef

$(eval $(call BuildPackage,gesftpserver))
