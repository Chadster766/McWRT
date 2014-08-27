#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=SDL_mixer
PKG_VERSION:=1.2.11
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.libsdl.org/projects/SDL_mixer/release/
PKG_MD5SUM:=65ada3d997fe85109191a5fb083f248c

PKG_FIXUP:=autoreconf
PKG_MACRO_PATHS:=acinclude
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

TARGET_LDFLAGS += -Wl,-rpath-link,$(STAGING_DIR)/usr/lib

define Package/libsdl-mixer
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Simple DirectMedia Layer Sound Mixer
  URL:=http://www.libsdl.org/projects/SDL_mixer/
  DEPENDS:=+libsdl +libmikmod +libvorbisidec
endef

define Package/libsdl-mixer/description
  SDL_mixer is a sample multi-channel audio mixer library.
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--disable-sdltest \
		--enable-music-cmd \
		--enable-music-wave \
		--enable-music-mod \
		--enable-music-mod-shared \
		--enable-music-midi \
		--disable-music-ogg \
		--enable-music-ogg-tremor \
		--enable-music-ogg-shared \
		--disable-music-flac \
		--enable-music-flag-shared \
		--disable-music-mp3 \
		--enable-music-mp3-shared \
		--disable-smpegtest \
		--disable-music-mp3-mad-gpl \
		--with-sdl-exec-prefix="$(STAGING_DIR)/usr" \
	)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/SDL $(1)/usr/lib/pkgconfig
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/SDL/SDL_mixer.h \
		$(1)/usr/include/SDL/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libSDL_mixer*.{a,la,so*} \
		$(1)/usr/lib/
	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/* \
		$(1)/usr/lib/pkgconfig/
endef

define Package/libsdl-mixer/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libSDL_mixer*.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libsdl-mixer))
