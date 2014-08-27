#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pmacct
PKG_VERSION:=0.14.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.pmacct.net/
PKG_MD5SUM:=4588531f0e8033fe82ad7fb22d1bbdb7

PKG_BUILD_DEPENDS:=libpcap

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)

PKG_CONFIG_DEPENDS:= \
	CONFIG_IPV6 \

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/pmacct/Default
  SECTION:=admin
  CATEGORY:=Administration
  SUBMENU:=Pmacct suite
  URL:=http://www.pmacct.net/
  DEPENDS=+libpcap
endef

define Package/pmacct-client
  $(call Package/pmacct/Default)
  TITLE:=Command-line client to gather data from the IMT plugin
  VARIANT:=basic
endef

define Package/nfacctd/Default
  $(call Package/pmacct/Default)
  TITLE:=NetFlow accounting daemon
endef

define Package/pmacctd/Default
  $(call Package/pmacct/Default)
  TITLE:=IP pcap-based accounting daemon
endef

define Package/sfacctd/Default
  $(call Package/pmacct/Default)
  TITLE:=sFlow accounting daemon
endef

define Package/uacctd/Default
  $(call Package/pmacct/Default)
  DEPENDS+= +iptables-mod-ulog
  TITLE:=ulog accounting daemon
endef


CONFIGURE_ARGS+= \
	$(call autoconf_bool,CONFIG_IPV6,ipv6) \
	--with-pcap-includes="$(STAGING_DIR)/usr/include" \
	--with-pcap-libs="$(STAGING_DIR)/usr/lib" \
	--enable-ulog \

ifeq ($(BUILD_VARIANT),basic)

  CONFIGURE_ARGS += \
	--disable-mysql \
	--disable-pgsql \
	--disable-sqlite3 \

endif

ifeq ($(BUILD_VARIANT),mysql)

  CONFIGURE_ARGS += \
	--enable-mysql \
	--with-mysql-includes="$(STAGING_DIR)/usr/include" \
	--with-mysql-libs="$(STAGING_DIR)/usr/lib/mysql" \
	--disable-pgsql \
	--disable-sqlite3 \

  CONFIGURE_VARS+= \
	LIBS="-lz" \

endif

ifeq ($(BUILD_VARIANT),pgsql)

  CONFIGURE_ARGS += \
	--disable-mysql \
	--enable-pgsql \
	--with-pgsql-includes="$(STAGING_DIR)/usr/include" \
	--with-pgsql-libs="$(STAGING_DIR)/usr/lib" \
	--disable-sqlite3 \

endif

ifeq ($(BUILD_VARIANT),sqlite)

  CONFIGURE_ARGS += \
	--disable-mysql \
	--disable-pgsql \
	--enable-sqlite3 \
	--with-sqlite3-includes="$(STAGING_DIR)/usr/include" \
	--with-sqlite3-libs="$(STAGING_DIR)/usr/lib" \

endif


define Package/pmacct-client/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/src/pmacct $(1)/usr/sbin/
endef


# $(1): build variant {basic,mysql,pgsql,sqlite}
# $(2): package name prefix {nfacctd,pmacctd,sfacctd,uacctd}
# $(3): package name suffix {,-mysql,-pgsql,-sqlite}
# $(4): additional title (with MySQL support, ...)
# $(5): additional dependencies (+libmysqlclient, ...)

define BuildPackage/Template

  define Package/$(2)$(3)
    $(call Package/$(2)/Default)
    DEPENDS+= $(5)
    TITLE+= $(4)
    VARIANT:=$(1)
  endef

  define Package/$(2)$(3)/conffiles
/etc/default/$(2)
/etc/$(2).conf
  endef

  define Package/$(2)$(3)/install
	$(INSTALL_DIR) $$(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/$(2) $$(1)/usr/sbin/
	$(INSTALL_DIR) $$(1)/etc
	$(INSTALL_DATA) ./files/$(2).conf $$(1)/etc/
	$(INSTALL_DIR) $$(1)/etc/default
	$(INSTALL_DATA) ./files/$(2).default $$(1)/etc/default/$(2)
	$(INSTALL_DIR) $$(1)/etc/init.d
	$(INSTALL_BIN) ./files/$(2).init $$(1)/etc/init.d/$(2)
  endef

  $$(eval $$(call BuildPackage,$(2)$(3)))

endef

$(eval $(call BuildPackage,pmacct-client))

$(eval $(call BuildPackage/Template,basic,nfacctd,,,))
$(eval $(call BuildPackage/Template,basic,pmacctd,,,))
$(eval $(call BuildPackage/Template,basic,sfacctd,,,))
$(eval $(call BuildPackage/Template,basic,uacctd,,,))

$(eval $(call BuildPackage/Template,mysql,nfacctd,-mysql,with MySQL support,+libmysqlclient))
$(eval $(call BuildPackage/Template,mysql,pmacctd,-mysql,with MySQL support,+libmysqlclient))
$(eval $(call BuildPackage/Template,mysql,sfacctd,-mysql,with MySQL support,+libmysqlclient))
$(eval $(call BuildPackage/Template,mysql,uacctd,-mysql,with MySQL support,+libmysqlclient))

$(eval $(call BuildPackage/Template,pgsql,nfacctd,-pgsql,with PostgreSQL support,+libpq))
$(eval $(call BuildPackage/Template,pgsql,pmacctd,-pgsql,with PostgreSQL support,+libpq))
$(eval $(call BuildPackage/Template,pgsql,sfacctd,-pgsql,with PostgreSQL support,+libpq))
$(eval $(call BuildPackage/Template,pgsql,uacctd,-pgsql,with PostgreSQL support,+libpq))

$(eval $(call BuildPackage/Template,sqlite,nfacctd,-sqlite,with SQLite support,+libsqlite3 +zlib))
$(eval $(call BuildPackage/Template,sqlite,pmacctd,-sqlite,with SQLite support,+libsqlite3 +zlib))
$(eval $(call BuildPackage/Template,sqlite,sfacctd,-sqlite,with SQLite support,+libsqlite3 +zlib))
$(eval $(call BuildPackage/Template,sqlite,uacctd,-sqlite,with SQLite support,+libsqlite3 +zlib))
