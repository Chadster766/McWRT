#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sscep
PKG_VERSION:=20040325+ng2
PKG_RELEASE:=1

PKG_SOURCE:=${PKG_NAME}.tgz
PKG_SOURCE_URL:=http://www.klake.org/~jt/sscep/
PKG_MD5SUM:=b509cdf0de952841471e77499aa6e4ca

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/sscep
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libopenssl
  TITLE:=A Simple SCEP (Simple Certificate Enrollment Protocol) client
  URL:=http://www.klake.org/~jt/sscep/
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="$(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/sscep/conffiles
/etc/sscep.conf
endef

define Package/sscep/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/sscep.conf $(1)/etc/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sscep $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,sscep))
