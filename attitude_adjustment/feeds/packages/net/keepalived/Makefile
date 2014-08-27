#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=keepalived
PKG_VERSION:=1.1.19
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:= http://www.keepalived.org/software
PKG_MD5SUM:=a35b8d9d462810f7650d292bd7457523

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/keepalived
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Failover and monitoring daemon for LVS clusters
  URL:=http://www.keepalived.org/
  DEPENDS:=+libpopt +libopenssl
endef

define Package/keepalived/description
 Failover and monitoring daemon for Linux Virtual Server (LVS) clusters.
endef

define Package/keepalived/conffiles
/etc/keepalived/keepalived.conf
endef

acvar=$(subst -,_,$(subst .,_,$(subst /,_,$(1))))

CONFIGURE_ARGS+= \
	--enable-shared \
	--disable-static \
	--disable-debug \
	--disable-strip \
	--disable-lvs \
	--disable-lvs-syncd \
	--with-kernel-dir="$(LINUX_DIR)"

CONFIGURE_VARS+= \
	ac_cv_file_$(call acvar,$(LINUX_DIR)/net/core/link_watch.c)=yes

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		STRIP="/bin/true" \
		all install
endef

define Package/keepalived/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/keepalived $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/genhash $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/keepalived
	$(INSTALL_CONF) $(PKG_INSTALL_DIR)/etc/keepalived/keepalived.conf $(1)/etc/keepalived/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/keepalived.init $(1)/etc/init.d/keepalived
endef

$(eval $(call BuildPackage,keepalived))
