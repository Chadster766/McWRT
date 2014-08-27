#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v3.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=fancontroled
PKG_VERSION:=0.1
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar
PKG_SOURCE_URL:=http://code.lukaperkov.net/fancontroled/
PKG_MD5SUM:=8c3a7efa99e58216ad4633d1e6ed1358

include $(INCLUDE_DIR)/package.mk

define Package/fancontroled
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=fancontroled
  DEPENDS:=+libsensors
  URL:=http://fancontroled.lukaperkov.net/
endef

define Package/fancontroled/description
	fancontroled is linux daemon written in C implementing a temperature dependent fan speed control.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
	$(TARGET_CONFIGURE_OPTS) \
	CFLAGS="$(TARGET_CFLAGS) $(EXTRA_CPPFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)" \
	INC_PATH="-I$(STAGING_DIR)/usr/include/ -I$(STAGING_DIR)/usr/lib/"

	$(STRIP) $(PKG_BUILD_DIR)/$(PKG_NAME)

	mkdir -p $(PKG_INSTALL_DIR)/usr/sbin
	$(MAKE) -C $(PKG_BUILD_DIR) \
	DESTDIR="$(PKG_INSTALL_DIR)" \
	install
endef

define Package/fancontroled/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/$(PKG_NAME) $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/fancontroled.init $(1)/etc/init.d/fancontroled
endef

$(eval $(call BuildPackage,fancontroled))
