#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=kamailio
PKG_VERSION:=1.5.2
PKG_RELEASE:=1
PKG_VARIANT:=-tls

PKG_SOURCE_URL:=http://www.kamailio.org/pub/kamailio/1.5.2/src/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)$(PKG_VARIANT)_src.tar.gz
PKG_MD5SUM:=26b0d0d3c67c2d981bee86a803cca51f

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)$(PKG_VARIANT)

PKG_BUILD_DEPENDS:=curl expat libxml2 mysql openldap openssl pcre postgresql radiusclient-ng

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += $(FPIC)

define Package/kamailio/Default
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Telephony
  URL:=http://www.kamailio.org/
endef


define Package/kamailio
$(call Package/kamailio/Default)
  TITLE:=Mature and flexible open source SIP server
  MENU:=1
endef

define Package/kamailio-example
$(call Package/kamailio/Default)
  TITLE:=Kamailio example config
  DEPENDS:=kamailio \
	+kamailio-mod-acc \
	+kamailio-mod-maxfwd \
	+kamailio-mod-mi-fifo \
	+kamailio-mod-pv \
	+kamailio-mod-registrar \
	+kamailio-mod-rr \
	+kamailio-mod-sl \
	+kamailio-mod-siputils \
	+kamailio-mod-textops \
	+kamailio-mod-tm \
	+kamailio-mod-uri-db \
	+kamailio-mod-usrloc \
	+kamailio-mod-xlog
endef

define Package/kamailio-example/conffiles
/etc/default/kamailio
/etc/kamailio/kamailio.cfg
endef


define Package/kamailio-tools
$(call Package/kamailio/Default)
  TITLE:=Kamailio control tools
  DEPENDS:=kamailio
endef

define Package/kamailio-tools/conffiles
/etc/kamailio/kamctlrc
endef


PKG_EXCL_MODULES:= \
	auth_diameter \
	carrierroute \
	db_berkeley \
	db_oracle \
	db_unixodbc \
	mediaproxy \
	mi_xmlrpc \
	osp \
	perl \
	perlvdb \
	purple \
	snmpstats \

PKG_MAKE_ARGS:= \
	cfg-dir="etc/kamailio/" \
	bin-dir="usr/sbin/" \
	data-dir="var/lib/kamailio/" \
	lib-dir="usr/lib/kamailio/" \
	modules-dir="usr/lib/kamailio/modules/" \
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
		PCREDEFS:="$(TARGET_CPPFLAGS)" \
		PCRELIBS:="$(TARGET_LDFLAGS)" \
		CROSS_COMPILE=1 \
		all
	$(MAKE) -C $(PKG_BUILD_DIR) $(PKG_MAKE_ARGS) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
endef

