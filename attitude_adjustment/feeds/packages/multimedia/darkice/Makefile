#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=darkice
PKG_VERSION:=1.0
PKG_RELEASE:=4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://darkice.googlecode.com/files/
PKG_MD5SUM:= 1804e63d42a9703d01fe378c9a77c473

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/darkice
  SECTION:=multimedia
  CATEGORY:=Multimedia
  SUBMENU:=Streaming
  TITLE:=DarkIce is a live audio streamer
  DEPENDS:=+lame-lib +librt +libpthread +libstdcpp
  MAINTAINER:=Artur Wronowski <arteqw@gmail.com>
endef

define Package/darkice/description
DarkIce is a live audio streamer. It records audio from an audio interface (e.g. sound card), encodes it and sends it to a streaming server.
endef

CONFIGURE_ARGS += \
	--with-lame \
	--without-aacplus \
	--without-alsa \
	--without-faac \
	--without-jack \
	--without-samplerate \
	--without-twolame \
	--without-vorbis \
	--with-lame-prefix="$(STAGING_DIR)/usr" \

define Package/darkice/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/darkice $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/etc/darkice.cfg $(1)/etc/
endef

define Package/darkice/conffiles
/etc/darkice.cfg
endef

$(eval $(call BuildPackage,darkice))
