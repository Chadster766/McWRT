#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ostiary
PKG_VERSION:=4.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://ingles.homeunix.net/software/ost/latest/
PKG_MD5SUM:=420d1fda246097d7e4d1277f39069578

include $(INCLUDE_DIR)/package.mk

define Package/ostiary
  SECTION:=admin
  CATEGORY:=Administration
  TITLE:=Simple, Secure Remote Script Execution
  URL:=http://ingles.homeunix.net/software/ost/index.html
endef

define Package/ostiary/description
 Designed to allow you to run a fixed set of commands remotely,
 without giving everyone else access to the same commands.

 Provides - ostiaryd daemon (~25kb)
          - ostclient (~9kb)
endef

EXTRA_CFLAGS += $(TARGET_CPPFLAGS)

CONFIGURE_ARGS += \
    --prefix="/usr/bin" \
    --sysconfdir="/etc/ostiaryd" \
    CFLAGS="$(TARGET_CFLAGS) $(EXTRA_CFLAGS)"

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		INSTALL_OPTS="" \
		STRIP="" \
		ostiaryd ostclient
endef

define Package/ostiary/install
	$(INSTALL_DIR) $(1)/etc/ostiaryd
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/tests/ostiary.cfg $(1)/etc/ostiaryd/ostiary.cfg
	$(INSTALL_DIR) $(1)/etc/ostiaryd/scripts
	$(INSTALL_BIN) ./files/{ssh_enable,ssh_disable} $(1)/etc/ostiaryd/scripts
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/ostiaryd.init $(1)/etc/init.d/ostiaryd
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/{ostiaryd,ostclient} $(1)/usr/bin/
endef




define Package/ostiary/postinst
#!/bin/sh
# check if we are on real system
if [ -z "$${IPKG_INSTROOT}" ]; then
	echo "Enabling rc.d symlink for ostiaryd"
	/etc/init.d/ostiaryd enable
fi
exit 0
endef

define Package/ostiary/prerm
#!/bin/sh
# check if we are on real system
if [ -z "$${IPKG_INSTROOT}" ]; then
	/etc/init.d/ostiaryd stop
	echo "Removing rc.d symlink for ostiaryd"
	/etc/init.d/ostiaryd disable
	echo "Any config backups and scripts you created are not removed automatically."
	echo "To finish cleanup, backup any files you want to keep, then run rm -f /etc/ostiaryd"
fi
exit 0
endef


$(eval $(call BuildPackage,ostiary))
