#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mii-tool
PKG_VERSION:=1.9.1.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://downloads.openwrt.org/sources
PKG_MD5SUM:=f65dbeb1c2392acffd4c94138b1f2e52

include $(INCLUDE_DIR)/package.mk

define Package/mii-tool
  SECTION:=net
  CATEGORY:=Network
  TITLE:=configure media type using MII commands
  URL:=http://freshmeat.net/projects/mii-tool/
endef

define Package/mii-tool/description
	The mii-tool command allows you to set or autodetect the media type
	or mii chipset-based ethernet devices. It traditionally had been
	distributed in the net-tools package. This is a single distribution
	optimized for embedded systems and fully automated cross/-sysroot-builds
endef

MAKE_FLAGS += CFLAGS="$(TARGET_CFLAGS) -D_GNU_SOURCE -I. -idirafter ./include/"

define Package/mii-tool/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/mii-tool $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,mii-tool))
