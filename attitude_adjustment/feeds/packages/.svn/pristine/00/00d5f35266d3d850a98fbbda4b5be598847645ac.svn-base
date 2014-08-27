#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=openser
PKG_VERSION:=1.0.1
PKG_RELEASE:=1
PKG_MD5SUM:=eebc16665003ee92daf96b216d6f9563
# PKG_VARIANT:=-tls # for tls version
PKG_VARIANT:=

PKG_SOURCE_URL:=http://openser.org/pub/openser/$(PKG_VERSION)/src
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)$(PKG_VARIANT)_src.tar.gz

include $(INCLUDE_DIR)/package.mk

define Package/openser
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Telephony
  TITLE:=Configurable, free SIP server
  URL:=http://www.openser.org
  MENU:=1
endef

define Package/openser/conffiles
/etc/openser/openser.cfg
endef

define Package/openser-mod-acc
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser +libradiusclient-ng
  TITLE:=Accounting support
  SUBMENU:=Telephony
endef

define Package/openser-mod-auth
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser
  TITLE:=Authentication support
  SUBMENU:=Telephony
endef

define Package/openser-mod-auth-db
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser
  TITLE:=Authentication with database support
  SUBMENU:=Telephony
endef

define Package/openser-mod-auth-radius
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser +libradiusclient-ng
  TITLE:=Authentication with RADIUS support
  SUBMENU:=Telephony
endef

define Package/openser-mod-avp-radius
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser +libradiusclient-ng
  TITLE:=AVP with RADIUS support
  SUBMENU:=Telephony
endef

define Package/openser-mod-avpops
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser
  TITLE:=AVP options support
  SUBMENU:=Telephony
endef

define Package/openser-mod-dispatcher
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser
  TITLE:=Dispatcher support
  SUBMENU:=Telephony
endef

define Package/openser-mod-diversion
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser
  TITLE:=Diversion support
  SUBMENU:=Telephony
endef

define Package/openser-mod-flatstore
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser
  TITLE:=Flatstore support
  SUBMENU:=Telephony
endef

define Package/openser-mod-gflags
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser
  TITLE:=Gflags support
  SUBMENU:=Telephony
endef

define Package/openser-mod-group
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser
  TITLE:=Group support
  SUBMENU:=Telephony
endef

define Package/openser-mod-group-radius
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser +libradiusclient-ng
  TITLE:=Group with RADIUS support
  SUBMENU:=Telephony
endef

define Package/openser-mod-lcr
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser openser-mod-mysql
  TITLE:=Least Cost Routing support
  SUBMENU:=Telephony
endef

define Package/openser-mod-mediaproxy
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser
  TITLE:=Mediaproxy support
  SUBMENU:=Telephony
endef

define Package/openser-mod-msilo
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser
  TITLE:=MSilo support
  SUBMENU:=Telephony
endef

define Package/openser-mod-mysql
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser +libmysqlclient
  TITLE:=MySQL support
  SUBMENU:=Telephony
endef

define Package/openser-mod-nathelper
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser
  TITLE:=NAT helper support
  SUBMENU:=Telephony
endef

define Package/openser-mod-options
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser
  TITLE:=Options support
  SUBMENU:=Telephony
endef

define Package/openser-mod-pdt
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser
  TITLE:=PDT support
  SUBMENU:=Telephony
endef

define Package/openser-mod-permissions
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser
  TITLE:=Permissions support
  SUBMENU:=Telephony
endef

define Package/openser-mod-pike
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser
  TITLE:=PIKE support
  SUBMENU:=Telephony
endef

define Package/openser-mod-sms
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser
  TITLE:=SMS support
  SUBMENU:=Telephony
endef

define Package/openser-mod-speeddial
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser
  TITLE:=Speeddial supporte
  SUBMENU:=Telephony
endef

define Package/openser-mod-uac
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser
  TITLE:=UAC support
  SUBMENU:=Telephony
endef

define Package/openser-mod-uac-redirect
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser
  TITLE:=UAC with redirect support
  SUBMENU:=Telephony
endef

define Package/openser-mod-uri
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser
  TITLE:=URI support
  SUBMENU:=Telephony
endef

define Package/openser-mod-uri-db
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser +openser-mod-mysql
  TITLE:=URI with database support
  SUBMENU:=Telephony
endef

define Package/openser-mod-uri-radius
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser +libradiusclient-ng
  TITLE:=URI with RADIUS support
  SUBMENU:=Telephony
endef

