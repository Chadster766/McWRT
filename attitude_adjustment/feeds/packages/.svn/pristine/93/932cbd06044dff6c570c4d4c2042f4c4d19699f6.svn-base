#
# Copyright (C) 2009-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
#
include $(TOPDIR)/rules.mk


PKG_NAME:=freeswitch
PKG_DISTNAME:=$(PKG_NAME)_git
PKG_VERSION:=1.0.7
PKG_RELEASE:=2


#
# The latest FS git hash in PKG_REV can be obtained from http://fisheye.freeswitch.org
#
PKG_REV:=54b4b088d911d4c80f224ed561d8e278229a75ff
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://git.$(PKG_NAME).org/$(PKG_NAME).git
PKG_SOURCE_SUBDIR:=$(PKG_DISTNAME)
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR)-$(shell echo $(PKG_REV)|cut -b -9).tar.bz2
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)


PKG_FIXUP:=autoreconf
PKG_BUILD_PARALLEL:=1


FS_MOD_AVAILABLE:= \
	alsa \
	amr \
	amrwb \
	avmd \
	blacklist \
	bv \
	callcenter \
	cdr-csv \
	cdr-pg-csv \
	cdr-sqlite \
	celt \
	cepstral \
	cidlookup \
	cluechoo \
	codec2 \
	commands \
	conference \
	console \
	curl \
	dahdi-codec \
	db \
	dialplan-asterisk \
	dialplan-directory \
	dialplan-xml \
	dingaling \
	directory \
	distributor \
	dptools \
	easyroute \
	enum \
	erlang-event \
	esf \
	event-multicast \
	event-socket \
	event-zmq \
	expr \
	fifo \
	file-string \
	flite \
	fsk \
	fsv \
	g723-1 \
	g729 \
	gsmopen \
	h26x \
	h323 \
	hash \
	http-cache \
	ilbc \
	java \
	json-cdr \
	lcr \
	local-stream \
	logfile \
	loopback \
	lua \
	managed \
	mp4 \
	native-file \
	nibblebill \
	openzap \
	opus \
	portaudio \
	portaudio-stream \
	radius-cdr \
	rss \
	rtmp \
	say-de \
	say-en \
	say-es \
	say-fr \
	say-hu \
	say-it \
	say-nl \
	say-pt \
	say-ru \
	say-th \
	say-zh \
	shell-stream \
	silk \
	siren \
	skinny \
	skypopen \
	sms \
	snapshot \
	snipe-hunt \
	sndfile \
	snmp \
	snom \
	sofia \
	spandsp \
	speex \
	spidermonkey \
	spidermonkey-core-db \
	spidermonkey-curl \
	spidermonkey-odbc \
	spidermonkey-socket \
	spidermonkey-teletone \
	spy \
	stress \
	syslog \
	tone-stream \
	tts-commandline \
	unimrcp \
	valet-parking \
	vmd \
	voicemail \
	xml-cdr \
	xml-curl \
	xml-rpc \
	yaml \


PKG_CONFIG_DEPENDS:= \
	CONFIG_FS_SOFIA_WITH_IPV6 \
	CONFIG_FS_SOFIA_WITH_MYSQL \
	CONFIG_FS_SOFIA_WITH_ODBC \
	CONFIG_FS_SOFIA_WITH_OPT \
	CONFIG_FS_SOFIA_WITH_SCTP \
	CONFIG_FS_SOFIA_WITH_ZRTP_GIT \
	$(patsubst %,CONFIG_PACKAGE_$(PKG_NAME)-mod-%,$(FS_MOD_AVAILABLE)) \


include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk


TAR_OPTIONS+= --strip-components=1 -C $(PKG_BUILD_DIR)


define Package/$(PKG_NAME)/Default
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Telephony
  URL:=http://www.$(PKG_NAME).org/
  MAINTAINER:=Mazilo <OpenWRT@slickdeals.endjunk.com>
endef


define Package/$(PKG_NAME)
$(call Package/$(PKG_NAME)/Default)
  TITLE:=FreeSWITCH open source telephony platform
  DEPENDS:= +libreadline +libncurses +libpthread +libstdcpp +libopenssl +libpq +librt +zlib
  MENU:=1
endef


define Package/$(PKG_NAME)/description
  FreeSWITCH is an open source telephony platform designed to
  facilitate the creation of voice and chat driven products scaling
  from a soft-phone up to a soft-switch.  See http://wiki.$(PKG_NAME).org
endef


define Package/$(PKG_NAME)/config
    config FS_SOFIA_WITH_IPV6
    depends on PACKAGE_$(PKG_NAME)
    bool "Compile with IPV6 support"
    default y
    help
            Compile $(PKG_NAME) with IPV6 Support.

    config FS_SOFIA_WITH_OPT
    depends on PACKAGE_$(PKG_NAME)
    bool "Compile with optimization"
    default y
    help
            Compile $(PKG_NAME) with optimization enabled.
endef


