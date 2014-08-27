# 
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=smap
PKG_VERSION:=0.6.0
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-20081016.tar.gz
PKG_SOURCE_URL:=http://www.wormulon.net/smap/
PKG_MD5SUM:=814456ccc8fea5688382b7ec55fe44eb

PKG_BUILD_DIR:=$(BUILD_DIR)/smap-$(PKG_VERSION)/smap
PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

LIBTOOL="$(STAGING_DIR)/host/bin/libtool"

define Package/smap
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Telephony
  TITLE:=A SIP network discovery tool
  MAINTAINER:=Daniel Dickinson <openwrt@cshore.neomailbox.net>
  URL:=http://www.wormulon.net/smap/
  DEPENDS:=+libpthread
endef

define Package/smap/description
 Discovers and identifies SIP devices on the network including hardware
 phones, softphones, PBX software, and PBX equipment.
endef

# define Package/smap/conffiles
# /etc/config/smap
# endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		LD="$(TARGET_LD)" \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include -DRAW_SOCKET -DHAVE_RANDOM -DSMAP_OS=linux" \
		LDFLAGS="$(TARGET_LDFLAGS) -lm -lpthread" \
		LIBTOOL="$(LIBTOOL)" \
		smap
endef

define Package/smap/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/smap $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/share/smap
	$(CP) $(PKG_BUILD_DIR)/fingerprint.db $(1)/usr/share/smap
endef

$(eval $(call BuildPackage,smap))
