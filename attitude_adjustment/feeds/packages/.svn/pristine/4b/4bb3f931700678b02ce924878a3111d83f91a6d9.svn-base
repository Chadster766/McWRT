#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=shell-fm
PKG_REV:=c274aa51b52185b289ac22f0cb95420a1c19868b
PKG_VERSION:=0.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://github.com/jkramer/shell-fm.git
PKG_SOURCE_VERSION:=$(PKG_REV)

include $(INCLUDE_DIR)/package.mk

define Package/shell-fm
  SECTION:=sound
  CATEGORY:=Sound
  DEPENDS:=+libao +libpthread +libmad
  TITLE:=Console Based Last.FM Radio Player
  URL:=http://nex.scrapping.cc
endef

define Package/shell-fm/description
	Shell.FM is a console based player for the streams provided
	by Last.FM for the Linux platform. It's lightweight and easy to use.
endef

define Package/shell-fm/conffiles
/etc/shell-fm.rc
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I./include/ -DLIBAO $(TARGET_CPPFLAGS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		LDFLAGS="$(TARGET_LDFLAGS) -lmad -lao -lpthread" \
		all install
endef

define Package/shell-fm/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/shell-fm $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_CONF) ./files/shell-fm.rc $(1)/etc/shell-fm.rc
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/shell-fm.init $(1)/etc/init.d/shell-fm
endef

$(eval $(call BuildPackage,shell-fm))