define Package/$(PKG_NAME)-mod-sofia/config
    config FS_SOFIA_WITH_MYSQL
    depends on PACKAGE_$(PKG_NAME)-mod-sofia
    bool "Compile with MySQL"
    default n
    select PACKAGE_libmysqlclient
    help
            Compile $(PKG_NAME) with MySQL.

    config FS_SOFIA_WITH_ODBC
    depends on PACKAGE_$(PKG_NAME)-mod-sofia
    depends on PACKAGE_sqliteodbc
    depends on PACKAGE_unixodbc_svn
    bool "Compile with ODBC support (Requires unixodbc_svn NOT YET AVAILABLE)"
    default n
    help
            Compile $(PKG_NAME)-mod-sofia with ODBC Support. Since both sqliteodbc and unixodbc_svn packages are not yet available on OpenWRT, $(PKG_NAME)-mod-sofia CAN NOT be compiled with ODBC support.

    config FS_SOFIA_WITH_SCTP
    depends on PACKAGE_$(PKG_NAME)-mod-sofia
    depends on PACKAGE_sctp
    bool "Compile with SCTP (Stream Control Transfer Protocol) support"
    default y
    help
            Compile $(PKG_NAME)-mod-sofia with SCTP support.

    config FS_SOFIA_WITH_ZRTP_GIT
    depends on PACKAGE_$(PKG_NAME)-mod-sofia
    depends on PACKAGE_libzrtpcpp
    bool "Compile with ZRTP support (Requires libzrtpcpp NOT YET AVAILABLE)"
    default n
    #select PACKAGE_libzrtpcpp
    help
            Compile $(PKG_NAME)-mod-sofia with ZRTP Support. Since libzrtpcpp package is not yet available on OpenWRT, $(PKG_NAME)-mod-sofia CAN NOT be compiled with ZRTP support.
endef


define Package/$(PKG_NAME)-example-config
$(call Package/$(PKG_NAME)/Default)
  TITLE:=FreeSWITCH example config (commented)
  DEPENDS:= $(PKG_NAME)
endef


define Package/$(PKG_NAME)-example-config/description
  The default configuration included with FreeSWITCH. It is not intended to be
  a final product, and is thus moved to /usr/share/$(PKG_NAME)/examples/conf to
  be an example of how FreeSWITCH can be configured.
endef


define Package/$(PKG_NAME)-config-upstream-defaults
$(call Package/$(PKG_NAME)/Default)
  TITLE:=FreeSWITCH upstream config
  DEPENDS:= $(PKG_NAME)
endef


define Package/$(PKG_NAME)-config-upstream-defaults/description
  The default configuration included with FreeSWITCH. It is not intended to be
  a final product, and but is included for those who don't want to use the
  OpenWRT configuration and/or want to try things as prepared by upstream.
endef


define Package/$(PKG_NAME)-config-minimal
$(call Package/$(PKG_NAME)/Default)
  TITLE:=FreeSWITCH minimal config
  DEPENDS:= $(PKG_NAME)
endef


define Package/$(PKG_NAME)-config-minimal/description
  A minimal configuration of FreeSWITCH for OpenWRT devices.  Is what the UCI
  configuration began with.
endef


define Package/$(PKG_NAME)-collection-upstream-defaults
$(call Package/$(PKG_NAME)/Default)
  TITLE:=FS upstream def sample conf
  DEPENDS:= $(PKG_NAME) \
		+$(PKG_NAME)-config-upstream-defaults \
		+$(PKG_NAME)-mod-callcenter \
		+$(PKG_NAME)-mod-codec2 \
		+$(PKG_NAME)-mod-commands \
		+$(PKG_NAME)-mod-conference \
		+$(PKG_NAME)-mod-curl \
		+$(PKG_NAME)-mod-dialplan-xml \
		+$(PKG_NAME)-mod-dingaling \
		+$(PKG_NAME)-mod-dptools \
		+$(PKG_NAME)-mod-event-socket \
		+$(PKG_NAME)-mod-g723-1 \
		+$(PKG_NAME)-mod-g729 \
		+$(PKG_NAME)-mod-hash \
		+$(PKG_NAME)-mod-http-cache \
		+$(PKG_NAME)-mod-ilbc \
		+$(PKG_NAME)-mod-local-stream \
		+$(PKG_NAME)-mod-lua \
		+$(PKG_NAME)-mod-native-file \
		+$(PKG_NAME)-mod-sndfile \
		+$(PKG_NAME)-mod-sofia \
		+$(PKG_NAME)-mod-spandsp \
		+$(PKG_NAME)-mod-speex \
		+$(PKG_NAME)-mod-syslog \
		+$(PKG_NAME)-mod-tone-stream \
		+$(PKG_NAME)-mod-xml-curl \
		+$(PKG_NAME)-mod-xml-rpc \
		+$(PKG_NAME)-tools
endef


