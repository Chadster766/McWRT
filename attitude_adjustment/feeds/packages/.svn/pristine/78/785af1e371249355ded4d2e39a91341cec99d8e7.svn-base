#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=asterisk18
PKG_VERSION:=1.8.10.1
PKG_RELEASE:=2

PKG_SOURCE:=asterisk-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://downloads.asterisk.org/pub/telephony/asterisk/releases/
PKG_MD5SUM:=415738d347b9037cbe5f8bfbe66843de

PKG_BUILD_DIR=$(BUILD_DIR)/asterisk-$(PKG_VERSION)

include $(INCLUDE_DIR)/ltqtapi.mk
include $(INCLUDE_DIR)/package.mk

define Package/asterisk18/Default
  SUBMENU:=Telephony
  SECTION:=net
  CATEGORY:=Network
  URL:=http://www.asterisk.org/
endef

define Package/asterisk18/Default/description
 Asterisk is a complete PBX in software. It provides all of the features
 you would expect from a PBX and more. Asterisk does voice over IP in three
 protocols, and can interoperate with almost all standards-based telephony
 equipment using relatively inexpensive hardware.
endef


define Package/asterisk18
$(call Package/asterisk18/Default)
  TITLE:=Complete open source PBX, v1.8x
  MENU:=1
  DEPENDS:= +libopenssl +libncurses +libpopt +libpthread +zlib @!TARGET_avr32
endef

define Package/asterisk18/description
$(call Package/asterisk18/Default/description)
endef

define Package/asterisk18-sounds
$(call Package/asterisk18/Default)
  TITLE:=Sound files
  DEPENDS:= asterisk18
endef

define Package/asterisk18-sounds/description
$(call Package/asterisk18/Default/description)
 This package contains sound files for Asterisk.
endef

define Package/asterisk18-voicemail
$(call Package/asterisk18/Default)
  TITLE:=Voicemail support
  DEPENDS:= asterisk18
endef

define Package/asterisk18-voicemail/description
$(call Package/asterisk18/Default/description)
 This package contains voicemail related modules for Asterisk.
endef

define Package/asterisk18-app-meetme
$(call Package/asterisk18/Default)
  TITLE:=conferencing support
  DEPENDS:= asterisk18 +dahdi-tools-libtonezone +kmod-dahdi-linux
endef

define Package/asterisk18-app-meetme/description
$(call Package/asterisk18/Default/description)
 This package provides the MeetMe application driver Conferencing support to
 Asterisk.
endef

define Package/asterisk18-chan-iax2
$(call Package/asterisk18/Default)
  TITLE:=IAX support
  DEPENDS:= asterisk18 +asterisk18-res-crypto
endef

define Package/asterisk18-chan-iax2/description
$(call Package/asterisk18/Default/description)
 This package provides IAX support to
 Asterisk.
endef

define Package/asterisk18-cdr
$(call Package/asterisk18/Default)
  TITLE:=CDR support
  DEPENDS:= asterisk18 @BROKEN
endef

define Package/asterisk18-cdr/description
$(call Package/asterisk18/Default/description)
 This package provides Call Detail Record support to
 Asterisk.
endef

define Package/asterisk18-res-musiconhold
$(call Package/asterisk18/Default)
  TITLE:=MOH support
  DEPENDS:= asterisk18
endef

define Package/asterisk18-res-musiconhold/description
$(call Package/asterisk18/Default/description)
 This package provides Music On Hold support to
 Asterisk.
endef

define Package/asterisk18-res-srtp
$(call Package/asterisk18/Default)
  TITLE:=SRTP support
  DEPENDS:= asterisk18 libsrtp +asterisk18-res-crypto
endef

define Package/asterisk18-res-srtp/description
$(call Package/asterisk18/Default/description)
 This package provides SRTP support to
 Asterisk.
endef

define Package/asterisk18-chan-gtalk
$(call Package/asterisk18/Default)
  TITLE:=GTalk support
  DEPENDS:= asterisk18 +libiksemel
endef

define Package/asterisk18-chan-gtalk/description
$(call Package/asterisk18/Default/description)
 This package provides the channel chan_gtalk and res_jabber for GTalk
 support to Asterisk.
