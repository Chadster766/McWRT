#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sudo
PKG_VERSION:=1.7.8p1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.sudo.ws/sudo/dist
PKG_MD5SUM:=5552b9aabc43e34bbc8dc557c098a933

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/sudo
  SECTION:=admin
  CATEGORY:=Administration
  TITLE:=Delegate authority to run commands
  URL:=http://www.sudo.ws/
endef

define Package/sudo/description
 Sudo (su "do") allows a system administrator to delegate authority to
 give certain users (or groups of users) the ability to run some (or
 all) commands as root or another user while providing an audit trail of
 the commands and their arguments.
endef

define Package/sudo/conffiles
/etc/sudoers
endef

CONFIGURE_ARGS+= \
	--without-pam \
	--disable-pam-session \
	--with-editor=/bin/vi \
	--without-lecture \
	--disable-zlib

CONFIGURE_VARS+= \
	sudo_cv_uid_t_len=10 \
	sudo_cv_func_unsetenv_void=no

define Package/sudo/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/sudo $(1)/usr/bin/
	chmod 4755 $(1)/usr/bin/sudo
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/visudo $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc
	$(CP) $(PKG_INSTALL_DIR)/etc/sudoers $(1)/etc/
	chmod 0440 $(1)/etc/sudoers
	$(INSTALL_DIR) $(1)/etc/sudoers.d
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/sudo_noexec.so $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/sudo.init $(1)/etc/init.d/sudo
endef

define Package/sudo/postinst
#!/bin/sh

[ -n "$$IPKG_INSTROOT" ] || {
	/etc/init.d/sudo enable
	/etc/init.d/sudo start
}
endef

$(eval $(call BuildPackage,sudo))
