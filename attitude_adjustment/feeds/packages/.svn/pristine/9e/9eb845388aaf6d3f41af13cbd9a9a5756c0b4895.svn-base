#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=scponly
PKG_VERSION:=4.8
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=139ac9abd7f3b8dbc5c5520745318f8a

include $(INCLUDE_DIR)/package.mk

define Package/scponly
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Alternative 'shell' (of sorts) for sysadmins
  URL:=http://sourceforge.net/projects/scponly
endef

define Package/scponly/description
	"scponly" is an alternative 'shell' (of sorts) for system
	administrators who would like to provide access to remote
	users to both read and write local files without providing
	any remote execution priviledges. Functionally, it is best
	described as a wrapper to the mostly trusted suite of ssh
	applications.
endef

CONFIGURE_ARGS += \
	ac_cv_path_scponly_PROG_SFTP_SERVER="/usr/libexec/sftp-server" \
	\
	--enable-winscp-compat \
	--enable-scp-compat \
	--disable-rsync-compat \
	--with-sftp-server="/usr/libexec/sftp-server" \

define Package/scponly/postinst
#!/bin/sh
echo "/usr/bin/scponly" >> $${IPKG_INSTROOT}/etc/shells
endef

define Package/scponly/install  
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/bin/
endef

$(eval $(call BuildPackage,scponly))
