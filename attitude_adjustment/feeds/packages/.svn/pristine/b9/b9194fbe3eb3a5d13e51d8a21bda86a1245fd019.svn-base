#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=kamailio3
PKG_VERSION:=3.3.0
PKG_RELEASE:=1

PKG_SOURCE_URL:=http://www.kamailio.org/pub/kamailio/$(PKG_VERSION)/src/
PKG_SOURCE:=kamailio-$(PKG_VERSION)$(PKG_VARIANT)_src.tar.gz
PKG_MD5SUM:=c5261066b5c8b617b6a9408679ad514f

INCL_MODULES:=
KAM_MODULES:=

include $(INCLUDE_DIR)/nls.mk
include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += $(FPIC)
PREBUILT_STAMP=$(STAGING_DIR)/stamp/.$(PKG_NAME)_prebuilt
TAR_CMD:=$(HOST_TAR) -C $(PKG_BUILD_DIR) --strip-components 1 $(TAR_OPTIONS)

define Package/kamailio3/Default
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Telephony
  URL:=http://www.kamailio.org/
  DEPENDS:=$(ICONV_DEPENDS) +libncurses +libpthread +libreadline
endef

define Package/kamailio3
$(call Package/kamailio3/Default)
  TITLE:=Mature and flexible open source SIP server version 3.x
  MENU:=1
endef

define Package/kamailio3-example
$(call Package/kamailio3/Default)
  TITLE:=Kamailio3 example config
  DEPENDS:=kamailio3 \
	+kamailio3-mod-acc \
	+kamailio3-mod-maxfwd \
	+kamailio3-mod-mi-fifo \
	+kamailio3-mod-pv \
	+kamailio3-mod-registrar \
	+kamailio3-mod-rr \
	+kamailio3-mod-sl \
	+kamailio3-mod-siputils \
	+kamailio3-mod-textops \
	+kamailio3-mod-tm \
	+kamailio3-mod-uri-db \
	+kamailio3-mod-usrloc \
	+kamailio3-mod-xlog
endef

define Package/kamailio3-example/conffiles
/etc/default/kamailio
/etc/kamailio/kamailio.cfg
endef

define Package/kamailio3-tools
$(call Package/kamailio3/Default)
  TITLE:=Kamailio3 control tools
  DEPENDS:=kamailio3 +bash
endef

define Package/kamailio3-tools/conffiles
/etc/kamailio/kamctlrc
endef