endef

define Package/asterisk18-chan-mobile
$(call Package/asterisk18/Default)
  TITLE:=Mobile channel support
  DEPENDS:= asterisk18 +bluez-libs
endef

define Package/asterisk18-chan-mobile/description
$(call Package/asterisk18/Default/description)
 This package provides the channel chan_mobile support to Asterisk.
endef

define Package/asterisk18-res-timing-timerfd
$(call Package/asterisk18/Default)
  TITLE:= Timerfd Timing Interface
  DEPENDS:= asterisk18
endef

define Package/asterisk18-res-timing-pthread
$(call Package/asterisk18/Default)
  TITLE:= pthread Timing Interface
  DEPENDS:= asterisk18
endef

define Package/asterisk18-res-fax
$(call Package/asterisk18/Default)
  TITLE:=Generic FAX Resource for FAX technology resource modules
  DEPENDS:= asterisk18 +asterisk18-res-timing-pthread
endef

define Package/asterisk18-res-fax-spandsp
$(call Package/asterisk18/Default)
  TITLE:=Spandsp T.38 and G.711 FAX Resource
  DEPENDS:= asterisk18 +asterisk18-res-fax +libspandsp
endef

define Package/asterisk18-chan-mgcp
$(call Package/asterisk18/Default)
  TITLE:=MGCP channel support
  DEPENDS:= asterisk18
endef

define Package/asterisk18-chan-mgcp/description
$(call Package/asterisk18/Default/description)
 This package provides the channel chan_mgcp support to Asterisk.
endef

define Package/asterisk18-chan-skinny
$(call Package/asterisk18/Default)
  TITLE:=Skinny channel support
  DEPENDS:= asterisk18
endef

define Package/asterisk18-chan-skinny/description
$(call Package/asterisk18/Default/description)
 This package provides the channel chan_skinny support to Asterisk.
endef

define Package/asterisk18-curl
$(call Package/asterisk18/Default)
  TITLE:=CURL support
  DEPENDS:= asterisk18 +libcurl
endef

define Package/asterisk18-curl/description
$(call Package/asterisk18/Default/description)
 This package provides CURL
 support to Asterisk.
endef

define Package/asterisk18-mysql
$(call Package/asterisk18/Default)
  TITLE:=MySQL support
  DEPENDS:= asterisk18 +PACKAGE_asterisk18-mysql:libmysqlclient
endef

define Package/asterisk18-mysql/description
$(call Package/asterisk18/Default/description)
 This package provides MySQL
 support to Asterisk.
endef

define Package/asterisk18-chan-lantiq
$(call Package/asterisk18/Default)
  TITLE:=Lantiq TAPI support
  DEPENDS:= asterisk18 $(LTQ_TAPI_DEPENDS)
  URL:=http://git.nanl.de/?p=asterisk_channel_lantiq.git
  MAINTAINER:=Mirko Vogt <mirko@openwrt.org>
endef

define Package/asterisk18-chan-lantiq/description
$(call Package/asterisk18/Default/description)
 This package provides the channel chan_lantiq support to Asterisk.
endef

ifneq ($(SDK)$(CONFIG_PACKAGE_asterisk18-app-meetme),)
  CONFIGURE_ARGS+= \
	--with-dahdi="$(STAGING_DIR)/usr"
else
  CONFIGURE_ARGS+= \
	--without-dahdi
endif

ifneq ($(SDK)$(CONFIG_PACKAGE_asterisk18-chan-gtalk),)
  CONFIGURE_ARGS+= \
	--with-gnutls="$(STAGING_DIR)/usr" \
	--with-iksemel="$(STAGING_DIR)/usr"
  SITE_VARS+= \
	ac_cv_lib_iksemel_iks_start_sasl=yes \
	ac_cv_lib_gnutls_gnutls_bye=yes
else
  CONFIGURE_ARGS+= \
	--without-gnutls \
	--without-iksemel
endif

ifneq ($(SDK)$(CONFIG_PACKAGE_asterisk18-curl),)
  CONFIGURE_ARGS+= \
	--with-curl="$(STAGING_DIR)/usr"
