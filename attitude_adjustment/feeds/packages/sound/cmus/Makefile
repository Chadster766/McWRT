#
# Copyright (C) 2011 Johannes Weißl <jargon@molb.org>
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=cmus
PKG_VERSION:=2.4.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-v$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/cmus
PKG_MD5SUM:=0c5a9f4032e632e5f6b6a49f53df1e7e

PKG_INSTALL:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/cmus-v$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/cmus
  SECTION:=sound
  CATEGORY:=Sound
  DEPENDS:= \
	+libpthread +librt +libncursesw $(ICONV_DEPENDS) \
	+AUDIO_SUPPORT:alsa-lib \
	+BUILD_PATENTED:libmad +libvorbisidec +libflac
  TITLE:=C* Music Player
  URL:=http://cmus.sourceforge.net/
endef

define Package/cmus/description
	C* Music Player is a modular and very configurable ncurses-based
	audio player. It has some interesting features like configurable
	colorscheme, mp3 and ogg streaming, it can be controlled with an
	UNIX socket, filters, album/artists sorting and a vi-like
	configuration interface.
endef

define Build/Configure
	# this is *NOT* GNU configure
	( cd $(PKG_BUILD_DIR); \
		CROSS="$(TARGET_CROSS)" \
		CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS) -I$(ICONV_PREFIX)/include" \
		LDFLAGS="$(TARGET_LDFLAGS) -Wl,-rpath-link=$(STAGING_DIR)/usr/lib -L$(ICONV_PREFIX)/lib" \
		PKG_CONFIG_PATH="$(STAGING_DIR)/usr/lib/pkgconfig" \
		PKG_CONFIG_LIBDIR="$(STAGING_DIR)/usr/lib/pkgconfig" \
		./configure \
		prefix="/usr" \
		CONFIG_FLAC=a \
		CONFIG_MAD=$(if $(CONFIG_BUILD_PATENTED),a,n) \
		CONFIG_MODPLUG=n \
		CONFIG_MIKMOD=n \
		CONFIG_MPC=n \
		CONFIG_VORBIS=a \
		CONFIG_TREMOR=y \
		CONFIG_WAVPACK=n \
		CONFIG_MP4=n \
		CONFIG_AAC=n \
		CONFIG_FFMPEG=n \
		CONFIG_ROAR=n \
		CONFIG_PULSE=n \
		CONFIG_ALSA=$(if $(CONFIG_AUDIO_SUPPORT),a,n) \
		CONFIG_AO=n \
		CONFIG_ARTS=n \
		CONFIG_OSS=y \
		CONFIG_SUN=n \
		CONFIG_WAVEOUT=n \
		DEBUG=0 \
	)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		V=$(V) \
		install
endef

define Package/cmus/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/lib/cmus/ip
	$(INSTALL_DIR) $(1)/usr/lib/cmus/op
	$(INSTALL_DIR) $(1)/usr/share/cmus
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/cmus $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/cmus/ip/*.so $(1)/usr/lib/cmus/ip/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/cmus/op/*.so $(1)/usr/lib/cmus/op/
	$(CP) $(PKG_INSTALL_DIR)/usr/share/cmus/{rc,default.theme} $(1)/usr/share/cmus/
endef

$(eval $(call BuildPackage,cmus))

