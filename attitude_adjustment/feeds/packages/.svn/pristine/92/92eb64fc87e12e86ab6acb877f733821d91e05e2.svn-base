#
# Copyright (C) 2007-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=karma
PKG_VERSION:=20060124
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://theta44.org/software/
PKG_MD5SUM:=e9d4ccbda89b4b1cd70eefb1db339d0a

include $(INCLUDE_DIR)/package.mk

define Package/karma
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=wireless
  DEPENDS:=+kmod-madwifi +ruby +libpcap +libncurses
  TITLE:=set of tools for assessing the security of wireless clients
  URL:=http://theta44.org/karma/index.html
endef

define Package/karma/description
  KARMA is a set of tools for assessing the security of wireless clients
  at multiple layers. Wireless sniffing tools discover clients and their
  preferred/trusted networks by passively listening for 802.11 Probe Request
  frames.
endef

define Package/karma/conffiles
/etc/karma-lan.xml
/etc/karma-scan.xml
/etc/karma.xml
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/src/ \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
		LDLIBS="$(TARGET_LDFLAGS) -lpcap -lncurses"
endef

define Package/karma/install	
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/karma $(1)/usr/sbin/karma-ruby
	$(CP) $(PKG_BUILD_DIR)/src/karma $(1)/usr/sbin/karma
	$(INSTALL_DIR) $(1)/etc
	$(CP) $(PKG_BUILD_DIR)/etc/* $(1)/etc/
endef

$(eval $(call BuildPackage,karma))
