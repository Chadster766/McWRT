#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=opensips
PKG_VERSION:=1.5.3
PKG_RELEASE:=1
PKG_VARIANT:=-tls

PKG_SOURCE_URL:=http://opensips.org/pub/opensips/$(PKG_VERSION)/src
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)$(PKG_VARIANT)_src.tar.gz
PKG_MD5SUM:=8a03167420c31da15405bed7630ed3e2

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)$(PKG_VARIANT)

PKG_BUILD_DEPENDS:=curl expat libxml2 mysql openldap openssl pcre postgresql radiusclient-ng

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += $(FPIC)

define Package/opensips/Default
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Telephony
  URL:=http://www.opensips.org/
endef


define Package/opensips
$(call Package/opensips/Default)
  TITLE:=Open SIP Server
  MENU:=1
endef

define Package/opensips-example
$(call Package/opensips/Default)
  TITLE:=OpenSIPS example config
  DEPENDS:=opensips \
	+opensips-mod-acc \
	+opensips-mod-maxfwd \
	+opensips-mod-mi-fifo \
	+opensips-mod-registrar \
	+opensips-mod-rr \
	+opensips-mod-signaling \
	+opensips-mod-sl \
	+opensips-mod-textops \
	+opensips-mod-tm \
	+opensips-mod-uri \
	+opensips-mod-uri-db \
	+opensips-mod-usrloc \
	+opensips-mod-xlog
endef

define Package/opensips-example/conffiles
/etc/default/opensips
/etc/opensips/opensips.cfg
endef


define Package/opensips-tools
$(call Package/opensips/Default)
  TITLE:=OpenSIPS control tools
  DEPENDS:=opensips
endef

define Package/opensips-tools/conffiles
/etc/opensips/opensipsctlrc
endef


PKG_EXCL_MODULES:= \
	auth_diameter \
	carrierroute \
	db_berkeley \
	db_oracle \
	db_unixodbc \
	mediaproxy \
	mi_xmlrpc \
	mmgeoip \
	osp \
	perl \
	perlvdb \
	snmpstats \

PKG_MAKE_ARGS:= \
	cfg-dir="etc/opensips/" \
	bin-dir="usr/sbin/" \
	data-dir="var/lib/opensips/" \
	lib-dir="usr/lib/opensips/" \
	modules-dir="usr/lib/opensips/modules/" \
	exclude_modules="$(PKG_EXCL_MODULES)" \


define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) $(PKG_MAKE_ARGS) \
		prefix=/ \
		extra_defs="-DUSE_PTHREAD_MUTEX "  \
		CC="$(TARGET_CC)" \
		ARCH="$(ARCH)"  \
		CFLAGS="$(TARGET_CFLAGS)" \
		ISSMP="no" \
		LOCALBASE="$(STAGING_DIR)/usr" \
		SYSBASE="$(STAGING_DIR)/usr" \
		PCRECFG= \
		all
	$(MAKE) -C $(PKG_BUILD_DIR) $(PKG_MAKE_ARGS) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
endef