define Package/kamailio3/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/sbin/kamailio \
		$(1)/usr/sbin/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/sbin/sercmd \
		$(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib/kamailio/modules
	$(INSTALL_DIR) $(1)/usr/lib/kamailio/modules_k
	$(INSTALL_DIR) $(1)/usr/lib/kamailio/modules_s
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/kamailio/lib*.so* \
		$(1)/usr/lib/kamailio/
endef

define Package/kamailio3-example/install
	$(INSTALL_DIR) $(1)/etc/kamailio
	$(CP) \
		$(PKG_INSTALL_DIR)/etc/kamailio/kamailio.cfg \
		$(1)/etc/kamailio/
	$(SED) 's,$(PKG_INSTALL_DIR),,g' \
		$(1)/etc/kamailio/kamailio.cfg
	$(SED) 's,//lib/kamailio,/usr/lib/kamailio,g' \
		$(1)/etc/kamailio/kamailio.cfg
	$(INSTALL_DIR) $(1)/etc/default
	$(INSTALL_DATA) ./files/kamailio.default $(1)/etc/default/kamailio
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/kamailio.init $(1)/etc/init.d/kamailio
endef

define Package/kamailio3-tools/install
	$(INSTALL_DIR) $(1)/etc/kamailio
	$(CP) \
		$(PKG_BUILD_DIR)/utils/kamctl/kamctlrc \
		$(1)/etc/kamailio/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/sbin/kam{ctl,dbctl} \
		$(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib/kamailio
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/kamailio/kamctl \
		$(1)/usr/lib/kamailio/
endef

define BuildPlugin
  define Package/kamailio3-mod-$(1)
    $$(call Package/kamailio3/Default)
    TITLE:= kamailio3 $(3) module
    DEPENDS:=kamailio3 $$(foreach m,$(4),+kamailio3-mod-$$m) $(5)
  endef

  define Package/kamailio3-mod-$(1)/install
	[ -z "$(2)" ] || $(INSTALL_DIR) $$(1)/usr/lib/kamailio/$(6)
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/kamailio/$(6)/$(2).so \
			$$(1)/usr/lib/kamailio/$(6)/ ;
  endef

  INCL_MODULES+=$(2)
  KAM_MODULES+=kamailio3-mod-$(1)
endef

$(eval $(call BuildPlugin,acc,acc,Accounting,tm,,modules_k))
$(eval $(call BuildPlugin,acc-radius,acc_radius,Accounting for RADIUS backend,acc,+libradiusclient-ng,modules_k))
$(eval $(call BuildPlugin,alias-db,alias_db,Database-backend aliases,db-sqlite,,modules_k))
$(eval $(call BuildPlugin,auth,auth,Authentication Framework,,,modules))
$(eval $(call BuildPlugin,auth-db,auth_db,Database-backend authentication,auth db-sqlite,,modules_k))
#$(eval $(call BuildPlugin,auth-diameter,auth_diameter,Diameter-backend authentication,sl,,modules_k))
$(eval $(call BuildPlugin,auth-radius,auth_radius,RADIUS-backend authentication,auth,+libradiusclient-ng,modules_k))
$(eval $(call BuildPlugin,avpops,avpops,AVP operation,,,modules))
$(eval $(call BuildPlugin,benchmark,benchmark,Config benchmark,,,modules_k))
#$(eval $(call BuildPlugin,carrierroute,carrierroute,Carrier Routing,,+libconfuse,modules))
$(eval $(call BuildPlugin,cfgutils,cfgutils,Config utilities,,,modules_k))
$(eval $(call BuildPlugin,cfg-db,cfg_db,Load core and module parameters from database,db-sqlite,,modules))
$(eval $(call BuildPlugin,cfg-rpc,cfg_rpc,Update core and module parameters at runtime via RPC interface,,,modules))
#$(eval $(call BuildPlugin,cpl-c,cpl-c,Call Processing language interpreter,tm sl usrloc,+libxml2,modules_k))
$(eval $(call BuildPlugin,ctl,ctl,BINRPC transport interface,,,modules))
#$(eval $(call BuildPlugin,db-berkeley,db_berkeley,Berkeley DB Database-backend,,,modules))
$(eval $(call BuildPlugin,db-flatstore,db_flatstore,Fast writing-only text database-backed,,,modules))
$(eval $(call BuildPlugin,db-mysql,db_mysql,MySQL database-backend,,+libmysqlclient,modules))
#$(eval $(call BuildPlugin,db-oracle,db_oracle,Oracle Database-backend,,,modules_k))
#$(eval $(call BuildPlugin,db-postgres,db_postgres,PostgreSQL Database-backend,,+libpq,modules))
$(eval $(call BuildPlugin,db-sqlite,db_sqlite,Sqlite DB support,,+libsqlite3,modules_k))
$(eval $(call BuildPlugin,db-text,db_text,Text database-backend,,,modules_k))
$(eval $(call BuildPlugin,db-unixodbc,db_unixodbc,UnixODBC Database-backend,,+unixodbc,modules_k))
$(eval $(call BuildPlugin,dialog,dialog,Dialog support,rr tm,,modules_k))
$(eval $(call BuildPlugin,dialplan,dialplan,Dialplan management,,+libpcre,modules))
$(eval $(call BuildPlugin,dispatcher,dispatcher,Dispatcher,,,modules_s))
$(eval $(call BuildPlugin,diversion,diversion,Diversion header insertion,,,modules_s))
$(eval $(call BuildPlugin,domain,domain,Multi-domain support,,,modules_k))
$(eval $(call BuildPlugin,domainpolicy,domainpolicy,Domain policy,,,modules_k))
$(eval $(call BuildPlugin,enum,enum,ENUM lookup,,,modules))
$(eval $(call BuildPlugin,exec,exec,External exec,,,modules_k))
$(eval $(call BuildPlugin,group,group,Database-backend user-groups,,,modules_k))
$(eval $(call BuildPlugin,h350,h350,H.350,ldap,+libopenldap,modules_k))
$(eval $(call BuildPlugin,htable,htable,Hash Table,,,modules_k))
$(eval $(call BuildPlugin,imc,imc,IM conferencing,db-mysql tm,,modules_k))
$(eval $(call BuildPlugin,ipops,ipops,IP and IPv6 operations module,,,modules))
$(eval $(call BuildPlugin,kex,kex,Core extensions,,,modules_k))
$(eval $(call BuildPlugin,lcr,lcr,Least Cost Routing,tm,+libpcre,modules))
$(eval $(call BuildPlugin,ldap,ldap,LDAP connector,,+libopenldap,modules_k))
$(eval $(call BuildPlugin,maxfwd,maxfwd,Max-Forward processor,,,modules_k))
$(eval $(call BuildPlugin,mediaproxy,mediaproxy,Automatic NAT traversal,dialog,,modules))
$(eval $(call BuildPlugin,mi-datagram,mi_datagram,Datagram support for Management Interface,,,modules_k))
$(eval $(call BuildPlugin,mi-fifo,mi_fifo,FIFO support for Management Interface,,,modules_k))
#$(eval $(call BuildPlugin,mi-xmlrpc,mi_xmlrpc,XML-RPC support for Management Interface,,+xmlrpc-c,modules_k))
$(eval $(call BuildPlugin,mi-rpc,mi_rpc,RPC support for Management Interface,,,modules))
$(eval $(call BuildPlugin,misc_radius,misc_radius,Generic RADIUS,,+libradiusclient-ng,modules_k))
$(eval $(call BuildPlugin,msilo,msilo,SIP message silo,tm,,modules_k))
$(eval $(call BuildPlugin,nat_traversal,nat_traversal,NAT traversal,dialog sl tm,,modules_k))
$(eval $(call BuildPlugin,nathelper,nathelper,NAT traversal helper,,+rtpproxy,modules_k))
#$(eval $(call BuildPlugin,osp,osp,OSP peering,auth rr sl tm textops,+libosp,modules_k))
$(eval $(call BuildPlugin,path,path,SIP path insertion,rr,,modules_k))
$(eval $(call BuildPlugin,pdt,pdt,Prefix-to-Domain translator,,,modules_k))
$(eval $(call BuildPlugin,peering,peering,Perring,,+libradiusclient-ng,modules))
#$(eval $(call BuildPlugin,perl,perl,Perl,sl,+perl,modules_k))
#$(eval $(call BuildPlugin,perlvdb,perlvdb,Perl Virtual DB,perl,,modules_k))
$(eval $(call BuildPlugin,permissions,permissions,Permissions control,,,modules_k))
$(eval $(call BuildPlugin,pike,pike,Flood detector,,,modules_k))
$(eval $(call BuildPlugin,presence,presence,Presence server,sl tm,+libxml2,modules_k))
$(eval $(call BuildPlugin,presence-dialoginfo,presence_dialoginfo,Dialog Event presence,presence,,modules_k))
$(eval $(call BuildPlugin,presence-mwi,presence_mwi,Message Waiting Indication presence,presence,,modules_k))
$(eval $(call BuildPlugin,presence-xml,presence_xml,XCAP presence,presence xcap-client,,modules_k))
$(eval $(call BuildPlugin,pua,pua,Presence User Agent,tm,+libxml2,modules_k))
$(eval $(call BuildPlugin,pua-bla,pua_bla,Bridged Line Appearence PUA,presence pua usrloc,,modules_k))
$(eval $(call BuildPlugin,pua-dialoginfo,pua_dialoginfo,Dialog Event PUA,dialog pua,,modules_k))
$(eval $(call BuildPlugin,pua-mi,pua_mi,PUA Management Interface,pua,,modules_k))
$(eval $(call BuildPlugin,pua-usrloc,pua_usrloc,PUA User Location,pua usrloc,,modules_k))
$(eval $(call BuildPlugin,pua-xmpp,pua_xmpp,PUA XMPP,presence pua xmpp,,modules_k))
#$(eval $(call BuildPlugin,purple,purple,Multi-protocol IM gateway,presence pua,+libpurple,modules_k))
$(eval $(call BuildPlugin,pv,pv,Pseudo-Variables,,,modules_k))
$(eval $(call BuildPlugin,qos,qos,QoS control,dialog,,modules_k))
$(eval $(call BuildPlugin,ratelimit,ratelimit,Traffic shapping,,,modules))
$(eval $(call BuildPlugin,regex,regex,Regular Expression,,+libpcre,modules_k))
$(eval $(call BuildPlugin,registrar,registrar,SIP Registrar,usrloc,,modules_k))
$(eval $(call BuildPlugin,rls,rls,Resource List Server,presence pua tm,+libxml2,modules_k))
$(eval $(call BuildPlugin,rr,rr,Record-Route and Route,,,modules_k))
$(eval $(call BuildPlugin,rtimer,rtimer,Routing Timer,,,modules_k))
$(eval $(call BuildPlugin,sanity,sanity,SIP sanity checks,sl,,modules))
#$(eval $(call BuildPlugin,seas,seas,SIP Express Application Server,tm,,modules_k))
$(eval $(call BuildPlugin,sipcapture,sipcapture,SIP capture module,,,modules))
$(eval $(call BuildPlugin,siptrace,siptrace,SIP trace,,,modules_k))
$(eval $(call BuildPlugin,siputils,siputils,SIP utilities,sl,,modules_k))
$(eval $(call BuildPlugin,sl,sl,Stateless replier,,,modules))
$(eval $(call BuildPlugin,sms,sms,SIP-to-SMS IM gateway,tm,,modules))
#$(eval $(call BuildPlugin,snmpstats,snmpstats,SNMP,presence usrloc,+libnetsnmp,modules_k))
$(eval $(call BuildPlugin,speeddial,speeddial,Per-user speed-dial controller,,,modules_k))
$(eval $(call BuildPlugin,sqlops,sqlops,SQL operations,,,modules_k))
$(eval $(call BuildPlugin,statistics,statistics,Script statistics,,,modules_k))
$(eval $(call BuildPlugin,sst,sst,SIP Session Timer,dialog sl,,modules_k))
$(eval $(call BuildPlugin,textops,textops,Text operations,,,modules_k))
$(eval $(call BuildPlugin,tls,tls,TLS operations,,+libopenssl,modules))
$(eval $(call BuildPlugin,tm,tm,Transaction,,,modules))
$(eval $(call BuildPlugin,tmx,tmx,Transaction module extensions,,,modules_k))
$(eval $(call BuildPlugin,uac,uac,User Agent Client,tm,,modules_k))
$(eval $(call BuildPlugin,uac-redirect,uac_redirect,User Agent Client redirection,tm,,modules_k))
$(eval $(call BuildPlugin,uri-db,uri_db,Database-backend SIP URI checking,,,modules_k))
$(eval $(call BuildPlugin,userblacklist,userblacklist,User blacklists,,,modules_k))
$(eval $(call BuildPlugin,usrloc,usrloc,User location,,,modules_k))
$(eval $(call BuildPlugin,utils,utils,Misc utilities,,+libcurl +libxml2,modules))
$(eval $(call BuildPlugin,xcap-client,xcap_client,XCAP Client,,+libcurl,modules_k))
$(eval $(call BuildPlugin,xlog,xlog,Advanced logger,,,modules_k))
$(eval $(call BuildPlugin,xmpp,xmpp,SIP-to-XMPP Gateway,tm,+libexpat,modules_k))

PKG_MAKE_ARGS:= \
	cfg-dir="/etc/kamailio/" \
	bin-dir="/usr/sbin/" \
	data-dir="/var/lib/kamailio/" \
	lib-dir="/usr/lib/kamailio/" \
	modules-dir="/usr/lib/kamailio/modules/" \
	include_modules="$(INCL_MODULES)" \
	$(KAM3_EXTRA_ARGS) \
	TLS_HOOKS=1 extra_defs="-DUSE_PTHREAD_MUTEX " \
	CFLAGS="$(TARGET_CFLAGS)" \
	ISSMP="no" \
	LOCALBASE="$(STAGING_DIR)/usr" \
	SYSBASE="$(STAGING_DIR)/usr" \
	PCREDEFS:="$(TARGET_CPPFLAGS)" \
	PCRELIBS:="$(TARGET_LDFLAGS)" \
	cfg_target:=/etc/kamailio/ \
	PREFIX:=/usr \
	prefix:=/usr quiet=verbose

define Build/Compile
	if ! [ -f $(PREBUILT_STAMP) ]; then echo $(MAKE) -C $(PKG_BUILD_DIR) $(PKG_MAKE_ARGS) \
		CC="$(TARGET_CC)" \
		ARCH="$(ARCH)"  \
		EXTRA_LIBS=-L$(STAGING_DIR)/usr/lib/ \
		all && touch $(PREBUILT_STAMP); \
	fi
	$(MAKE) -C $(PKG_BUILD_DIR) $(PKG_MAKE_ARGS) \
		CC="$(TARGET_CC)" \
		ARCH="$(ARCH)"  \
		DESTDIR=$(PKG_INSTALL_DIR) \
		prefix=/ \
		install-cfg
	$(MAKE) -C $(PKG_BUILD_DIR) $(PKG_MAKE_ARGS) \
		CC="$(TARGET_CC)" \
		ARCH="$(ARCH)"  \
		DESTDIR=$(PKG_INSTALL_DIR) \
		prefix=/usr \
		install-bin install-modules-all install-utils
endef

$(eval $(call BuildPackage,kamailio3))
$(eval $(call BuildPackage,kamailio3-example))
$(eval $(call BuildPackage,kamailio3-tools))
$(foreach m,$(KAM_MODULES),$(eval $(call BuildPackage,$(m))))