define Package/$(PKG_NAME)-collection-minimal
$(call Package/$(PKG_NAME)/Default)
  TITLE:=FreeSWITCH minimal package collection
  DEFAULT:=y
  DEPENDS:= $(PKG_NAME) \
		+$(PKG_NAME)-config-minimal \
		+$(PKG_NAME)-mod-codec2 \
		+$(PKG_NAME)-mod-commands \
		+$(PKG_NAME)-mod-dialplan-xml \
		+$(PKG_NAME)-mod-dptools \
		+$(PKG_NAME)-mod-event-socket \
		+$(PKG_NAME)-mod-hash \
		+$(PKG_NAME)-mod-local-stream \
		+$(PKG_NAME)-mod-sndfile \
		+$(PKG_NAME)-mod-sofia \
		+$(PKG_NAME)-mod-syslog \
		+$(PKG_NAME)-mod-tone-stream \
		+$(PKG_NAME)-tools
endef


define Package/$(PKG_NAME)-sounds-en
$(call Package/$(PKG_NAME)/Default)
  TITLE:=FreeSWITCH english sounds
  DEPENDS:= $(PKG_NAME)
endef


define Package/$(PKG_NAME)-sounds-moh
$(call Package/$(PKG_NAME)/Default)
  TITLE:=FreeSWITCH music-on-hold sounds
  DEPENDS:= $(PKG_NAME)
endef


define Package/$(PKG_NAME)-tools
$(call Package/$(PKG_NAME)/Default)
  TITLE:=FreeSWITCH control tools
  DEPENDS:= $(PKG_NAME)
endef

define Package/libopenzap
$(call Package/$(PKG_NAME)/Default)
  TITLE:=FreeSWITCH openzap library
  DEPENDS:=+libpthread
endef


