#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dsl_qos_queue
PKG_VERSION:=0.9.3
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.sonicspike.net/software/download/
PKG_MD5SUM:=ccbe8d28c3508c67f669fe98532533be

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/dsl-qos-queue
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+iptables +kmod-ipt-queue +iptables-mod-ipopt +libipq
  TITLE:=ADSL/ATM Bandwith management
  URL:=http://www.sonicspike.net/software/
endef

define Build/Configure
endef

define Build/Compile
	$(call Build/Compile/Default, \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include/libipq $(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		LIBIPQ="-lipq" \
		all \
	)
endef

define Package/dsl-qos-queue/install	
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dsl_qos_train $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ipt_rules $(1)/usr/sbin
endef

$(eval $(call BuildPackage,dsl-qos-queue))
