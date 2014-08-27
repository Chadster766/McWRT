# 
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gst-ffmpeg
PKG_RELEASE:=1
PKG_SOURCE_URL:=http://gstreamer.freedesktop.org/src/gst-ffmpeg
PKG_VERSION:=0.10.11
PKG_MD5SUM:=0d23197ba7ac06ea34fa66d38469ebe5

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

FFMPEG_ENCODERS:= \
	ac3 \
	jpegls \
	mpeg1video \
	mpeg2video \
	mpeg4 \
	pcm_s16be \
	pcm_s16le \
	png \
	vorbis \
	zlib \

FFMPEG_DECODERS:= \
	aac \
	ac3 \
	atrac3 \
	gif \
	h264 \
	jpegls \
	mp2 \
	mp3 \
	mpeg1video \
	mpeg2video \
	mpeg4 \
	mpeg4aac \
	mpegvideo \
	pcm_s16be \
	pcm_s16le \
	png \
	vorbis \
	wmav1 \
	wmav2 \
	zlib \

FFMPEG_MUXERS:= \
	ac3 \
	ffm \
	h264 \
	mp3 \
	mp4 \
	mpeg1video \
	mpeg2video \
	mpegts \
	ogg \
	oss \
	rtp \

FFMPEG_DEMUXERS:= \
	ac3 \
	ffm \
	h264 \
	mp3 \
	mpegps \
	mpegts \
	mpegvideo \
	ogg \
	rm \
	rtsp \
	sdp \
	v4l2 \

FFMPEG_PARSERS:= \
	aac \
	ac3 \
	h264 \
	mpegaudio \
	mpegvideo \
	mpeg4video \

FFMPEG_PROTOCOLS:= \
	file http pipe rtp tcp udp

PKG_CONFIG_DEPENDS:= \
	$(patsubst %,CONFIG_GST_FFMPEG_ENCODER_%,$(FFMPEG_ENCODERS)) \
	$(patsubst %,CONFIG_GST_FFMPEG_DECODER_%,$(FFMPEG_DECODERS)) \
	$(patsubst %,CONFIG_GST_FFMPEG_MUXER_%,$(FFMPEG_DEMUXERS)) \
	$(patsubst %,CONFIG_GST_FFMPEG_DEMUXER_%,$(FFMPEG_DEMUXERS)) \
	$(patsubst %,CONFIG_GST_FFMPEG_PARSER_%,$(FFMPEG_PARSERS)) \
	$(patsubst %,CONFIG_GST_FFMPEG_PROTOCOL_%,$(FFMPEG_PROTOCOLS))

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/gst-ffmpeg
  SECTION:=multimedia
  CATEGORY:=Multimedia
  TITLE:=GStreamer ffmpeg bindings
  URL:=http://www.gstreamer.net/modules/gst-ffmpeg.html
  MAINTAINER:=Okkel Klaver <info@vanhetland.nl>
  DEPENDS:= +libgstreamer +gst-mod-alsa +libbz2 +libgstaudio
endef

define Package/gst-ffmpeg/description
  GStreamer ffmpeg bindings
endef

define Package/gst-ffmpeg/config
source "$(SOURCE)/Config.in"
endef

FILTER_CONFIG= \
	$(foreach c, $(3), \
		$(if $(CONFIG_GST_FFMPEG_$(1)_$(c)),--enable-$(2)='$(c)') \
	)

FFMPEG_CONFIGURE_ENCODERS:=$(call FILTER_CONFIG,ENCODER,encoder,$(FFMPEG_ENCODERS))
FFMPEG_CONFIGURE_DECODERS:=$(call FILTER_CONFIG,DECODER,decoder,$(FFMPEG_DECODERS))
FFMPEG_CONFIGURE_MUXERS:=$(call FILTER_CONFIG,MUXER,muxer,$(FFMPEG_MUXERS))
FFMPEG_CONFIGURE_DEMUXERS:=$(call FILTER_CONFIG,DEMUXER,demuxer,$(FFMPEG_DEMUXERS))
FFMPEG_CONFIGURE_PARSERS:=$(call FILTER_CONFIG,PARSER,parser,$(FFMPEG_PARSERS))
FFMPEG_CONFIGURE_PROTOCOLS:=$(call FILTER_CONFIG,PROTOCOL,protocol,$(FFMPEG_PROTOCOLS))

GST_VERSION:=0.10

CONFIGURE_ARGS += \
	--with-ffmpeg-extra-configure="--target-os=linux \
		--disable-bsfs \
		--disable-devices \
		--disable-encoders \
		$(FFMPEG_CONFIGURE_ENCODERS) \
		--disable-decoders \
		$(FFMPEG_CONFIGURE_DECODERS) \
		--disable-muxers \
		$(FFMPEG_CONFIGURE_MUXERS) \
		--disable-demuxers \
		$(FFMPEG_CONFIGURE_DEMUXERS) \
		--disable-parsers \
		$(FFMPEG_CONFIGURE_PARSERS) \
		--disable-protocols \
		$(FFMPEG_CONFIGURE_PROTOCOLS)"

# XXX: trick to force use of embedded FFmpeg headers
TARGET_CFLAGS += $(TARGET_CPPFLAGS)
TARGET_CPPFLAGS :=

TARGET_LDFLAGS += $(FPIC)

define Build/Prepare
$(call Build/Prepare/Default)
endef

define Package/gst-ffmpeg/install
	$(INSTALL_DIR) $(1)/usr/lib/gstreamer-$(GST_VERSION)
	( cd $(PKG_INSTALL_DIR); $(CP) \
                ./usr/lib/gstreamer-$(GST_VERSION)/libgstffmpeg.so* \
                $(1)/usr/lib/gstreamer-$(GST_VERSION)/ \
        )
endef

$(eval $(call BuildPackage,gst-ffmpeg))