define Package/opensips/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/sbin/opensips \
		$(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib/opensips/modules
endef

define Package/opensips-example/install
	$(INSTALL_DIR) $(1)/etc/opensips
	$(CP) \
		$(PKG_INSTALL_DIR)/etc/opensips/opensips.cfg \
		$(1)/etc/opensips/
	$(SED) 's,$(PKG_INSTALL_DIR),,g' \
		$(1)/etc/opensips/opensips.cfg
	$(INSTALL_DIR) $(1)/etc/default
	$(INSTALL_DATA) ./files/opensips.default $(1)/etc/default/opensips
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/opensips.init $(1)/etc/init.d/opensips
endef

define Package/opensips-tools/install
	$(INSTALL_DIR) $(1)/etc/opensips
	$(CP) \
		$(PKG_INSTALL_DIR)/etc/opensips/opensipsctlrc \
		$(1)/etc/opensips/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/sbin/opensips{ctl,dbctl,unix} \
		$(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib/opensips
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/opensips/opensipsctl \
		$(1)/usr/lib/opensips/
	$(SED) 's,/bin/bash,/bin/sh,g' \
		-e 's,$(PKG_INSTALL_DIR),,g' \
		$(1)/usr/sbin/opensips{ctl,dbctl}
endef

define BuildPlugin
  define Package/opensips-mod-$(1)
    $$(call Package/opensips/Default)
    TITLE:= OpenSIPS $(3) module
    DEPENDS:=opensips $$(foreach m,$(4),+opensips-mod-$$m) $(5)
  endef

  define Package/opensips-mod-$(1)/install
	[ -z "$(2)" ] || $(INSTALL_DIR) $$(1)/usr/lib/opensips/modules/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/opensips/modules/$(2).so \
			$$(1)/usr/lib/opensips/modules/ ;
  endef

  $$(eval $$(call BuildPackage,opensips-mod-$(1)))
endef

$(eval $(call BuildPackage,opensips))
$(eval $(call BuildPackage,opensips-example))
$(eval $(call BuildPackage,opensips-tools))
$(eval $(call BuildPlugin,acc,acc,Accounting,tm,))
$(eval $(call BuildPlugin,alias-db,alias_db,Database-backend aliases,,))
$(eval $(call BuildPlugin,auth,auth,Authentication Framework,signaling,))
$(eval $(call BuildPlugin,auth-db,auth_db,Database-backend authentication,auth,))
$(eval $(call BuildPlugin,auth-radius,auth_radius,RADIUS-backend authentication,auth,+libradiusclient-ng))
$(eval $(call BuildPlugin,avp-radius,avp_radius,RADIUS-backend for AVP loading,,+libradiusclient-ng))
$(eval $(call BuildPlugin,avpops,avpops,AVP operation,,))
$(eval $(call BuildPlugin,benchmark,benchmark,Config benchmark,,))
$(eval $(call BuildPlugin,call-control,call_control,Prepair application,dialog,))
#$(eval $(call BuildPlugin,carrierroute,carrierroute,Carrier Routing,,+libconfuse))
$(eval $(call BuildPlugin,cfgutils,cfgutils,Config utilities,,))
$(eval $(call BuildPlugin,closeddial,closeddial,PBX-like features,,))
$(eval $(call BuildPlugin,cpl-c,cpl-c,Call Processing language interpreter,tm sl usrloc,+libxml2))
#$(eval $(call BuildPlugin,db-berkeley,db_berkeley,Berkeley DB Database-backend,,))
$(eval $(call BuildPlugin,db-flatstore,db_flatstore,Fast writing-only text database-backed,,))
$(eval $(call BuildPlugin,db-mysql,db_mysql,MySQL database-backend,,+libmysqlclient))
#$(eval $(call BuildPlugin,db-oracle,db_oracle,Oracle Database-backend,,))
$(eval $(call BuildPlugin,db-postgres,db_postgres,PostgreSQL Database-backend,,+libpq))
$(eval $(call BuildPlugin,db-text,db_text,Text database-backend,,))
#$(eval $(call BuildPlugin,db-unixodbc,db_unixodbc,UnixODBC Database-backend,,))
$(eval $(call BuildPlugin,dialog,dialog,Dialog support,rr tm,))
$(eval $(call BuildPlugin,dialplan,dialplan,Dialplan management,,))
$(eval $(call BuildPlugin,dispatcher,dispatcher,Dispatcher,,))
$(eval $(call BuildPlugin,diversion,diversion,Diversion header insertion,,))
$(eval $(call BuildPlugin,domain,domain,Multi-domain support,,))
$(eval $(call BuildPlugin,domainpolicy,domainpolicy,Domain policy,,))
$(eval $(call BuildPlugin,drouting,drouting,Dynaming routing,,))
$(eval $(call BuildPlugin,enum,enum,ENUM lookup,,))
$(eval $(call BuildPlugin,exec,exec,External exec,,))
$(eval $(call BuildPlugin,gflags,gflags,Global shared flags,,))
$(eval $(call BuildPlugin,group,group,Database-backend user-groups,,))
$(eval $(call BuildPlugin,group-radius,group_radius,RADIUS-backend user-groups,,+libradiusclient-ng))
$(eval $(call BuildPlugin,h350,h350,H.350,ldap,+libopenldap))
$(eval $(call BuildPlugin,identity,identity,Identity,,+libopenssl))
$(eval $(call BuildPlugin,imc,imc,IM conferencing,db-mysql tm,))
$(eval $(call BuildPlugin,jabber,jabber,Jabber,tm,+libexpat))
$(eval $(call BuildPlugin,lcr,lcr,Least Cost Routing,tm,))
$(eval $(call BuildPlugin,ldap,ldap,LDAP connector,,+libopenldap))
$(eval $(call BuildPlugin,load_balancer,load_balancer,Load balancer,dialog,))
$(eval $(call BuildPlugin,localcache,localcache,Local cache,,))
$(eval $(call BuildPlugin,mangler,mangler,SIP mangler,,))
$(eval $(call BuildPlugin,maxfwd,maxfwd,Max-Forward processor,,))
$(eval $(call BuildPlugin,mi-datagram,mi_datagram,Datagram support for Management Interface,,))
$(eval $(call BuildPlugin,mi-fifo,mi_fifo,FIFO support for Management Interface,,))
#$(eval $(call BuildPlugin,mi-xmlrpc,mi_xmlrpc,XML-RPC support for Management Interface,,+xmlrpc-c))
#$(eval $(call BuildPlugin,mediaproxy,mediaproxy,NAT traversal,,+mediaproxy))
#$(eval $(call BuildPlugin,mmgeoip,mmgeoip,MaxMind GeoIP,,+libgeoip))
$(eval $(call BuildPlugin,msilo,msilo,SIP message silo,tm,))
$(eval $(call BuildPlugin,nat_traversal,nat_traversal,NAT traversal,dialog sl tm,))
$(eval $(call BuildPlugin,nathelper,nathelper,NAT traversal helper,,+rtpproxy))
$(eval $(call BuildPlugin,options,options,OPTIONS server replier,sl,))
#$(eval $(call BuildPlugin,osp,osp,OSP peering,auth rr sl tm textops,+libosp))
$(eval $(call BuildPlugin,path,path,SIP path insertion,rr,))
$(eval $(call BuildPlugin,pdt,pdt,Prefix-to-Domain translator,,))
$(eval $(call BuildPlugin,peering,peering,Perring,,+libradiusclient-ng))
#$(eval $(call BuildPlugin,perl,perl,Perl,sl,+perl))
#$(eval $(call BuildPlugin,perlvdb,perlvdb,Perl Virtual DB,perl,))
$(eval $(call BuildPlugin,permissions,permissions,Permissions control,,))
$(eval $(call BuildPlugin,pike,pike,Flood detector,,))
$(eval $(call BuildPlugin,presence,presence,Presence server,signaling,+libxml2))
$(eval $(call BuildPlugin,presence-dialoginfo,presence_dialoginfo,Dialog Event presence,presence,))
$(eval $(call BuildPlugin,presence-mwi,presence_mwi,Message Waiting Indication presence,presence,))
$(eval $(call BuildPlugin,presence-xcapdiff,presence_xcapdiff,XCAP-Diff presence,presence pua,))
$(eval $(call BuildPlugin,presence-xml,presence_xml,XCAP presence,presence xcap-client,))
$(eval $(call BuildPlugin,pua,pua,Presence User Agent,tm,+libxml2))
$(eval $(call BuildPlugin,pua-bla,pua_bla,Bridged Line Appearence PUA,presence pua usrloc,))
$(eval $(call BuildPlugin,pua-dialoginfo,pua_dialoginfo,Dialog Event PUA,dialog pua,))
$(eval $(call BuildPlugin,pua-mi,pua_mi,PUA Management Interface,pua,))
$(eval $(call BuildPlugin,pua-usrloc,pua_usrloc,PUA User Location,pua usrloc,))
$(eval $(call BuildPlugin,pua-xmpp,pua_xmpp,PUA XMPP,presence pua xmpp,))
$(eval $(call BuildPlugin,qos,qos,QoS control,dialog,))
$(eval $(call BuildPlugin,ratelimit,ratelimit,Traffic shapping,,))
$(eval $(call BuildPlugin,regex,regex,Regular Expression,,+libpcre))
$(eval $(call BuildPlugin,registrar,registrar,SIP Registrar,signaling usrloc,))
$(eval $(call BuildPlugin,rls,rls,Resource List Server,presence pua signaling tm,+libxml2))
$(eval $(call BuildPlugin,rr,rr,Record-Route and Route,,))
$(eval $(call BuildPlugin,seas,seas,SIP Express Application Server,tm,))
$(eval $(call BuildPlugin,signaling,signaling,Signaling,sl tm,))
$(eval $(call BuildPlugin,siptrace,siptrace,SIP trace,,))
$(eval $(call BuildPlugin,sl,sl,Stateless replier,,))
$(eval $(call BuildPlugin,sms,sms,SIP-to-SMS IM gateway,tm,))
#$(eval $(call BuildPlugin,snmpstats,snmpstats,SNMP,presence usrloc,+libnetsnmp))
$(eval $(call BuildPlugin,speeddial,speeddial,Per-user speed-dial controller,,))
$(eval $(call BuildPlugin,statistics,statistics,Script statistics,,))
$(eval $(call BuildPlugin,sst,sst,SIP Session Timer,dialog sl,))
$(eval $(call BuildPlugin,textops,textops,Text operations,,))
$(eval $(call BuildPlugin,tlsops,tlsops,TLS operations,,+libopenssl))
$(eval $(call BuildPlugin,tm,tm,Transaction,,))
$(eval $(call BuildPlugin,uac,uac,User Agent Client,tm,))
$(eval $(call BuildPlugin,uac-redirect,uac_redirect,User Agent Client redirection,tm,))
$(eval $(call BuildPlugin,uri,uri,Generic URI operation,,))
$(eval $(call BuildPlugin,uri-db,uri_db,Database-backend SIP URI checking,,))
$(eval $(call BuildPlugin,uri-radius,uri_radius,RADIUS-backend SIP URI checking,,+libradiusclient-ng))
$(eval $(call BuildPlugin,userblacklist,userblacklist,User blacklists,,))
$(eval $(call BuildPlugin,usrloc,usrloc,User location,,))
$(eval $(call BuildPlugin,xcap-client,xcap_client,XCAP Client,,+libcurl +libxml2))
$(eval $(call BuildPlugin,xlog,xlog,Advanced logger,,))
$(eval $(call BuildPlugin,xmpp,xmpp,SIP-to-XMPP Gateway,tm,+libexpat))
