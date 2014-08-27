#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=squid
PKG_VERSION:=2.7.STABLE9
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.squid-cache.org/Versions/v2/2.7/
PKG_MD5SUM:=3c6642c85470b1079207d43bba25a819

include $(INCLUDE_DIR)/package.mk

define Package/squid/Default
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Web Servers/Proxies
  URL:=http://www.squid-cache.org/
endef

define Package/squid
  $(call Package/squid/Default)
  MENU:=1
  DEPENDS:=+libopenssl +libpthread
  TITLE:=full-featured Web proxy cache
endef

define Package/squid/description
	Squid is a high-performance proxy caching server for web clients,
	supporting FTP, gopher, and HTTP data objects. Unlike traditional
	caching software, Squid handles all requests in a single,
	non-blocking, I/O-driven process.
endef

define Package/squid/conffiles
/etc/squid/mime.conf
/etc/squid/squid.conf
endef

define Package/squid-mod-cachemgr
  $(call Package/squid/Default)
  DEPENDS:=squid
  TITLE:=Web based proxy manager and reporting tool
endef

define Package/squid-mod-basic-auth-getpwnam
  $(call Package/squid/Default)
  DEPENDS:=squid
  TITLE:=getpwnam basic authentication helper
endef

define Package/squid-mod-basic-auth-ncsa
  $(call Package/squid/Default)
  DEPENDS:=squid
  TITLE:=NCSA basic authentication helper
endef

define Package/squid-mod-basic-auth-smb
  $(call Package/squid/Default)
  DEPENDS:=squid
  TITLE:=Samba basic authentication helper
endef

define Package/squid-mod-digest-auth-password
  $(call Package/squid/Default)
  DEPENDS:=squid
  TITLE:=Password digest authentication helper
endef

define Package/squid-mod-external-acl-ip-user
  $(call Package/squid/Default)
  DEPENDS:=squid
  TITLE:=IP user external ACL helper
endef

define Package/squid-mod-external-acl-unix-group
  $(call Package/squid/Default)
  DEPENDS:=squid
  TITLE:=Unix group external ACL helper
endef

define Package/squid-mod-ntlm-auth-fakeauth
  $(call Package/squid/Default)
  DEPENDS:=squid
  TITLE:=Fakeauth NTLM authentication helper
endef

define Package/squid-mod-ntlm-auth-smb-auth
  $(call Package/squid/Default)
  DEPENDS:=squid
  TITLE:=Samba NTLM authentication helper
endef

CONFIGURE_ARGS += \
	--datadir=/usr/share/squid \
	--libexecdir=/usr/lib/squid \
	--sysconfdir=/etc/squid \
	--enable-shared \
	--enable-static \
	--enable-x-accelerator-vary \
	--with-pthreads \
	--with-dl \
	--enable-icmp \
	--enable-kill-parent-hack \
	--enable-arp-acl \
	--enable-ssl \
	--enable-htcp \
	--enable-err-languages=English \
	--enable-default-err-language=English \
	--enable-linux-netfilter \
	--enable-icmp \
	--enable-external-acl-helpers="" \
	--enable-underscores \
	--enable-cache-digests \
	--enable-referer-log \
	--enable-delay-pools \
	--enable-useragent-log \
	--with-openssl=$(STAGING_DIR)/usr \
	--enable-auth="basic digest ntlm" \
	--enable-basic-auth-helpers="getpwnam NCSA SMB" \
	--enable-ntlm-auth-helpers="fakeauth SMB" \
	--enable-digest-auth-helpers="password" \
	--enable-external-acl-helpers="ip_user unix_group" \
	--enable-storeio=null,ufs \
	--enable-epoll \
	--with-maxfd=4096 \
	--without-libcap

CONFIGURE_VARS += \
	ac_cv_header_linux_netfilter_ipv4_h=yes \
	ac_cv_epoll_works=yes \

define Build/Compile
	# pass INCLUDES to compile host sources against our OpenSSL, not the host one
	$(MAKE) -C $(PKG_BUILD_DIR)/lib \
		all
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
endef

define BuildPlugin
  define Package/$(1)/install
	$(INSTALL_DIR) $$(1)/usr/lib/squid
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/squid/$(2) $$(1)/usr/lib/squid/
  endef

  $$(eval $$(call BuildPackage,$(1)))
endef

define Package/squid/install
	$(INSTALL_DIR) $(1)/etc/squid
	$(CP) $(PKG_INSTALL_DIR)/etc/squid/mime.conf $(1)/etc/squid/
	$(CP) $(PKG_INSTALL_DIR)/etc/squid/squid.conf $(1)/etc/squid/
	$(INSTALL_DIR) $(1)/usr/share/squid
	$(CP) $(PKG_INSTALL_DIR)/usr/share/squid/* $(1)/usr/share/squid/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/squid $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib/squid
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/squid/{unlinkd,pinger,logfile-daemon} $(1)/usr/lib/squid/
endef

define Package/squid-mod-cachemgr/install
	$(INSTALL_DIR) $(1)/www/cgi-bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/squid/cachemgr.cgi $(1)/www/cgi-bin/
endef

$(eval $(call RequireHeader,/usr/include/openssl/ssl.h, \
	$(PKG_NAME) requires the openssl development (named like libssl-dev, \
	depending of your package manager) package be installed on the host-system. \
))

$(eval $(call BuildPackage,squid))
$(eval $(call BuildPackage,squid-mod-cachemgr))
$(eval $(call BuildPlugin,squid-mod-basic-auth-getpwnam,getpwname_auth))
$(eval $(call BuildPlugin,squid-mod-basic-auth-ncsa,ncsa_auth))
$(eval $(call BuildPlugin,squid-mod-basic-auth-smb,smb_auth))
$(eval $(call BuildPlugin,squid-mod-digest-auth-password,digest_pw_auth))
$(eval $(call BuildPlugin,squid-mod-external-acl-ip-user,ip_user_check))
$(eval $(call BuildPlugin,squid-mod-external-acl-unix-group,squid_unix_group))
$(eval $(call BuildPlugin,squid-mod-ntlm-auth-fakeauth,fakeauth_auth))
$(eval $(call BuildPlugin,squid-mod-ntlm-auth-smb-auth,ntlm_auth))