define Download/files
 define Download/$(1)
  FILE:=$(2)
  URL:=$(if $(4),$(4),http://files.$(PKG_NAME).org)/$(5)
  MD5SUM:=$(3)
 endef
 define Prepare/$(1)
  $(LN) $(DL_DIR)/$(2) $(PKG_BUILD_DIR)/$(6)
 endef
 $$(eval $$(call Download,$(1)))
endef


MAKE_INSTALL_TARGETS:=install


ifneq ($(CONFIG_PACKAGE_$(PKG_NAME)-mod-celt),)
 $(eval $(call Download/files,celt,celt-0.10.0.tar.gz,a656f6176e9be84027c0e51ceedd710f,,downloads/libs/,libs/))
endif


ifneq ($(CONFIG_PACKAGE_$(PKG_NAME)-mod-curl),)
 $(eval $(call Download/files,json-cdr,json-c-0.9.tar.gz,4653b3b9c568bb9c782178abfaac128d,,downloads/libs/,libs/))
endif


ifneq ($(CONFIG_PACKAGE_$(PKG_NAME)-mod-event-zmq),)
 $(eval $(call Download/files,event-zmq,zeromq-2.1.9.tar.gz,94c5e0262a79c5f82bc0b178c1f8a33d,http://download.zeromq.org http://download.zeromq.org/historic,,libs/))
endif


ifneq ($(CONFIG_PACKAGE_$(PKG_NAME)-mod-flite),)
 $(eval $(call Download/files,flite,flite-1.5.1-current.tar.bz2,36e5d1c1307161211c8ffe9a75a623ae,,downloads/libs/,libs/))
endif


ifneq ($(CONFIG_PACKAGE_$(PKG_NAME)-mod-opus),)
 $(eval $(call Download/files,opus,opus-0.9.0.tar.gz,8a729db587430392e64280a499e9d061,,downloads/libs/,libs/))
endif


ifneq ($(CONFIG_PACKAGE_$(PKG_NAME)-sounds-en),)
 $(eval $(call Download/files,sounds-en,$(PKG_NAME)-sounds-en-us-callie-8000-1.0.16.tar.gz,bde0883eb75fe06f39bde11f50a1f858,,,))
	MAKE_INSTALL_TARGETS+= sounds-install
endif


ifneq ($(CONFIG_PACKAGE_$(PKG_NAME)-sounds-moh),)
 $(eval $(call Download/files,sounds-moh,$(PKG_NAME)-sounds-music-8000-1.0.8.tar.gz,f63ef3cc3507af079ae5c7b8b8a01e42,,,))
	MAKE_INSTALL_TARGETS+= moh-install
endif


#
# XXX: -fgnu89-inline is not supported on all compiler versions, so only enable it on avr32 where it solves build issues
#
ifneq ($(CONFIG_avr32),)
  TARGET_CFLAGS += -fgnu89-inline
  TARGET_CXXFLAGS += -fgnu89-inline
endif


ifneq ($(CONFIG_USE_EGLIBC),)
  ifeq ($(CONFIG_EGLIBC_OPTION_EGLIBC_UTMP),)
    TARGET_CFLAGS += -DNO_GETLOGIN
  endif
endif


FS_TARGET_CFLAGS:= ${TARGET_CFLAGS} -DLUA_USE_LINUX $(FPIC) -std=gnu99 -Wno-format
FS_TARGET_CXXFLAGS:= ${TARGET_CXXFLAGS} -DLUA_USE_LINUX $(FPIC) -Wno-format
FS_TARGET_CPPFLAGS:= -I. -I./lua $(if $(CONFIG_PACKAGE_$(PKG_NAME)-mod-erlang-event),-I$(STAGING_DIR_HOST)/lib/erlang/lib/erl_interface-3.6/include) $(if $(CONFIG_PACKAGE_$(PKG_NAME)-mod-json-cdr),-Wno-error) ${TARGET_CPPFLAGS}


CONFIGURE_ARGS+= \
	--host=$(GNU_TARGET_NAME) \
	--build=$(GNU_HOST_NAME) \
	--prefix="/usr/share/$(PKG_NAME)" \
	--bindir="/usr/bin" \
	--libdir="/usr/lib" \
	--srcdir="$(PKG_BUILD_DIR)" \
	--sysconfdir="/etc/$(PKG_NAME)" \
	--with-modinstdir="/usr/lib/$(PKG_NAME)" \
	--with-random="/dev/urandom" \
	$(if $(CONFIG_FS_SOFIA_WITH_MYSQL),--with-mysql,--without-mysql) \
	$(call autoconf_bool,CONFIG_FS_SOFIA_WITH_IPV6,ipv6) \
	$(call autoconf_bool,CONFIG_FS_SOFIA_WITH_OPT,optimization) \
	$(if $(CONFIG_PACKAGE_$(PKG_NAME)-mod-celt),--with-ogg="$(STAGING_DIR)/usr",--without-iogg) \
	$(if ($(CONFIG_PACKAGE_$(PKG_NAME)-mod-curl)|$(CONFIG_PACKAGE_$(PKG_NAME)-mod-xml-cdr)|$(CONFIG_PACKAGE_$(PKG_NAME)-mod-xml-curl)),--with-libcurl="$(STAGING_DIR)/usr",--without-libcurl) \
	$(if $(CONFIG_PACKAGE_$(PKG_NAME)-mod-java),--with-java="$(STAGING_DIR)/usr",--without-java) \
	$(call autoconf_bool,CONFIG_PACKAGE_$(PKG_NAME)-mod-spandsp,builtin-tiff) \
	$(call autoconf_bool,CONFIG_FS_SOFIA_WITH_ZRTP_GIT,zrtp) \
	$(call autoconf_bool,CONFIG_FS_SOFIA_WITH_ODBC,core-odbc-support) \
	$(if $(CONFIG_FS_SOFIA_WITH_ODBC),--with-odbc="$(STAGING_DIR)/usr") \
	$(if $(CONFIG_FS_SOFIA_WITH_ODBC),--with-odbc-lib="$(STAGING_DIR)/usr/lib") \


CONFIGURE_VARS+= \
	config_TARGET_CC="${TARGET_CC}" \
	config_TARGET_CFLAGS="${FS_TARGET_CPPFLAGS} ${FS_TARGET_CFLAGS}" \
	config_TARGET_CXX="${TARGET_CXX}" \
	config_TARGET_CXXFLAGS="${FS_TARGET_CPPFLAGS} ${FS_TARGET_CXXFLAGS}" \
	config_TARGET_CPPFLAGS="${FS_TARGET_CPPFLAGS}" \
	config_TARGET_READLINE_INC="${FS_TARGET_CPPFLAGS}" \
	config_TARGET_READLINE_LIBS="${TARGET_LDFLAGS} -lreadline -lncurses" \
	config_TARGET_LIBS="${TARGET_LDFLAGS} -lpthread" \
	config_BUILD_CC="${HOSTCC}" \
	config_BUILD_CFLAGS="${HOST_CFLAGS}" \
	CC_FOR_BUILD="${HOSTCC}" \
	LDFLAGS="$$$$LDFLAGS" \
	LDFLAGS_FOR_BUILD="${HOST_LDFLAGS}" \
	CC="${TARGET_CC}" \
	CXX="${TARGET_CXX}" \
	CFLAGS="${FS_TARGET_CPPFLAGS} ${FS_TARGET_CFLAGS}" \
	CXXFLAGS="${FS_TARGET_CPPFLAGS} ${FS_TARGET_CXXFLAGS}" \
	CPPFLAGS="${FS_TARGET_CPPFLAGS}" \
	LDFLAGS="${TARGET_LDFLAGS} -Wl,-rpath-link=\"${STAGING_DIR}/usr/lib\"" \
	PATH="${PATH}:${STAGING_DIR}" \
	HOST_CC="/usr/bin/cc" \
	HOST_CXX="/usr/bin/g++" \
	CROSS_COMPILE="1" \
	ax_cv_c_compiler_vendor="gnu" \
	ac_cv_dev_urandom="yes" \
	ac_cv_file_dbd_apr_dbd_mysql_c="no" \
	ac_cv_file__dev_random="no" \
	ac_cv_file__dev_urandom="yes" \
	ac_cv_file___dev_urandom_="yes" \
	ac_cv_func_mmap_fixed_mapped="yes" \
	ac_cv_func_pthread_rwlock_init="yes" \
	ac_cv_struct_rlimit="yes" \
	apr_cv_mutex_recursive="yes" \
	apr_cv_process_shared_works="no" \
	apr_cv_tcp_nodelay_with_cork="yes" \
	apr_cv_type_rwlock_t="yes" \
	$(if $(CONFIG_PACKAGE_$(PKG_NAME)-mod-perl),ac_cv_path_PERL="yes",) \
	libzmq_cv_cxx_werror_flag="" \
	ac_cv_path_PKG_CONFIG=no \
	ac_cv_path_LIBGNUTLS_CONFIG=no


define Build/Prepare
	$(call Build/Prepare/Default)
	$(call Prepare/celt)
	$(call Prepare/event-zmq)
	$(call Prepare/flite)
	$(call Prepare/json-cdr)
	$(call Prepare/opus)
	$(call Prepare/sounds-en)
	$(call Prepare/sounds-moh)
endef


define Build/Configure
	(cd $(PKG_BUILD_DIR); $(AM_TOOL_PATHS) ./bootstrap.sh)
	$(call Build/Configure/Default,)
	$(foreach m,$(FS_MOD_AVAILABLE),
		$(if $(CONFIG_PACKAGE_$(PKG_NAME)-mod-$(m)),
			$(SED) 's|^[ #]*\([^#]*/mod_$(subst -,_,$(m))\)$$$$|\1|g' $(PKG_BUILD_DIR)/modules.conf
		,
			$(SED) 's|^\([^#]*/mod_$(subst -,_,$(m))\)$$$$|#\1|g' $(PKG_BUILD_DIR)/modules.conf
		)
	)
endef


define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		AR="$(TARGET_CROSS)ar" \
		RANLIB="$(TARGET_CROSS)ranlib" \
		CC="$(TARGET_CC)" \
		CXX="$(TARGET_CXX)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		LDFLAGS_FOR_BUILD="$(HOST_LDFLAGS)" \
		all $(MAKE_INSTALL_TARGETS)
	$(SED) 's|^\([ \t]*\)\(.*\)"mod_console"\(.*\)$$$$|\1<!-- \2"mod_console"\3 -->|' $(PKG_INSTALL_DIR)/etc/$(PKG_NAME)/autoload_configs/modules.conf.xml
	$(SED) 's|^\([ \t]*\)<!-- *\(.*\)"mod_syslog"\(.*\) *-->\(.*\)$$$$|\1\2"mod_syslog"\3\4|' $(PKG_INSTALL_DIR)/etc/$(PKG_NAME)/autoload_configs/modules.conf.xml
endef


define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/$(PKG_NAME)
	$(CP) $(PKG_INSTALL_DIR)/usr/share/$(PKG_NAME)/include/* $(1)/usr/include/$(PKG_NAME)/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib$(PKG_NAME).{a,so*} $(1)/usr/lib/
endef


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/$(PKG_NAME) $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib$(PKG_NAME).so.* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/$(PKG_NAME)
	$(INSTALL_DIR) $(1)/etc/default
	$(INSTALL_DATA) ./files/$(PKG_NAME).default $(1)/etc/default/$(PKG_NAME)
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/$(PKG_NAME).init $(1)/etc/init.d/$(PKG_NAME)
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) ./files/$(PKG_NAME).hotplug $(1)/etc/hotplug.d/iface/90-$(PKG_NAME)
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/$(PKG_NAME).config $(1)/etc/config/$(PKG_NAME)
endef


define Package/$(PKG_NAME)-collection-upstream-defaults/install
	$(INSTALL_DIR) $(1)/etc/$(PKG_NAME)
endef


define Package/$(PKG_NAME)-example-config/install
	$(INSTALL_DIR) $(1)/usr/share/$(PKG_NAME)/examples/conf
	$(CP) $(PKG_INSTALL_DIR)/etc/$(PKG_NAME)/* $(1)/usr/share/$(PKG_NAME)/examples/conf/
endef


define Package/$(PKG_NAME)-config-upstream-defaults/install
	$(INSTALL_DIR) $(1)/etc/$(PKG_NAME)
	$(CP) $(PKG_INSTALL_DIR)/etc/$(PKG_NAME)/* $(1)/etc/$(PKG_NAME)/
endef


define Package/$(PKG_NAME)-collection-minimal/install
	$(INSTALL_DIR) $(1)/etc/$(PKG_NAME)
endef


define Package/$(PKG_NAME)-collection-uci-minimal/install
	$(INSTALL_DIR) $(1)/etc/$(PKG_NAME)
endef


define Package/$(PKG_NAME)-config-minimal/install
	$(INSTALL_DIR) $(1)/etc/$(PKG_NAME)
	$(CP) ./files/etc.minimal/* $(1)/etc/$(PKG_NAME)/
endef


define Package/$(PKG_NAME)-sounds-en/install
	$(INSTALL_DIR) $(1)/usr/share/$(PKG_NAME)/sounds
	$(CP) $(PKG_INSTALL_DIR)/usr/share/$(PKG_NAME)/sounds/en $(1)/usr/share/$(PKG_NAME)/sounds/
endef


define Package/$(PKG_NAME)-sounds-moh/install
	$(INSTALL_DIR) $(1)/usr/share/$(PKG_NAME)/sounds
	$(CP) $(PKG_INSTALL_DIR)/usr/share/$(PKG_NAME)/sounds/music $(1)/usr/share/$(PKG_NAME)/sounds/
endef

define Package/libopenzap/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libopenzap.so* $(1)/usr/lib/
endef

define Package/$(PKG_NAME)-tools/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/fs_cli $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/fs_encode $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/fs_ivrd $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/fsxs $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/gentls_cert $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/tone2wav $(1)/usr/bin/
endef


#
# 1. Name
# 2. Title
# 3. Files
# 4. Inter Depends
# 5. Extra Depends
#
define BuildPlugin
  define Package/$(PKG_NAME)-mod-$(1)
    $$(call Package/$(PKG_NAME)/Default)
    TITLE:= FS $(2) module
    DEPENDS:= $(PKG_NAME) $(foreach m,$(4),+$(PKG_NAME)-mod-$(m)) $(5)
  endef

  define Package/$(PKG_NAME)-mod-$(1)/install
	[ -z "$(3)" ] || $(INSTALL_DIR) $$(1)/usr/lib/$(PKG_NAME)
	for f in $(3); do \
		$(CP) \
			$(PKG_INSTALL_DIR)/usr/lib/$(PKG_NAME)/$$$$$$$${f}.so \
			$$(1)/usr/lib/$(PKG_NAME)/ ; \
	done
	if [ -d "./files/etc.packages/$(1)" ]; then \
		$(INSTALL_DIR) $$(1)/etc/$(PKG_NAME) ; \
		$(CP) \
			./files/etc.packages/$(1)/* \
			$$(1)/etc/$(PKG_NAME)/ ; \
	fi
  endef

  define Package/$(PKG_NAME)-mod-$(1)/postinst
#!/bin/sh
sed -i -e 's|^\([ \t]*\)<!-- *\(.*\)"mod_$(subst -,_,$(1))"\(.*\) *-->\(.*\)$$$$|\1\2"mod_$(subst -,_,$(1))"\3\4|' $$$${IPKG_INSTROOT}/etc/$(PKG_NAME)/autoload_configs/modules.conf.xml
  endef

  define Package/$(PKG_NAME)-mod-$(1)/postrm
#!/bin/sh
sed -i -e 's|^\([ \t]*\)\(.*\)"mod_$(subst -,_,$(1))"\(.*\)$$$$|\1<!-- \2"mod_$(subst -,_,$(1))"\3 -->|' $$$${IPKG_INSTROOT}/etc/$(PKG_NAME)/autoload_configs/modules.conf.xml
  endef

  $$(eval $$(call BuildPackage,$(PKG_NAME)-mod-$(1)))
endef


$(eval $(call BuildPackage,$(PKG_NAME)))
$(eval $(call BuildPackage,$(PKG_NAME)-collection-upstream-defaults))
$(eval $(call BuildPackage,$(PKG_NAME)-collection-minimal))
$(eval $(call BuildPackage,$(PKG_NAME)-sounds-en))
$(eval $(call BuildPackage,$(PKG_NAME)-sounds-moh))
$(eval $(call BuildPackage,$(PKG_NAME)-tools))
$(eval $(call BuildPackage,$(PKG_NAME)-example-config))
$(eval $(call BuildPackage,$(PKG_NAME)-config-upstream-defaults))
$(eval $(call BuildPackage,$(PKG_NAME)-config-minimal))
$(eval $(call BuildPackage,libopenzap))


#
# $(eval $(call BuildPlugin,Name,Title,Files,Inter Depends,Extra Depends))
#
$(eval $(call BuildPlugin,alsa,Alsa endpoint,mod_alsa,,+alsa-lib))
$(eval $(call BuildPlugin,amr,GSM-AMR codec,mod_amr,,))
$(eval $(call BuildPlugin,amrwb,GSM-AMRWB,mod_amrwb,,))
$(eval $(call BuildPlugin,avmd,Advanced Voice Mail Detection,mod_avmd,,))
$(eval $(call BuildPlugin,bv,codec mod_bv,mod_bv,,))
$(eval $(call BuildPlugin,blacklist,application blacklist,mod_blacklist,,))
$(eval $(call BuildPlugin,callcenter,Call Center,mod_callcenter,,))
$(eval $(call BuildPlugin,cdr-csv,CSV-CDR handler,mod_cdr_csv,,))
$(eval $(call BuildPlugin,cdr-pg-csv,Postgress CDR handler,mod_cdr_pg_csv,,+coreutils +libpq))
$(eval $(call BuildPlugin,cdr-sqlite,SQLite3 CDR handler,mod_cdr_sqlite,,+libsqlite3))
$(eval $(call BuildPlugin,celt,CELT ultra-low delay codec,mod_celt,,+libogg))
$(eval $(call BuildPlugin,cepstral,Cepstral interface,mod_cepstral,,@BROKEN)) # needs <swift.h>
$(eval $(call BuildPlugin,cidlookup,applications mod_cidlookup,mod_cidlookup,,+libcurl))
$(eval $(call BuildPlugin,cluechoo,Framework Demo,mod_cluechoo,,))
$(eval $(call BuildPlugin,codec2,CoDec 2,mod_codec2,,))
$(eval $(call BuildPlugin,commands,API commands,mod_commands,,))
$(eval $(call BuildPlugin,conference,Conference room,mod_conference,,))
$(eval $(call BuildPlugin,console,Console logger,mod_console,,))
$(eval $(call BuildPlugin,curl,HTTP request,mod_curl,,+libcurl))
$(eval $(call BuildPlugin,dahdi-codec,DAHDI codecs,mod_dahdi_codec,,))
$(eval $(call BuildPlugin,db,Database backend,mod_db,,))
$(eval $(call BuildPlugin,dialplan-asterisk,Asterisk dialplan parser,mod_dialplan_asterisk,,))
$(eval $(call BuildPlugin,dialplan-directory,Dialplan directory,mod_dialplan_directory,,))
$(eval $(call BuildPlugin,dialplan-xml,Dialplan-XML interface,mod_dialplan_xml,,))
$(eval $(call BuildPlugin,dingaling,Jabber interface,mod_dingaling,,))
$(eval $(call BuildPlugin,directory,application mod_directory,mod_directory,,))
$(eval $(call BuildPlugin,distributor,application mod_distributor,mod_distributor,,))
$(eval $(call BuildPlugin,dptools,Dialplan tools,mod_dptools,,))
$(eval $(call BuildPlugin,easyroute,DID routing,mod_easyroute,,))
$(eval $(call BuildPlugin,enum,ENUM routing,mod_enum,,))
$(eval $(call BuildPlugin,erlang-event,Erlang event handler,mod_erlang_event,,+erlang))
$(eval $(call BuildPlugin,esf,Extra SIP Functionality,mod_esf,,))
$(eval $(call BuildPlugin,event-multicast,Multicast event handler,mod_event_multicast,,))
$(eval $(call BuildPlugin,event-socket,Socket event handler,mod_event_socket,,))
$(eval $(call BuildPlugin,event-zmq,Socket event handler by Zero MQ,mod_event_zmq,,))
$(eval $(call BuildPlugin,expr,Expression evaluation,mod_expr,,))
$(eval $(call BuildPlugin,fifo,FIFO,mod_fifo,,))
$(eval $(call BuildPlugin,file-string,File streaming,mod_file_string,,@OBSOLETE)) # merged into dptools
$(eval $(call BuildPlugin,flite,Festival TTS,mod_flite,,@(!(armeb||avr32)||BROKEN)))
$(eval $(call BuildPlugin,fsk,Bell-202 1200-baud FSK decoder,mod_fsk,,))
$(eval $(call BuildPlugin,fsv,Video,mod_fsv,,))
$(eval $(call BuildPlugin,g723-1,G.723.1 codec,mod_g723_1,,))
$(eval $(call BuildPlugin,g729,G.729 codec,mod_g729,,))
$(eval $(call BuildPlugin,gsmopen,Short Message Service (SMS),mod_gsmopen,spandsp,+alsa-lib @BROKEN)) # needs gsmlib
$(eval $(call BuildPlugin,h26x,H26X linear codec,mod_h26x,,))
$(eval $(call BuildPlugin,h323,h323 endpoint,mod_h323,,@BROKEN)) # missing header files (other packages needed)
$(eval $(call BuildPlugin,hash,API for manipulating a hash table,mod_hash,,))
$(eval $(call BuildPlugin,http-cache,HTTP GET with caching,mod_http_cache,,+libcurl))
$(eval $(call BuildPlugin,ilbc,ILBC codec,mod_ilbc,,))
$(eval $(call BuildPlugin,java,Java language interface,mod_java,,@BROKEN)) # needs java
$(eval $(call BuildPlugin,json-cdr,JSon-CDR interface,mod_json_cdr,,+libcurl))
$(eval $(call BuildPlugin,lcr,Least Cost Routing,mod_lcr,,))
$(eval $(call BuildPlugin,local-stream,Local streaming,mod_local_stream,,))
$(eval $(call BuildPlugin,logfile,File logger,mod_logfile,,))
$(eval $(call BuildPlugin,loopback,Loopback endpoint,mod_loopback,,))
$(eval $(call BuildPlugin,lua,Lua language interface,mod_lua,,+libstdcpp))
$(eval $(call BuildPlugin,managed,language mod_managed,mod_managed,,+glib2 @BROKEN)) # needs Mono
$(eval $(call BuildPlugin,mp4,MP4 File Format support for video apps.,mod_mp4,,@BROKEN)) # needs libmp4v2
$(eval $(call BuildPlugin,native-file,Native files,mod_native_file,,))
$(eval $(call BuildPlugin,nibblebill,Billing,mod_nibblebill,,))
$(eval $(call BuildPlugin,openzap,Zaptel hardware interface,mod_openzap ozmod_analog ozmod_analog_em ozmod_isdn ozmod_skel ozmod_zt,,+libpcap +libopenzap))
$(eval $(call BuildPlugin,opus,Opus CoDec,mod_opus,,))
$(eval $(call BuildPlugin,portaudio,Local audio endpoint,mod_portaudio,,+alsa-lib))
$(eval $(call BuildPlugin,portaudio-stream,portaudio stream format,mod_portaudio_stream,,+alsa-lib))
$(eval $(call BuildPlugin,radius-cdr,Radius-CDR interface,mod_radius_cdr,,+freeradius2 @BROKEN)) # fails in freeradius-client
$(eval $(call BuildPlugin,rss,RRS feeds via TTS,mod_rss,,))
$(eval $(call BuildPlugin,rtmp,RTMP Protocol Handler,mod_rtmp,,))
$(eval $(call BuildPlugin,say-de,German say,mod_say_de,,))
$(eval $(call BuildPlugin,say-en,English say,mod_say_en,,))
$(eval $(call BuildPlugin,say-es,Spanish say,mod_say_es,,))
$(eval $(call BuildPlugin,say-fr,French say,mod_say_fr,,))
$(eval $(call BuildPlugin,say-hu,Hungarian say,mod_say_hu,,))
$(eval $(call BuildPlugin,say-it,Italian say,mod_say_it,,))
$(eval $(call BuildPlugin,say-nl,Dutch say,mod_say_nl,,))
$(eval $(call BuildPlugin,say-pt,Portugeese say,mod_say_pt,,))
$(eval $(call BuildPlugin,say-ru,Russian say,mod_say_ru,,))
$(eval $(call BuildPlugin,say-th,Thai say,mod_say_th,,))
$(eval $(call BuildPlugin,say-zh,Chineese say,mod_say_zh,,))
$(eval $(call BuildPlugin,shell-stream,Shell stream,mod_shell_stream,,))
$(eval $(call BuildPlugin,silk,codec mod_silk,mod_silk,,))
$(eval $(call BuildPlugin,siren,G.722.1 codec,mod_siren,,))
$(eval $(call BuildPlugin,skinny,endpoint mod_skinny,mod_skinny,,))
$(eval $(call BuildPlugin,skypopen,Skype compatible endpoint,mod_skypopen,,@FEATURE_drawing-backend_libX11))
$(eval $(call BuildPlugin,sms,SMS application,mod_sms,,))
$(eval $(call BuildPlugin,snapshot,application mod_snapshot,mod_snapshot,,))
$(eval $(call BuildPlugin,sndfile,Multi-Format file transcoder,mod_sndfile,,))
$(eval $(call BuildPlugin,snipe-hunt,application mod_snipe_hunt,mod_snipe_hunt,,))
$(eval $(call BuildPlugin,snmp,Simple Network Management Protocol,mod_snmp,,+libnetsnmp))
$(eval $(call BuildPlugin,snom,SNOM specific features,mod_snom,,))
$(eval $(call BuildPlugin,sofia,SIP,mod_sofia,,))
$(eval $(call BuildPlugin,spandsp,Span DSP,mod_spandsp,,+libjpeg))
$(eval $(call BuildPlugin,speex,Speex codec,mod_speex,,))
$(eval $(call BuildPlugin,spidermonkey,JavaScript,mod_spidermonkey,,@BROKEN)) # fails in js
$(eval $(call BuildPlugin,spidermonkey-core_db,JavaScript DB,mod_spidermonkey_core_db,,@BROKEN))
$(eval $(call BuildPlugin,spidermonkey-curl,JavaScript Curl,mod_spidermonkey_curl,,@BROKEN))
$(eval $(call BuildPlugin,spidermonkey-odbc,JavaScript ODBC,mod_spidermonkey_odbc,,@BROKEN))
$(eval $(call BuildPlugin,spidermonkey-socket,JavaScript socket,mod_spidermonkey_socket,,@BROKEN))
$(eval $(call BuildPlugin,spidermonkey-teletone,JavaScript teletone,mod_spidermonkey_teletone,,@BROKEN))
$(eval $(call BuildPlugin,spy,User spy,mod_spy,,))
$(eval $(call BuildPlugin,stress,application mod_stress,mod_stress,,))
$(eval $(call BuildPlugin,syslog,SysLog logger,mod_syslog,,))
$(eval $(call BuildPlugin,tone-stream,Tone generation stream,mod_tone_stream,,))
$(eval $(call BuildPlugin,tts-commandline,ASR TTS cli,mod_tts_commandline,,))
$(eval $(call BuildPlugin,unimrcp,MRCP interface,mod_unimrcp,,@BROKEN))
$(eval $(call BuildPlugin,valet-parking,application mod_valet_parking,mod_valet_parking,,))
$(eval $(call BuildPlugin,vmd,VoiceMail beep detection,mod_vmd,,))
$(eval $(call BuildPlugin,voicemail,VoiceMail,mod_voicemail,,))
$(eval $(call BuildPlugin,xml-cdr,XML-CDR handler,mod_xml_cdr,,+libcurl))
$(eval $(call BuildPlugin,xml-curl,XML-Curl gateway,mod_xml_curl,,+libcurl))
$(eval $(call BuildPlugin,xml-rpc,XML-RPC interface,mod_xml_rpc,,))
$(eval $(call BuildPlugin,yaml,YAML dialplans,mod_yaml,,))