define Package/kamailio/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/sbin/kamailio \
		$(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib/kamailio/modules
endef

define Package/kamailio-example/install
	$(INSTALL_DIR) $(1)/etc/kamailio
	$(CP) \
		$(PKG_INSTALL_DIR)/etc/kamailio/kamailio.cfg \
		$(1)/etc/kamailio/
	$(SED) 's,$(PKG_INSTALL_DIR),,g' \
		$(1)/etc/kamailio/kamailio.cfg
	$(INSTALL_DIR) $(1)/etc/default
	$(INSTALL_DATA) ./files/kamailio.default $(1)/etc/default/kamailio
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/kamailio.init $(1)/etc/init.d/kamailio
endef

define Package/kamailio-tools/install
	$(INSTALL_DIR) $(1)/etc/kamailio
	$(CP) \
		$(PKG_INSTALL_DIR)/etc/kamailio/kamctlrc \
		$(1)/etc/kamailio/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/sbin/kam{ctl,dbctl,unix} \
		$(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib/kamailio
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/kamailio/kamctl \
		$(1)/usr/lib/kamailio/
	$(SED) 's,/bin/bash,/bin/sh,g' \
		-e 's,$(PKG_INSTALL_DIR),,g' \
		$(1)/usr/sbin/kam{ctl,dbctl}
endef

define BuildPlugin
  define Package/kamailio-mod-$(1)
    $$(call Package/kamailio/Default)
    TITLE:= kamailio $(3) module
    DEPENDS:=kamailio $$(foreach m,$(4),+kamailio-mod-$$m) $(5)
  endef

  define Package/kamailio-mod-$(1)/install
	[ -z "$(2)" ] || $(INSTALL_DIR) $$(1)/usr/lib/kamailio/modules/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/kamailio/modules/$(2).so \
			$$(1)/usr/lib/kamailio/modules/ ;
  endef

  $$(eval $$(call BuildPackage,kamailio-mod-$(1)))
endef

$(eval $(call BuildPackage,kamailio))
$(eval $(call BuildPackage,kamailio-example))
$(eval $(call BuildPackage,kamailio-tools))
$(eval $(call BuildPlugin,acc,acc,Accounting,tm,))
$(eval $(call BuildPlugin,alias-db,alias_db,Database-backend aliases,,))
$(eval $(call BuildPlugin,auth,auth,Authentication Framework,,))
$(eval $(call BuildPlugin,auth-db,auth_db,Database-backend authentication,auth,))
#$(eval $(call BuildPlugin,auth-diameter,auth_diameter,Diameter-backend authentication,sl,))
$(eval $(call BuildPlugin,auth-radius,auth_radius,RADIUS-backend authentication,auth,+libradiusclient-ng))
$(eval $(call BuildPlugin,avpops,avpops,AVP operation,,))
$(eval $(call BuildPlugin,benchmark,benchmark,Config benchmark,,))
#$(eval $(call BuildPlugin,carrierroute,carrierroute,Carrier Routing,,+libconfuse))
$(eval $(call BuildPlugin,cfgutils,cfgutils,Config utilities,,))
$(eval $(call BuildPlugin,cpl-c,cpl-c,Call Processing language interpreter,tm sl usrloc,+libxml2))
#$(eval $(call BuildPlugin,db-berkeley,db_berkeley,Berkeley DB Database-backend,,))
$(eval $(call BuildPlugin,db-flatstore,db_flatstore,Fast writing-only text database-backed,,))
$(eval $(call BuildPlugin,db-mysql,db_mysql,MySQL database-backend,,+libmysqlclient))
#$(eval $(call BuildPlugin,db-oracle,db_oracle,Oracle Database-backend,,))
$(eval $(call BuildPlugin,db-postgres,db_postgres,PostgreSQL Database-backend,,+libpq))
$(eval $(call BuildPlugin,db-text,db_text,Text database-backend,,))
#$(eval $(call BuildPlugin,db-unixodbc,db_unixodbc,UnixODBC Database-backend,,))
$(eval $(call BuildPlugin,dialog,dialog,Dialog support,rr tm,))
$(eval $(call BuildPlugin,dialplan,dialplan,Dialplan management,,+libpcre))
$(eval $(call BuildPlugin,dispatcher,dispatcher,Dispatcher,,))
$(eval $(call BuildPlugin,diversion,diversion,Diversion header insertion,,))
$(eval $(call BuildPlugin,domain,domain,Multi-domain support,,))
$(eval $(call BuildPlugin,domainpolicy,domainpolicy,Domain policy,,))
$(eval $(call BuildPlugin,enum,enum,ENUM lookup,,))
$(eval $(call BuildPlugin,exec,exec,External exec,,))
$(eval $(call BuildPlugin,group,group,Database-backend user-groups,,))
$(eval $(call BuildPlugin,h350,h350,H.350,ldap,+libopenldap))
$(eval $(call BuildPlugin,htable,htable,Hash Table,,))
$(eval $(call BuildPlugin,imc,imc,IM conferencing,db-mysql tm,))
$(eval $(call BuildPlugin,jabber,jabber,Jabber,tm,+libexpat))
$(eval $(call BuildPlugin,lcr,lcr,Least Cost Routing,tm,+libpcre))
$(eval $(call BuildPlugin,ldap,ldap,LDAP connector,,+libopenldap))
$(eval $(call BuildPlugin,maxfwd,maxfwd,Max-Forward processor,,))
$(eval $(call BuildPlugin,mi-datagram,mi_datagram,Datagram support for Management Interface,,))
$(eval $(call BuildPlugin,mi-fifo,mi_fifo,FIFO support for Management Interface,,))
#$(eval $(call BuildPlugin,mi-xmlrpc,mi_xmlrpc,XML-RPC support for Management Interface,,+xmlrpc-c))
#$(eval $(call BuildPlugin,mediaproxy,mediaproxy,NAT traversal,,+mediaproxy))
$(eval $(call BuildPlugin,misc_radius,misc_radius,Generic RADIUS,,+libradiusclient-ng))
$(eval $(call BuildPlugin,msilo,msilo,SIP message silo,tm,))
$(eval $(call BuildPlugin,nat_traversal,nat_traversal,NAT traversal,dialog sl tm,))
$(eval $(call BuildPlugin,nathelper,nathelper,NAT traversal helper,,+rtpproxy))
#$(eval $(call BuildPlugin,osp,osp,OSP peering,auth rr sl tm textops,+libosp))
$(eval $(call BuildPlugin,path,path,SIP path insertion,rr,))
$(eval $(call BuildPlugin,pdt,pdt,Prefix-to-Domain translator,,))
$(eval $(call BuildPlugin,peering,peering,Perring,,+libradiusclient-ng))
#$(eval $(call BuildPlugin,perl,perl,Perl,sl,+perl))
#$(eval $(call BuildPlugin,perlvdb,perlvdb,Perl Virtual DB,perl,))
$(eval $(call BuildPlugin,permissions,permissions,Permissions control,,))
$(eval $(call BuildPlugin,pike,pike,Flood detector,,))
$(eval $(call BuildPlugin,presence,presence,Presence server,sl tm,+libxml2))
$(eval $(call BuildPlugin,presence-dialoginfo,presence_dialoginfo,Dialog Event presence,presence,))
$(eval $(call BuildPlugin,presence-mwi,presence_mwi,Message Waiting Indication presence,presence,))
$(eval $(call BuildPlugin,presence-xml,presence_xml,XCAP presence,presence xcap-client,))
$(eval $(call BuildPlugin,pua,pua,Presence User Agent,tm,+libxml2))
$(eval $(call BuildPlugin,pua-bla,pua_bla,Bridged Line Appearence PUA,presence pua usrloc,))
$(eval $(call BuildPlugin,pua-dialoginfo,pua_dialoginfo,Dialog Event PUA,dialog pua,))
$(eval $(call BuildPlugin,pua-mi,pua_mi,PUA Management Interface,pua,))
$(eval $(call BuildPlugin,pua-usrloc,pua_usrloc,PUA User Location,pua usrloc,))
$(eval $(call BuildPlugin,pua-xmpp,pua_xmpp,PUA XMPP,presence pua xmpp,))
#$(eval $(call BuildPlugin,purple,purple,Multi-protocol IM gateway,presence pua,+libpurple))
$(eval $(call BuildPlugin,pv,pv,Pseudo-Variables,,))
$(eval $(call BuildPlugin,qos,qos,QoS control,dialog,))
$(eval $(call BuildPlugin,ratelimit,ratelimit,Traffic shapping,,))
$(eval $(call BuildPlugin,regex,regex,Regular Expression,,+libpcre))
$(eval $(call BuildPlugin,registrar,registrar,SIP Registrar,usrloc,))
$(eval $(call BuildPlugin,rls,rls,Resource List Server,presence pua tm,+libxml2))
$(eval $(call BuildPlugin,rr,rr,Record-Route and Route,,))
$(eval $(call BuildPlugin,rtimer,rtimer,Routing Timer,,))
$(eval $(call BuildPlugin,sanity,sanity,SIP sanity checks,sl,))
#$(eval $(call BuildPlugin,seas,seas,SIP Express Application Server,tm,))
$(eval $(call BuildPlugin,siptrace,siptrace,SIP trace,,))
$(eval $(call BuildPlugin,siputils,siputils,SIP utilities,sl,))
$(eval $(call BuildPlugin,sl,sl,Stateless replier,,))
$(eval $(call BuildPlugin,sms,sms,SIP-to-SMS IM gateway,tm,))
#$(eval $(call BuildPlugin,snmpstats,snmpstats,SNMP,presence usrloc,+libnetsnmp))
$(eval $(call BuildPlugin,speeddial,speeddial,Per-user speed-dial controller,,))
$(eval $(call BuildPlugin,sqlops,sqlops,SQL operations,,))
$(eval $(call BuildPlugin,statistics,statistics,Script statistics,,))
$(eval $(call BuildPlugin,sst,sst,SIP Session Timer,dialog sl,))
$(eval $(call BuildPlugin,textops,textops,Text operations,,))
$(eval $(call BuildPlugin,tlsops,tlsops,TLS operations,,+libopenssl))
$(eval $(call BuildPlugin,tm,tm,Transaction,,))
$(eval $(call BuildPlugin,uac,uac,User Agent Client,tm,))
$(eval $(call BuildPlugin,uac-redirect,uac_redirect,User Agent Client redirection,tm,))
$(eval $(call BuildPlugin,uri-db,uri_db,Database-backend SIP URI checking,,))
$(eval $(call BuildPlugin,userblacklist,userblacklist,User blacklists,,))
$(eval $(call BuildPlugin,usrloc,usrloc,User location,,))
$(eval $(call BuildPlugin,utils,utils,Misc utilities,,+libcurl))
$(eval $(call BuildPlugin,xcap-client,xcap_client,XCAP Client,,+libcurl))
$(eval $(call BuildPlugin,xlog,xlog,Advanced logger,,))
$(eval $(call BuildPlugin,xmpp,xmpp,SIP-to-XMPP Gateway,tm,+libexpat))
