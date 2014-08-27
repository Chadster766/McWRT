#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=scmpc
PKG_VERSION:=0.2.2
PKG_RELEASE:=4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://download.berlios.de/scmpc
PKG_MD5SUM:=f42482e4dbf398df92a36d5610b403e5

include $(INCLUDE_DIR)/package.mk

define Package/scmpc
  SECTION:=sound
  CATEGORY:=Sound
  TITLE:=A MPD client which submits your tracks to Last.Fm
  DEPENDS:=+libcurl +libdaemon +argtable +confuse +libpthread
  URL:=http://scmpc.berlios.de/
endef

define Package/scmpc/description
  scmpc is a multithreaded, robust client for MPD to submit tracks to Audioscrobbler (Last.fm).
endef

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--disable-rpath \
	--without-libiconv-prefix \
	--without-libintl-prefix \

MAKE_FLAGS += \
	$(TARGET_CONFIGURE_OPTS) \
	$(1) \

TARGET_LDFLAGS += \
	-Wl,-rpath-link=$(STAGING_DIR)/usr/lib

define Package/scmpc/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/src/scmpc $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/scmpc.init $(1)/etc/init.d/scmpc
	$(INSTALL_DIR) $(1)/etc
	$(CP) $(PKG_BUILD_DIR)/examples/scmpc.conf $(1)/etc/
endef

define Package/scmpc/conffiles
/etc/scmpc.conf
endef

$(eval $(call BuildPackage,scmpc))