else
  CONFIGURE_ARGS+= \
	--without-curl
endif

ifneq ($(SDK)$(CONFIG_PACKAGE_asterisk18-mysql),)
  CONFIGURE_ARGS+= \
	--with-mysqlclient="$(STAGING_DIR)/usr/bin"
else
  CONFIGURE_ARGS+= \
	--without-mysqlclient
endif

ifneq ($(SDK)$(CONFIG_PACKAGE_asterisk18-res-fax-spandsp),)
  CONFIGURE_ARGS+= \
    --with-spandsp="$(STAGING_DIR)/usr"
else
  CONFIGURE_ARGS+= \
    --without-spandsp
endif

ifneq ($(SDK)$(CONFIG_PACKAGE_asterisk18-chan-mobile),)
  CONFIGURE_ARGS+= \
	--with-bluetooth="$(STAGING_DIR)/usr"
else
  CONFIGURE_ARGS+= \
	--without-bluetooth
endif

ifneq ($(SDK)$(CONFIG_PACKAGE_asterisk18-res-srtp),)
  CONFIGURE_ARGS+= \
	--with-srtp="$(STAGING_DIR)/usr"
else
  CONFIGURE_ARGS+= \
	--without-srtp
endif

CONFIGURE_ARGS+= \
	--without-curses \
	--with-gsm=internal \
	--without-cap \
	--without-gtk \
	--without-gtk2 \
	--without-isdnnet \
	--without-kde \
	--without-misdn \
	--without-nbs \
	--with-ncurses="$(STAGING_DIR)/usr" \
	--without-netsnmp \
	--without-newt \
	--without-odbc \
	--without-ogg \
	--without-osptk \
	--with-popt="$(STAGING_DIR)/usr" \
	--without-pri \
	--without-qt \
	--without-radius \
	--without-sdl \
	--without-suppserv \
	--without-tds \
	--without-termcap \
	--without-tinfo \
	--without-vorbis \
	--without-vpb \
	--with-z="$(STAGING_DIR)/usr" \
	--with-sounds-cache="$(DL_DIR)" \
	--disable-xmldoc

EXTRA_CFLAGS+= $(TARGET_CPPFLAGS)
EXTRA_LDFLAGS+= $(TARGET_LDFLAGS)

define Build/Prepare
	$(call Build/Prepare/Default)