define Package/openser-mod-xlog
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=openser
  TITLE:=Advanced logging support
  SUBMENU:=Telephony
endef

# Select here the modules for the ipk package
OPENSER_MODULES := sl tm rr maxfwd usrloc registrar dbtext textops exec

OPENSER_MODULE_FILES := $(foreach module,$(OPENSER_MODULES),modules/$(module)/$(module).so)
OPENSER_MODULES := $(patsubst %,modules/%,$(OPENSER_MODULES))

define Build/Compile	
	$(MAKE) -C $(PKG_BUILD_DIR) \
		prefix=/ \
		extra_defs="-DUSE_PTHREAD_MUTEX "  \
		CC="$(TARGET_CC)" \
		ARCH="$(ARCH)"  \
		CFLAGS="$(TARGET_CFLAGS) $(FPIC)" \
		LOCALBASE="$(STAGING_DIR)/usr" \
		all utils/gen_ha1/gen_ha1
endef

define Package/openser/install	
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/sbin/
	$(CP) $(PKG_BUILD_DIR)/utils/gen_ha1/gen_ha1 $(1)/usr/sbin/openser_gen_ha1
	$(CP) $(PKG_BUILD_DIR)/scripts/sc $(1)/usr/sbin/openserctl
	chmod 744 $(1)/usr/sbin/openserctl
	$(CP) $(PKG_BUILD_DIR)/scripts/sc.dbtext $(1)/usr/sbin/dbtextctl
	chmod 744 $(1)/usr/sbin/dbtextctl
	$(INSTALL_DIR) $(1)/usr/lib/openser/modules
	(cd $(PKG_BUILD_DIR);\
		$(CP) $(OPENSER_MODULE_FILES) $(1)/usr/lib/openser/modules/; \
	)
	$(INSTALL_DIR) $(1)/etc/openser
	$(CP) $(PKG_BUILD_DIR)/etc/openser.cfg $(1)/etc/openser/
endef

define BuildPlugin
  define Package/$(1)/install
  	[ -z "$(2)" ] || $(INSTALL_DIR) $$(1)/usr/lib/openser/modules/
	$(CP) $(PKG_BUILD_DIR)/modules/$(2)/$(2).so \
			$$(1)/usr/lib/openser/modules/ ;
  endef

  $$(eval $$(call BuildPackage,$(1)))
endef

$(eval $(call BuildPackage,openser))
$(eval $(call BuildPlugin,openser-mod-acc,acc))
$(eval $(call BuildPlugin,openser-mod-auth,auth))
$(eval $(call BuildPlugin,openser-mod-auth-db,auth_db))
$(eval $(call BuildPlugin,openser-mod-auth-radius,auth_radius))
$(eval $(call BuildPlugin,openser-mod-avp-radius,avp_radius))
$(eval $(call BuildPlugin,openser-mod-avpops,avpops))
$(eval $(call BuildPlugin,openser-mod-dispatcher,dispatcher))
$(eval $(call BuildPlugin,openser-mod-diversion,diversion))
$(eval $(call BuildPlugin,openser-mod-flatstore,flatstore))
$(eval $(call BuildPlugin,openser-mod-gflags,gflags))
$(eval $(call BuildPlugin,openser-mod-group,group))
$(eval $(call BuildPlugin,openser-mod-group-radius,group_radius))
$(eval $(call BuildPlugin,openser-mod-lcr,lcr))
$(eval $(call BuildPlugin,openser-mod-mediaproxy,mediaproxy))
$(eval $(call BuildPlugin,openser-mod-msilo,msilo))
$(eval $(call BuildPlugin,openser-mod-mysql,mysql))
$(eval $(call BuildPlugin,openser-mod-nathelper,nathelper))
$(eval $(call BuildPlugin,openser-mod-options,options))
$(eval $(call BuildPlugin,openser-mod-pdt,pdt))
$(eval $(call BuildPlugin,openser-mod-permissions,permissions))
$(eval $(call BuildPlugin,openser-mod-pike,pike))
$(eval $(call BuildPlugin,openser-mod-sms,sms))
$(eval $(call BuildPlugin,openser-mod-speeddial,speeddial))
$(eval $(call BuildPlugin,openser-mod-uac,uac))
$(eval $(call BuildPlugin,openser-mod-uac-redirect,uac_redirect))
$(eval $(call BuildPlugin,openser-mod-uri,uri))
$(eval $(call BuildPlugin,openser-mod-uri-db,uri_db))
$(eval $(call BuildPlugin,openser-mod-uri-radius,uri_radius))
$(eval $(call BuildPlugin,openser-mod-xlog,xlog))
