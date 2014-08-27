#
# Copyright (C) 2005-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mt-daapd
PKG_VERSION:=0.2.4.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=67bef9fb14d487693b0dfb792c3f1b05

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/mt-daapd
  SECTION:=sound
  CATEGORY:=Sound
  DEPENDS:=+libgdbm +libhowl +libid3tag +libpthread
  TITLE:=A multi-threaded DAAP (Digital Audio Access Protocol) daemon
  URL:=http://www.fireflymediaserver.org/
endef

define Package/mt-daapd/description
	mt-daapd is a program which acts as an iTunes (DAAP) server for Linux.
endef

define Package/mt-daapd/conffiles
/etc/mt-daapd.conf
/etc/mt-daapd.playlist
endef

CONFIGURE_ARGS += \
	--enable-mdns \
	--enable-howl \
	--with-id3tag="$(STAGING_DIR)/usr" \
	--with-gdbm-includes="$(STAGING_DIR)/usr/include" \
	--with-gdbm-libs="$(STAGING_DIR)/usr/lib" \
	--with-howl-includes="$(STAGING_DIR)/usr/include/howl" \
	--with-howl-libs="$(STAGING_DIR)/usr/lib" \
	--without-static-libs \

define Package/mt-daapd/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/contrib/mt-daapd.conf $(1)/etc/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/contrib/mt-daapd.playlist $(1)/etc/
	$(INSTALL_DIR) $(1)/usr/share
	$(CP) $(PKG_INSTALL_DIR)/usr/share/mt-daapd $(1)/usr/share/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/mt-daapd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/mt-daapd.init $(1)/etc/init.d/mt-daapd
endef

$(eval $(call BuildPackage,mt-daapd))
