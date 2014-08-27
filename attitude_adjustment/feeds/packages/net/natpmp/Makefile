#
# Copyright (C) 2007-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=natpmp
PKG_VERSION:=0.2.3
PKG_RELEASE:=5

PKG_SOURCE_URL:=http://download.savannah.nongnu.org/releases/natpmp/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_MD5SUM:=f696565d59c488b9ad9f43625be93377

include $(INCLUDE_DIR)/package.mk

define Package/natpmp
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+ip
  SUBMENU:=Firewall
  TITLE:=A daemon implementing NAT-PMP
  URL:=http://savannah.nongnu.org/projects/natpmp
endef

define Package/natpmp/description
	This is a daemon implementing NAT-PMP. NAT-PMP is a protocol for
	handling port forwarding requests from clients behind a NAT.
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CC="$(TARGET_CC)" \
		LD="$(TARGET_CC)" \
		CFLAGS="$(strip $(TARGET_CFLAGS))" \
		CPPFLAGS="$$$$CPPFLAGS $(TARGET_CPPFLAGS) -DIPTABLES_PATH=\\\"/usr/sbin\\\"" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		prefix="$(PKG_INSTALL_DIR)/usr"
endef

define Package/natpmp/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/natpmp $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/natpmp.config $(1)/etc/config/natpmp
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/natpmp.init $(1)/etc/init.d/natpmp
endef

define Package/natpmp/conffiles
/etc/config/natpmp
endef

$(eval $(call BuildPackage,natpmp))
