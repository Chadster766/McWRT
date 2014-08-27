#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pianobar
PKG_VERSION:=2012.01.10
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://6xq.net/static/projects/pianobar
PKG_MD5SUM:=448c57d65c71158674e8b30a8cfa217e

PKG_BUILD_DEPENDS:=libao libfaad2 libmad
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/pianobar/Default
  DEPENDS:=+libgnutls
  TITLE:=Pandora radio
  URL:=http://6xq.net/projects/pianobar/
endef

define Package/pianobar/Default/description
  Pianobar is a free/open-source, console-based replacement for Pandora’s flash
  player.
endef

define Package/pianobar
  $(call Package/pianobar/Default)
  SECTION:=sound
  CATEGORY:=Sound
  DEPENDS+= +libao +libfaad2 +libmad +libpiano +libpthread
  TITLE+= console client
endef

define Package/pianobar/description
  $(call Package/pianobar/Default/description)
endef

define Package/libpiano
  $(call Package/pianobar/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE+= library
endef

define Package/libpiano/description
  $(call Package/pianobar/Default/description)
  This package contains the libpiano shared library.
endef

TARGET_CFLAGS += -std=c99

MAKE_FLAGS += \
	DYNLINK="1" \
	PREFIX="/usr"

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/*.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libpiano.{a,so*} $(1)/usr/lib/
endef

define Package/pianobar/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/pianobar $(1)/usr/bin/
endef

define Package/libpiano/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libpiano.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,pianobar))
$(eval $(call BuildPackage,libpiano))
