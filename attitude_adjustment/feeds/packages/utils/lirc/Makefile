#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
# we use the $LINUX_DIR variable, which is defined in kernel.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=lirc
PKG_VERSION:=0.8.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/${PKG_NAME}
PKG_MD5SUM:=fb1dcddbc8ca3d03c7f3a2d40cf28b71

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define  Package/lirc/Default
    SECTION:=utils
    CATEGORY:=Utilities
    URL:=http://lirc.org/
endef

define  Package/lirc
    $(call Package/lirc/Default)
    TITLE:=LIRC - Linux Infrared Remote Control
    MENU:=1
endef

define Package/lirc/description
  LIRC is a package that allows you to decode and send 
  infra-red signals of many (but not all) commonly used 
  remote controls.
  
  This package contains only lircd and lircd.conf!

tested drivers
--------------
ok:
userspace,accent,act200l,act220l,adaptec,alsa_usb,animax,asusdh,
atilibusb,atiusb,audio_alsa,breakoutbox,bte,bw6130,cmdir,creative,
creative_infracd,devinput,digimatrix,dsp,dvico,ea65,exaudiohauppauge,
hauppauge_dvb,hercules_smarttv_stereo,i2cuser,igorplugusb,imon,
imon_24g,imon_knob,imon_pad,imon_rsc,irdeo,irdeo_remote,it87,knc_one,
leadtek_pvr2000,livedrive_midi,livedrive_seq,logitech,macmini,mceusb,
mceusb2,mediafocusI,mouseremote,mouseremote_ps2,mp3anywhere,
packard_bell,parallel,pcmak,pcmak_usb,pctv,pixelview_bt878,provideo,
realmagic,remotemaster,sasem,sb0540,serial,silitek,sir,streamzap,tekram,
tekram_bt829,tira,ttusbir,tuxbox,tvbox,udp,uirt2,uirt2_raw,usb_uirt_raw,
usbx,serial

nok:
avermedia,avermedia_vdomate,avermedia98,bestbuy,bestbuy2,chronos,comX,
cph06x,flyvideo,gvbctv5pci,irreal,kworld,leadtek_0007,leadtek_0010,lptX,
nslu2,pixelview_pak,pixelview_pro,sa1100,

unknown: 
audio (no portaudio), caraca (no caraca package), iguanaIR (missing headers), 
irman (no libirman)
  
endef


define  Package/lircdaemonadd
    $(call Package/lirc/Default)
    DEPENDS:=lirc
    TITLE:=Daemon Additional Files
endef

define Package/lircdaemonadd/description
  Contains those additional daemon-tools:
   -irrecord
   -lircmd
endef

define  Package/lirctools
    $(call Package/lirc/Default)
    DEPENDS:=lirc
    TITLE:=LIRC tools
endef

define Package/lirctools/description
  This package contains those lirc-tools:
   -ircat:   prints config strings to standard output, can be used to provide remote control input to scripts
   -irexec:  execute programs according to the pressed remote control buttons
   -irpty:   pseudo tty driver. Connects to lircd via socket to receive infra-red codes and converts them to key strokes
   -irsend:  application for sending IR-codes via lirc
   -irw:     watch the codes as lircd recognize them
   -lircrcd: daemon that manages current mode for all applications
   -mode2:   shows the pulse/space length of a remote button
endef

CONFIGURE_ARGS += \
	--disable-nls \
	--disable-static \
	--with-kerneldir="$(LINUX_DIR)" \
	--with-driver="mceusb" \
	--without-x \
	--with-pic \
	--with-gnu-ld \
	--enable-sandboxed

define Package/lirc/conffiles
/etc/lircd.conf
endef

MAKE_FLAGS += \
	DESTDIR="$(PKG_INSTALL_DIR)" \
	SUBDIRS="daemons tools"

define  Package/lirc/install
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/daemons/lircd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/contrib/lircd.conf $(1)/etc/
endef
#todo:
#	$(CP) $(PKG_BUILD_DIR)/drivers/lirc_dev/lirc_dev.ko
#	$(CP) $(PKG_BUILD_DIR)/drivers/lirc_mceusb2/lirc_mceusb2.ko
#depends on the selected module

define  Package/lircdaemonadd/install
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/daemons/{irrecord,lircmd} $(1)/usr/sbin/
endef

define  Package/lirctools/install
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tools/irsend $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tools/irw $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tools/mode2 $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,lirc))
$(eval $(call BuildPackage,lircdaemonadd))
$(eval $(call BuildPackage,lirctools))
