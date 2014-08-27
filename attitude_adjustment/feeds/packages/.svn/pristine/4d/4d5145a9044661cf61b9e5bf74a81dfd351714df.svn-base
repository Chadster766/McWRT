#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rssh
PKG_VERSION:=2.3.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/rssh
PKG_MD5SUM:=b0c147602fcc95737ed50573b92fc468

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/rssh
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=SSH
  TITLE:=restricted shell for use with OpenSSH
  URL:=http://www.pizzashack.org/rssh/index.shtml
endef

define Package/rssh/description
	rssh is a restricted shell for use with OpenSSH, allowing only scp and/or sftp.
	It now also includes support for rdist, rsync, and cvs. For example, if you have
	a server which you only want to allow users to copy files off of via scp, without
	providing shell access, you can use rssh to do that.
endef

define Package/rssh/conffiles
/etc/rssh.conf
endef

CONFIGURE_ARGS += \
	--with-sftp-server="/usr/libexec/sftp-server" \

CONFIGURE_VARS += \
	ac_cv_func_malloc_0_nonnull=yes \

define Package/rssh/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/$(PKG_NAME).conf $(1)/etc/
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/$(PKG_NAME) $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/rssh_chroot_helper $(1)/usr/lib/
endef

define Package/rssh/postinst
#!/bin/sh
grep rssh $${IPKG_INSTROOT}/etc/shells || \
	echo "/usr/bin/rssh" >> $${IPKG_INSTROOT}/etc/shells
endef

$(eval $(call BuildPackage,rssh))
