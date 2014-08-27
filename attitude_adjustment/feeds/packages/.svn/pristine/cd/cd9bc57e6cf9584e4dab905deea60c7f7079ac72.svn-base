#
# Copyright (C) 2009-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=openl2tp
PKG_VERSION:=1.8
PKG_RELEASE:=4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/openl2tp
PKG_MD5SUM:=e3d08dedfb9e6a9a1e24f6766f6dadd0

PKG_BUILD_DEPENDS:=libreadline libncurses USE_UCLIBC:librpc
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_PARALLEL:=1
PPPD_VERSION:=2.4.5

include $(INCLUDE_DIR)/package.mk

define Package/openl2tp/Default
  SUBMENU:=VPN
  SECTION:=net
  CATEGORY:=Network
  TITLE:=An L2TP (Layer 2 Tunneling Protocol) daemon
  URL:=http://www.openl2tp.org/
  PROVIDES:=openl2tp
  DEPENDS:=+ppp-mod-pppol2tp
endef

define Package/openl2tp/description/Default
 OpenL2TP is an open source L2TP client/server, written by Katalix Systems
 specifically for Linux. It has been designed for use as an enterprise 
 L2TP VPN server or in commercial, Linux-based, embedded networking
 products and is able to support hundreds of sessions, each with different
 configuration.
endef


define Package/openl2tp-full
$(call Package/openl2tp/Default)
  TITLE+=(full)
  VARIANT:=full
  DEPENDS+=+portmap +libreadline +libncurses $(LIBRPC_DEPENDS)
endef

define Package/openl2tp-full/description
$(call Package/openl2tp/description/Default)
 .
 This package a full-featured, RPC-enabled OpenL2TP.
endef


define Package/openl2tp-mini
$(call Package/openl2tp/Default)
  TITLE+=(mini)
  VARIANT:=mini
endef

define Package/openl2tp-mini/description
$(call Package/openl2tp/description/Default)
 .
 This package a minimal-featured, RPC-disabled OpenL2TP.
endef


MAKE_FLAGS+= \
	KERNEL_SRCDIR="$(LINUX_DIR)" \
	KERNEL_BLDDIR="$(LINUX_DIR)" \
	PPPD_VERSION="$(PKG_PPPD_VERSION)" \
	READLINE_DIR="$(STAGING_DIR)/usr" \
	PPPD_SRCDIR="$(STAGING_DIR)/usr/include" \
	PPPD_LIBDIR="$(STAGING_DIR)/usr/lib" \
	CFLAGS.optimize="$(TARGET_CFLAGS)" \
	CPPFLAGS-y="-I$(STAGING_DIR)/usr/include" \
	L2TP_FEATURE_LOCAL_CONF_FILE=y \
	CROSS_COMPILE="$(TARGET_CROSS)" \

ifeq ($(BUILD_VARIANT),full)

  MAKE_FLAGS += \
	L2TP_FEATURE_RPC_MANAGEMENT=y \
	L2TP_FEATURE_LOCAL_STAT_FILE=y \
	LIBS.dmalloc="-L$(STAGING_DIR)/usr/lib $(LIBRPC)" \

endif
ifeq ($(BUILD_VARIANT),mini)

  MAKE_FLAGS += \
	L2TP_FEATURE_RPC_MANAGEMENT= \
	L2TP_FEATURE_LOCAL_STAT_FILE= \

endif

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) ./src/*.[ch] $(PKG_BUILD_DIR)/
endef

define Package/openl2tp-full/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/l2tpconfig  $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/openl2tpd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib/pppd/$(PPPD_VERSION)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pppd/openl2tp.so $(1)/usr/lib/pppd/$(PPPD_VERSION)/
	$(INSTALL_DIR) $(1)/usr/lib/openl2tp
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/plugins/event_sock.so $(1)/usr/lib/openl2tp/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/plugins/ipsec.so $(1)/usr/lib/openl2tp/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/plugins/ppp_null.so $(1)/usr/lib/openl2tp/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/plugins/ppp_unix.so $(1)/usr/lib/openl2tp/
endef

define Package/openl2tp-mini/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/openl2tpd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib/openl2tp
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/plugins/ipsec.so $(1)/usr/lib/openl2tp/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/plugins/ppp_unix.so $(1)/usr/lib/openl2tp/
endef

$(eval $(call BuildPackage,openl2tp-full))
$(eval $(call BuildPackage,openl2tp-mini))
