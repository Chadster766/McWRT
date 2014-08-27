## Copyright (C) 2009-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gst-plugins-good
PKG_VERSION:=0.10.23
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://gstreamer.freedesktop.org/src/gst-plugins-good/
PKG_MD5SUM:=9cddbb0e7e90677f0cc05c23feffef5c

PKG_BUILD_DEPENDS:= gstreamer gst-plugins-base liboil

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/gstreamer/Default
  CATEGORY:=Multimedia
  SECTION:=multimedia
  TITLE:=GStreamer
  URL:=http://gstreamer.freedesktop.org/
endef

define Package/gstreamer/description/Default
 GStreamer open source multimedia framework
endef


define Package/gst-plugins-good
$(call Package/gstreamer/Default)
  TITLE+= plugins collection (good)
  DEPENDS+= $(GST_DEPENDS)
endef

define Package/gts-plugins-good/description
$(call Package/gstreamer/description/Default)
 .
 This meta package contains only dependencies to the other plugins from
 the good plugins collection.
endef


GST_VERSION:=0.10

CONFIGURE_ARGS += \
	--disable-debug \
	--disable-examples \
	\
	--disable-aalib \
	--disable-alpha \
	--disable-annodex \
	--disable-apetag \
	--disable-auparse \
	--disable-avi \
	--disable-bz2 \
	--disable-cairo \
	--disable-directsound \
	--disable-dv1394 \
	--disable-effectv \
	--disable-esd \
	--disable-flv \
	--disable-flx \
	--disable-gconf \
	--disable-gconftool \
	--disable-gdk_pixbuf \
	--disable-goom \
	--disable-goom2k1 \
	--disable-gst_v4l2 \
	--disable-hal \
	--disable-jpeg \
	--disable-law \
	--disable-libcaca \
	--disable-libdv \
	--disable-libpng \
	--disable-matroska \
	--disable-monoscope \
	--disable-osx_audio \
	--disable-osx_video \
	--disable-pulse \
	--disable-qtdemux \
	--disable-rtspmanager \
	--disable-smpte \
	--disable-shout2 \
	--disable-speex \
	--disable-sunaudio \
	--disable-taglib \
	--disable-videobox \
	--disable-videocrop \
	--disable-videofilter \
	--disable-videomixer \
	--disable-wavpack \
	--disable-x \
	--disable-xshm \
	--disable-xvideo \
	--disable-y4m \
	--disable-zlib \
	\
	--without-libiconv-prefix \
	--without-libintl-prefix \
	--without-x \

EXTRA_LDFLAGS+= \
	-Wl,-rpath-link=$(STAGING_DIR)/usr/lib \


define Package/gst-plugins-good/install
	/bin/true
endef


# 1: short name
# 2: description
# 3: dependencies on other gstreamer libraries (short name)
# 4: dependencies on other gstreamer plugins (short name)
# 5: dependencies on other packages
define GstBuildPlugin

  GST_DEPENDS += +gst-mod-$(1)

  define Package/gst-mod-$(1)
    $(call Package/gstreamer/Default)
    TITLE+= $(2) plugin (good)
    DEPENDS+= +libgstreamer $$(foreach p,$(3),+libgst$$(p)) $$(foreach p,$(4),+gst-mod-$$(p)) $(5)
  endef

  define Package/gst-mod-$(1)/description
   $(call Package/gstreamer/description/Default)
   .
   This package contains the GStreamer $(2) plugin.
  endef

  define Package/gst-mod-$(1)/install
	$(INSTALL_DIR) $$(1)/usr/lib/gstreamer-$(GST_VERSION)
	( cd $(PKG_INSTALL_DIR); $(CP) \
		./usr/lib/gstreamer-$(GST_VERSION)/libgst$(1).so* \
		$$(1)/usr/lib/gstreamer-$(GST_VERSION)/ \
	)
  endef

  $$(eval $$(call BuildPackage,gst-mod-$(1)))
endef

$(eval $(call GstBuildPlugin,audiofx,audio effects,audio fft controller,,))
$(eval $(call GstBuildPlugin,autodetect,format auto-detection,,,))
$(eval $(call GstBuildPlugin,cutter,audio cutter,audio,,))
$(eval $(call GstBuildPlugin,debug,debugging,,,))
$(eval $(call GstBuildPlugin,equalizer,audio equalizer,audio controller,,))
$(eval $(call GstBuildPlugin,flac,FLAC codec,audio pbutils tag,,+libflac))
$(eval $(call GstBuildPlugin,id3demux,ID3v1/v2 demuxer,pbutils tag,,))
$(eval $(call GstBuildPlugin,icydemux,icy demuxer,audio tag,,))
$(eval $(call GstBuildPlugin,interleave,audio interleave,audio,,))
$(eval $(call GstBuildPlugin,level,audio level,audio,,))
$(eval $(call GstBuildPlugin,multifile,multiple files access,,,))
$(eval $(call GstBuildPlugin,multipart,multipart stream handling,,,))
$(eval $(call GstBuildPlugin,ossaudio,OSS audio support,audio,,))
$(eval $(call GstBuildPlugin,replaygain,volume normalization,pbutils,,))
$(eval $(call GstBuildPlugin,rtp,RTP,audio rtp tag,,))
$(eval $(call GstBuildPlugin,rtsp,RTSP,interfaces rtp rtsp sdp,,))
$(eval $(call GstBuildPlugin,souphttpsrc,soup input,audio tag,,+libsoup))
$(eval $(call GstBuildPlugin,spectrum,spectrum data output,audio fft,,))
$(eval $(call GstBuildPlugin,udp,UDP,netbuffer,,))
$(eval $(call GstBuildPlugin,wavenc,Wav encoder,riff,,))
$(eval $(call GstBuildPlugin,wavparse,Wav parser,audio riff tag,,))

$(eval $(call BuildPackage,gst-plugins-good))
