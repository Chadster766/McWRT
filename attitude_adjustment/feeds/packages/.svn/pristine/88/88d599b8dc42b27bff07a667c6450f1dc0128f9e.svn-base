#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nufw
PKG_VERSION:=2.2.21
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.nufw.org/download/nufw/
PKG_MD5SUM:=7eb500e7ddb3db622dc6c5d43b495c9a

PKG_FIXUP:=autoreconf
PKG_REMOVE_FILES:=autogen.sh aclocal.m4

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/nufw/Default
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Firewall
  DEPENDS:=+glib2 +libgnutls +libsasl2 +libnetfilter-queue $(ICONV_DEPENDS) $(INTL_DEPENDS)
  TITLE:=nufw
  URL:=http://www.nufw.org/
endef

define Package/nufw/description
  NuFW adds user-based filtering to Netfilter,
  the state of the art IP filtering layer from
  the Linux kernel. Its exclusive algorithm allows
  authenticated filtering even on multiuser computers.
endef

define Package/nufw
  $(call Package/nufw/Default)
endef

define Package/nutcpc
  $(call Package/nufw/Default)
  TITLE:=client version
endef

define Package/nufw-python
  $(call Package/nufw/Default)
  TITLE:=python bindings
endef

define PyPackage/nufw-python/filespec
+|$(PYTHON_PKG_DIR)/nuauth_command/__init__.py
+|$(PYTHON_PKG_DIR)/nuauth_command/client.py
+|$(PYTHON_PKG_DIR)/nuauth_command/command_dec.py
+|$(PYTHON_PKG_DIR)/nuauth_command/version.py
endef


define Package/nufw-modules
  $(call Package/nufw/Default)
  TITLE:=NuFW modules
endef

CONFIGURE_ARGS += \
	--enable-static \
	--enable-shared \
	--disable-pam-nufw \
	--without-system-auth \
	--with-libgnutls-prefix="$(STAGING_DIR)/usr/" \
	--with-libgcrypt-prefix="$(STAGING_DIR)/usr/" \
	--with-libsasl-prefix="$(STAGING_DIR)/usr/"

define Build/Configure
	(cd $(PKG_BUILD_DIR) ; test -x ./autogen.sh && ./autogen.sh || true)
	$(call Build/Configure/Default)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS) \
			-I$(INTL_PREFIX)/include \
			-I$(ICONV_PREFIX)/include" \
		LDFLAGS="$(TARGET_LDFLAGS) \
			-L$(INTL_PREFIX)/lib \
			-L$(ICONV_PREFIX)/lib \
			-Wl,-rpath-link,$(STAGING_DIR)/usr/lib -liconv \
			-lgnutls -lnetfilter_queue -lpthread -lgcrypt" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef


define Package/nufw/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/nufw $(1)/usr/sbin/
endef

define Package/nutcpc/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/nutcpc $(1)/usr/bin/	
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnuclient.so* $(1)/usr/lib/
endef

define Package/nufw-modules/install
	$(INSTALL_DIR) $(1)/usr/lib/nuauth/modules/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/nuauth/modules/*.so* $(1)/usr/lib/nuauth/modules/
endef

$(eval $(call BuildPackage,nufw))
$(eval $(call BuildPackage,nutcpc))
$(eval $(call BuildPackage,nufw-modules))
$(eval $(call PyPackage,nufw-python))
$(eval $(call BuildPackage,nufw-python))