ifneq ($(CONFIG_TARGET_lantiq),)
	$(CP) ./src-lantiq/* $(PKG_BUILD_DIR)/
endif
endef

define Build/Configure
	$(call Build/Configure/Default,,$(SITE_VARS))
endef

define Build/Compile
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		include/asterisk/version.h \
		include/asterisk/buildopts.h defaults.h \
		makeopts.embed_rules
	ASTCFLAGS="$(EXTRA_CFLAGS) -DLOW_MEMORY" \
	ASTLDFLAGS="$(EXTRA_LDFLAGS)" \
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		ASTVARLIBDIR="/usr/lib/asterisk" \
		ASTDATADIR="/usr/lib/asterisk" \
		ASTKEYDIR="/usr/lib/asterisk" \
		ASTDBDIR="/usr/lib/asterisk" \
		NOISY_BUILD="1" \
		DEBUG="" \
		OPTIMIZE="" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install samples
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/asterisk-1.8/include/asterisk/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/asterisk/*.h $(1)/usr/include/asterisk-1.8/include/asterisk/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/asterisk.h $(1)/usr/include/asterisk-1.8/include/
endef

define Package/asterisk18/conffiles
/etc/asterisk/asterisk.conf
/etc/asterisk/modules.conf
/etc/asterisk/extensions.conf
/etc/asterisk/sip.conf
/etc/asterisk/sip_notify.conf
/etc/asterisk/features.conf
/etc/asterisk/indications.conf
/etc/asterisk/logger.conf
/etc/asterisk/manager.conf
/etc/asterisk/rtp.conf
/etc/default/asterisk
/etc/init.d/asterisk
endef

define Package/asterisk18/install
	$(INSTALL_DIR) $(1)/etc/asterisk
	for f in asterisk extensions features \
		indications logger manager modules \
		sip sip_notify rtp; do \
		$(CP) $(PKG_INSTALL_DIR)/etc/asterisk/$$$$f.conf $(1)/etc/asterisk/ ; \
	done
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/modules
	for f in app_dial app_echo app_playback app_macro \
		chan_sip res_rtp_asterisk res_rtp_multicast \
		codec_ulaw codec_gsm \
		format_gsm format_pcm format_wav format_wav_gsm \
		pbx_config \
		func_strings func_timeout func_callerid func_logic; do \
		$(CP) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/$$$$f.so $(1)/usr/lib/asterisk/modules/ ; \
	done
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/asterisk $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/default
	$(INSTALL_DATA)  ./files/asterisk.default $(1)/etc/default/asterisk
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN)  ./files/asterisk.init $(1)/etc/init.d/asterisk
endef

define Package/asterisk18-sounds/install
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/sounds/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/asterisk/sounds/en/* $(1)/usr/lib/asterisk/sounds/
	rm -f $(1)/usr/lib/asterisk/sounds/vm-*
	rm -f $(1)/usr/lib/asterisk/sounds/conf-*
endef


define Package/asterisk18-voicemail/conffiles
/etc/asterisk/voicemail.conf
endef

define Package/asterisk18-voicemail/install
	$(INSTALL_DIR) $(1)/etc/asterisk
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/asterisk/voicemail.conf $(1)/etc/asterisk/
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/modules
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/*voicemail.so $(1)/usr/lib/asterisk/modules/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/res_adsi.so $(1)/usr/lib/asterisk/modules/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/res_smdi.so $(1)/usr/lib/asterisk/modules/
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/sounds/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/asterisk/sounds/en/vm-*.gsm $(1)/usr/lib/asterisk/sounds/
endef

define Package/asterisk18-app-meetme/conffiles
/etc/asterisk/meetme.conf
endef

define Package/asterisk18-app-meetme/install
	$(INSTALL_DIR) $(1)/etc/asterisk
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/asterisk/meetme.conf $(1)/etc/asterisk/
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/modules
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/app_meetme.so $(1)/usr/lib/asterisk/modules/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/chan_dahdi.so $(1)/usr/lib/asterisk/modules/
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/sounds/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/asterisk/sounds/en/conf-*.gsm $(1)/usr/lib/asterisk/sounds/
endef

define Package/asterisk18-chan-iax2/conffiles
/etc/asterisk/iax.conf
/etc/asterisk/iaxprov.conf
endef

define Package/asterisk18-cdr/conffiles
/etc/asterisk/cdr.conf
/etc/asterisk/cdr_custom.conf
/etc/asterisk/cdr_manager.conf
/etc/asterisk/cdr_odbc.conf
/etc/asterisk/cdr_pgsql.conf
/etc/asterisk/cdr_tds.conf
endef

define Package/asterisk18-res-musiconhold/conffiles
/etc/asterisk/musiconhold.conf
endef

define Package/asterisk18-chan-iax2/install
	$(INSTALL_DIR) $(1)/etc/asterisk
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/asterisk/iax.conf $(1)/etc/asterisk/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/asterisk/iaxprov.conf $(1)/etc/asterisk/
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/modules
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/chan_iax2.so $(1)/usr/lib/asterisk/modules/
endef

define Package/asterisk18-cdr/install
	$(INSTALL_DIR) $(1)/etc/asterisk
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/asterisk/cdr*.conf $(1)/etc/asterisk/
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/modules
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/*cdr*.so $(1)/usr/lib/asterisk/modules/
endef

define Package/asterisk18-res-musiconhold/install
	$(INSTALL_DIR) $(1)/etc/asterisk
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/asterisk/musiconhold.conf $(1)/etc/asterisk/
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/modules
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/res_musiconhold.so $(1)/usr/lib/asterisk/modules/
endef

define Package/asterisk18-chan-gtalk/conffiles
/etc/asterisk/gtalk.conf
/etc/asterisk/jabber.conf
endef

define Package/asterisk18-chan-gtalk/install
	$(INSTALL_DIR) $(1)/etc/asterisk
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/asterisk/gtalk.conf $(1)/etc/asterisk/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/asterisk/jabber.conf $(1)/etc/asterisk/
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/modules
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/chan_gtalk.so $(1)/usr/lib/asterisk/modules/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/res_jabber.so $(1)/usr/lib/asterisk/modules/
endef

define Package/asterisk18-chan-mobile/conffiles
/etc/asterisk/chan_mobile.conf
endef

define Package/asterisk18-chan-mobile/install
	$(INSTALL_DIR) $(1)/etc/asterisk
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/asterisk/chan_mobile.conf $(1)/etc/asterisk/
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/modules
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/chan_mobile.so $(1)/usr/lib/asterisk/modules/
endef

define Package/asterisk18-res-timing-timerfd/install
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/modules
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/res_timing_timerfd.so $(1)/usr/lib/asterisk/modules/
endef

define Package/asterisk18-res-timing-pthread/install
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/modules
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/res_timing_pthread.so $(1)/usr/lib/asterisk/modules/
endef

define Package/asterisk18-res-fax/conffiles
/etc/asterisk/res_fax.conf
endef

define Package/asterisk18-res-fax/install
	$(INSTALL_DIR) $(1)/etc/asterisk
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/asterisk/res_fax.conf $(1)/etc/asterisk/
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/modules
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/res_fax.so $(1)/usr/lib/asterisk/modules/
endef

define Package/asterisk18-res-fax-spandsp/install
	$(INSTALL_DIR) $(1)/etc/asterisk
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/modules
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/res_fax_spandsp.so $(1)/usr/lib/asterisk/modules/
endef

define Package/asterisk18-chan-mgcp/conffiles
/etc/asterisk/mgcp.conf
endef

define Package/asterisk18-chan-mgcp/install
	$(INSTALL_DIR) $(1)/etc/asterisk
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/asterisk/mgcp.conf $(1)/etc/asterisk/
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/modules
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/chan_mgcp.so $(1)/usr/lib/asterisk/modules/
endef

define Package/asterisk18-chan-skinny/conffiles
/etc/asterisk/skinny.conf
endef

define Package/asterisk18-chan-skinny/install
	$(INSTALL_DIR) $(1)/etc/asterisk
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/asterisk/skinny.conf $(1)/etc/asterisk/
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/modules
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/chan_skinny.so $(1)/usr/lib/asterisk/modules/
endef

define Package/asterisk18-curl/install
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/modules
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/func_curl.so $(1)/usr/lib/asterisk/modules/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/res_curl.so $(1)/usr/lib/asterisk/modules/
endef

define Package/asterisk18-mysql/conffiles
/etc/asterisk/app_mysql.conf
/etc/asterisk/res_config_mysql.conf
/etc/asterisk/cdr_mysql.conf
endef

define Package/asterisk18-mysql/install
	$(INSTALL_DIR) $(1)/etc/asterisk
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/asterisk/app_mysql.conf $(1)/etc/asterisk/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/asterisk/res_config_mysql.conf $(1)/etc/asterisk/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/asterisk/cdr_mysql.conf $(1)/etc/asterisk/
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/modules
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/app_mysql.so $(1)/usr/lib/asterisk/modules/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/cdr_mysql.so $(1)/usr/lib/asterisk/modules/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/res_config_mysql.so $(1)/usr/lib/asterisk/modules/
endef

define Package/asterisk18-chan-lantiq/conffiles
/etc/asterisk/lantiq.conf
endef

define Package/asterisk18-chan-lantiq/install
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/modules
	$(INSTALL_DIR) $(1)/etc/asterisk
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/asterisk/lantiq.conf $(1)/etc/asterisk/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/chan_lantiq.so $(1)/usr/lib/asterisk/modules/
endef

define Package/asterisk18-res-srtp/install
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/modules
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/res_srtp.so $(1)/usr/lib/asterisk/modules/
endef

define Buildasterisk18ModuleTemplate

  define Package/asterisk18-$(subst _,-,$(1))
  $$(call Package/asterisk18/Default)
    TITLE:=$(2) support
    DEPENDS:= asterisk18 $(4)
  endef

  define Package/asterisk18-$(subst _,-,$(1))/description
  $$(call Package/asterisk18/Default/description)
    This package provides support $(3) in Asterisk.
  endef

  define Package/asterisk18-$(subst _,-,$(1))/install
	$(INSTALL_DIR) $$(1)/usr/lib/asterisk/modules
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/$(1).so $$(1)/usr/lib/asterisk/modules/
  endef

  $$(eval $$(call BuildPackage,asterisk18-$(subst _,-,$(1))))
endef

$(eval $(call BuildPackage,asterisk18))
$(eval $(call BuildPackage,asterisk18-voicemail))
$(eval $(call BuildPackage,asterisk18-sounds))
#$(eval $(call BuildPackage,asterisk18-app-meetme))
$(eval $(call BuildPackage,asterisk18-chan-iax2))
$(eval $(call BuildPackage,asterisk18-cdr))
$(eval $(call BuildPackage,asterisk18-res-musiconhold))
$(eval $(call BuildPackage,asterisk18-chan-gtalk))
$(eval $(call BuildPackage,asterisk18-chan-mobile))
$(eval $(call BuildPackage,asterisk18-res-fax))
$(eval $(call BuildPackage,asterisk18-res-fax-spandsp))
$(eval $(call BuildPackage,asterisk18-res-timing-timerfd))
$(eval $(call BuildPackage,asterisk18-res-timing-pthread))
$(eval $(call BuildPackage,asterisk18-chan-mgcp))
$(eval $(call BuildPackage,asterisk18-chan-skinny))
$(eval $(call BuildPackage,asterisk18-curl))
$(eval $(call BuildPackage,asterisk18-mysql))
$(eval $(call BuildPackage,asterisk18-chan-lantiq))
$(eval $(call BuildPackage,asterisk18-res-srtp))
$(eval $(call Buildasterisk18ModuleTemplate,app_authenticate,Authenticate,support for executing arbitrary authenticate commands))
$(eval $(call Buildasterisk18ModuleTemplate,app_chanisavail,Channel availability check,support for checking if a channel is available))
$(eval $(call Buildasterisk18ModuleTemplate,app_chanspy,Channel listen in,support for listening in on any channel))
$(eval $(call Buildasterisk18ModuleTemplate,app_directed_pickup,Directed call pickup,support for directed call pickup))
$(eval $(call Buildasterisk18ModuleTemplate,app_exec,Exec application,support for application execution))
$(eval $(call Buildasterisk18ModuleTemplate,app_minivm,Minimal voicemail system,a voicemail system in small building blocks working together based on the Comedian Mail voicemail system))
$(eval $(call Buildasterisk18ModuleTemplate,app_readexten,Extension to variable,a trivial application to read an extension into a variable))
$(eval $(call Buildasterisk18ModuleTemplate,app_read,Variable read,a trivial application to read a variable))
$(eval $(call Buildasterisk18ModuleTemplate,app_sayunixtime,Say Unix time,an application to say Unix time))
$(eval $(call Buildasterisk18ModuleTemplate,app_sms,SMS,SMS support (ETSI ES 201 912 protocol 1)))
$(eval $(call Buildasterisk18ModuleTemplate,app_stack,Stack applications, stack applications Gosub Return etc., +asterisk18-res-agi))
$(eval $(call Buildasterisk18ModuleTemplate,app_system,System exec,support for executing system commands))
$(eval $(call Buildasterisk18ModuleTemplate,app_talkdetect,File playback with audio detect,for file playback with audio detect))
$(eval $(call Buildasterisk18ModuleTemplate,app_waituntil,Sleep,support sleeping until the given epoch))
$(eval $(call Buildasterisk18ModuleTemplate,app_while,While loop,a while loop implementation))
$(eval $(call Buildasterisk18ModuleTemplate,chan_agent,Agents proxy channel, an implementation of agents proxy channel))
$(eval $(call Buildasterisk18ModuleTemplate,chan_local,Local proxy channel, an implementation of local proxy channel))
$(eval $(call Buildasterisk18ModuleTemplate,codec_alaw,Signed linear to alaw translation,translation between signed linear and alaw codecs))
$(eval $(call Buildasterisk18ModuleTemplate,codec_a_mu,Alaw to ulaw translation,translation between alaw and ulaw codecs))
$(eval $(call Buildasterisk18ModuleTemplate,codec_g722,G.722,a high bit rate 48/56/64Kbps ITU standard codec))
$(eval $(call Buildasterisk18ModuleTemplate,codec_g726,Signed linear to G.726 translation,translation between signed linear and ITU G.726-32kbps codecs))
$(eval $(call Buildasterisk18ModuleTemplate,format_g726,G.726,support for headerless G.726 16/24/32/40kbps data format))
$(eval $(call Buildasterisk18ModuleTemplate,format_g729,G.729,support for raw headerless G729 data))
$(eval $(call Buildasterisk18ModuleTemplate,format_sln,Raw slinear format,support for raw slinear format))
$(eval $(call Buildasterisk18ModuleTemplate,format_sln16,Raw slinear 16 format,support for Raw slinear 16 format))
$(eval $(call Buildasterisk18ModuleTemplate,func_db,Database interaction,functions for interaction with the database))
$(eval $(call Buildasterisk18ModuleTemplate,func_devstate,Blinky lights control,functions for manually controlled blinky lights))
$(eval $(call Buildasterisk18ModuleTemplate,func_vmcount,vmcount dialplan,a vmcount dialplan function))
$(eval $(call Buildasterisk18ModuleTemplate,func_extstate,Hinted extension state,retrieving the state of a hinted extension for dialplan control))
$(eval $(call Buildasterisk18ModuleTemplate,func_global,Global variable,global variable dialplan functions))
$(eval $(call Buildasterisk18ModuleTemplate,func_shell,Shell,support for shell execution))
$(eval $(call Buildasterisk18ModuleTemplate,pbx_ael,Asterisk Extension Logic,support for symbolic Asterisk Extension Logic))
$(eval $(call Buildasterisk18ModuleTemplate,res_ael_share,Shareable AEL code,support for shareable AEL code mainly between internal and external modules))
$(eval $(call Buildasterisk18ModuleTemplate,pbx_spool,Call Spool,outgoing call spool support))
$(eval $(call Buildasterisk18ModuleTemplate,res_agi,Asterisk Gateway Interface,support for the Asterisk Gateway Interface extension))
$(eval $(call Buildasterisk18ModuleTemplate,res_crypto,Provide Crypto,Cryptographic Signature capability))
$(eval $(call Buildasterisk18ModuleTemplate,app_alarmreceiver,Alarm receiver,Central Station Alarm receiver for Ademco Contact ID))
$(eval $(call Buildasterisk18ModuleTemplate,app_setcallerid,Set callerid,support for setting callerid))
$(eval $(call Buildasterisk18ModuleTemplate,app_verbose,Verbose logging,Verbose logging application))
$(eval $(call Buildasterisk18ModuleTemplate,func_channel,Channel info,Channel info dialplan function))
$(eval $(call Buildasterisk18ModuleTemplate,func_blacklist,Blacklist on callerid,looking up the callerid number and see if it is blacklisted))
$(eval $(call Buildasterisk18ModuleTemplate,app_originate,Originate a call,originating an outbound call and connecting it to a specified extension or application))
$(eval $(call Buildasterisk18ModuleTemplate,func_uri,URI encoding and decoding,Encodes and decodes URI-safe strings))
$(eval $(call Buildasterisk18ModuleTemplate,app_disa,Direct Inward System Access,Direct Inward System Access))
$(eval $(call Buildasterisk18ModuleTemplate,app_senddtmf,Send DTMF digits,Sends arbitrary DTMF digits))
$(eval $(call Buildasterisk18ModuleTemplate,func_cut,CUT function,CUT function))
$(eval $(call Buildasterisk18ModuleTemplate,res_clioriginate,Calls via CLI,Originate calls via the CLI))
$(eval $(call Buildasterisk18ModuleTemplate,app_mixmonitor,Record a call and mix the audio,record a call and mix the audio during the recording))
$(eval $(call Buildasterisk18ModuleTemplate,app_playtones,Playtones application,play a tone list))
$(eval $(call Buildasterisk18ModuleTemplate,app_record,Record sound file,to record a sound file))
