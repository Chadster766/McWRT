#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=amwall
PKG_VERSION:=0.1.0
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://ftp.amselinux.net/source/
PKG_MD5SUM:=a8b6438e7fd9ccadc9f68824ae62349c

include $(INCLUDE_DIR)/package.mk

define Package/amwall
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libamsel
  TITLE:=PF to iptables
  URL:=http://www.amselinux.net/
  SUBMENU:=Firewall
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		V= \
		LDIR="$(PKG_BUILD_DIR)" \
		AMSEL_INCLUDE="$(STAGING_DIR)/usr/include" \
		OPT_LDFLAGS="$(TARGET_LDFLAGS)" \
		PREFIX="/usr" \
		CONFIG="$(PKG_BUILD_DIR)/etc/amsel" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		SHELL="/bin/bash" \
		all install
endef

define Package/amwall/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/* $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/* $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/amsel
	$(CP) $(PKG_INSTALL_DIR)/etc/amsel* $(1)/etc/
endef

define Package/amwall/conffiles
/etc/amsel/firewall.conf
endef

$(eval $(call BuildPackage,amwall))
